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
/*! \brief Provides sfa::Intersector
\author Josh Anghel <joshanghel@cognitics.net>
\date 4 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"

namespace sfa {

/*!    \class sfa::Intersector Intersector.h Intersector.h
\brief Intersector

Provides an algorithm to efficiently report if two Geometries intersect. The algorithm will return True immediately if any intersection
is found. Note that no intersection point is actually determined or reported by this algorithm, only its existence are investigated.

Usage:
\code
    GeometrySP a, b;
    bool result;

//    Called through the chain Geometry->Relate->intersects->Intersector
    result = a->intersects(b);

//    Or called directly
    result = sfa::Intersector::apply(a,b);
\endcode

This is equivalent to calling the Geometry::intersects method.

Ignores Z values, using only X and Y values from each Geometry. As a result, Geometries reported to intersect by this method may not
intersect using the Intersector3D algorithm since two Geometries shadows may overlap without them actually intersecting in 3D space themselves.
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class Intersector
    {
    protected:
        Intersector(void) {}
        ~Intersector(void) {}

//!    Returns True if two Points intersect, False otherwise. Ignores Z values.
        static bool Point_Point(const Geometry* a, const Geometry* b);

//! Returns True if a Point is found to lie on a LineString, False otherwise. Ignores Z values.
        static bool Point_LineString(const Geometry* a, const Geometry* b);

//! Returns True if a Point is on or inside a Polygon, False otherwise. Ignores Z values.
        static bool Point_Polygon(const Geometry* a, const Geometry* b);

//! Returns True if two LineStrings overlap at at least one Point, False otherwise. Ignores Z values.
        static bool LineString_LineString(const Geometry* a, const Geometry* b);

//! Returns True if any portion of a LineString is on or inside a Polygon, False otherwise. Ignores Z values.
        static bool LineString_Polygon(const Geometry* a, const Geometry* b);

//! Returns True if any portion of a Polygon is on or inside of another Polygon, False otherwise. Ignores Z values.
        static bool Polygon_Polygon(const Geometry* a, const Geometry* b);
        
//! Returns True if a PolyhedralSurface intersects another Geometry. Itterates through all child Polygons (pathces). Ignores Z values.
        static bool Surface_Geometry(const Geometry* a, const Geometry* b);

//! Returns True if any Geometry in a GeometryCollection intersects another Geometry. Ignores Z values.
        static bool Collection_Geometry(const Geometry* a, const Geometry* b);

    public:
//!\brief returns true if the two Geometries intersect. Ignores Z values.
        static bool apply(const Geometry* a, const Geometry* b);
    };

}