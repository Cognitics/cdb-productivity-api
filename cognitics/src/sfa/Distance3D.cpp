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
#include "sfa/Distance3D.h"
#include "sfa/Projection2D.h"

namespace sfa {

    double Distance3D::comparablePointPoint(const Point* a, const Point* b)
    {
        double d[3] = { a->X() - b->X() , a->Y() - b->Y() , a->Z() - b->Z() };
        return (d[0]*d[0] + d[1]*d[1] + d[2]*d[2]);
    }

    double Distance3D::comparablePointLine(const Point* a, const Point* p1, const Point* p2)
    {
        double x21 = p2->X() - p1->X();
        double x01 = a->X() - p1->X();
        double y21 = p2->Y() - p1->Y();
        double y01 = a->Y() - p1->Y();
        double z21 = p2->Z() - p1->Z();
        double z01 = a->Z() - p1->Z();

        double tn = x21*x01 + y21*y01 + z21*z01;
        double td = x21*x21 + y21*y21 + z21*z21;
        
    //    Negative t
        if (td < 0 != tn < 0) return x01*x01 + y01*y01 + z01*z01;
    //    Less than 1
        if (td > tn)
        {
            double t = tn/td;
            double x = a->X() - (p1->X() + t*x21);
            double y = a->Y() - (p1->Y() + t*y21);
            double z = a->Z() - (p1->Z() + t*z21);
            return x*x + y*y + z*z;
        }
        else return comparablePointPoint(a,p2);
    }

    double Distance3D::comparableLineLine(const Point* a, const Point* b, const Point* c, const Point* d)
    {
        double u[3] = { b->X() - a->X() , b->Y() - a->Y() , b->Z() - a->Z() };
        double v[3] = { d->X() - c->X() , d->Y() - c->Y() , d->Z() - c->Z() };
        double w[3] = { a->X() - c->X() , a->Y() - c->Y() , a->Z() - c->Z() };

        double da = u[0]*u[0] + u[1]*u[1] + u[2]*u[2];
        double db = u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
        double dc = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
        double dd = u[0]*w[0] + u[1]*w[1] + u[2]*w[2];
        double de = v[0]*w[0] + v[1]*w[1] + v[2]*w[2];

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
            double f = (-dd + db);
            if (f < 0)            sN = 0;
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

        double dP[3] = { w[0] + (sc*u[0]) - (tc*v[0]) , w[1] + (sc*u[1]) - (tc*v[1]) , w[2] + (sc*u[2]) - (tc*v[2]) };
        return dP[0]*dP[0] + dP[1]*dP[1] + dP[2]*dP[2];
    }

    double Distance3D::comparablePointPolygon(const Point* a, const Polygon* polygon)
    {
        Projection2D proj(polygon);

        Polygon* poly_t = proj.transformPolygonTo2D(polygon);
        Point* p_t = proj.transformPointTo2D(a);

        Distance op;
        double distance = op.comparablePointPolygon(p_t,poly_t) + p_t->Z()*p_t->Z();
        delete poly_t;
        delete p_t;
        return distance;
    }

}