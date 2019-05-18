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
/*! \brief Provides sfa::collinear, sfa::between, and sfa::CrossProduct
\author Josh Anghel <janghel@cognitics.net>
\date 2 September 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Point.h"

namespace sfa {

/*!    \brief Collinear

Tests if 3 Points are collinear using linear interpolation. Ignores z values.
\return True if p1, p2, and p are all collinear, False otherwise.
*/
    bool Collinear(const Point& p1, const Point& p2, const Point& p);
    bool Collinear(const Point* p1, const Point* p2, const Point* p);

/*!    \brief Collinear3D

Tests if 3 Points are collinear using linear interpolation. Does not ignore z values.
\return True if p1, p2, and p are all collinear, False otherwise.
*/
    bool Collinear3D(const Point& p1, const Point& p2, const Point& p);
    bool Collinear3D(const Point* p1, const Point* p2, const Point* p);

/*!    \brief Between

Tests if a Point lies between two other Points. This is equivalent to testing if the Point lies on or in the bounding
box created by the two other Points. Ignores z values.
\return True if p is within the bounding box create by p1 and p2, False otherwise.
*/
    bool Between(const Point& p1, const Point& p2, const Point& p);
    bool Between(const Point* p1, const Point* p2, const Point* p);

/*!    \brief Between3D

Tests if a Point lies between two other Points. This is equivalent to testing if the Point lies on or in the bounding
box created by the two other Points. Does not ignore z values.
\return True if p is within the bounding box create by p1 and p2, False otherwise.
*/
    bool Between3D(const Point& p1, const Point& p2, const Point& p);
    bool Between3D(const Point* p1, const Point* p2, const Point* p);

/*!    \brief CollinearAndExclusiveBetween

A point is exclusively between if and only p is between p1 and p2, but does not equal either p1 nor p2.
Note that this is the same as saying p intersects the interior of the Line created by p1 and p2. Ignores z values.
\return True if p intersects the interior of the Line created by p1 and p2.
*/
    bool CollinearAndExclusiveBetween(const Point& p1, const Point& p2, const Point& p);
    bool CollinearAndExclusiveBetween(const Point* p1, const Point* p2, const Point* p);

/*!    \brief CollinearAndExclusiveBetween

A point is exclusively between if and only p is between p1 and p2, but does not equal either p1 nor p2.
Note that this is the same as saying p intersects the interior of the Line created by p1 and p2. Ignores z values.
\return True if p intersects the interior of the Line created by p1 and p2.
*/
    bool CollinearAndExclusiveBetween3D(const Point* p1, const Point* p2, const Point* p);
    bool CollinearAndExclusiveBetween3D(const Point& p1, const Point& p2, const Point& p);

/*!    \brief CrossProduct

Returns the normalized z value of the cross product of two vectors created by p1-p2 and p3-p4. This check is used
to find the orientation of two vectors with respect to each other. If p1->p2 is to the right of p3->p4 then 1 is
returned. If p1->p2 is to the left of p3->p4 then -1 is returned. If both p1->p2 and p3->p4 are parallel then 0
is returned. Ignores z values.
\param p1 First Point of first ray
\param p2 Second Point of first ray
\param p3 First Point of second ray
\param p4 Second Point of second ray
\return -1,0, or 1 indicating the direction p1->p2 is with respect to p3->p4
*/
    int CrossProduct(const Point& p1, const Point& p2, const Point& p3, const Point& p4);
    int CrossProduct(const Point* p1, const Point* p2, const Point* p3, const Point* p4);

}