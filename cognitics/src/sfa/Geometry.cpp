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

#include "sfa/Geometry.h"
#include "sfa/Point.h"
#include "sfa/LineString.h"
#include "sfa/Polygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/GeometryCollection.h"
#include "sfa/MultiPoint.h"
#include "sfa/MultiLineString.h"
#include "sfa/MultiPolygon.h"

#include "sfa/Buffer.h"
#include "sfa/Buffer3D.h"
#include "sfa/Distance.h"
#include "sfa/Distance3D.h"
#include "sfa/Overlay.h"
#include "sfa/Overlay3D.h"
#include "sfa/Relate.h"

#include <algorithm>
#include <sstream>
#include <exception>
#include <stdexcept>

namespace sfa
{
    double SFA_EPSILON = 1e-7;

    GeometryBase::~GeometryBase(void)
    {
    }

    GeometryBase::GeometryBase(void)
    {
    }

    Geometry::~Geometry(void)
    {

    }

    Geometry::Geometry(void)
    {
        this->coordinateSystem = NULL;
    }

    Geometry* Geometry::copy(void) const
    {
        Geometry *result;
        int type = getWKBGeometryType(false,false);
        switch(type)
        {
        case wkbPoint:
            result = new Point(static_cast<const Point*>(this));
            break;
        case wkbLineString:
            result = new LineString(static_cast<const LineString*>(this));
            break;
        case wkbPolygon:
        case wkbTriangle:
            result = new Polygon(static_cast<const Polygon*>(this));
            break;
        case wkbMultiPoint:
            result = new MultiPoint(static_cast<const MultiPoint*>(this));
            break;
        case wkbMultiLineString:
            result = new MultiLineString(static_cast<const MultiLineString*>(this));
            break;
        case wkbMultiPolygon:
            result = new MultiPolygon(static_cast<const MultiPolygon*>(this));
            break;
        case wkbGeometryCollection:
            result = new GeometryCollection(static_cast<const GeometryCollection*>(this));
            break;
        case wkbPolyhedralSurface:
        case wkbTIN:
            result = new PolyhedralSurface(static_cast<const PolyhedralSurface*>(this));
            break;
        default:
            result = NULL;
        }
        if(result)
            result->setCoordinateSystem(getCoordinateSystem());
        return result;
    }

    GeometrySP Geometry::getShared(Geometry* geometry)
    {
        if (!geometry) return GeometrySP();
        int type = geometry->getWKBGeometryType(false,false);
        switch(type)
        {
        case wkbPoint:
            return PointSP(static_cast<Point*>(geometry));
        case wkbLineString:
            return LineStringSP(static_cast<LineString*>(geometry));
        case wkbPolygon:
        case wkbTriangle:
            return PolygonSP(static_cast<Polygon*>(geometry));
        case wkbMultiPoint:
            return MultiPointSP(static_cast<MultiPoint*>(geometry));
        case wkbMultiLineString:
            return MultiLineStringSP(static_cast<MultiLineString*>(geometry));
        case wkbMultiPolygon:
            return MultiPolygonSP(static_cast<MultiPolygon*>(geometry));
        case wkbGeometryCollection:
            return GeometryCollectionSP(static_cast<GeometryCollection*>(geometry));
        case wkbPolyhedralSurface:
        case wkbTIN:
            return PolyhedralSurfaceSP(static_cast<PolyhedralSurface*>(geometry));
        default:
            return GeometrySP();
        }
    }

    cts::CS_CoordinateSystem* Geometry::getCoordinateSystem(void) const
    {
        return coordinateSystem;
    }

    void Geometry::setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform)
    {
        this->coordinateSystem = coordinateSystem;
    }

    void Geometry::setEpsilon(double newEpsilon)
    {
        SFA_EPSILON = newEpsilon;
    }

    std::string Geometry::asText(bool tag) const
    {
        std::stringstream os("", std::stringstream::out);
        os << std::fixed;
        os.precision(6);
        toText(os, tag, is3D(), isMeasured());
        return os.str();
    }

    void Geometry::toText(std::ostream &os, bool tag, bool withZ, bool withM) const
    {
        if(!tag)
            return;
        std::string geometryType = getGeometryType();
        std::transform(geometryType.begin(), geometryType.end(), geometryType.begin(), toupper);
        if((geometryType == "LINE") || (geometryType == "LINEARRING"))
            geometryType = "LINESTRING";
        os << geometryType << " ";
        if(withZ)
            os << "Z";
        if(withM)
            os << "M";
    }

    ccl::binary Geometry::asBinary(WKBByteOrder byteOrder) const
    {
        std::stringstream os("", std::stringstream::binary | std::stringstream::out);
        os << (char)byteOrder;
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> geometryType(getWKBGeometryType(is3D(), isMeasured()));
            os << geometryType;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> geometryType(getWKBGeometryType(is3D(), isMeasured()));
            os << geometryType;
        }
        toBinary(os, byteOrder, is3D(), isMeasured());
        std::string str = os.str();
        const char *temp = str.c_str();
        return ccl::binary(str.begin(),str.end());
    }

    bool Geometry::equals(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.equals();
    }
    
    bool Geometry::equals3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.equals();
    }

    bool Geometry::disjoint(const Geometry* another) const
    {
        return !intersects(another);
    }

    bool Geometry::disjoint3D(const Geometry* another) const
    {
        return !intersects3D(another);
    }

    bool Geometry::intersects(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.intersects();
    }

    bool Geometry::intersects3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.intersects();
    }

    bool Geometry::touches(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.touches();
    }

    bool Geometry::touches3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.touches();
    }

    bool Geometry::crosses(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.crosses();
    }

    bool Geometry::crosses3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.crosses();
    }

    bool Geometry::within(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.within();
    }

    bool Geometry::within3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.within();
    }

    bool Geometry::contains(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.contains();
    }

    bool Geometry::contains3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.contains();
    }

    bool Geometry::overlaps(const Geometry* another) const
    {
        Relate matrix(this,another);
        return matrix.overlaps();
    }

    bool Geometry::overlaps3D(const Geometry* another) const
    {
        Relate matrix(this,another,true);
        return matrix.overlaps();
    }

    bool Geometry::relate(const Geometry* another, const std::string &matrix) const
    {
        Relate relateOP(this,another);
        return relateOP.relate(matrix);
    }

    bool Geometry::relate3D(const Geometry* another, const std::string &matrix) const
    {
        Relate relateOP(this,another,true);
        return relateOP.relate(matrix);
    }

    Geometry* Geometry::locateAlong(double mValue) const
    {
        return locateBetween(mValue,mValue);
    }

    Geometry* Geometry::locateBetween(double mStart, double mEnd) const
    {
        return NULL;
    }

    double Geometry::distance(const Geometry* another) const
    {
        Distance op;
        return op.apply(this,another);
    }

    double Geometry::distance3D(const Geometry* another) const
    {
        Distance3D op;
        return op.apply(this,another);
    }

    Geometry* Geometry::buffer(double distance, int EndcapPolicy, bool UnionPolicy, int interpolationN) const
    {
        Buffer buff(interpolationN, EndcapPolicy, UnionPolicy);
        return buff.apply(this,distance);
    }

    Geometry* Geometry::buffer3D(double distance, int depth) const
    {
        Buffer3D buff(this,distance,depth);
        return buff.apply();
    }

    Geometry* Geometry::intersection(const Geometry* another) const
    {
        Overlay op(this,another);
        return op.computeOverlay(OVERLAY_INTERSECTION);
    }

    Geometry* Geometry::intersection(const Geometry* another, bool saveZ) const
    {
        Overlay op(this,another);
        op.saveZ = saveZ;
        return op.computeOverlay(OVERLAY_INTERSECTION);
    }

    Geometry* Geometry::intersection3D(const Geometry* another) const
    {
        Overlay3D op(this,another);
        return op.computeOverlay(OVERLAY_INTERSECTION);
    }

    Geometry* Geometry::Union(const Geometry* another) const
    {
        Overlay op(this,another);
        return op.computeOverlay(OVERLAY_UNION);
    }

    Geometry* Geometry::Union3D(const Geometry* another) const
    {
        Overlay3D op(this,another);
        return op.computeOverlay(OVERLAY_UNION);
    }

    Geometry* Geometry::difference(const Geometry* another) const
    {
        Overlay op(this,another);
        return op.computeOverlay(OVERLAY_DIFFERENCE);
    }

    Geometry* Geometry::difference3D(const Geometry* another) const
    {
        Overlay3D op(this,another);
        return op.computeOverlay(OVERLAY_DIFFERENCE);
    }

    Geometry* Geometry::symDifference(const Geometry* another) const
    {
        Overlay op(this,another);
        return op.computeOverlay(OVERLAY_SYMDIFFERENCE);
    }

    Geometry* Geometry::symDifference3D(const Geometry* another) const
    {
        Overlay3D op(this,another);
        return op.computeOverlay(OVERLAY_SYMDIFFERENCE);
    }

    WKBGeometryType Geometry::sGetWKBGeometryType(WKBGeometryType type, bool withZ, bool withM)
    {
        int itype = (int)type;
        if (itype < 1000)
        {
            return type;
        }
        // 1000 - 1999 is Z
        if (itype < 2000)
        {
            if (!withZ)
                return (WKBGeometryType)(itype - 1000);
            return type;
        }
        // 2000 - 2999 is M
        if (itype < 3000)
        {
            if (!withM)
                return (WKBGeometryType)(itype - 2000);
            return type;
        }
        // 3000 - 3999 is ZM
        if (itype < 3000)
        {
            if (!withM && !withZ)
                return (WKBGeometryType)(itype - 3000);
            if (withM && !withZ)
                return (WKBGeometryType)(itype - 1000);
            if (!withM && withZ)
                return (WKBGeometryType)(itype - 2000);
            return type;
        }
        return wkbUnknownSFA;
    }

    std::string Geometry::getGeometryTypeText(WKBGeometryType type)
    {
        switch (type)
        {
        case wkbPoint:
            return "wkbPoint";
        case wkbPointZ:
            return "wkbPointZ";
        case wkbPointZM:
            return "wkbPointZM";
        case wkbLineString:
            return "wkbLineString";
        case wkbLineStringZ:
            return "wkbLineStringZ";
        case wkbLineStringZM:
            return "wkbLineStringZM";
        case wkbPolygon:
            return "wkbPolygon";
        case wkbPolygonZ:
            return "wkbPolygonZ";
        case wkbPolygonZM:
            return "wkbPolygonZM";
        case wkbTriangle:
            return "wkbTriangle";
        case wkbTriangleZ:
            return "wkbTriangleZ";
        case wkbTriangleZM:
            return "wkbTriangleZM";
        case wkbMultiPoint:
            return "wkbMultiPoint";
        case wkbMultiPointZ:
            return "wkbMultiPointZ";
        case wkbMultiPointZM:
            return "wkbMultiPointZM";
        case wkbMultiLineString:
            return "wkbMultiLineString";
        case wkbMultiLineStringZ:
            return "wkbMultiLineStringZ";
        case wkbMultiLineStringZM:
            return "wkbMultiLineStringZM";
        case wkbMultiPolygon:
            return "wkbMultiPolygon";
        case wkbMultiPolygonZ:
            return "wkbMultiPolygonZ";
        case wkbMultiPolygonZM:
            return "wkbMultiPolygonZM";
        case wkbGeometryCollection:
            return "wkbGeometryCollection";
        case wkbGeometryCollectionZ:
            return "wkbGeometryCollectionZ";
        case wkbGeometryCollectionZM:
            return "wkbGeometryCollectionZM";
        case wkbPolyhedralSurface:
            return "wkbPolyhedralSurface";
        case wkbPolyhedralSurfaceZ:
            return "wkbPolyhedralSurfaceZ";
        case wkbPolyhedralSurfaceZM:
            return "wkbPolyhedralSurfaceZM";
        case wkbTIN:
            return "wkbTIN";
        case wkbTINZ:
            return "wkbTINZ";
        case wkbTINZM:
            return "wkbTINZM";
        default:
            return "Unknown";
        }
    }

}