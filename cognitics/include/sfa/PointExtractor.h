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

#include "Geometry.h"
#include "Point.h"
#include "LineString.h"
#include "Polygon.h"
#include "PolyhedralSurface.h"
#include "GeometryCollection.h"

namespace sfa {

    class PointExtractor
    {
    protected:
        PointExtractor(void) { }
        ~PointExtractor(void) { }

        static void ExtractPoint(Point* point, PointList& list);
        static void ExtractLineString(LineString* linestring, PointList& list);
        static void ExtractPolygon(Polygon* polygon, PointList& list);
        static void ExtractPolyhedralSurface(PolyhedralSurface* polyhedralsurface, PointList& list);
        static void ExtractGeometryCollection(GeometryCollection* geometrycollection, PointList& list);
        static void ExtractPoints(Geometry* geometry, PointList& list);

    public:
        static PointList apply(Geometry* geometry);
        static void apply(Geometry* geometry, PointList& list);
    };

}