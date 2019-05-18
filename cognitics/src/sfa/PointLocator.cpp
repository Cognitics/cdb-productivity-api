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
#include "sfa/PointLocator.h"
#include "sfa/LineString.h"
#include "sfa/Polygon.h"
#include "sfa/MultiPoint.h"
#include "sfa/MultiLineString.h"
#include "sfa/MultiPolygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/PointMath.h"
#include "sfa/RingMath.h"

namespace sfa {

    bool PointLocator::PointOnLine(const Point* p, const LineString* line)
    {
        for (int i = 0; i < line->getNumPoints() - 1; i++)
            if (Collinear(line->getPointN(i),line->getPointN(i+1),p))
                if (Between(line->getPointN(i),line->getPointN(i+1),p)) return true;

        return false;
    }

    void PointLocator::updateLocation(Location l)
    {
        if (l == BOUNDARY && loc == BOUNDARY) loc = INTERIOR;
        else if (l == BOUNDARY || loc == BOUNDARY) loc = BOUNDARY;
        else loc = l < loc ? l : loc;
    }

    Location PointLocator::Point_Point(const Point* p, const Geometry* geom)
    {
        return (p->equals(geom) ? INTERIOR: EXTERIOR);
    }

    Location PointLocator::Point_LineString(const Point* p, const Geometry* geom)
    {
        const LineString* line = dynamic_cast<const LineString*>(geom);
        if (!line->isRing())
        {
            if (p->equals(line->getStartPoint()) || p->equals(line->getEndPoint())) return BOUNDARY;
        }
        if (PointOnLine(p,line)) return INTERIOR;
        else return EXTERIOR;
    }

    Location PointLocator::Point_Polygon(const Point* p, const Geometry* geom)
    {
        const Polygon* poly = dynamic_cast<const Polygon*>(geom);
        if (PointOnLine(p,poly->getExteriorRing())) return BOUNDARY;
        //    Repeat for holes
        for (int i = 0; i < poly->getNumInteriorRing(); i++)
            if (PointOnLine(p,poly->getInteriorRingN(i))) return BOUNDARY;

        return PointInPolygon(p,poly) ? INTERIOR : EXTERIOR;
    }

    Location PointLocator::Point_Geometry(const Point* p, const Geometry* geom)
    {
        if (geom->isEmpty()) return EXTERIOR;

        switch(geom->getWKBGeometryType(false,false))
        {
        case (wkbPoint):
            return Point_Point(p,geom);
        case (wkbLineString):
            return Point_LineString(p,geom);
        case (wkbPolygon):
        case (wkbTriangle):
            return Point_Polygon(p,geom);
        case (wkbMultiPoint):
        case (wkbMultiLineString):
        case (wkbMultiPolygon):
        case (wkbGeometryCollection):
            return Point_Multi(p,geom);
        case (wkbPolyhedralSurface):
        case (wkbTIN):
            return Point_PolySurface(p,geom);
        default:
            return UNKNOWN;
        }
    }

    Location PointLocator::Point_Multi(const Point* p, const Geometry* geom)
    {
        const GeometryCollection* col = dynamic_cast<const GeometryCollection*>(geom);
        for (int i = 1; i < col->getNumGeometries() + 1; i++)
            updateLocation(Point_Geometry(p,col->getGeometryN(i)));
        return loc;
    }

    Location PointLocator::Point_PolySurface(const Point* p, const Geometry* geom)
    {
    //Treat like a MultiPolygon
        const PolyhedralSurface* surface = dynamic_cast<const PolyhedralSurface*>(geom);
        for (int i = 0; i < surface->getNumPatches(); i++)
            updateLocation(Point_Polygon(p,surface->getPatchN(i)));
        return loc;
    }

    bool PointLocator::intersects(const Point* p, const Geometry* geom)
    {
        Location result = apply(p,geom);
        if (result == BOUNDARY || result == INTERIOR) return true;
        else return false;
    }

    Location PointLocator::apply(const Point* p, const Geometry* geom)
    {
        PointLocator ptLoc;
        return ptLoc.Point_Geometry(p,geom);
    }

}