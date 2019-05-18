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
#include "sfa/PointMath.h"
#include "sfa/PointLineIntersector.h"

namespace sfa {

    bool Collinear(const Point& p1, const Point& p2, const Point& p)
    {
        int loc = PointLineIntersector::apply(p1, p2, p);
        return (loc != PointLineIntersector::LEFT &&
                loc != PointLineIntersector::RIGHT);
    }

    bool Collinear(const Point* p1, const Point* p2, const Point* p)
    {
        return Collinear(*p1,*p2,*p);
    }

    bool Collinear3D(const Point& p1, const Point& p2, const Point& p)
    {
        int loc = PointLineIntersector::apply3D(p1, p2, p);
        return (loc != PointLineIntersector::DISJOINT);
    /*
        if ((p.equals3D(&p1)) || (p.equals3D(&p2)))    return true;
        double d1, d2;
        //XY plane
        d1 = (p2.Y() - p.Y())*(p.X() - p1.X());
        d2 = (p.Y() - p1.Y())*(p2.X() - p.X());
        if (!(d1 < d2 + SFA_EPSILON && d1 > d2 - SFA_EPSILON)) return false;
        //YZ
        d1 = (p2.Y() - p.Y())*(p.Z() - p1.Z());
        d2 = (p.Y() - p1.Y())*(p2.Z() - p.Z());
        if (!(d1 < d2 + SFA_EPSILON && d1 > d2 - SFA_EPSILON)) return false;
        //ZX
        d1 = (p2.Z() - p.Z())*(p.X() - p1.X());
        d2 = (p.Z() - p1.Z())*(p2.X() - p.X());
        if (!(d1 < d2 + SFA_EPSILON && d1 > d2 - SFA_EPSILON)) return false;
        return true;
    */
    }

    bool Collinear3D(const Point* p1, const Point* p2, const Point* p)
    {
        return Collinear3D(*p1,*p2,*p);
    }

    bool Between(const Point& p1, const Point& p2, const Point& p)
    {
        bool xMax = p.X() > std::max<double>(p1.X(), p2.X()) + SFA_EPSILON;
        bool xMin = p.X() < std::min<double>(p1.X(), p2.X()) - SFA_EPSILON;
        bool yMax = p.Y() > std::max<double>(p1.Y(), p2.Y()) + SFA_EPSILON;
        bool yMin = p.Y() < std::min<double>(p1.Y(), p2.Y()) - SFA_EPSILON;

        return !( xMax || xMin || yMax || yMin);
    }

    bool Between(const Point* p1, const Point* p2, const Point* p)
    {
        return Between(*p1,*p2,*p);
    }

    bool Between3D(const Point& p1, const Point& p2, const Point& p)
    {
        return !(    p.X() > std::max<double>(p1.X(), p2.X()) || p.X() < std::min<double>(p1.X(), p2.X()) ||
                    p.Y() > std::max<double>(p1.Y(), p2.Y()) || p.Y() < std::min<double>(p1.Y(), p2.Y()) ||
                    p.Z() > std::max<double>(p1.Z(), p2.Z()) || p.Z() < std::min<double>(p1.Z(), p2.Z())
                    );
    }

    bool Between3D(const Point* p1, const Point* p2, const Point* p)
    {
        return Between3D(*p1,*p2,*p);
    }

    bool CollinearAndExclusiveBetween(const Point& p1, const Point& p2, const Point& p)
    {
        int loc = PointLineIntersector::apply(p1, p2, p);
        return loc == PointLineIntersector::BETWEEN;
    }

    bool CollinearAndExclusiveBetween(const Point* p1, const Point* p2, const Point* p)
    {
        return CollinearAndExclusiveBetween(*p1,*p2,*p);
    }

    bool CollinearAndExclusiveBetween3D(const Point& p1, const Point& p2, const Point& p)
    {
        int loc = PointLineIntersector::apply3D(p1, p2, p);
        return loc == PointLineIntersector::BETWEEN;
    }

    bool CollinearAndExclusiveBetween3D(const Point* p1, const Point* p2, const Point* p)
    {
        return CollinearAndExclusiveBetween3D(*p1,*p2,*p);
    }

    int CrossProduct(const Point& p1, const Point& p2, const Point& p3, const Point& p4)
    {
        Point p = p4 + (p1 - p3);
        if ((p2 - p1).length2() > (p - p1).length2())
        {
            int loc = PointLineIntersector::apply(p1, p2, p);
            if (loc == PointLineIntersector::LEFT)
                return 1;
            if (loc == PointLineIntersector::RIGHT)
                return -1;
        }
        else
        {
            int loc = PointLineIntersector::apply(p1, p, p2);
            if (loc == PointLineIntersector::LEFT)
                return -1;
            if (loc == PointLineIntersector::RIGHT)
                return 1;
        }
        return 0;
    }

    int CrossProduct(const Point* p1, const Point* p2, const Point* p3, const Point* p4)
    {
        return CrossProduct(*p1,*p2,*p3,*p4);
    }

}