/****************************************************************************
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
#pragma once

#include "Point.h"

namespace sfa {

    class PointLineIntersector
    {
    protected:
        PointLineIntersector(void) { }
        ~PointLineIntersector(void) { }

    public:
        static const int UNKNOWN;            //    An error occured
        static const int DISJOINT;            //    The point is not collinear.
        static const int LEFT;                //    The point is to the left of the line
        static const int RIGHT;                //    The point is to the right of the line
        static const int COLLINEAR_BEFORE;    //    The point is collinear and before p1
        static const int COLLINEAR_AFTER;    //    The point is collinear and after p2
        static const int BETWEEN;            //    The point is between p1 and p2
        static const int EQUALS_P1;            //    The point equals p1
        static const int EQUALS_P2;            //    The point equals p2

/*!    \brief apply

Returns the location of a Point relative to a line segment. Operations are done in 2D.
\param p1 First point of segment.
\param p2 Second point of segment.
\param p Point to test.
\return The location of the Point as a flag.
*/
        static int apply(const Point& p1, const Point& p2, const Point& p);

/*!    \brief apply

Returns the location of a Point relative to a line segment. Operations are done in 3D.
\param p1 First point of segment.
\param p2 Second point of segment.
\param p Point to test.
\return The location of the Point as a flag.
*/
        static int apply3D(const Point& p1, const Point& p2, const Point& p);
    };

}