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
#include "sfa/RingMath.h"
#include "sfa/Projection2D.h"
#include <cmath>

namespace sfa {

/****************************************************************************
    Area Algorithms
****************************************************************************/

    double GetRingArea(const LineString* ring)
    {
        if(!ring || !ring->isClosed()) return 0;
        double area = 0;
        for (int i = 0; i < ring->getNumPoints() - 1; i++)
            area += (ring->getPointN(i)->X() + ring->getPointN(i+1)->X())*(ring->getPointN(i)->Y() - ring->getPointN(i+1)->Y());
        return -area*0.5;
    }

    double GetPolygonArea(const Polygon* polygon)
    {
        double area = GetRingArea(polygon->getExteriorRing());
        for (int i = 0; i < polygon->getNumInteriorRing(); i++)
            area += GetRingArea(polygon->getInteriorRingN(i));
        return area;
    }

/****************************************************************************
    Centroid Algorithms
****************************************************************************/

    Point GetRingCentroid(const LineString* ring)
    {
        double x = 0;
        double y = 0;

        for (int i = 0; i < ring->getNumPoints() - 1; i++)
        {
            double b = ring->getPointN(i)->X()*ring->getPointN(i+1)->Y() - ring->getPointN(i)->Y()*ring->getPointN(i+1)->X();
            x += (ring->getPointN(i)->X() + ring->getPointN(i+1)->X())*b;
            y += (ring->getPointN(i)->Y() + ring->getPointN(i+1)->Y())*b;
        }

        double area = GetRingArea(ring);
        x /= 6*area;
        y /= 6*area;

        return Point(x,y);
    }

    Point GetPolygonCentroid(const Polygon* polygon)
    {
        double    x = 0;
        double    y = 0;

    //    Add contribution of exterior ring
        LineString* ring = polygon->getExteriorRing();

        for (int i = 0; i < ring->getNumPoints() - 1; i++)
        {
            double b = ring->getPointN(i)->X()*ring->getPointN(i+1)->Y() - ring->getPointN(i)->Y()*ring->getPointN(i+1)->X();
            x += (ring->getPointN(i)->X() + ring->getPointN(i+1)->X())*b;
            y += (ring->getPointN(i)->Y() + ring->getPointN(i+1)->Y())*b;
        }

    //    Add contribution of interior rings
        for (int j = 0; j < polygon->getNumInteriorRing(); j++)
        {
            ring = polygon->getInteriorRingN(j);
            for (int i = 0; i < ring->getNumPoints() - 1; i++)
            {
                double b = ring->getPointN(i)->X()*ring->getPointN(i+1)->Y() - ring->getPointN(i)->Y()*ring->getPointN(i+1)->X();
                x += (ring->getPointN(i)->X() + ring->getPointN(i+1)->X())*b;
                y += (ring->getPointN(i)->Y() + ring->getPointN(i+1)->Y())*b;
            }
        }

    //    Factor in area
        double area = GetPolygonArea(polygon);
        x /= 6*area;
        y /= 6*area;

        return Point(x,y);
    }


/****************************************************************************
    Orientation Algorithms
****************************************************************************/

    Orientation ComputeOrientation(const LineString& ring)
    {
        double area = GetRingArea(&ring);
        if (area < -SFA_EPSILON) return ORIENTATION_CW;
        else if (area > SFA_EPSILON) return ORIENTATION_C_CW;
        else return ORIENTATION_UNKNOWN;
    }

    Orientation ComputeOrientation(const LineString* ring)
    {
        return ComputeOrientation(*ring);
    }

    Orientation ComputeOrientation(const Polygon& polygon)
    {
        return ComputeOrientation(polygon.getExteriorRing());
    }

    Orientation ComputeOrientation(const Polygon* polygon)
    {
        return ComputeOrientation(polygon->getExteriorRing());
    }

    bool IsPolygonConsistent(const Polygon& polygon)
    {
        Orientation outer = ComputeOrientation(polygon);
        for (int i = 0; i < polygon.getNumInteriorRing(); i++)
            if (ComputeOrientation(polygon.getInteriorRingN(i)) == outer) return false;
        return true;
    }

    bool IsPolygonConsistent(const Polygon* polygon)
    {
        return IsPolygonConsistent(*polygon);
    }

    void CorrectPolygon(Polygon& polygon)
    {
        if (polygon.isEmpty()) return;
        if (ComputeOrientation(polygon) != ORIENTATION_C_CW)
            polygon.getExteriorRing()->reverse();
        for (int i = 0; i < polygon.getNumInteriorRing(); i++)
        {
            LineString* temp = polygon.getInteriorRingN(i);
            if (ComputeOrientation(temp) != ORIENTATION_CW) temp->reverse();
        }
    }

    void CorrectPolygon(Polygon* polygon)
    {
        CorrectPolygon(*polygon);
    }

    void Correct3DPolygon(Polygon& polygon)
    {
        if (polygon.isEmpty()) return;
        Projection2D proj(&polygon);
        Polygon* temp = proj.transformPolygonTo2D(&polygon);
        if (ComputeOrientation(temp) != ORIENTATION_C_CW) polygon.getExteriorRing()->reverse();
        for (int i = 1; i < polygon.getNumInteriorRing(); i++)
        {
            if (ComputeOrientation(temp->getInteriorRingN(i)) != ORIENTATION_CW) polygon.getInteriorRingN(i)->reverse();
        }
    }

    void Correct3DPolygon(Polygon* polygon)
    {
        Correct3DPolygon(*polygon);
    }

/****************************************************************************
    Point Location Algorithms
****************************************************************************/

    bool UpdateCount(const Point& point, const LineString& ring, int& Rcross, int& Lcross)
    {
        for (int i = 1; i < ring.getNumPoints(); i++)
        {
            Point p1 = *(ring.getPointN(i-1));
            Point p2 = *(ring.getPointN(i));

            if (point.equals(&p1)) return true;

            double    height = point.Y();
            double    x;
            double    Rstrad;
            double    Lstrad;

            //Rstrad is false if the point only touches ray, but is mostly below it                
            //Lstrad is false if the point only touches ray, but is mostly above it
            Rstrad = ( p2.Y() > height - SFA_EPSILON ) != ( p1.Y() > height - SFA_EPSILON );                
            Lstrad = ( p2.Y() < height + SFA_EPSILON ) != ( p1.Y() < height + SFA_EPSILON );

            if (Rstrad || Lstrad)
            {
                x = ( (p2.X()-point.X())*(p1.Y()-height) - (p1.X()-point.X())*(p2.Y()-height) ) 
                    / ( p1.Y() - p2.Y() );
                if ( Rstrad && x > SFA_EPSILON ) Rcross++;
                if ( Lstrad && x < -SFA_EPSILON ) Lcross++;
                if (abs(x) < SFA_EPSILON)
                    return true;
            }
        }

        return false;
    }

    bool PointInRing(const Point* point, const LineString* ring)
    {
        return PointInRing(*point,*ring);
    }

    bool PointInRing(const Point& point, const LineString& ring)
    {
        int Rcross = 0;
        int Lcross = 0;

        if (UpdateCount(point,ring,Rcross,Lcross)) return true;

    //check cross countings to determine intersection
        if ( (Rcross % 2) != (Lcross % 2) )        return true;
        else if ( (Rcross % 2) == 1 )            return true;
        else                                    return false;
    }

    bool PointInPolygon(const Point* point, const Polygon* polygon)
    {
        return PointInPolygon(*point, *polygon);
    }

    bool PointInPolygon(const Point& point, const Polygon& polygon)
    {
        int Rcross = 0;
        int Lcross = 0;
        
        if(polygon.getExteriorRing() == NULL)
            return false;

        if (UpdateCount(point,*(polygon.getExteriorRing()),Rcross,Lcross)) return true;

        for (int j = 0; j < polygon.getNumInteriorRing(); j++)
        {
            if (UpdateCount(point,*(polygon.getInteriorRingN(j)),Rcross,Lcross)) return true;
        }

    //check cross countings to determine intersection
        if ( (Rcross % 2) != (Lcross % 2) )        return true;
        else if ( (Rcross % 2) == 1 )            return true;
        else                                    return false;
    }

}