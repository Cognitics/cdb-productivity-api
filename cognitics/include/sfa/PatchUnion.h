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
/*! \brief Provides sfa::PatchUnion
\author Josh Anghel <janghel@cognitics.net>
\date 3 September 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Polygon.h"

namespace sfa {

/*!\class PatchUnion PatchUnion.h PatchUnion.h
\brief PatchUnion

Constructs the union or boundary LineString for a PolyhedralSurface. Will work with any collection of Polygons that is
isomorphic to a PolyhedralSurface.

Usage:
\code
    sfa::PolygonList list;
    LineStringSP boundary;

    boundary = sfa::PatchUnion::apply(list);
\endcode

Note that this is s full 3D method only indirectly. It will not create any new Points, it only uses the currently existing
Points in the Polygons to create the boundary around the PolyhedralSurface. This also means that the LineString returned 
will contain PointSP that are pointers to the same Points found in the given Polygons, so use caution if you intend to alter
the resulting LineString in any way.
*/
    class PatchUnion
    {
    protected:
        PatchUnion(void) {}
        ~PatchUnion(void) {}

    public:

/*!\brief apply

Construsts the union (boundary) of a PolygonList that is isomorphic to a PolyhedralSurface.
\param list The PolygonList you wish to process
\return A LineStringSP with the result, will be null if there is no result or the resulting boundary is closed.
\throws runtime_error is thrown if it is found that the given list does not represent a valid PolyhedralSurface
*/
        static LineString* apply(const PolygonList& list);
    };

}