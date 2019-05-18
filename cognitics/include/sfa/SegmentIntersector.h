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
/*! \brief Provides sfa::SegmentIntersector
\author Josh Anghel <janghel@cognitics.net>
\date 2 September 2010
*/
#pragma once

#include "Point.h"
#include "LineString.h"
#include "PointMath.h"

#include <cmath>

namespace sfa {


/*! \class sfa::SegmentIntersector SegmentIntersector.h SegmentIntersector.h
\brief SegmentIntersector

Algorithm for the intersection of two Lines given in the form of their endpoints.

Usage:
\code
    PointSP        segment1[2];        //    End points of segment 1
    PointSP        segment2[2];        //    End points of segment 2

    bool        result;                //    Intersection results
    GeometrySP    result_geometry;    //    Place holder for resulting geometry

//    Test only if the two segments intersect in the xy plane
    result = SegmentIntersector::Intersects(segment1[0],segment1[1],segment2[0],segment2[1]);

//    Test only if the two segments intersect
    result = SegmentIntersector::Intersects3D(segment1[0],segment1[1],segment2[0],segment2[1]);

//    Test if the two segments intersets and report the resulting point in the xy plane
    result = SegmentIntersector::Intersection(segment1[0],segment1[1],segment2[0],segment2[1],result_geometry);

//    Test if the two segments intersect and report the reuslting point
    result = SegmentIntersector::Intersection(segment1[0],segment1[1],segment2[0],segment2[1],result_geometry);
\endcode
*/
    class SegmentIntersector
    {
    protected:
        SegmentIntersector(void){}
        ~SegmentIntersector(void){}

        static bool handleParallel(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result);
        static bool handleParallel3D(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result);

    public:
        static bool Intersects(const Point* p1, const Point* p2, const Point* p3, const Point* p4);
        static bool Intersects3D(const Point* p1, const Point* p2, const Point* p3, const Point* p4);
        static bool Intersection(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result);
        static bool Intersection3D(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result);

    };

}