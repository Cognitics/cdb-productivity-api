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

#include "sfa/Overlay.h"
#include "sfa/EdgeIntersector.h"
#include "sfa/GeometryCollection.h"
#include "sfa/EnvelopeCheck.h"
#include "sfa/EdgeGroupBuilder.h"
#include "sfa/RingMath.h"

namespace sfa {

    bool Overlay::errorOccured(void) const
    {
        return ProcessingError != 0x00;
    }

    bool Overlay::isInResult(Label l, OverlayOperation op)
    {
        return isInResult(l.loc[0],l.loc[1],op);
    }

    bool Overlay::isInResult(Location l0, Location l1, OverlayOperation op)
    {
        if (l0 == BOUNDARY) l0 = INTERIOR;
        if (l1 == BOUNDARY) l1 = INTERIOR;
        if (op == OVERLAY_INTERSECTION)        return l0 == INTERIOR && l1 == INTERIOR;
        else if (op == OVERLAY_UNION)        return l0 == INTERIOR || l1 == INTERIOR;
        else if (op == OVERLAY_DIFFERENCE)    return l0 == INTERIOR && l1 != INTERIOR;
        else                                return (l0 == INTERIOR && l1 != INTERIOR) || (l0 != INTERIOR && l1 == INTERIOR);
    }

    void Overlay::resetEdges(void)
    {
        for (EdgeNodeEndList::iterator edge = ends.begin(); edge != ends.end(); ++edge)
        {
            (*edge)->setInResult(false);
            (*edge)->setNext(NULL);
            (*edge)->setVisited(false);
        }
    }

    void Overlay::gatherIncludedAreaEdges(OverlayOperation op)
    {
        for (EdgeNodeEndList::iterator edge = ends.begin(); edge != ends.end(); ++edge)
        {
        /*    Check if edge is an area edge to be included. Since we want the rings to be defined CCW,
            the INTERIOR should be to the left.
        */
            if (!isInResult((*edge)->getLeft(),op) || !(*edge)->isArea()) continue;

        //    Do not add this edge if its symetric edge is also in the result
            EdgeNodeEnd* sym = (*edge)->getSym();
            if (isInResult(sym->getLeft(),op) && (*edge)->isArea()) continue;

        //    Don't add duplicate edges
            bool add = true;

            EdgeNodeEndList::iterator lookup;
            for (lookup = includedAreaEdges.begin(); lookup != includedAreaEdges.end(); ++lookup)
            {
            //    Don't add duplicate edges
                if ((*edge)->equals(*lookup))
                {
                    (*edge)->setInResult(false);
                    add = false;
                    break;
                }
            }

            if (add)
            {
                includedAreaEdges.push_back(*edge);
                (*edge)->setInResult(true);
            }
        }
    }

    void Overlay::gatherIncludedLineEdges(OverlayOperation op)
    {
    /*
        For including Line edges we don't need to delete any symetric edges, but we do need to ensure
        that we do not add any edges equal to another into the results. So if an equal edge or symetric
        edge is encounted, just don't add this one.

        Also, do not add this line if it is found that both the left and right side of it are also in the 
        result, it is redundant and would cause inValid geometry collections.
    */
        for (EdgeNodeEndList::iterator edge = ends.begin(); edge != ends.end(); ++edge)
        {
            if (!isInResult((*edge)->getOn(),op)) continue;

            if (isInResult((*edge)->getLeft(),op) || isInResult((*edge)->getRight(),op)) continue;

            bool add = true;
            for (EdgeNodeEndList::iterator lookup = includedLineEdges.begin(); lookup != includedLineEdges.end(); ++lookup)
            {
                if ((*edge)->equals(*lookup))
                {
                    (*edge)->setInResult(false);
                    add = false;
                    break;
                }
            }

            if (add)
            {
                (*edge)->setInResult(true);
                includedLineEdges.push_back(*edge);
            }
        }
    }

    void Overlay::gatherIncludedPoints(OverlayOperation op)
    {
    //    Check all isolated points first
        for (PointNodeList::iterator point = isolatedPoints.begin(); point != isolatedPoints.end(); ++point)
        {
            if (isInResult((*point)->getLabel(),op)) includedPoints.push_back(*point);
        }

    //    Check all the end points of EdgeNodeEnds that have not been added to the result.
        for (EdgeNodeEndList::iterator edge = ends.begin(); edge != ends.end(); ++edge)
        {
            if ((*edge)->isInResult() || (*edge)->getSym()->isInResult()) continue;
            if (isInResult((*edge)->getLeft(),op) || isInResult((*edge)->getRight(),op)) continue;
            Label p = (*edge)->getOrigin()->getLabel();
        //    Since we never backtracked point labels from the parent edges, if the point has an incomplete label, use its parent edge to complete the label
            if (p.loc[0] == UNKNOWN) p.loc[0] = (*edge)->getOn(0);
            if (p.loc[1] == UNKNOWN) p.loc[1] = (*edge)->getOn(1);
            if (isInResult(p,op)) includedPoints.push_back((*edge)->getOrigin());
        }
    }

    LineStringList Overlay::getMinimalRings(void)
    {
        // if we are in saveZ mode, we need to make sure any non-3D point connected to a 3D point is assigned a valid Z
        // this provides us with valid intersection Z points in our assembled linestring
        if(saveZ)
        {
            for(size_t i = 0, c = includedAreaEdges.size(); i < c; ++i)
            {
                EdgeNodeEnd *edge = includedAreaEdges[i];
                if(!edge->getNext())
                    continue;
                int indexN = edge->getParent()->getNumPoints() - 1;
                PointNode *edgePointNodeN = edge->getParent()->getPointN(indexN);
                EdgeNodeEnd *nextEdge = edge->getNext();
                PointNode *nextPointNode0 = nextEdge->getParent()->getPointN(0);
                if(!edgePointNodeN->getPoint()->is3D() && nextPointNode0->getPoint()->is3D())
                    edgePointNodeN->setPoint(*nextPointNode0->getPoint());
                if(edgePointNodeN->getPoint()->is3D() && !nextPointNode0->getPoint()->is3D())
                    nextPointNode0->setPoint(*edgePointNodeN->getPoint());
            }
        }

        LineStringList minimumRings;
        for (int i = 0; i < int(includedAreaEdges.size()); i++)
        {
            if (includedAreaEdges[i]->isVisited()) continue;

            LineString* ring = new LineString;
            ring->addPoint(new Point(includedAreaEdges[i]->getOrigin()->getPoint()));
            EdgeNodeEnd* next = includedAreaEdges[i];

            next->visit();
            next->appendToLineString(ring);
            next = next->getNext();

        //    Build ring until closed
            while (!ring->isClosed() && next)
            {
                if (next->isVisited())
                {
                    ring->addPoint(*ring->getPointN(0));
                    break;
                    /*
                //    Critical processing error. Bad data has resulted in an infinite loop, must terminate manually and clean up data.
                    ProcessingError = 0x01;
                    delete ring;
                    for (unsigned int i = 0; i < minimumRings.size(); i++)
                        delete minimumRings[i];
                    minimumRings.clear();
                    
                //    ccl::Log::instance()->write(ccl::LALERT,"A:" + geo[0]->asText());
                //    ccl::Log::instance()->write(ccl::LALERT,"B:" + geo[1]->asText());

                    return minimumRings;
                    */
                }

                next->visit();
                next->appendToLineString(ring);
                next = next->getNext();
            }

        //    Add Ring to list if its a valid ring
            if (abs(GetRingArea(ring)) < ring->getPointN(0)->length()*SFA_EPSILON) continue;

            minimumRings.push_back(ring);
        }


        // if we are in saveZ mode, we probably have one or more series of points without Z values
        // locate these and interpolate the points between valid z-values
        if(saveZ)
        {
            for(LineStringList::iterator it = minimumRings.begin(), end = minimumRings.end(); it != end; ++it)
            {
                sfa::LineString *ring = *it;
                int ringSize = ring->getNumPoints();
                while(true)
                {
                    // find an start point that has a Z point followed by a non-Z point
                    int startIndex = -1;
                    for(int i = 0; i < ringSize - 1; ++i)
                    {
                        sfa::Point *point = ring->getPointN(i);
                        int nextIndex = (i + 1 < ringSize) ? i + 1 : 0;
                        sfa::Point *nextPoint = ring->getPointN(nextIndex);
                        if(point->is3D() && !nextPoint->is3D())
                            startIndex = i;
                    }
                    // if we don't find any, this ring is done
                    if(startIndex < 0)
                        break;

                    // find an end point that has a non-Z point followed by a Z point
                    // this wraps the endpoint, so it should never fail if a startIndex is found
                    int endIndex = -1;
                    for(int i = 0; i < ringSize - 1; ++i)
                    {
                        sfa::Point *point = ring->getPointN(i);
                        int nextIndex = (i + 1 < ringSize) ? i + 1 : 0;
                        sfa::Point *nextPoint = ring->getPointN(nextIndex);
                        if(!point->is3D() && nextPoint->is3D())
                        {
                            if((endIndex < 0) || (i > startIndex))
                            {
                                endIndex = nextIndex;
                                break;
                            }
                        }
                    }

                    // interpolate between them (with cycle around)
                    sfa::Point *startPoint = ring->getPointN(startIndex);
                    sfa::Point *endPoint = ring->getPointN(endIndex);
                    double startZ = startPoint->Z();
                    double endZ = endPoint->Z();
                    if(endIndex < startIndex)
                        endIndex += ringSize;
                    double total_dist = 0.0f;
                    sfa::Point *prevPoint = startPoint;
                    for(int i = startIndex + 1; i < endIndex; ++i)
                    {
                        sfa::Point *point = (i < ringSize) ? ring->getPointN(i) : ring->getPointN(i - ringSize);
                        total_dist += prevPoint->distance(point);
                        prevPoint = point;
                    }
                    double dist = 0.0f;
                    prevPoint = startPoint;
                    for(int i = startIndex + 1; i < endIndex; ++i)
                    {
                        sfa::Point *point = (i < ringSize) ? ring->getPointN(i) : ring->getPointN(i - ringSize);
                        dist += prevPoint->distance(point);
                        double z = startZ + ((dist / total_dist) * (endZ - startZ));
                        point->setZ(z);
                        prevPoint = point;
                    }
                }


            }
        }

        return minimumRings;
    }

    LineStringList Overlay::getLines(OverlayOperation op)
    {
        LineStringList result;
        for (EdgeNodeEndList::iterator it = includedLineEdges.begin(); it != includedLineEdges.end(); ++it)
        {
            LineString* next = new LineString;
            next->addPoint(new Point((*it)->getOrigin()->getPoint()));
            (*it)->appendToLineString(next);
            result.push_back(next);
        }

        return result;
    }

    PolygonList Overlay::constructPolygons(LineStringList rings)
    {
        PolygonList result;
        LineStringList interior;
        //    Sort rings
        for (LineStringList::iterator it = rings.begin(); it != rings.end(); ++it)
        {
            if (ComputeOrientation(*it) == ORIENTATION_C_CW)
            {
                Polygon* next = new Polygon;
                next->addRing(new LineString(*it));
                result.push_back(next);
            }
            else interior.push_back(*it);
        }

        //    Add interior rings to their parent polygons.
        for (LineStringList::iterator line = interior.begin(); line != interior.end(); ++line)
        {
        //    Find the polygon to insert this ring into with a point in polygon test
            for (PolygonList::iterator polygon = result.begin(); polygon != result.end(); ++polygon)
            {
                if ((*line)->getStartPoint()->intersects(*polygon))
                {
                    (*polygon)->addRing(new LineString(*line));
                }
            }
        }

        return result;
    }

    Geometry* Overlay::handleEmpty(OverlayOperation op, bool A, bool B)
    {
        if (op == OVERLAY_INTERSECTION) return NULL;
        else if (op == OVERLAY_UNION)
        {
            if (!A && !B) return NULL;
            else if (A) return geo[0]->copy();
            else return geo[1]->copy();
        }
        else if (op == OVERLAY_DIFFERENCE)
        {
            if (A) return geo[0]->copy();
            else return NULL;
        }
        else    //    OVERLAY_SYMDIFFERENCE
        {
            if (!A && !B) return NULL;
            else if (A) return geo[0]->copy();
            else return geo[1]->copy();
        }
    }

    Geometry* Overlay::handleDisjoint(OverlayOperation op)
    {
        if (op == OVERLAY_DIFFERENCE) return geo[0]->copy();
        else if (op == OVERLAY_UNION || op == OVERLAY_SYMDIFFERENCE)
        {
            GeometryCollection* result = new GeometryCollection;
            result->addGeometry(geo[0]->copy());
            result->addGeometry(geo[1]->copy());
            return result;
        }
        else return NULL;
    }

    Overlay::Overlay(const Geometry* a, const Geometry* b) : saveZ(false)
    {
        geo[0] = a;
        geo[1] = b;
        egb = NULL;
    }

    Overlay::Overlay(const GraphOperation* operation) : saveZ(false)
    {
        geo[0] = operation->getGeometry(0);
        geo[1] = operation->getGeometry(1);
        egb = NULL;
    }

    Overlay::~Overlay(void)
    {
        if (egb) delete egb;
    }

    Geometry* Overlay::computeOverlay(OverlayOperation op)
    {
    //    Reset error flag
        ProcessingError = 0x00;

    //    Handle Empty case
        bool A = true;
        bool B = true;
        if (!geo[0]) A = false;
        else if (geo[0]->isEmpty()) A = false;
         if (!geo[1]) B = false;
        else if (geo[1]->isEmpty()) B = false;

        if (!(A && B)) return handleEmpty(op,A,B);

    //    Handle disjoint
        if (!EnvelopeCheck::apply(geo[0],geo[1])) return handleDisjoint(op);

        constructGraphs(saveZ);

    /*    Only create new Graphs and intersect them if not already done, else proceed to parsing the information about the labeled edges.
        This is a great way to reduce the time needed to compute multiple overlays of the same geometries by a large amount. */
        if (!egb)
        {
        //    Compute self intersections
            intersectGraphs();

            gatherEdges();
            gatherSplitEdges();
            gatherPoints();

        //    Construct groups and link edges
            egb = new EdgeGroupBuilder(splitEdges);
            egb->constructGroups();
            egb->completeLabels();

        //    Update parents with information from the EdgeGroups
            egb->updateParents();

            ends = egb->getEnds();
            createIsolatedEnds();

        //    Complete the labels of any unknown locations
            completeLabels(0);
            completeLabels(1);
        }
        else resetEdges();

    //    Build resulting geometries =====================================================

    //    Gather all potential area edges
        gatherIncludedAreaEdges(op);
    //    Link all potential area edges
        egb->linkEnds();

        gatherIncludedLineEdges(op);
        gatherIncludedPoints(op);

    //    Get the minimal rings. Consider not even linking the maximal rings...see what the time difference is
        LineStringList rings = getMinimalRings();

    //    Check for errors in rings
        if (errorOccured())
            return NULL;

    //    Construct polygons, because we used minimal rings, we know that if any point of a hole is inside a polygon, the entire hole is in the polygon properly
        PolygonList areas = constructPolygons(rings);

    //    Get the LineStrings to be included in the result
        LineStringList lines = getLines(op);

        GeometryCollection* result = new GeometryCollection;
    
        for (PolygonList::iterator it = areas.begin(); it != areas.end(); ++it)
            result->addGeometry(*it);
        for (LineStringList::iterator it = lines.begin(); it != lines.end(); ++it)
            result->addGeometry(*it);
        for (PointNodeList::iterator it = includedPoints.begin(); it != includedPoints.end(); ++it)
            if (!(*it)->getPoint()->intersects(result)) result->addGeometry(new Point((*it)->getPoint()));


        // We made copies of all the ring linestrings in the polygons so 
        // we'll clean that up now
        for (size_t r = 0, rc = rings.size(); r < rc; r++)
            delete rings[r];
        rings.clear();

    //    Clean up working space
        includedAreaEdges.clear();
        includedLineEdges.clear();
        includedPoints.clear();
        
        if (result->isEmpty())
        {
            delete result;
            return NULL;
        }
        else if (result->getNumGeometries() == 1)
        {
            Geometry* temp = result->getGeometryN(1)->copy();
            delete result;
            return temp;
        }
        else return result;
    }

}