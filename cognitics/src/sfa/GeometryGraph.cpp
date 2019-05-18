/*************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/

#include "sfa/GeometryGraph.h"

#include "sfa/EdgeIntersector.h"

#include "sfa/Point.h"
#include "sfa/LineString.h"
#include "sfa/Polygon.h"
#include "sfa/MultiPoint.h"
#include "sfa/MultiLineString.h"
#include "sfa/MultiPolygon.h"
#include "sfa/PolyhedralSurface.h"
#include <exception>
#include <stdexcept>

namespace sfa {

    GeometryGraph::GeometryGraph(const Geometry* geom, int id, bool saveZ) : saveZ(saveZ)
    {
        arg = id;
        parent = geom;
        simple = true;
        addGeometry(geom);
    }

    GeometryGraph::~GeometryGraph(void)
    {
    //    Delete all isolated points
        for (PointNodeList::iterator it = points.begin(), end = points.end(); it != end; it++)
            delete (*it);

    //    Delete all Edges
        for (EdgeNodeList::iterator it = edges.begin(), end = edges.end(); it != end; it++)
            delete (*it);
    }

    void GeometryGraph::addGeometry(const Geometry* geom)
    {
        int type = geom->getWKBGeometryType(false,false);

        switch(type)
        {
        case (wkbPoint):
            addPoint(geom);
            break;
        case (wkbLineString):
            addLineString(geom);
            break;
        case (wkbPolygon):
        case (wkbTriangle):
            addPolygon(geom);
            break;
        case (wkbMultiPoint):
            addMultiPoint(geom);
            break;
        case (wkbMultiLineString):
            addMultiLineString(geom);
            break;
        case (wkbMultiPolygon):
            addMultiPolygon(geom);
            break;
        case (wkbPolyhedralSurface):
        case (wkbTIN):
            addPolySurface(geom);
            break;
        case (wkbGeometryCollection):
            addGeometryCollection(geom);
            break;
        default:
            throw std::runtime_error("GeometryGraph::addGeometry() Invalid geometry given!");
        }
    }

    void GeometryGraph::addPoint(const Geometry* geom)
    {
    //    Assumed simple
        Label l(0);
        l.loc[arg] = INTERIOR;
        PointNode* point = new PointNode(dynamic_cast<const Point*>(geom),l,arg,true);
        addIsolatedPoint(point);
    }

    void GeometryGraph::addLineString(const Geometry* geom)
    {
    //    Cannot be assumed simple
        simple = false;

        const LineString* line = dynamic_cast<const LineString*>(geom);

        if (line->isClosed()) 
        {
            addLinearRing(geom);
            return;
        }

        //    Creat Label for enpoints
        Label b;
        b.dim = 0;
        b.loc[arg] = BOUNDARY;

        //    Special Case: Using a collapse line to represent a single boundary point
        if (line->getNumPoints() == 2 && line->getStartPoint()->equals(line->getEndPoint()))
        {
            PointNode* node = new PointNode(line->getStartPoint(),b,arg);
            addBoundaryPoint(node);
            addIsolatedPoint(node);
            return;
        }

        //    Create label for edge
        Label el, e, er;
        el.dim = 2; e.dim = 1; er.dim = 2;
        el.loc[arg] = EXTERIOR; e.loc[arg] = INTERIOR; er.loc[arg] = EXTERIOR; 

        EdgeNode* edge = new EdgeNode(el,e,er,arg);

        //    Add first and last Boundary Points
        PointNode* start = new PointNode(line->getStartPoint(),b,arg);
        PointNode* end = new PointNode(line->getEndPoint(),b,arg);
        addBoundaryPoint(start);
        addBoundaryPoint(end);

        //    Add the points to the edge
        edge->addPointNode(start);
        for (int i = 1; i < line->getNumPoints() - 1; i++)
        {
            edge->addPoint(line->getPointN(i));
        }
        edge->addPointNode(end);
        edges.push_back(edge);
    }

    void GeometryGraph::addLinearRing(const Geometry* geom)
    {
    //    Assumed to be simple
        const LineString* line = dynamic_cast<const LineString*>(geom);

        //    Create label for edge
        Label el, e, er;
        el.dim = 2; e.dim = 1; er.dim = 2;
        el.loc[arg] = EXTERIOR; e.loc[arg] = INTERIOR; er.loc[arg] = EXTERIOR; 

        EdgeNode* edge = new EdgeNode(el,e,er,arg);
        //    Add first point
        edge->addPoint(line->getPointN(0));
        PointNode* start = edge->getPointN(0);

        //    Add the points to the edge
        for (int i = 1; i < line->getNumPoints()-1; i++)
        {
            edge->addPoint(line->getPointN(i));
        }

        //    Add back the same pointer for the first point
        edge->addPoint(start->getPoint());
        edges.push_back(edge);
    }
    
    void GeometryGraph::addPolygonRing(const Geometry* geom, Location left, Location right)
    {
    //    Assumed to be simple
        const LineString* line = dynamic_cast<const LineString*>(geom);

        //    Create label for edge
        Label el, e, er;
        el.dim = 2; e.dim = 1; er.dim = 2;
        el.loc[arg] = left; e.loc[arg] = BOUNDARY; er.loc[arg] = right; 

        EdgeNode* edge = new EdgeNode(el,e,er,arg, saveZ);

        //    Add first point
        edge->addPoint(line->getPointN(0));
        PointNode* start = edge->getPointN(0);

        //    Add the points to the edge
        for (int i = 1; i < line->getNumPoints()-1; i++)
        {
            edge->addPoint(line->getPointN(i));
        }

        //    Add back the same pointer for the first point
        edge->addPoint(start->getPoint());
        edges.push_back(edge);
    }

    void GeometryGraph::addPolygon(const Geometry* geom)
    {
    //    Assumed to be simple
        //    Polygons are defined counterClockWise for the exterior ring, and clockwise for interior rings
        //    Add Exterior Ring
        const Polygon* polygon = dynamic_cast<const Polygon*>(geom);

    //    Check for collapsed area case. This special case is used to represent a boundary line with no area
        if (polygon->getExteriorRing()->getNumPoints() == 3)
        {
            if (polygon->getExteriorRing()->getStartPoint()->equals(polygon->getExteriorRing()->getEndPoint()))
            {
                Label b;
                b.dim = 1;
                b.loc[arg] = BOUNDARY;
                Label ext;
                ext.dim = 2;
                ext.loc[arg] = EXTERIOR;

                EdgeNode* edge = new EdgeNode(ext,b,ext,arg);
                edge->addPoint(polygon->getExteriorRing()->getPointN(0));
                edge->addPoint(polygon->getExteriorRing()->getPointN(1));
                edges.push_back(edge);
                return;
            }
        }

    /*    
        OPTIONAL 1
        Add support for clockwise rings?
        Simple and easy to add support for the rings, they just need to be defined with the edges being the
        other direction; however, this more than doubles the time needed to simple add the ring since the 
        area of the ring is determined in order to find its orientation. So the trade up is time for robustness

        bool CCW = isRingCounterClockwise(polygon->getExteriorRing());
        addPolygonRing(polygon->getExteriorRing(),CCW ? INTERIOR : EXTERIOR, CCW ? EXTERIOR : INTERIOR);
    */

        addPolygonRing(polygon->getExteriorRing(),INTERIOR,EXTERIOR);

        //    Add Interior Rings
        for (int i = 0; i < polygon->getNumInteriorRing(); i++)
        {
    /*
        OPTIONAL 1 Continued:
        Assumes that the interior rings are oriented oposite the exterior ring.

            addPolygonRing(polygon->getInteiorRingN(i),CCW ? EXTERIOR : INTERIOR, CCW ? INTERIOR : EXTERIOR);

        OPTIONAL 2
        Add support for random interior ring orientation?
        In otherwords check each ring and add it appropriately. This would allow the interior and exterior rings
        to be the same direction and it would also allow the interior rings to be randomly oriented as well, but
        still give same results. Once again, the time needed for that is much much more since the area of every
        ring would be determined to find how each ring is oriented.

            bool CCW = isRingCounterClockwise(polygon->getInteriorRingN(i));
            addPolygonRing(polygon->getInteriorRingN(i),CCW ? EXTEIRIOR : INTERIOR, CCW ? INTERIOR : EXTERIOR);
    */
            addPolygonRing(polygon->getInteriorRingN(i),INTERIOR,EXTERIOR);
        }
    }

    void GeometryGraph::addPolySurface(const Geometry* geom)
    {
        simple = false;

    /*    !!WARNING!! There are no specifications or examples of polyhedral surfaces in relation operators,
        this is because they are often considered invalid to a 2D view, ie they have a polygon that overlaps
        another at some point if they curve back under themselves which they are allowed to do.

        Luckily by the specification of a polyhedral surface, the boundary will be handled implicitely because
        it follows the same rules for constructing the boundary as the mod-2 rule does for MultiPolygons.

        So, we can just create a multi polygon from the polyhedral surface and feed it to the geometry graph.
        Or rather, we can simply add each polygon to the graph individually

    OPTIMIZATION:
        Since all the interior boundary edges of individual polygons will be handled by Relate anyway, why not
        dispose of them quickly and more efficiently. The PatchUnion used in finding the Boundary of a polyhedral
        surface is much faster. Note that this will only work if the PolyhedralSurface is already well defined in 
        2D space, it will fail if the PolyhedralSurface is 3D, so we must revert to handling every Polygon
        individually.
    */
        LineString* boundary = dynamic_cast<LineString*>(geom->getBoundary());
        if (boundary)
        {
            if (!boundary->isEmpty())
            {
                addPolygonRing(geom->getBoundary(),INTERIOR,EXTERIOR);
                delete boundary;
                return;
            }
            delete boundary;
        }


        const PolyhedralSurface* polysurface = dynamic_cast<const PolyhedralSurface*>(geom);
        for (int i = 0; i < polysurface->getNumPatches(); i++)
            addPolygon(polysurface->getPatchN(i));
    }

    void GeometryGraph::addGeometryCollection(const Geometry* geom)
    {
        simple = false;
        const GeometryCollection* m = dynamic_cast<const GeometryCollection*>(geom);
        for (int i = 1; i < m->getNumGeometries() + 1; i++)
        {
            addGeometry(m->getGeometryN(i));
        }
    }

    void GeometryGraph::addMultiPoint(const Geometry* geom)
    {
        simple = false;
        const MultiPoint* mPoint = dynamic_cast<const MultiPoint*>(geom);
        for (int i = 1; i < mPoint->getNumGeometries() + 1; i++)
        {
            addPoint(mPoint->getGeometryN(i));
        }
    }

    void GeometryGraph::addMultiLineString(const Geometry* geom)
    {
        simple = false;
        const MultiLineString* mLine = dynamic_cast<const MultiLineString*>(geom);
        for (int i = 1; i < mLine->getNumGeometries() + 1; i++)
        {
            addLineString(mLine->getGeometryN(i));
        }
    }

    void GeometryGraph::addMultiPolygon(const Geometry* geom)
    {
        simple = false;
        const MultiPolygon* mPolygon = dynamic_cast<const MultiPolygon*>(geom);
        for (int i = 1; i < mPolygon->getNumGeometries() + 1; i++)
        {
            addPolygon(mPolygon->getGeometryN(i));
        }
    }

    void GeometryGraph::addPoint(PointNode* point)
    {
        points.push_back(point);
    }

    void GeometryGraph::addEdge(EdgeNode* edge)
    {
        edges.push_back(edge);
    }

    void GeometryGraph::addIsolatedPoint(PointNode* a)
    {
        int i;
        for (i = 0; i < int(points.size()); i++)
        {
            if (a->equals(points[i]))
                break;
        }
        if (i == points.size())
            points.push_back(a);
        else
            delete a;
    }

    void GeometryGraph::addBoundaryPoint(PointNode* a)
    {
        int i;
        for (i = 0; i < int(boundaryPoints.size()); i++)
        {
            if (a->equals(boundaryPoints[i]))
                break;
        }
        if (i == boundaryPoints.size())
            boundaryPoints.push_back(a);
        else
        {
        //    Mod-2 rule
            if (boundaryPoints[i]->getLocation(arg) == BOUNDARY) boundaryPoints[i]->setLocation(arg,INTERIOR);
            else boundaryPoints[i]->setLocation(arg,BOUNDARY);
        }
    }

    void GeometryGraph::computeSelfIntersections(void)
    {
        if (simple) return;
        EdgeIntersector ei(edges,edges,true);
    }

    PointNodeList GeometryGraph::getPointNodes(void)
    {
        return points;
    }

    PointNodeList GeometryGraph::getIsolatedPoints(void)
    {
    //    Collect isolated points and boundary points. Some boundary points may be isolated and some isolated points may be boundary points.
        PointNodeList isolatedPoints;
        for (int i = 0; i < int(points.size()); i++)
        {
            if (points[i]->isIsolated() && !points[i]->getLocation(arg) == BOUNDARY) isolatedPoints.push_back(points[i]);
        }
        for (int i = 0; i < int(boundaryPoints.size()); i++)
        {
            if (boundaryPoints[i]->isIsolated() && boundaryPoints[i]->getLocation(arg) == BOUNDARY) isolatedPoints.push_back(boundaryPoints[i]);
        }
        return isolatedPoints;
    }

    EdgeNodeList GeometryGraph::getEdgeNodes(void)
    {
        return edges;
    }

    void GeometryGraph::getEdgeNodes(EdgeNodeList &intersecting, EdgeNodeList &isolated)
    {
        for (EdgeNodeList::iterator it = edges.begin(); it != edges.end(); ++it)
        {
            if ((*it)->isIsolated()) isolated.push_back(*it);
            else intersecting.push_back(*it);
        }
    }

    EdgeNodeList GeometryGraph::getIsolatedEdges(void)
    {
        EdgeNodeList isolatedEdges;
        for (int i = 0; i < int(edges.size()); i++)
        {
            if (edges[i]->isIsolated()) isolatedEdges.push_back(edges[i]);
        }
        return isolatedEdges;
    }

    const Geometry* GeometryGraph::getGeometry(void)
    {
        return parent;
    }

}