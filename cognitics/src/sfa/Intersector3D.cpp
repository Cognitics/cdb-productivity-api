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
#include "sfa/Intersector3D.h"
#include "sfa/Point.h"
#include "sfa/LineString.h"
#include "sfa/Polygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/GeometryCollection.h"

//algorithms
#include "sfa/Projection2D.h"
#include "sfa/RingMath.h"
#include "sfa/SegmentIntersector.h"
#include "sfa/EnvelopeCheck.h"

namespace sfa {

    bool Intersector3D::Point_Point(const Geometry* a, const Geometry* b)
    {
        return a->equals3D(b);
    }

    bool Intersector3D::Point_LineString(const Geometry* a, const Geometry* b)
    {
        const Point* p = dynamic_cast<const Point*>(a);
        const LineString* l = dynamic_cast<const LineString*>(b);

        if (p->equals3D(l->getStartPoint()) || p->equals3D(l->getEndPoint())) return true;
        for (int i = 0; i < l->getNumPoints() - 1; i++)
        {
            Point* p1 = l->getPointN(i);
            Point* p2 = l->getPointN(i+1);
            if (Collinear3D(p1,p2,p))
                if (Between3D(p1,p2,p)) return true;
        }
        return false;
    }

    bool Intersector3D::Point_Polygon(const Geometry* a, const Geometry* b)
    {
        if (!EnvelopeCheck::apply3D(a,b)) return false;
        else
        {
            Projection2D proj(dynamic_cast<const Polygon*>(b));
            Point* point = dynamic_cast<Point*>(proj.transformGeometryTo2D(a));
            Polygon* polygon = dynamic_cast<Polygon*>(proj.transformGeometryTo2D(b));

            bool result = false;

            if (point->is3D()) result = false;
            else result = PointInPolygon(point,polygon);

            delete point;
            delete polygon;
            return result;
        }
    }

    bool Intersector3D::LineString_LineString(const Geometry* a, const Geometry* b)
    {
        if (!EnvelopeCheck::apply3D(a,b)) return false;

        const LineString* l1 = dynamic_cast<const LineString*>(a);
        const LineString* l2 = dynamic_cast<const LineString*>(b);

        for (int i = 0; i < (l1->getNumPoints()-1); i++ )
        {
            for (int j = 0; j < (l2->getNumPoints()-1); j++ )
            {
                if (SegmentIntersector::Intersects3D(l1->getPointN(i),l1->getPointN(i+1),l2->getPointN(j),l2->getPointN(j+1)))
                    return true;
            }
        }
        return false;
    }

    bool Intersector3D::LineString_Polygon(const Geometry* a, const Geometry* b)
    {
        if (!EnvelopeCheck::apply3D(a,b)) return false;

        const LineString* l = dynamic_cast<const LineString*>(a);
        const Polygon* other = dynamic_cast<const Polygon*>(b);

        //    Test Exterior Ring
        if (LineString_LineString(l,other->getExteriorRing())) return true;

        //    Test Interior Rings
        for (int i = 0; i < other->getNumInteriorRing(); i++)
        {
            if (LineString_LineString(l,other->getInteriorRingN(i))) return true;
        }

        //    Check if LineString is within the polygon
        return Point_Polygon(l->getPointN(0),other);
    }

    bool Intersector3D::Polygon_Polygon(const Geometry* a, const Geometry* b)
    {
        if (!EnvelopeCheck::apply3D(a,b)) return false;

        const Polygon* A = dynamic_cast<const Polygon*>(a);
        const Polygon* B = dynamic_cast<const Polygon*>(b);

        //    Create lists of rings
        LineStringList Alist;
        LineStringList Blist;

        Alist.push_back(A->getExteriorRing());
        Blist.push_back(B->getExteriorRing());

        for (int i = 0; i < A->getNumInteriorRing(); i++)
        {
            Alist.push_back(A->getInteriorRingN(i));
        }
        for (int i = 0; i < B->getNumInteriorRing(); i++)
        {
            Blist.push_back(B->getInteriorRingN(i));
        }

        //    Test rings for intersection
        for (LineStringList::iterator Ait = Alist.begin(); Ait != Alist.end(); ++Ait)
        {
            for (LineStringList::iterator Bit = Blist.begin(); Bit != Blist.end(); ++Bit)
            {
                for (int h = 0; h < (*Ait)->getNumPoints()-1; h++)
                {
                    for (int k = 0; k < (*Bit)->getNumPoints()-1; k++)
                    {
                        if (SegmentIntersector::Intersects3D((*Ait)->getPointN(h),(*Ait)->getPointN(h+1),
                        (*Bit)->getPointN(k),(*Bit)->getPointN(k+1)))
                        return true;
                    }
                }
            }
        }

        //    Test for within relation
        if (Point_Polygon(Alist.front()->getPointN(0),B)) return true;
        else if (Point_Polygon(Blist.front()->getPointN(0),A)) return true;
        else return false;
    }

    bool Intersector3D::Surface_Geometry(const Geometry* a, const Geometry* b)
    {
        const PolyhedralSurface* surface = dynamic_cast<const PolyhedralSurface*>(a);
        for (int i = 0; i < surface->getNumPatches(); i++)
        {
            if (apply(surface->getPatchN(i),b)) return true;
        }
        return false;
    }

    bool Intersector3D::Collection_Geometry(const Geometry* a, const Geometry* b)
    {
        const GeometryCollection* collection = dynamic_cast<const GeometryCollection*>(a);
        for (int i = 1; i < collection->getNumGeometries() + 1; i++)
        {
            if (apply(collection->getGeometryN(i),b)) return true;
        }
        return false;
    }

    bool Intersector3D::apply(const Geometry* a, const Geometry* b)
    {
        int typeA = a->getWKBGeometryType(false,false);
        int typeB = b->getWKBGeometryType(false,false);

        switch(typeA)
        {
        case (wkbPoint):
            switch(typeB)
            {
            case (wkbPoint):
                return Point_Point(a,b);
            case (wkbLineString):
                return Point_LineString(a,b);
            case (wkbPolygon):
            case (wkbTriangle):
                return Point_Polygon(a,b);
            case (wkbMultiPoint):
            case (wkbMultiLineString):
            case (wkbMultiPolygon):
            case (wkbGeometryCollection):
                return Collection_Geometry(b,a);
            case (wkbPolyhedralSurface):
            case (wkbTIN):
                return Surface_Geometry(b,a);
            default:
                break;
            }
            break;
        case (wkbLineString):
            switch(typeB)
            {
            case (wkbPoint):
                return Point_LineString(b,a);
            case (wkbLineString):
                return LineString_LineString(a,b);
            case (wkbPolygon):
            case (wkbTriangle):
                return LineString_Polygon(a,b);
            case (wkbMultiPoint):
            case (wkbMultiLineString):
            case (wkbMultiPolygon):
            case (wkbGeometryCollection):
                return Collection_Geometry(b,a);
            case (wkbPolyhedralSurface):
            case (wkbTIN):
                return Surface_Geometry(b,a);
            default:
                break;
            }
            break;
        case (wkbPolygon):
        case (wkbTriangle):
            switch(typeB)
            {
            case (wkbPoint):
                return Point_Polygon(b,a);
            case (wkbLineString):
                return LineString_Polygon(b,a);
            case (wkbPolygon):
            case (wkbTriangle):
                return Polygon_Polygon(a,b);
            case (wkbMultiPoint):
            case (wkbMultiLineString):
            case (wkbMultiPolygon):
            case (wkbGeometryCollection):
                return Collection_Geometry(b,a);
            case (wkbPolyhedralSurface):
            case (wkbTIN):
                return Surface_Geometry(b,a);
            default:
                break;
            }
            break;
        case (wkbMultiPoint):
        case (wkbMultiLineString):
        case (wkbMultiPolygon):
        case (wkbGeometryCollection):
            return Collection_Geometry(a,b);
        case (wkbPolyhedralSurface):
        case (wkbTIN):
            return Surface_Geometry(a,b);
        default:
            break;
        }

        //default to false return
        return false;
    }

}