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
#include "sfa/SegmentIntersector.h"    

namespace sfa {

    //private
    bool SegmentIntersector::handleParallel(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result)
    {
        if (!Collinear(p1,p2,p3)) return false; 
        //parallel and non collinear case

        //between flags
        bool Bp1 = Between(p3,p4,p1);
        bool Bp3 = Between(p1,p2,p3);
        bool Bp2 = Between(p3,p4,p2);
        bool Bp4 = Between(p1,p2,p4);

         //if none of the points are between each other, there is no intersection
        if(!(Bp1 || Bp2 || Bp3 || Bp4)) return false;

        //flags for if any of the points are equal to the other end points
        bool p1p3 = p1->equals(p3);
        bool p1p4 = p1->equals(p4);
        bool p2p3 = p2->equals(p3);
        bool p2p4 = p2->equals(p4);

        //result might be a point, if they intersect at the end points, but only one endpoint
        if ( !Bp2 && ((p1p3 && !Bp4) || (p1p4 && !Bp3)) )
        {
            result = new Point(p1);
            return true;
        } 
        else if ( !Bp1 && ((p2p3 && !Bp4) || (p2p4 && !Bp3)) )
        {
            result = new Point(p2);
            return true;
        }

        
        std::vector<const Point*> points;

        if (Bp1) points.push_back(p1);
        if (Bp3 && !p1p3) points.push_back(p3);
        if (Bp2 && !p2p3) points.push_back(p2);
        if (Bp4) points.push_back(p4);
        if (points.size() == 0)
        {
            return false;
        }
        else if (points.size() == 1)
        {
            result = new Point(points.at(0));
            return true;
        }
        //result is a segment
        result = new LineString;
        static_cast<LineString*>(result)->addPoint(new Point(points[0]));
        static_cast<LineString*>(result)->getPointN(0)->clearZ();
        static_cast<LineString*>(result)->addPoint(new Point(points[1]));
        static_cast<LineString*>(result)->getPointN(1)->clearZ();
        return true;
    }

    bool SegmentIntersector::handleParallel3D(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry* &result)
    {
        if (!Collinear3D(p1,p2,p3)) return false; 
        //parallel and non collinear case

        //between flags
        bool Bp1 = Between3D(p3,p4,p1);
        bool Bp3 = Between3D(p1,p2,p3);
        bool Bp2 = Between3D(p3,p4,p2);
        bool Bp4 = Between3D(p1,p2,p4);

         //if none of the points are between each other, there is no intersection
        if(!(Bp1 || Bp2 || Bp3 || Bp4)) return false;

        //flags for if any of the points are equal to the other end points
        bool p1p3 = p1->equals3D(p3);
        bool p1p4 = p1->equals3D(p4);
        bool p2p3 = p2->equals3D(p3);
        bool p2p4 = p2->equals3D(p4);

        //result might be a point, if they intersect at the end points, but only one endpoint
        if ( !Bp2 && ((p1p3 && !Bp4) || (p1p4 && !Bp3)) )
        {
            result = new Point(p1);
            return true;
        } 
        else if ( !Bp1 && ((p2p3 && !Bp4) || (p2p4 && !Bp3)) )
        {
            result = new Point(p2);
            return true;
        }

        //result is a segment
        result = new LineString;
        std::vector<const Point*> points;

        if (Bp1) points.push_back(p1);
        if (Bp3 && !p1p3) points.push_back(p3);
        if (Bp2 && !p2p3) points.push_back(p2);
        if (Bp4) points.push_back(p4);

        static_cast<LineString*>(result)->addPoint(new Point(points[0]));
        static_cast<LineString*>(result)->addPoint(new Point(points[1]));
        return true;
    }

    //public
    bool SegmentIntersector::Intersects(const Point* p1, const Point* p2, const Point* p3, const Point* p4)
    {
        Point u = *p2 - *p1;
        Point v = *p4 - *p3;
        Point w = *p1 - *p3;

        double denom = v.X()*u.Y() - v.Y()*u.X();

        if (abs(denom) < SFA_EPSILON)
        {
            Geometry* temp = NULL;
            bool result = handleParallel(p1,p2,p3,p4,temp);
            if (temp) delete temp;
            return result;
        }

        double s = (v.Y()*w.X() - v.X()*w.Y())/denom;
        if (s < -SFA_EPSILON || s > 1+SFA_EPSILON) return false;
        double t = (u.Y()*w.X() - u.X()*w.Y())/denom;
        if (t < -SFA_EPSILON || t > 1+SFA_EPSILON) return false;

        return true;
    }

    bool SegmentIntersector::Intersects3D(const Point* p1, const Point* p2, const Point* p3, const Point* p4)
    {
        //check if the lines are parallel
        double dx1 = p2->X() - p1->X();
        double dy1 = p2->Y() - p1->Y();
        double dz1 = p2->Z() - p1->Z();
        double dx2 = p4->X() - p3->X();
        double dy2 = p4->Y() - p3->Y();
        double dz2 = p4->Z() - p3->Z();
        //if (dx1,dy1,dz1) is a multiple of (dx2,dy2,dz2) then they are parallel
        if (    (dx1 == dx2 && dy1 == dy2 && dz1 == dz2) 
                ||
                ( (dx1 != 0) && (dy1*(dx2/dx1) == dy2 && dz1*(dx2/dx1) == dz2) )
                ||
                ( (dy1 != 0) && (dx1*(dy2/dy1) == dx2 && dz1*(dy2/dy1) == dz2) )
                ||
                (dx1*(dz2/dz1) == dx2 && dy1*(dz2/dz1) == dy2) )
        {
            if (!Collinear3D(p1,p2,p3)) return false; 
            //parallel and non collinear case

            //between flags
            bool Bp1 = Between3D(p3,p4,p1);
            bool Bp3 = Between3D(p1,p2,p3);
            bool Bp2 = Between3D(p3,p4,p2);
            bool Bp4 = Between3D(p1,p2,p4);

             //if none of the points are between each other, there is no intersection
            if(!(Bp1 || Bp2 || Bp3 || Bp4)) return false;
            return true;
        }

        double denom;
        double t1;

        denom = dy1*dx2 - dx1*dy2;
        if (denom == 0.0)
        {
            //test which axis to use to resolve for denom and t1
            if ((dx1+dx2) == 0) //use yz plane
            {
                denom = dz1*dy2 - dy1*dz2;
                t1 = ((p3->Z() - p1->Z())*dy2 - (p3->Y() - p1->Y())*dz2)/denom;
            }
            else //use zx plane
            {
                denom = dx1*dz2 - dz1*dx2;
                t1 = ((p3->X() - p1->X())*dz2 - (p3->Z() - p1->Z())*dx2)/denom;
            }
        }
        else
        {
            t1 = ((p3->Y() - p1->Y())*dx2 - (p3->X() - p1->X())*dy2)/denom;
        }

        double x = p1->X() + dx1*t1;
        double y = p1->Y() + dy1*t1;

        //test Z
        double z1 = p1->Z() + dz1*t1;
        //find t2
        double t2;

        if        (dx2 != 0)    t2 = (x - p3->X())/dx2;
        else if (dy2 != 0)    t2 = (y - p3->Y())/dy2;
        else                t2 = (z1 - p3->Z())/dz2;

        double z2 = p3->Z() + (p4->Z() - p3->Z())*t2;

        if (z1 != z2) return false;
    
        Point test(x,y,z1);

        if (Between3D(p1,p2,&test) && Between3D(p3,p4,&test))
            return true;
        else return false;
    }

    //public
    bool SegmentIntersector::Intersection(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result)
    {
        Point u = *p2 - *p1;
        Point v = *p4 - *p3;
        Point w = *p1 - *p3;

        double denom = v.X()*u.Y() - v.Y()*u.X();

        if (abs(denom) < SFA_EPSILON)
            return handleParallel(p1,p2,p3,p4,result);

        double s = (v.Y()*w.X() - v.X()*w.Y())/denom;
        if (s < -SFA_EPSILON || s > 1+SFA_EPSILON) return false;
        double t = (u.Y()*w.X() - u.X()*w.Y())/denom;
        if (t < -SFA_EPSILON || t > 1+SFA_EPSILON) return false;

        result = new Point( *p1 + u*s );
        return true;
    }

    bool SegmentIntersector::Intersection3D(const Point* p1, const Point* p2, const Point* p3, const Point* p4, Geometry*& result)
    {
        //check if the lines are parallel
        double dx1 = p2->X() - p1->X();
        double dy1 = p2->Y() - p1->Y();
        double dz1 = p2->Z() - p1->Z();
        double dx2 = p4->X() - p3->X();
        double dy2 = p4->Y() - p3->Y();
        double dz2 = p4->Z() - p3->Z();
        //if (dx1,dy1,dz1) is a multiple of (dx2,dy2,dz2) then they are parallel
        if (dx1 == dx2 && dy1 == dy2 && dz1 == dz2) 
        {
            return handleParallel(p1,p2,p3,p4,result);
        }
        else if (dx1 != 0)
        {
            if (dy1*(dx2/dx1) == dy2 && dz1*(dx2/dx1) == dz2) return handleParallel(p1,p2,p3,p4,result);
        }
        else if (dy1 != 0)
        {
            if (dx1*(dy2/dy1) == dx2 && dz1*(dy2/dy1) == dz2) return handleParallel(p1,p2,p3,p4,result);
        }
        else
        {
            if (dx1*(dz2/dz1) == dx2 && dy1*(dz2/dz1) == dy2) return handleParallel(p1,p2,p3,p4,result);
        }


        double denom;
        double t1;

        denom = dy1*dx2 - dx1*dy2;
        if (denom == 0.0)
        {
            //test which axis to use to resolve for denom and t1
            if ((dx1+dx2) == 0) //use yz plane
            {
                denom = dz1*dy2 - dy1*dz2;
                t1 = ((p3->Z() - p1->Z())*dy2 - (p3->Y() - p1->Y())*dz2)/denom;
            }
            else //use zx plane
            {
                denom = dx1*dz2 - dz1*dx2;
                t1 = ((p3->X() - p1->X())*dz2 - (p3->Z() - p1->Z())*dx2)/denom;
            }
        }
        else
        {
            t1 = ((p3->Y() - p1->Y())*dx2 - (p3->X() - p1->X())*dy2)/denom;
        }

        double x = p1->X() + dx1*t1;
        double y = p1->Y() + dy1*t1;

        //test Z
        double z1 = p1->Z() + dz1*t1;
        //find t2
        double t2;

        if        (dx2 != 0)    t2 = (x - p3->X())/dx2;
        else if (dy2 != 0)    t2 = (y - p3->Y())/dy2;
        else                t2 = (z1 - p3->Z())/dz2;

        double z2 = p3->Z() + (p4->Z() - p3->Z())*t2;

        if (z1 != z2) return false;
    
        Point* test = new Point(x,y,z1);
        if (z1 == 0) test->clearZ();

        if (Between3D(p1,p2,test) && Between3D(p3,p4,test))
        {
            result = test;
            return true;
        }
        delete test;
        return false;
    }

}