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
#include "sfa/PointExtractor.h"
#include "sfa/Point.h"
#include "sfa/LineString.h"
#include "sfa/Polygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/GeometryCollection.h"

namespace sfa {

    void PointExtractor::ExtractPoint(Point* point, PointList& list)
    {
        list.push_back(point);
    }

    void PointExtractor::ExtractLineString(LineString* linestring, PointList& list)
    {
        for (int i=0, n=linestring->getNumPoints(); i<n; i++)
            list.push_back(linestring->getPointN(i));
    }

    void PointExtractor::ExtractPolygon(Polygon* polygon, PointList& list)
    {
        ExtractLineString(polygon->getExteriorRing(), list);
        for (int i=0, n=polygon->getNumInteriorRing(); i<n; i++)
            ExtractLineString(polygon->getInteriorRingN(i), list);
    }

    void PointExtractor::ExtractPolyhedralSurface(PolyhedralSurface* polyhedralsurface, PointList& list)
    {
        for (int i=0, n=polyhedralsurface->getNumPatches(); i<n; i++)
            ExtractPolygon(polyhedralsurface->getPatchN(i), list);
    }

    void PointExtractor::ExtractGeometryCollection(GeometryCollection* geometrycollection, PointList& list)
    {
        for (int i=0, n=geometrycollection->getNumGeometries(); i<n; i++)
            ExtractPoints(geometrycollection->getGeometryN(i+1), list);
    }

    void PointExtractor::ExtractPoints(Geometry* geometry, PointList& list)
    {
        switch (geometry->getWKBGeometryType(false, false))
        {
        case wkbPoint: 
            ExtractPoint(static_cast<Point*>(geometry), list);
            break;
        case wkbLineString: 
            ExtractLineString(static_cast<LineString*>(geometry), list);
            break;
        case wkbTriangle:
        case wkbPolygon: 
            ExtractPolygon(static_cast<Polygon*>(geometry), list);
            break;
        case wkbPolyhedralSurface: 
        case wkbTIN:
            ExtractPolyhedralSurface(static_cast<PolyhedralSurface*>(geometry), list);
            break;
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            ExtractGeometryCollection(static_cast<GeometryCollection*>(geometry), list);
            break;
        default:
            break;
        }
    }

    PointList PointExtractor::apply(Geometry* geometry)
    {
        PointList list;
        apply(geometry, list);
        return list;
    }

    void PointExtractor::apply(Geometry* geometry, PointList& list)
    {
        return ExtractPoints(geometry, list);
    }

}