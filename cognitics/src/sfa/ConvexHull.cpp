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
#include "sfa/ConvexHull.h"
#include "sfa/Point.h"
#include "sfa/Polygon.h"
#include "sfa/PointMath.h"
#include "sfa/RingMath.h"
#include <float.h>
#include <cmath>
#include <boost/foreach.hpp>
namespace sfa {

//MELKMAN ========================================================================================
    int MelkmanHull::isLeft(const Point& p, const Point& p1, const Point& p2)
    {
        double a = (p1.X() - p.X())*(p2.Y() - p.Y()) - (p2.X() - p.X())*(p1.Y() - p.Y());
        if (a < -SFA_EPSILON) return -1;
        else if (a > SFA_EPSILON) return 1;
        else return 0;
    }

    Geometry* MelkmanHull::apply(const LineString* a)
    {
        //check if the linestring is long enough
        if (a->getNumPoints() < 3)
            new LineString(a);

        std::vector<Point> points;
        for (int i = 0; i < a->getNumPoints(); i++)
        {
            points.push_back(*a->getPointN(i));
        }

        //remove any redundant points
        if (a->isClosed()) points.pop_back();

        //create the dequeue
        Point* D = new Point[2*points.size() + 1];
        int bottom = int(points.size()) - 2;
        int top = bottom + 3;

        //special case, the first three points are collinear
        //in that case keep moving down
        int first = 0;
        int second = 1;
        int third = 2;

        while(third < int(points.size()))
        {
            if (Collinear(points[first],points[second],points[third]))
            {
                second++;
                third++;
            }
            else break;
        }
        if (third >= int(points.size()))
            return new LineString(a);

        //set up the first three points so that they are in CCW order
        if (isLeft(points[first], points[second], points[third]) > 0)
        {
            D[bottom+1] = points[first];
            D[bottom+2] = points[second];
        }
        else
        {
            D[bottom+1] = points[second];
            D[bottom+2] = points[first];
        }
        D[bottom] = points[third];
        D[top] = points[third];

        for (int i = third++; i < int(points.size()); i++)
        {
            // test if next vertex is inside the deque hull
            if ((isLeft(D[bottom], D[bottom+1], points[i]) > 0) &&
                (isLeft(D[top-1], D[top], points[i]) > 0) )
                    continue;

            // Find the rightmost vertex to connect to
            while (isLeft(D[bottom], D[bottom+1], points[i]) <= 0)
                ++bottom;

            D[--bottom] = points[i];    // insert V[i] at bot of deque

            // find the leftmost vertex to connect to
            while (isLeft(D[top-1], D[top], points[i]) <= 0)
                --top;

            D[++top] = points[i];        // push V[i] onto top of deque
        }

        LineString* result = new LineString;
        for (int i = 0; i <= (top-bottom); i++)
        {
            D[bottom + i].setZ(0);
            result->addPoint(D[bottom + i]);
        }
        if (result->isEmpty()) 
        {
            delete result;
            return NULL;
        }
        else
        {
            Polygon* returnValue = new Polygon;
            returnValue->addRing(result);
            return returnValue;
        }
    }

//GRAHAM =========================================================================================

    void GrahamHull::findLowest(void)
    {
        int n = 0;
        for (int i = 0; i < num; i++)
        {
            if ( (P[n]->p.Y() > P[i]->p.Y()) || (P[n]->p.Y() == P[i]->p.Y() && P[n]->p.X() > P[i]->p.X()) )
                n = i;
        }

        if (n != 0) swap(0,n);
    }

    int GrahamHull::comparePoints(GrahamPoint* a, GrahamPoint* b)
    {
        int comp = CrossProduct(P[0]->p,a->p,P[0]->p,b->p);
        if (comp > 0) return -1;
        else if (comp < 0) return 1;
        else
        {
            double x = abs(a->p.X() - P[0]->p.X()) - abs(b->p.X() - P[0]->p.X());
            double y = abs(a->p.Y() - P[0]->p.Y()) - abs(b->p.Y() - P[0]->p.Y());

            if (x<0 || y<0)
            {
                a->toDelete = true;
                return -1;
            }
            else if (x>0 || y>0)
            {
                b->toDelete = true;
                return 1;
            }
            else
            {
                return 0;
            }
        }
    }

    void GrahamHull::swap(int i, int j)
    {
        GrahamPoint* temp = P[i];
        P[i] = P[j];
        P[j] = temp;
    }

    void GrahamHull::quickSort(int left, int right)
    {
        int i = left, j = right;
        GrahamPoint* pivot = P[(left + right) / 2];

        while (i <= j) {
            while (comparePoints(P[i],pivot) < 0)
                i++;
            while (comparePoints(P[j],pivot) > 0)
                j--;
            if (i <= j) {
                swap(i,j);
                i++;
                j--;
            }
        };

        if (left < j)
            quickSort(left, j);
        if (i < right)
            quickSort(i, right);
    }

    void GrahamHull::sort(void)
    {
        quickSort(1,num-1);
    }

    void GrahamHull::compress(void)
    {
    /*    This method doesn't actually delete anything, it just pushes the "deleted" points to the end of 
        the vector and ignores them for the rest of the computation.
    */
        int j = 0;
        int i = 0;
        int nextGroup = 0;
        
        //pre compress first group to ensure that the first point is not deleted
        for (i = 1; i < num; i++)
        {
            if (P[0]->p.equals(&P[i]->p)) nextGroup++;
            else break;
        }
        j = 1;

        //compress the rest of the array
        for (i = nextGroup+1; i < num; i++)
        {
            bool deleteGroup = false;
            nextGroup = i;

            //Find the end of this duplicate group
            for (int k = i+1; k < num; k++)
            {
                if (P[k]->p.equals(&P[i]->p))
                {
                    if (P[k]->toDelete) deleteGroup = true;
                    nextGroup++;
                }
                else break;
            }

            if (!P[i]->toDelete && !deleteGroup)
            {
                P[j] = P[i];
                j++;
            }
            i = nextGroup;
        }

        num = j;
    }

    void GrahamHull::compute(void)
    {
        if (num == 0)
        {
            return;
        }
        else if (num == 1)
        {
            hull.push_back(_P[0].p);
        }
        else if (num == 2)
        {
            hull.push_back(_P[0].p);
            hull.push_back(_P[1].p);
        }
        else if (num == 3)
        {
            hull.push_back(_P[0].p);
            hull.push_back(_P[1].p);
            hull.push_back(_P[2].p);
            hull.push_back(_P[0].p);

            if (    (hull[1].X() - hull[0].X())*(hull[2].Y() - hull[0].Y()) <
                    (hull[2].X() - hull[0].X())*(hull[1].Y() - hull[0].Y()) )
                    std::reverse(hull.begin(),hull.end());
        }
        else
        {
            findLowest();
            sort();
            compress();

        /*    
            Quick test to ensure there is a hull..if there are only 2 points in the queue, then that means
            the rest were deleted because of collinearity, meaning the result is simple a line between the two
            points.
        */
            if (num == 2)
            {
                hull.push_back(P[0]->p);
                hull.push_back(P[1]->p);
                hull.push_back(hull.front());
                return;
            }

        //    Create Resulting Hull
            hull.push_back(P[0]->p);
            hull.push_back(P[1]->p);
            hull.push_back(P[2]->p);

            int i = 3;

            while (i < num)
            {
                Point p1 = hull[hull.size() - 2];
                Point p2 = hull.back();
                if (CrossProduct(p1,p2,p1,P[i]->p) > 0)
                {
                    hull.push_back(P[i]->p);
                    i++;
                }
                else
                {
                    hull.pop_back();
                }

            }
        
        //    Close hull
            hull.push_back(hull.front());
        }

    //    cleanup
        _P.clear();
        P.clear();
        num = int(hull.size());
    }
    
    GrahamHull::GrahamHull(const std::vector<Point>& points)
    {
        // First snap all points to a grid using SFA_EPSILON
        // This Graham Hull algorithm falls apart when points are too 
        // close together because the left/right detection fails.
        // There is a bug in this code that causes both points to be deleted
        // in some cases where two points are within SFA_EPSILON distance.
        // For now this is an effective workaround to that problem
        const double epsilon = SFA_EPSILON;
        std::set<sfa::Point> grid;//All unique points here
        BOOST_FOREACH(sfa::Point pt,points)
        {
            double closest_dist = DBL_MAX;
            BOOST_FOREACH(const sfa::Point &gridpt,grid)
            {
                closest_dist = std::min<double>(closest_dist,gridpt.distance(&pt));
            }
            // If pt is not within epsilon distance from any grid point, insert pt into the grid
            if(closest_dist>epsilon)
            {
                grid.insert(pt);
            }
        }
        std::vector<sfa::Point> snappedPoints;
        snappedPoints.insert(snappedPoints.begin(),grid.begin(),grid.end());

        num = int(snappedPoints.size());
        _P.reserve(num);
        for (int j = 0; j < num; j++)
        {
            _P.push_back(GrahamPoint(snappedPoints[j]));
            _P.back().p.setZ(0);
            P.push_back(&_P.back());
        }

        compute();
    }

    GrahamHull::GrahamHull(const MultiPoint* a)
    {
        num = a->getNumGeometries();
        _P.reserve(num);
        for (int j = 0; j < num; j++)
        {
            _P.push_back(GrahamPoint(*static_cast<Point*>(a->getGeometryN(j+1))));
            _P.back().p.setZ(0);
            P.push_back(&_P.back());
        }

        compute();
    }

    Geometry* GrahamHull::getHullGeometry(void)
    {
        if (num == 0)
            return NULL;
        else if (num == 1)
            return new Point(hull[0]);
        else
        {
            LineString* line = new LineString;
            for (std::vector<Point>::iterator it = hull.begin(), end = hull.end(); it != end; it++)
                line->addPoint(*it);
            if (num == 2)
                return line;
            else
            {
                Polygon* polygon = new Polygon;
                polygon->addRing(line);
                return polygon;
            }
        }
    }

    std::vector<Point> GrahamHull::getHullPoints(void)
    {
        return hull;
    }

    Geometry* GrahamHull::apply(const MultiPoint* a)
    {
        GrahamHull hull(a);
        return hull.getHullGeometry();
    }

    std::vector<Point> GrahamHull::apply(const std::vector<Point>& points)
    {
        GrahamHull hull(points);
        return hull.getHullPoints();
    }

}
