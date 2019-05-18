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
#include "sfa/PointLineIntersector.h"

namespace sfa {

    const int PointLineIntersector::UNKNOWN                = -1;
    const int PointLineIntersector::DISJOINT            = 0;
    const int PointLineIntersector::LEFT                = 1;
    const int PointLineIntersector::RIGHT                = 2;
    const int PointLineIntersector::COLLINEAR_BEFORE    = 3;
    const int PointLineIntersector::COLLINEAR_AFTER        = 4;
    const int PointLineIntersector::BETWEEN                = 5;
    const int PointLineIntersector::EQUALS_P1            = 6;
    const int PointLineIntersector::EQUALS_P2            = 7;

    int PointLineIntersector::apply(const Point& p1, const Point& p2, const Point& p)
    {
    //    Compute distance and location of point on line
        Point n = p2 - p1;
        Point r = p - p1;

#if 1
        double nlength = n.length();
        double t = (r.X()*n.X() + r.Y()*n.Y()) / nlength;
        double d = (r.X()*n.Y() - r.Y()*n.X()) / nlength;
        double e = SFA_EPSILON;
#else
        double nlength2 = n.length2();
        double t = (r.X()*n.X() + r.Y()*n.Y());
        t *= t; t /= nlength2;
        double d = (r.X()*n.Y() - r.Y()*n.X());
        d *= d; d /= nlength2;
        double e = SFA_EPSILON*SFA_EPSILON;
#endif

        if (d < -e)
            return LEFT;
        if (d > e)
            return RIGHT;
        if (t < -e)
            return COLLINEAR_BEFORE;
        if (t > 1+e)
            return COLLINEAR_AFTER;
        if (t > e && t < 1-e)
            return BETWEEN;
        if (t < e)
            return EQUALS_P1;
        if (t > 1-e)
            return EQUALS_P2;
        return UNKNOWN;
    }

    int PointLineIntersector::apply3D(const Point& p1, const Point& p2, const Point& p)
    {
    //    Compute distance and location of point on line
        Point n = p2 - p1;
        Point r = p - p1;

        double nlength = n.length();
        double t = r.dot(n) / nlength;
        double d = r.cross(n).length() / nlength;

        if (d > SFA_EPSILON)
            return DISJOINT;
        if (t < -SFA_EPSILON)
            return COLLINEAR_BEFORE;
        if (t > 1+SFA_EPSILON)
            return COLLINEAR_AFTER;
        if (t > SFA_EPSILON && t < 1-SFA_EPSILON)
            return BETWEEN;
        if (t < SFA_EPSILON)
            return EQUALS_P1;
        if (t > 1-SFA_EPSILON)
            return EQUALS_P2;
        return UNKNOWN;
    }
}