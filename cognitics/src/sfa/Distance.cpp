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
#include "sfa/Distance.h"
#include "sfa/RingMath.h"
#include <cmath>
#include <cfloat>

namespace sfa {

    double Distance::comparablePointPoint(const Point* a, const Point* b)
    {
        double dx = a->X() - b->X();
        double dy = a->Y() - b->Y();

        return (dx*dx + dy*dy);
    }

    double Distance::comparablePointLine(const Point* a, const Point* p1, const Point* p2)
    {
        double x21 = p2->X() - p1->X();
        double x01 = a->X() - p1->X();
        double y21 = p2->Y() - p1->Y();
        double y01 = a->Y() - p1->Y();

        double tn = x21*x01 + y21*y01;
        double td = x21*x21 + y21*y21;
        
    //    Negative t
        if (td < 0 != tn < 0) return x01*x01 + y01*y01;
    //    Less than 1
        if (td > tn)
        {
            double t = tn/td;
            double x = a->X() - (p1->X() + t*x21);
            double y = a->Y() - (p1->Y() + t*y21);
            return x*x + y*y;
        }
        else return comparablePointPoint(a,p2);
    }

    double Distance::comparableLineLine(const Point* a, const Point* b, const Point* c, const Point* d)
    {
        double u[2] = { b->X() - a->X() , b->Y() - a->Y() };
        double v[2] = { d->X() - c->X() , d->Y() - c->Y() };
        double w[2] = { a->X() - c->X() , a->Y() - c->Y() };

        double da = u[0]*u[0] + u[1]*u[1];
        double db = u[0]*v[0] + u[1]*v[1];
        double dc = v[0]*v[0] + v[1]*v[1];
        double dd = u[0]*w[0] + u[1]*w[1];
        double de = v[0]*w[0] + v[1]*w[1];

        double D = da*dc - db*db;
        double sc, sN, sD = D;
        double tc, tN, tD = D;

    //    Find the points to use for comparison and handle any cases that could result in division by 0.
        if ( D < SFA_EPSILON )    //    Segments are nearly parallel, use a
        {
            sN = 0;
            sD = 1;
            tN = de;
            tD = dc;
        }
        else
        {
            sN = db*de - dc*dd;
            tN = da*de - db*dd;
            if ( sN < 0 )        //    Nearest point is before a_b so use a
            {
                sN = 0;
                tN = de;
                tD = dc;
            }
            else if (sN > sD )    //    Nearest point is after a_b so use b
            {
                sN = sD;
                tN = de + db;
                tD = dc;
            }
        }

    //    Account for the fact that the closest points may not be actually on the line segment and correct for that.
        if (tN < 0.0)                    //    Point is before c_d so use c
        {
            tN = 0.0;
            if (-dd < 0) sN = 0;
            else if (-dd > da) sN = sD;
            else
            {
                sN = -dd;
                sD = da;
            }
        }
        else if (tN > tD)                //    Point is after c_d so use d
        {
            tN = tD;
            double f = (-dd -+ db);
            if (f < 0)        sN = 0;
            else if (f > da)    sN = sD;
            else
            {
                sN = f;
                sD = da;
            }
        }

    //    find the final distances and calculate the vector comparable distance between them.
        sc = abs(sN) < SFA_EPSILON ? 0 : sN / sD;
        tc = abs(tN) < SFA_EPSILON ? 0 : tN / tD;

        double dP[2] = { w[0] + (sc*u[0]) - (tc*v[0]) , w[1] + (sc*u[1]) - (tc*v[1]) };
        return dP[0]*dP[0] + dP[1]*dP[1];
    }

    double Distance::comparablePointLineString(const Point* a, const LineString* other)
    {
        double dist = DBL_MAX;
        if(other == NULL)
            return dist;
        for (int i = 0; i < other->getNumPoints() - 1; i++)
        {
            dist = std::min<double>(dist,comparablePointLine(
                a,
                other->getPointN(i),
                other->getPointN(i+1)));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparablePointPolygon(const Point* a, const Polygon* other)
    {
        if (PointInPolygon(a,other)) return 0;
        double dist = comparablePointLineString(a, other->getExteriorRing() );
        for (int i = 0; i < other->getNumInteriorRing(); i++)
        {
            dist = std::min<double>(dist,comparablePointLineString(a, other->getInteriorRingN(i) ));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparablePointPolyhedralSurface(const Point* a, const PolyhedralSurface* other)
    {
        double dist = DBL_MAX;
        for (int i = 0; i < other->getNumPatches(); i++)
        {
            dist = std::min<double>(dist,comparablePointPolygon(a, other->getPatchN(i) ));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparablePointCollection(const Point* a, const GeometryCollection* other)
    {
        double dist = DBL_MAX;
        for (int i = 1; i < other->getNumGeometries() + 1; i++)
        {
            Geometry* next = other->getGeometryN(i);
            dist = std::min<double>(dist,comparablePoint(a,next));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparablePoint(const Point* a, const Geometry* b)
    {
        switch(b->getWKBGeometryType(false,false))
        {
        case wkbPoint: return comparablePointPoint(a, static_cast<const Point*>(b) );
        case wkbLineString: return comparablePointLineString(a, static_cast<const LineString*>(b) );
        case wkbTriangle:
        case wkbPolygon: 
            return comparablePointPolygon(a, static_cast<const Polygon*>(b) );
        case wkbPolyhedralSurface: 
        case wkbTIN:
            return comparablePointPolyhedralSurface(a, static_cast<const PolyhedralSurface*>(b) );
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            return comparablePointCollection(a, static_cast<const GeometryCollection*>(b) ); 
        default: return DBL_MAX;
        }
    }

    double Distance::comparableLineLineString(const Point* a, const Point* b, const LineString* other)
    {
        double dist = DBL_MAX;
        for (int i = 0; i < other->getNumPoints() - 1; i++)
        {
            dist = std::min<double>(dist,comparableLineLine(a,b,other->getPointN(i),other->getPointN(i+1)));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparableLinePolygon(const Point* a, const Point* b, const Polygon* other)
    {
        if (a->intersects(other)) return 0;
        if (b->intersects(other)) return 0;
        double dist = comparableLineLineString(a,b,other->getExteriorRing());
        for (int i = 0; i < other->getNumInteriorRing(); i++)
        {
            dist = std::min<double>(dist,comparableLineLineString(a,b,other->getInteriorRingN(i)));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparableLinePolyhedralSurface(const Point* a, const Point* b, const PolyhedralSurface* other)
    {
        double dist = DBL_MAX;
        for (int i = 0; i < other->getNumPatches(); i++)
        {
            dist = std::min<double>(dist,comparableLinePolygon(a,b,other->getPatchN(i)));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparableLineCollection(const Point* a, const Point* b, const GeometryCollection* other)
    {
        double dist = DBL_MAX;
        for (int i = 1; i < other->getNumGeometries() + 1; i++)
        {
            Geometry* next = other->getGeometryN(i);
            dist = std::min<double>(dist,comparableLine(a,b, next ));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparableLine(const Point* a, const Point* b, const Geometry* other)
    {
        switch(other->getWKBGeometryType(false,false))
        {
        case wkbPoint: return comparablePointLine(static_cast<const Point*>(other),a,b);
        case wkbLineString: return comparableLineLineString(a, b, static_cast<const LineString*>(other));
        case wkbTriangle:
        case wkbPolygon: 
            return comparableLinePolygon(a, b, static_cast<const Polygon*>(other));
        case wkbPolyhedralSurface: 
        case wkbTIN:
            return comparableLinePolyhedralSurface(a, b, static_cast<const PolyhedralSurface*>(other));
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            return comparableLineCollection(a, b, static_cast<const GeometryCollection*>(other));
        default: return DBL_MAX;
        }
    }

    double Distance::comparableLineString(const LineString* line, const Geometry* other)
    {
        double dist = DBL_MAX;
        if(!line)
            return dist;
        for (int i = 0; i < line->getNumPoints() - 1; i++)
        {
            dist = std::min<double>(dist,comparableLine(line->getPointN(i),line->getPointN(i+1),other));
            if (dist <= SFA_EPSILON) return 0;
        }
        return dist;
    }

    double Distance::comparablePolygon(const Polygon* polygon, const Geometry* other)
    {
        int type = other->getWKBGeometryType(false,false);
        if (type == wkbPolygon || type == wkbTriangle)
        {
            double dist = comparableLineString(polygon->getExteriorRing(),other);
            for (int i = 0; i < polygon->getNumInteriorRing(); i++)
            {
                dist = std::min<double>(dist,comparableLineString(polygon->getInteriorRingN(i),other));
            }
            return dist;
        }
        else
        {
            double dist = apply(other,polygon);
            return dist*dist;
        }
    }

    double Distance::comparablePolyhedralSurface(const PolyhedralSurface* surface, const Geometry* other)
    {
        double dist = DBL_MAX;
        for (int i = 0; i < surface->getNumPatches(); i++)
        {
            dist = std::min<double>(dist,comparablePolygon(surface->getPatchN(i),other));
        }
        return dist;
    }

    double Distance::comparableCollection(const GeometryCollection* collection, const Geometry* other)
    {
        double dist = DBL_MAX;
        for (int i = 1; i < collection->getNumGeometries() + 1; i++)
        {
            Geometry* next = collection->getGeometryN(i);
            double comp = DBL_MAX;
            switch(next->getWKBGeometryType(false,false))
            {
            case wkbPoint: comp = comparablePoint(static_cast<const Point*>(next),other); break;
            case wkbLineString: comp = comparableLineString(static_cast<const LineString*>(next),other); break;
            case wkbTriangle:
            case wkbPolygon: 
                comp = comparablePolygon(static_cast<const Polygon*>(next),other); break;
            case wkbPolyhedralSurface: 
            case wkbTIN:
                comp = comparablePolyhedralSurface(static_cast<const PolyhedralSurface*>(next),other); break;
            default: break;
            }

            dist = std::min<double>(comp,dist);
        }
        return dist;
    }
    
    double Distance::apply(const Geometry* a, const Geometry* b)
    {
        if (a==b) return 0;        //    Shortcut to see if the pointers are pointing to the same geometry

        double comp = DBL_MAX;
        switch(a->getWKBGeometryType(false,false))
        {
        case wkbPoint: comp = comparablePoint(static_cast<const Point*>(a),b); break;
        case wkbLineString: comp = comparableLineString(static_cast<const LineString*>(a),b); break;
        case wkbTriangle:
        case wkbPolygon: 
            comp = comparablePolygon(static_cast<const Polygon*>(a),b); break;
        case wkbPolyhedralSurface: 
        case wkbTIN:
            comp = comparablePolyhedralSurface(static_cast<const PolyhedralSurface*>(a),b); break;
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            comp = comparableCollection(static_cast<const GeometryCollection*>(a),b); break;
        default: break;
        }

        return sqrt(comp);
    }

}