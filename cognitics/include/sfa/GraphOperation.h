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
/*! \brief Provides sfa::GraphOperation.
\author Josh Anghel <janghel@cognitics.net>
\date 6 November 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"
#include "GeometryGraph.h"
#include "EdgeNode.h"
#include "EdgeNodeEnd.h"
#include "PointNode.h"

namespace sfa {

/*! \class sfa::GraphOperation GraphOperation.h GraphOperation.h
\brief GraphOperation

A GraphOperation provides an interface for performing operations requiring the intersection of two GeometryGraphs. 
GraphOperation methods provide algorithms to intersect the edges of two GeometryGraphs, split the edges, and compute the Labels 
of all EdgeNodes and PointNodes.
*/
    class GraphOperation
    {
    protected:
        GraphOperation(bool saveZ = false);
        virtual ~GraphOperation(void);

        const Geometry* geo[2];
        GeometryGraph* arg[2];

        EdgeNodeList intersectingEdges;
        EdgeNodeList isolatedEdges;
        EdgeNodeList splitEdges;

        PointNodeList isolatedPoints;

        EdgeNodeEndList ends;
        bool saveZ;

    public:

        virtual const Geometry* getGeometry(int i) const;

        virtual GeometryGraph* getGeometryGraph(int i);
        virtual void setGeometryGraph(GeometryGraph* graph, int i);

//! Sorts EdgeNodes into intersecting and isolated EdgeNodes.
        virtual void gatherEdges(void);

//!    Create child EdgeNodes from intersecting EdgeNodes by splitting them at intersection points. Also clears intersectingEdges.
        virtual void gatherSplitEdges(void);

//!    Gathers all isolated and BOUNDARY PointNodes from both GeometryGraphs.
        virtual void gatherPoints(void);

//! Creates EdgeNodeEnds for all isolated EdgeNodes.
        virtual void createIsolatedEnds(void);

/*! Completes the Label for a single PointNode for a single Geometry.
\param p PointNode to complete Label for.
\param n Geometry ID to complete Label for.
*/
        virtual void completePointLabel(PointNode* p, int n);

/*!    Completes the Label for a single EdgeNode for a single Geometry.
\param e EdgeNode to complete Label for.
\param n Geometry ID to complete Label for.
*/
        virtual void completeEdgeLabel(EdgeNodeEnd* e, int n);

/*!    Completes the Labels for all PointNodes and EdgeNodes for a single Geometry.
\param n Geometry ID to complete Label for.
*/
        virtual void completeLabels(int n);

//!    Constructs GeometryGraphs out of the parent Geometry for this GraphOperation.
        virtual void constructGraphs(bool saveZ = false);

//!    Add a single EdgeNode to its parent GeometryGraph.
        virtual void addEdge(EdgeNode* e);

//!    Add a single PointNode to its parent GeometryGraph.
        virtual void addPoint(PointNode* p);

//!    Intersects all EdgeNodes of one GeometryGraph with another, storing all intersections in the EdgeNodes and flagging them as intersecting EdgeNodes.
        virtual void intersectGraphs(void);

    };

}