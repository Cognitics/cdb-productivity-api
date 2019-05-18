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
#include "sfa/PlanarGraphOperation.h"
#include "sfa/PointMath.h"

namespace sfa {

    PlanarGraphOperation::~PlanarGraphOperation(void)
    {
        for (std::vector<PlanarGraph*>::iterator it = planes.begin(), end = planes.end(); it != end; it++)
            delete (*it);
    }

    void PlanarGraphOperation::initialize(const Geometry* a, const Geometry* b)
    {
        queueGeometry(a,0);
        queueGeometry(b,1);

    //    Add polygons first
        addPolygons(0);
        addPolygons(1);
        addLines(0);
        addLines(1);
        addPoints(0);
        addPoints(1);

    //    Empty queue
        polygons[0].clear();
        polygons[1].clear();
        lines[0].clear();
        lines[1].clear();
        points[0].clear();
        points[1].clear();

    //    contrust graphs in the planes
        for (std::vector<PlanarGraph*>::iterator p = planes.begin(), end = planes.end() ; p != end; ++p)
            (*p)->constructGraphs();

    //    intersect planes
        intersectPlanes();
    }

    void PlanarGraphOperation::queueGeometry(const Geometry* geom, int arg)
    {
        int typeA = geom->getWKBGeometryType(false,false);
        switch (typeA)
        {
        case wkbPoint:
            queuePoint(geom,arg);
            break;
        case wkbLineString:
            queueLineString(geom,arg);
            break;
        case wkbPolygon:
        case wkbTriangle:
            queuePolygon(geom,arg);
            break;
        case wkbPolyhedralSurface:
        case wkbTIN:
            queueSurface(geom,arg);
            break;
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            queueCollection(geom,arg);
            break;
        default:
            throw std::runtime_error("PlanarGraphOperation::queueGeometry() invalid geometry");
        }
    }

    void PlanarGraphOperation::queuePoint(const Geometry* point, int arg)
    {
        points[arg].push_back(dynamic_cast<const Point*>(point));
    }

    void PlanarGraphOperation::queueLineString(const Geometry* line, int arg)
    {
        lines[arg].push_back(dynamic_cast<const LineString*>(line));
    }

    void PlanarGraphOperation::queuePolygon(const Geometry* polygon, int arg)
    {
        polygons[arg].push_back(dynamic_cast<const Polygon*>(polygon));
    }

    void PlanarGraphOperation::queueSurface(const Geometry* surface, int arg)
    {
        const PolyhedralSurface* s = dynamic_cast<const PolyhedralSurface*>(surface);
        for (int i = 0; i < s->getNumPatches(); i++)
            polygons[arg].push_back(s->getPatchN(i));
    }

    void PlanarGraphOperation::queueCollection(const Geometry* collection, int arg)
    {
        const GeometryCollection* c = dynamic_cast<const GeometryCollection*>(collection);
        for (int i = 1; i <= c->getNumGeometries(); i++)
            queueGeometry(c->getGeometryN(i),arg);
    }

    void PlanarGraphOperation::addPolygons(int arg)
    {
        for (std::vector<const Polygon*>::iterator it = polygons[arg].begin(); it != polygons[arg].end(); ++it)
            addPolygon(*it,arg);
    }

    void PlanarGraphOperation::addLines(int arg)
    {
        for (std::vector<const LineString*>::iterator it = lines[arg].begin(); it != lines[arg].end(); ++it)
            addLineString(*it,arg);
    }

    void PlanarGraphOperation::addPoints(int arg)
    {
        for (std::vector<const Point*>::iterator it = points[arg].begin(); it != points[arg].end(); ++it)
            addPoint(*it,arg);
    }

    void PlanarGraphOperation::addPolygon(const Geometry* polygon, int arg)
    {
        Projection2D proj(dynamic_cast<const Polygon*>(polygon));
        const Point* norm = proj.getNormal();

        for (int i = 0; i < int(planes.size()); i++)
        {
            if (planes[i]->getProjection().getNormal()->equals3D(norm))
            {
                Geometry* temp = planes[i]->getProjection().transformPointTo2D(proj.getOrigin());
                if (temp)
                {
                    bool result = temp->is3D();
                    delete temp;
                    if (result) continue;
                }

                planes[i]->add3DGeometry(polygon,arg);
                return;
            }
        }

    //    Create a new plane
        PlanarGraph* plane = this->createPlanarGraph(proj);
        plane->add3DGeometry(polygon,arg);
        planes.push_back(plane);
    }

    void PlanarGraphOperation::addSegment(const Point* p1, const Point* p2, int arg)
    {
        LineString line;
        line.addPoint(new Point(p1));
        line.addPoint(new Point(p2));

    //    use 0.0 as the default third point, if not valid, find a third point
        Point origin(0,0,0);

        if (p1->equals(&origin)) origin.setX(1.0);    //    Shift right
        if (p2->equals(&origin)) origin.setY(1.0);    //    Shift up

        Projection2D proj(p1,p2,&origin);
        const Point* norm = proj.getNormal();
        for (int i = 0; i < int(planes.size()); i++)
        {
            if (planes[i]->getProjection().getNormal()->equals3D(norm))
            {
                Point temp = planes[i]->getProjection().transformPointTo2D(*proj.getOrigin());
                if (temp.is3D())
                    continue;

                planes[i]->add3DGeometry(&line,arg);
                return;
            }
        }

    //    Create a new plane
        PlanarGraph* plane = createPlanarGraph(proj);
        plane->add3DGeometry(&line,arg);
        planes.push_back(plane);
    }

    void PlanarGraphOperation::addLineString(const Geometry* line, int arg)
    {
    //    Split a linestring up into sections of three untill only one or 2 points remain,
    //    Then just use 0,0 as the third point
        const LineString* linestring = dynamic_cast<const LineString*>(line);

        for (int i = 0; i < linestring->getNumPoints() - 1; i++)
            addSegment(linestring->getPointN(i),linestring->getPointN(i+1),arg);
    }

    void PlanarGraphOperation::addPoint(const Geometry* point, int arg)
    {
        for (int i = 0; i < int(planes.size()); i++)
        {
            if (!planes[i]->getProjection().transformGeometryTo2D(point)->is3D())
            {
                planes[i]->add3DGeometry(point,arg);
                return;
            }
        }

    //    Point is completly isolated
        isolatedPoints[arg].push_back(new Point(dynamic_cast<const Point*>(point)));
    }


//    ----------------------------------------------------------------------------------------------
//    INTERSECT planes - The meat and potatoes of the 3D relational system
    bool PlanarGraphOperation::doPlanesIntersect(int p1, int p2)
    {
        return (!planes[p1]->getProjection().getNormal()->equals(planes[p2]->getProjection().getNormal()));
    }

    void PlanarGraphOperation::getIntersectingLine(int p1, int p2, Point &p, Point &direction)
    {
    //    Intersecting two planes
    //    direction = n1 x n2
        const Point* norm[2] = { planes[p1]->getProjection().getNormal() , planes[p2]->getProjection().getNormal() };
        const Point* origin[2] = { planes[p1]->getProjection().getOrigin() , planes[p2]->getProjection().getOrigin() };

        direction = Point(
            norm[0]->Y()*norm[1]->Z() - norm[0]->Z()*norm[1]->Y(),
            norm[0]->Z()*norm[1]->X() - norm[0]->X()*norm[1]->Z(),
            norm[0]->X()*norm[1]->Y() - norm[0]->Y()*norm[1]->X()
            );

    //    represent the planes as ax + by + cz + d = 0
        double a[2] = { norm[0]->X() , norm[1]->X() };
        double b[2] = { norm[0]->Y() , norm[1]->Y() };
        double c[2] = { norm[0]->Z() , norm[1]->Z() };
        double d[2] = 
        {
            -( norm[0]->X()*origin[0]->X() + norm[0]->Y()*origin[0]->Y() + norm[0]->Z()*origin[0]->Z() ),
            -( norm[1]->X()*origin[1]->X() + norm[1]->Y()*origin[1]->Y() + norm[1]->Z()*origin[1]->Z() )
        };

        //Pick the direction coordinate that has the greatest abs value
        if (abs(direction.X()) > abs(direction.Y()) && abs(direction.X()) > abs(direction.Z()))
        {
            //set x to 0 and solve
            double denom = b[0]*c[1] - b[1]*c[0];
            double y = (c[0]*d[1] - c[1]*d[0])/denom;
            double z = -(b[0]*d[1] - b[1]*d[0])/denom;
            p = Point(0,y,z);
        }
        else if (abs(direction.Y()) > abs(direction.X()) && abs(direction.Y()) > abs(direction.Z()))
        {
            //set y to 0 and solve
            double denom = a[0]*c[1] - a[1]*c[0];
            double x = (c[0]*d[1] - c[1]*d[0])/denom;
            double z = -(a[0]*d[1] - a[1]*d[0])/denom;
            p = Point(x,0,z);
        }
        else
        {
            //set z to 0 and solve
            double denom = a[0]*b[1] - a[1]*b[0];
            double x = (b[0]*d[1] - b[1]*d[0])/denom;
            double y = (d[0]*a[1] - d[1]*a[0])/denom;
            p = Point(x,y,0);
            return;
        }
    }

    void PlanarGraphOperation::getIntersectionEdgeFromLine(Point* point, Point* direction, EdgeNode* edge, int source, EdgeNodeList &resultingEdges, PointNodeList &resultingPoints)
    {
        //Transfrom the point and direction into the reference frame of the edge
        Point P = planes[source]->getProjection().transformPointTo2D(*point);
        Point u = planes[source]->getProjection().transformPointTo2D(*direction);

        //Find all intersections
        for (int i = 0; i < edge->getNumPoints()-1; i++)
        {
        //    Determine the nature and location of the intersection.
            const Point* Q = edge->getPointN(i)->getPoint();
            const Point* end = edge->getPointN(i+1)->getPoint();
            Point v( end->X() - Q->X(), end->Y() - Q->Y() );
            Point w( P.X() - Q->X(), P.Y() - Q->Y() );

            double denom = u.X()*v.Y() - u.Y()*v.X();
            if (denom < SFA_EPSILON && denom > -SFA_EPSILON)    //Parallel
            {
                if (Collinear(&P,Q,end))
                {
                //Add the collinear edge directly to the result
                    EdgeNode* newEdge = new EdgeNode(edge->getLeft(),edge->getOn(),edge->getRight(),edge->getGeomArg());
                    newEdge->addPoint(Q);
                    newEdge->addPoint(end);
                    resultingEdges.push_back(newEdge);
                }
                //else ignore - no intersection
            }

            double t = (u.X()*w.Y() - u.Y()*w.X())/denom;
            if (t > -SFA_EPSILON && t < 1.0 + SFA_EPSILON )
            {
                //Construct new point and add it to the results...use interior unless it is a Boundary Point
                if (t < SFA_EPSILON && t > -SFA_EPSILON) resultingPoints.push_back(edge->getPointN(i));
                else if (t > 1 - SFA_EPSILON && t < 1 + SFA_EPSILON) resultingPoints.push_back(edge->getPointN(i+1));
                else
                {
                    Point intersection(t*v.X() + Q->X(), t*v.Y() + Q->Y());
                    PointNode* newPoint = new PointNode(&intersection, edge->getOn(),edge->getGeomArg());
                    resultingPoints.push_back(newPoint);
                }
            }
        }
    }

    void PlanarGraphOperation::getIntersectionEdgeFromArea(Point* point, Point* direction, EdgeNode* edge, int source, EdgeNodeList &resultingEdges, PointNodeList &resultingPoints)
    {
        //Transfrom the point and direction into the reference frame of the edge
        Point P = planes[source]->getProjection().transformPointTo2D(*point);
        Point u = planes[source]->getProjection().transformPointTo2D(*direction);

        //Map to intersection point and sort them simulatenously by distance along point
        std::map<double, Point*> intersections;

        //Find all intersections
        for (int i = 0; i < edge->getNumPoints()-1; i++)
        {
        //    Determine the nature and location of the intersection.
            const Point* Q = edge->getPointN(i)->getPoint();
            const Point* end = edge->getPointN(i+1)->getPoint();
            Point v( end->X() - Q->X(), end->Y() - Q->Y() );
            Point w( P.X() - Q->X(), P.Y() - Q->Y() );

            double denom = u.X()*v.Y() - u.Y()*v.X();
            if (denom < SFA_EPSILON && denom > -SFA_EPSILON)    //Parallel
            {
                if (Collinear(&P,Q,end))
                {
                //Add the collinear edge directly to the result
                    EdgeNode* newEdge = new EdgeNode(edge->getLeft(),edge->getOn(),edge->getRight(),edge->getGeomArg());
                    newEdge->addPoint(Q);
                    newEdge->addPoint(end);
                    resultingEdges.push_back(newEdge);
                }
                else continue;
            }

            double t = (u.X()*w.Y() - u.Y()*w.X())/denom;
            if (t > -SFA_EPSILON && t < 1 + SFA_EPSILON )
            {
                //Construct new point and add it to the map
                Point* intersection = new Point(t*v.X() + Q->X(), t*v.Y() + Q->Y());
                //Find s
                double s = -(u.X()*w.Y() - u.Y()*w.X())/denom;
                intersections[s] = intersection;
            }
        }

    //    Parse the results from the intersections to construct the valid geometries
    //    Construct labels to use
        Label points;
        points.dim = 0;
        points.loc[edge->getGeomArg()] = BOUNDARY;
        Label line;
        line.dim = 1;
        line.loc[edge->getGeomArg()] = INTERIOR;
        Label regions;
        regions.dim = 2;
        regions.loc[edge->getGeomArg()] = EXTERIOR;


        for (std::map<double,Point*>::iterator it = intersections.begin(); it != intersections.end(); ++it)
        {
        //    Create an interior edge out of this it and the next it
            PointNode* first = new PointNode(it->second,points,edge->getGeomArg());
            ++it;
            if (it == intersections.end())
                break;
            PointNode* last = new PointNode(it->second,points,edge->getGeomArg());

            if (first->equals(last)) resultingPoints.push_back(first);
            else
            {
                EdgeNode* nextEdge = new EdgeNode(regions,line,regions,edge->getGeomArg());
                nextEdge->addPointNode(first);
                nextEdge->addPointNode(last);
                resultingEdges.push_back(nextEdge);
            }
        }
    }

    void PlanarGraphOperation::combinePlanes(int p1, int p2)
    {
    /*    Get the line of intersection between two planes, then intersect all the edges of p1 with the line and
        add those to p2, then do the same for p2 and add them to p1
    */
        Point origin, direction;
        getIntersectingLine(p1,p2,origin,direction);

        //Loop through each edge in plane 1, and find its intersection with the line.
        //Transform that result into 3D then add it to p2.
        EdgeNodeList edges0 = planes[p1]->getGeometryGraph(0)->getEdgeNodes();
        EdgeNodeList edges1 = planes[p1]->getGeometryGraph(1)->getEdgeNodes();

        EdgeNodeList resultingEdges;
        PointNodeList resultingPoints;

        for (int i = 0; i <int(edges0.size()); i++)
        {
            if (edges0[i]->getLeft(0) == edges0[i]->getRight(0))
                getIntersectionEdgeFromLine(&origin,&direction,edges0[i],p1,resultingEdges,resultingPoints);
            else
                getIntersectionEdgeFromArea(&origin,&direction,edges0[i],p1,resultingEdges,resultingPoints);
        }
        for (int i = 0; i <int(edges1.size()); i++)
        {
            if (edges1[i]->getLeft(1) == edges1[i]->getRight(1))
                getIntersectionEdgeFromLine(&origin,&direction,edges1[i],p1,resultingEdges,resultingPoints);
            else
                getIntersectionEdgeFromArea(&origin,&direction,edges1[i],p1,resultingEdges,resultingPoints);
        }

    //    Add results to p2
        for (int i = 0; i < int(resultingEdges.size()); i++)
        {
        //    Transform results from 2D to 3D and then back to 2D of plane 2
            EdgeNode* transformed = new EdgeNode(resultingEdges[i]->getLeft(),resultingEdges[i]->getOn(),resultingEdges[i]->getRight(),resultingEdges[i]->getGeomArg());
            LineString line;

            for (int n = 0; n < resultingEdges[i]->getNumPoints(); n++)
            {
                PointNode* point_node = resultingEdges[i]->getPointN(n);
                Point* temp = planes[p1]->getProjection().transformPointTo3D(point_node->getPoint());
                Point* p = planes[p2]->getProjection().transformPointTo2D(temp);
                delete temp;
                transformed->addPointNode(new PointNode(p,point_node->getLabel(),point_node->getGeomArg()));
                line.addPoint(p);
            }

            planes[p2]->addEdge(transformed);
            planes[p2]->addGeometry(&line,transformed->getGeomArg());
        }
        for (int i = 0; i < int(resultingPoints.size()); i++)
        {
            PointNode* point_node = resultingPoints[i];
            Point* temp = planes[p1]->getProjection().transformPointTo3D(point_node->getPoint());
            Point* p = planes[p2]->getProjection().transformPointTo2D(temp);
            delete temp;
            planes[p2]->addPoint(new PointNode(p,point_node->getLabel(),point_node->getGeomArg(),true));
            planes[p2]->addGeometry(p,point_node->getGeomArg());
        }
    }

    void PlanarGraphOperation::intersectPlanes(void)
    {
        for (int i = 0; i < int(planes.size()); i++) {
            for (int j = i+1; j < int(planes.size()); j++)
            {
                if (doPlanesIntersect(i,j))
                {
                    combinePlanes(i,j);
                    //combinePlanes(j,i);    //    only add one graph to the other, if they are symetrically added, duplicated are formed
                }
            }
        }
    }


}