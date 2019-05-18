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

//#pragma optimize("", off)

#include "sfa/Factory.h"
#include "sfa/GeometryCollection.h"
#include "sfa/Point.h"
#include "sfa/MultiPoint.h"
#include "sfa/Curve.h"
#include "sfa/LineString.h"
#include "sfa/MultiCurve.h"
#include "sfa/MultiLineString.h"
#include "sfa/Surface.h"
#include "sfa/Polygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/MultiSurface.h"
#include "sfa/MultiPolygon.h"

#include <sstream>

namespace sfa
{


    Geometry* getGeometryFromText(const std::string wkt)
    {
        std::stringstream is(wkt, std::stringstream::binary | std::stringstream::in);
        is.seekg(0);

    //    Get tag
        std::string tag;
        is >> tag;

    //    Get flags
        bool withZ = false, withM = false;
        while (is.peek() == ' ') is.ignore(1);
        if (is.peek() != '(')
        {
            std::string flag;
            is >> flag;
            if (flag.find('Z') != flag.npos || flag.find('z') != flag.npos) withZ = true;
            if (flag.find('M') != flag.npos || flag.find('m') != flag.npos) withM = true;
        }

    //    Convert to lower case
        for (int i = 0; i < int(tag.length()); i++) tag[i] = tolower(tag[i]);

    //    Parse tag
        Geometry* geometry = NULL;
        if (tag.compare("point")==0) geometry = new Point;
        else if (tag.compare("linestring")==0 || tag.compare("line")==0 || tag.compare("linearring")==0) geometry = new LineString;
        else if (tag.compare("polygon")==0 || tag.compare("triangle") == 0) geometry = new Polygon;
        else if (tag.compare("multipoint")==0) geometry = new MultiPoint;
        else if (tag.compare("multilinestring")==0) geometry = new MultiLineString;
        else if (tag.compare("multipolygon")==0) geometry = new MultiPolygon;
        else if (tag.compare("geometrycollection")==0) geometry = new GeometryCollection;
        else if (tag.compare("polyhedralsurface")==0 || tag.compare("tin")==0 || tag.compare("polyhedron")==0) geometry = new PolyhedralSurface;
        else return NULL;
        //else throw std::runtime_error("GeometryCollection::fromText Unrecognized Geometry tag!");

        geometry->fromText(is,true,withZ,withM);
        return geometry;
    }


    Geometry* getGeometryFromBinary(const ccl::binary wkb)
    {
        std::string wkb_string(wkb.begin(),wkb.end());
        std::stringstream is(wkb_string, std::stringstream::binary | std::stringstream::in);
        is.seekg(0);

        ccl::uint32_t geometryType = 0;
        WKBByteOrder byteOrder = (WKBByteOrder)is.get();
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> tempType;
            is >> tempType;
            geometryType = tempType;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> tempType;
            is >> tempType;
            geometryType = tempType;
        }

        Geometry* geometry = NULL;
        switch(geometryType)
        {
            case wkbPoint:
            case wkbPointZ:
            case wkbPointM:
            case wkbPointZM:
                geometry = new Point();
                break;
            case wkbLineString:
            case wkbLineStringZ:
            case wkbLineStringM:
            case wkbLineStringZM:
                geometry = new LineString();
                break;
            case wkbPolygon:
            case wkbPolygonZ:
            case wkbPolygonM:
            case wkbPolygonZM:
            case wkbTriangle:
            case wkbTriangleZ:
            case wkbTriangleM:
            case wkbTriangleZM:
                geometry = new Polygon();
                break;
            case wkbMultiPoint:
            case wkbMultiPointZ:
            case wkbMultiPointM:
            case wkbMultiPointZM:
                geometry = new MultiPoint();
                break;
            case wkbMultiLineString:
            case wkbMultiLineStringZ:
            case wkbMultiLineStringM:
            case wkbMultiLineStringZM:
                geometry = new MultiLineString();
                break;
            case wkbMultiPolygon:
            case wkbMultiPolygonZ:
            case wkbMultiPolygonM:
            case wkbMultiPolygonZM:
                geometry = new MultiPolygon();
                break;
            case wkbGeometryCollection:
            case wkbGeometryCollectionZ:
            case wkbGeometryCollectionM:
            case wkbGeometryCollectionZM:
                geometry = new GeometryCollection();
                break;
            case wkbPolyhedralSurface:
            case wkbPolyhedralSurfaceZ:
            case wkbPolyhedralSurfaceM:
            case wkbPolyhedralSurfaceZM:
            case wkbTIN:
            case wkbTINZ:
            case wkbTINM:
            case wkbTINZM:
                geometry = new PolyhedralSurface();
                break;
        }

        bool withZ = false;
        bool withM = false;
        switch(geometryType)
        {
            case wkbPointM:
            case wkbPointZM:
            case wkbLineStringM:
            case wkbLineStringZM:
            case wkbPolygonM:
            case wkbPolygonZM:
            case wkbTriangleM:
            case wkbTriangleZM:
            case wkbMultiPointM:
            case wkbMultiPointZM:
            case wkbMultiLineStringM:
            case wkbMultiLineStringZM:
            case wkbMultiPolygonM:
            case wkbMultiPolygonZM:
            case wkbGeometryCollectionM:
            case wkbGeometryCollectionZM:
            case wkbPolyhedralSurfaceM:
            case wkbPolyhedralSurfaceZM:
            case wkbTINM:
            case wkbTINZM:
                withM = true;
                break;
            default:
                withM = false;
        }

        switch(geometryType)
        {
            case wkbPointZ:
            case wkbPointZM:
            case wkbLineStringZ:
            case wkbLineStringZM:
            case wkbPolygonZ:
            case wkbPolygonZM:
            case wkbTriangleZ:
            case wkbTriangleZM:
            case wkbMultiPointZ:
            case wkbMultiPointZM:
            case wkbMultiLineStringZ:
            case wkbMultiLineStringZM:
            case wkbMultiPolygonZ:
            case wkbMultiPolygonZM:
            case wkbGeometryCollectionZ:
            case wkbGeometryCollectionZM:
            case wkbPolyhedralSurfaceZ:
            case wkbPolyhedralSurfaceZM:
            case wkbTINZ:
            case wkbTINM:
            case wkbTINZM:
                withZ = true;
                break;
            default:
                withZ = false;
        }

        if (geometry)
            geometry->fromBinary(is, byteOrder, withZ, withM);
        return geometry;
    }

}
