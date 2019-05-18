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
#include "sfa/RelateCompute.h"
#include "sfa/EnvelopeCheck.h"
#include "sfa/EdgeGroupBuilder.h"

namespace sfa {

    RelateCompute::RelateCompute(const Geometry* a, const Geometry* b)
    {
        geo[0] = a;
        geo[1] = b;
    }

    RelateCompute::RelateCompute(const GraphOperation* other)
    {
        geo[0] = other->getGeometry(0);
        geo[1] = other->getGeometry(1);
    }

    void RelateCompute::updateIM(void)
    {
        for (EdgeNodeEndList::iterator e = ends.begin(); e != ends.end(); ++e)
        {
            (*e)->getOrigin()->updateIM(intersectionMatrix);
            (*e)->updateIM(intersectionMatrix);
        }

        for (PointNodeList::iterator p = isolatedPoints.begin(); p != isolatedPoints.end(); ++p)
            (*p)->updateIM(intersectionMatrix);    
    }

    de9im RelateCompute::computeDisjointIM(void)
    {
        if (!geo[0]->isEmpty())
        {
            intersectionMatrix.set(INTERIOR,EXTERIOR,geo[0]->getDimension());
            Geometry* boundary = geo[0]->getBoundary();
            if (boundary) 
            {
                intersectionMatrix.set(BOUNDARY,EXTERIOR,boundary->getDimension());
                delete boundary;
            }
        }
        if (!geo[1]->isEmpty())
        {
            intersectionMatrix.set(EXTERIOR,INTERIOR,geo[1]->getDimension());
            Geometry* boundary = geo[1]->getBoundary();
            if (boundary)
            {
                intersectionMatrix.set(EXTERIOR,BOUNDARY,boundary->getDimension());
                delete boundary;
            }
        }
        return intersectionMatrix;
    }

    de9im RelateCompute::computeIM(void)
    {
        //    Because Geometries are bounded to a plane and are finite EXTERIOR,EXTERIOR will always be 2
        intersectionMatrix.setAtLeast(EXTERIOR,EXTERIOR,2);

        //    Check to see if the geometries are empty
        if (geo[0]->isEmpty() || geo[1]->isEmpty()) return computeDisjointIM();

        //    Check to see if envelopes intersect
        if (!EnvelopeCheck::apply(geo[0],geo[1])) return computeDisjointIM();

        constructGraphs();

        intersectGraphs();

        gatherEdges();
        gatherSplitEdges();
        gatherPoints();

        //    Update Improper Intersections
        EdgeGroupBuilder *egb = new EdgeGroupBuilder(splitEdges);
        egb->constructGroups();
        egb->completeLabels();

        //    Update ends within EdgeGroups with information from the group
        egb->updateParents();

        ends = egb->getEnds();
        createIsolatedEnds();

        //    Don't need to link ends
        delete egb;
        
        //    Label Isolated Nodes
        completeLabels(0);
        completeLabels(1);

        //    Update Isolated Nodes
        updateIM();

        return intersectionMatrix;
    }

}