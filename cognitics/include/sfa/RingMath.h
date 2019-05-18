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
/*! \brief Provides area, centroid, orientation, and point in polygon algorithms for rings and polygons.
\author Josh Anghel <janghel@cognitics.net>
\date 2 September 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "LineString.h"
#include "Polygon.h"

namespace sfa {

/****************************************************************************
    Area Algorithms
****************************************************************************/
/*!\brief GetRingArea

Provides an Algorithm for acquiring the area of a LinearRing. The area will be positive if and only if the ring is defined 
counter-clockwise, and negative otherwise. Because of this, this function can be used to determine if a ring is defined 
clockwise or counterclockwise regardless if it is convex or concave. Assumes LinearRing to be simple and valid, also ignores
z values.
\param ring LinearRing to compute area for.
\return Area of passed LinearRing.
*/
    double GetRingArea(const LineString* ring);

/*!    \brief GetPolygonArea

Sums up the area contributed by each ring in the polygon, by calling GetRingArea for each ring. Assumes CCW orientation for 
exterior rings and CW orientation for inner rings (holes). Ignores z values.
\param polygon Polygon to compute area for.
\return Area of passed Polygon.
*/
    double GetPolygonArea(const Polygon* polygon);

/****************************************************************************
    Centroid Algorithms
****************************************************************************/
/*!    \brief GetRingCentroid

Computes the area weighted centroid of a single LinearRing. Due to the fact that a LinearRing may not be convex, the centroid
may not always be within the LinearRing itself. Assumes CCW orientation, a simple and valid LinearRing, and ignores z values.
\param ring LinearRing to compute a centroid for.
\return Centroid of the passed LinearRing.
*/
    Point GetRingCentroid(const LineString* ring);

/*!    \brief GetPolygonCentroid

Algorithm which computes the area weighted centroid of a single Polygon. Due to the fact that a Polygon may not be convex or 
may have interior rings (holes), the centroid may not always be within the Polygon itself. Assumes CCW orientation of exterior
ring and CW orientation of inner rings (holes). Also assumes the Polygon to be valid and simple. Ignores z values.
\param polygon Polygon to compute a centroid for.
\return Centroid of the passed polygon.
*/
    Point GetPolygonCentroid(const Polygon* polygon);

/****************************************************************************
    Orientation Algorithms
****************************************************************************/
    enum Orientation
    {
        ORIENTATION_CW            = 0,
        CLOCKWISE                = 0,
        ORIENTATION_C_CW        = 1,
        COUNTERCLOCKWISE        = 1,
        ORIENTATION_UNKNOWN        = 2
    };

    Orientation ComputeOrientation(const LineString& ring);
    Orientation ComputeOrientation(const LineString* ring);

//!    Only checks exterionr ring
    Orientation ComputeOrientation(const Polygon& polygon);
    Orientation ComputeOrientation(const Polygon* polygon);

/*!    \brief IsPolygonConsistent

A Polygon is consistently defined if:
    -    All interior rings have the same orientation
    -    All interior rings are orientated in the oposite direction of the exterior ring
The actually orientation does not matter, so long as the above statements are true.
\param polygon Polygon to test for consistency.
\return True if the passed Polygon matches the above criteria, False otherwise.
*/
    bool IsPolygonConsistent(const Polygon& polygon);
    bool IsPolygonConsistent(const Polygon* polygon);

/*!    \brief CorrectPolygon

Automatically checks and corrects the orrientations of all the rings in a given Polygon. The returned Polygon will always have
CCW outer rings and CW inner rings (holes). Assumes the Polygon to be simple and valid (the Polygon may self intersect only at 
individual Points so long as the Polygon does not "overlap" itself).
\param polygon Polygon to correct.
\return A correctly orientated Polygon.
*/
    void CorrectPolygon(Polygon& polygon);
    void CorrectPolygon(Polygon* polygon);

/*!    \brief Correct3DPolygon

Algorithm that projects a Polygon to 2D space and calls CorrectPolygon. Returns the reprojected 3D corrected Polygon.
\param polygon 3D Polygon to correct.
\return A 3D correctly orientated Polygon.
*/
    void Correct3DPolygon(Polygon& polygon);
    void Correct3DPolygon(Polygon* polygon);

/****************************************************************************
    Point Location Algorithms
****************************************************************************/
/*!    \brief PointInRing

Algorithm to determine if a Point is inside a single LinearRing by itterating around all edges of the LinearRing. Assumes CCW
orientation of the ring.
\param point Point to test.
\param ring LinearRing to test a Point against.
\return True if the passed Point is on or inside the passed LinearRing or False otherwise.
*/
    bool PointInRing(const Point* point, const LineString* ring);
    bool PointInRing(const Point& point, const LineString& ring);

/*!    \brief PointInPolygon

Algorithm to determine if a Point is inside a single Polygon by itterating around all edges of the Polygon. Assumes CCW orientation
of the outer rings and CW orientation for inner rings (holes). Also assumes the Polygon is simple (the polygon may self intersect only
if it does not "overlap" itself).
\param point Point to test.
\param polygon Polygon to test a Point against.
\return True if the passed Point is on or inside the passed Polygon or False otherwise.

*/
    bool PointInPolygon(const Point* point, const Polygon* polygon);
    bool PointInPolygon(const Point& point, const Polygon& polygon);


}