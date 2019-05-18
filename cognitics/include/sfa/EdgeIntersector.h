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
/*! \brief Provides sfa::EdgeIntersector.
\author Joshua Anghel <janghel@cognitics.net>
\date 20 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "PointNode.h"
#include "EdgeNode.h"

namespace sfa {

/*! \class sfa::EdgeIntersector EdgeIntersector.h EdgeIntersector.h
\brief EdgeIntersector

Intersects a set of EdgeNodes and adds any intersections found to those EdgeNodes.

Currently uses Brute force method to compare all segments of each EdgeNode, but would be more efficient if some sweepline 
was imlpemented for larger sets of EdgeNodes. Perhaps a Monotonic subdivision might also increase performance.
*/
    class EdgeIntersector
    {
    protected:

/*! \brief Checks if an intersection is trivial.

A trivial intersection occurs at least once on every edge that has more than two points, this is because the vertex point
is an "intersection" between the two segments which are touching it.
\param a First EdgeNode with potential intersection.
\param ai The index of the starting point on the frist EdgeNode to test for trivial intersection
\param b Second EdgeNode with potential intersection.
\param bi The index of the starting point on the second EdgeNode to test for trivial intersection
\return True if there is a trivial intersection between the two.
*/
        virtual bool isTrivial(EdgeNode* a, int ai, EdgeNode* b, int bi);

/*!    Checks if the points given are within range of each other to even intersect.
\param p1 Point 1 of Line 1.
\param p2 Point 2 of Line 1.
\param p3 Point 1 of Line 2.
\param p4 Point 2 of Line 2.
*/
        virtual bool isDisjoint(const Point* p1,const Point* p2,const Point* p3,const Point* p4);

/*! \brief Test for an intersection and add it to the list if one occurs.

Robust and can handle any two segments of two EdgeNodes without fail. Can account for parallel segments as well as segments
touching at an end point or overlaping along a line.
\param a First EdgeNode to find intersection for.
\param ai Index to first Point of the Line from a to test.
\param b Second EdgeNode to find intersection for.
\param bi Index to first Point of the Line from b to test.
*/
        virtual void addIntersection(EdgeNode* a, int ai, EdgeNode* b, int bi);

    public:
/*! \brief Both initialize and intersect the two sets of edges

If selfTest is set, it will know that A[n] == B[n] since A and B are equal. This allows fewer tests to be run.
Currently uses a brute force test, but will implement a sweepline soon.
*/
        EdgeIntersector(void) { }
        EdgeIntersector(EdgeNodeList A, EdgeNodeList B, bool selfTest = false);
        ~EdgeIntersector(void) {}

    };

}