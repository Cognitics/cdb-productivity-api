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

#include "sfa/GraphOperation.h"
#include "sfa/PointLocator.h"
#include "sfa/EdgeIntersector.h"

namespace sfa {

    GraphOperation::GraphOperation(bool saveZ) : saveZ(saveZ)
    {
        arg[0] = arg[1] = NULL;
    }

    GraphOperation::~GraphOperation(void)
    {
        if (arg[0]) delete arg[0];
        if (arg[1]) delete arg[1];
    }

    const Geometry* GraphOperation::getGeometry(int i) const
    {
        return geo[i];
    }

    GeometryGraph* GraphOperation::getGeometryGraph(int i)
    {
        return arg[i];
    }

    void GraphOperation::setGeometryGraph(GeometryGraph* graph, int i)
    {
        arg[i] = graph;
    }

    void GraphOperation::gatherEdges(void)
    {
        arg[0]->getEdgeNodes(intersectingEdges,isolatedEdges);
        arg[1]->getEdgeNodes(intersectingEdges,isolatedEdges);
    }

    void GraphOperation::gatherSplitEdges(void)
    {
        for (EdgeNodeList::iterator it = intersectingEdges.begin(), end = intersectingEdges.end(); it != end; ++it)
        {
            EdgeNodeList split = (*it)->split();
            splitEdges.insert(splitEdges.end(),split.begin(),split.end());
        }
    }

    void GraphOperation::gatherPoints(void)
    {
        isolatedPoints = arg[0]->getIsolatedPoints();
        PointNodeList temp;
        temp = arg[1]->getIsolatedPoints();
        isolatedPoints.insert(isolatedPoints.end(),temp.begin(),temp.end());
    }

    void GraphOperation::createIsolatedEnds(void)
    {
        for (EdgeNodeList::iterator it = isolatedEdges.begin(), end = isolatedEdges.end(); it != end; ++it)
        {
            ends.push_back((*it)->getEnd(0));
            ends.push_back((*it)->getEnd(1));
        }
    }

    void GraphOperation::completePointLabel(PointNode* p, int n)
    {
    //    If we want to ensure that a point is on the boundary if it touches 2 polygons only at a single point, we must try to find the point in 
    //    The listing of all the current points first...If the point is found to be an endpoint, it will speed up the process actually, otherwise
    //    it is a performance hit, but a necessary one
        if (p->getLocation(n) != UNKNOWN) return;
        for (EdgeNodeEndList::iterator it = ends.begin(), end = ends.end(); it != end; ++it)
        {
            if (p->equals((*it)->getOrigin()) && (*it)->getOrigin()->getLocation(n) != UNKNOWN )
            {
                p->setLocation(n,(*it)->getOrigin()->getLocation(n));
                return;
            }
        }

    //    The value wasn't found above
        //if (p->getLocation(n)==UNKNOWN) p->setLocation(n,PointLocator::apply(p->getPoint(),geo[n]));
        p->setLocation(n,PointLocator::apply(p->getPoint(),geo[n]));
    }

    void GraphOperation::completeEdgeLabel(EdgeNodeEnd* e, int n)
    {
    /*    At this point, if there is an unknown label, this edge is isolated from that geometry, so it is either
        INTERIOR or EXTERIOR, a point location of one end point is sufficient. If the geometry has dimension 0
        we know that this edge is EXTERIOR of that point...the truth is that it may intersect it, but any
        intersection will be picked up by the poin instead.
    */
        Location edgeLoc = e->getOn(n);
        if (edgeLoc == UNKNOWN)
            edgeLoc = (geo[n]->getDimension() > 0 ? PointLocator::apply(e->getOrigin()->getPoint(),geo[n]) : EXTERIOR);
        if (e->getOn(n)        == UNKNOWN) e->setOn(n,edgeLoc);
        if (e->getLeft(n)    == UNKNOWN) e->setLeft(n,edgeLoc);
        if (e->getRight(n)    == UNKNOWN) e->setRight(n,edgeLoc);
        if (e->getOrigin()->getLocation(n) == UNKNOWN) 
            completePointLabel(e->getOrigin(),n);
            //e->getOrigin()->setLocation(n,edgeLoc);
    }

    void GraphOperation::completeLabels(int n)
    {
        //    Compltete labels for all ends and points
        for (PointNodeList::iterator p = isolatedPoints.begin(), end = isolatedPoints.end(); p != end; ++p)
            completePointLabel(*p,(n+1)%2);        

        //    Complete labels of isolated edges
        for (EdgeNodeEndList::iterator e = ends.begin(), end = ends.end(); e != end; ++e)
            completeEdgeLabel(*e,(n+1)%2);
    }

    void GraphOperation::constructGraphs(bool saveZ)
    {
        if (arg[0] && arg[1]) return;

        arg[0] = new GeometryGraph(geo[0],0, saveZ);
        arg[1] = new GeometryGraph(geo[1],1, saveZ);
    }

    void GraphOperation::addEdge(EdgeNode* edge)
    {
        arg[edge->getGeomArg()]->addEdge(edge);
    }

    void GraphOperation::addPoint(PointNode* point)
    {
        arg[point->getGeomArg()]->addPoint(point);
    }

    void GraphOperation::intersectGraphs(void)
    {
        arg[0]->computeSelfIntersections();
        arg[1]->computeSelfIntersections();
        EdgeIntersector ei(arg[0]->getEdgeNodes(),arg[1]->getEdgeNodes());
    }
}