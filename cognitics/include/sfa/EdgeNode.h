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
/*! \brief provides sfa::EdgeNode.
\author Josh Anghel <janghel@cognitics.net>
\date 4 November 2010
*/
#pragma once

#include "PointNode.h"

namespace sfa {

    class EdgeNode;
    typedef std::vector<EdgeNode*> EdgeNodeList;

    class EdgeNodeEnd;
    typedef std::vector<EdgeNodeEnd*> EdgeNodeEndList;

/*! \class sfa::EdgeNode EdgeNode.h EdgeNode.h
\brief EdgeNode

Provides a single directed edge of labeled points. An EdgeNode can store sorted intersection points along it and create other "split" EdgeNodes derived from itself. 

A "interseting" EdgeNode is an EdgeNode that has at least one intersection Point with another EdgeNode or has at least one self intersection.
A "split" EdgeNode is an EdgeNode that has intersections only at the two edge points and is derived from another EdgeNode.
An "isoalted" EdgeNode is an EdgeNode with no intersections along it (this included self intersections).

An EdgeNode also stores Labels for the regions to the left and right of the EdgeNode as well as along the EdgeNode.
The Labels within EdgeNodes are never used directly, only the Labels created in an EdgeNode's EdgeNodeEnds will be used directly. This is because systems build in the 
sfa do not update an EdgeNode's Labels, they instead update the Labels of the EdgeNode's EdgeNodeEnds which are created from a single EdgeNodes "split" EdgeNodes.
*/
    class EdgeNode
    {
    protected:
        int                arg;

//!    These points are owned by this EdgeNode and are thus DELETED upon destruction
        PointNodeList    points;

        //std::vector<double> intersections;
        std::set<double> intersections;

        Label            left;
        Label            on;
        Label            right;
        bool            isolated;
        bool            saveZ;


//!    These split Edges are owned by this EdgeNode and are thus DELETED upon destruction
        EdgeNode*        parent;        //    Null if root EdgeNode
        EdgeNodeList    children;

        EdgeNodeEnd*    start;
        EdgeNodeEnd*    end;

        void            createSplitEdge(double dist0, double dist1);

    public:
        EdgeNode(void);
        EdgeNode(Label left, Label on, Label right, int geo, bool saveZ = false);
        ~EdgeNode(void);

        int                getGeomArg(void) const;
        void            setGeomArg(int geo);        

        void            addPoint(const Point* p);
        void            addPoint(const Point& p);
        void            addPointNode(PointNode* p);
        void            clearPoints(void);
        PointNode*        getStartPoint(void) const;
        PointNode*        getEndPoint(void) const;
        int                getNumPoints(void) const;
        PointNode*        getPointN(int n) const;

        Label            getLeft(void) const;
        Location        getLeft(int n) const;
        void            setLeft(Label label);
        void            setLeft(int n, Location l);
        Label            getOn(void) const;
        Location        getOn(int n) const;
        void            setOn(Label label);
        void            setOn(int n, Location l);
        Label            getRight(void) const;
        Location        getRight(int n) const;
        void            setRight(Label label);
        void            setRight(int n, Location l);

        bool            isIsolated(void) const;
        void            setIsolated(bool flag);

        bool            equals(const EdgeNode* other) const;

//!    Adds an intersection to this EdgeNode, dist is expect to NOT be normalized.
        void            addIntersection(int index, double dist);

        void            createEnds(void);
        EdgeNodeEnd*    getEnd(int i);

        EdgeNode*        getParent(void) const;

        EdgeNodeList    split(void);

    };

}