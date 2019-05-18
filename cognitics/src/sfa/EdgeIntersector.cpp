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
#include "sfa/EdgeIntersector.h"
#include "sfa/PointMath.h"

namespace sfa {

    bool EdgeIntersector::isTrivial(EdgeNode* a, int ai, EdgeNode* b, int bi)
    {
        if (a != b) return false;
        else if (ai==bi) return true;
        else if (ai == bi+1 || ai == bi-1) return true;
        else if (a->getStartPoint() == a->getEndPoint())
        {
            int size = a->getNumPoints();
            if (ai==0 && bi==size-2) return true;
            else if (bi==0 && ai==size-2) return true;
            else return false;
        }
        else return false;
    }

    bool EdgeIntersector::isDisjoint(const Point* p1, const Point* p2, const Point* p3, const Point* p4)
    {
        double Amin[2] = { std::min<double>(p1->X(),p2->X()), std::min<double>(p1->Y(),p2->Y()) };
        double Amax[2] = { std::max<double>(p1->X(),p2->X()), std::max<double>(p1->Y(),p2->Y()) };
        double Bmin[2] = { std::min<double>(p3->X(),p4->X()), std::min<double>(p3->Y(),p4->Y()) };
        double Bmax[2] = { std::max<double>(p3->X(),p4->X()), std::max<double>(p3->Y(),p4->Y()) };

        if (Amin[0] > Bmax[0] + SFA_EPSILON) return true;
        else if (Amin[1] > Bmax[1] + SFA_EPSILON) return true;
        else if (Amax[0] < Bmin[0] - SFA_EPSILON) return true;
        else if (Amax[1] < Bmin[1] - SFA_EPSILON) return true;
        else return false;
    }

    void EdgeIntersector::addIntersection(EdgeNode* e1, int ai, EdgeNode* e2, int bi)
    {
        if (isTrivial(e1,ai,e2,bi)) return;

        const Point* p1 = e1->getPointN(ai)->getPoint();
        const Point* p2 = e1->getPointN(ai+1)->getPoint();
        const Point* p3 = e2->getPointN(bi)->getPoint();
        const Point* p4 = e2->getPointN(bi+1)->getPoint();

        if(isDisjoint(p1,p2,p3,p4)) return;

        double dx1 = p2->X() - p1->X();
        double dy1 = p2->Y() - p1->Y();
        double dx2 = p4->X() - p3->X();
        double dy2 = p4->Y() - p3->Y();

        double denom = dx2*dy1 - dx1*dy2;

        //    Parallel case
        if (denom < SFA_EPSILON && denom > -SFA_EPSILON)
        {
            if (!Collinear(p1,p2,p3)) return; 

            //flags for if any of the points are equal to the other end points
            bool p1p3 = p1->equals(p3);
            bool p1p4 = p1->equals(p4);
            bool p2p3 = p2->equals(p3);
            bool p2p4 = p2->equals(p4);

            // finish finding between flags
            bool Bp1 = Between(p3,p4,p1);
            bool Bp2 = Between(p3,p4,p2);
            bool Bp3 = Between(p1,p2,p3);
            bool Bp4 = Between(p1,p2,p4);

             //if none of the points are between each other, there is no intersection, we already checked for this
            //if(!(Bp1 || Bp2 || Bp3 || Bp4)) return;

            e1->setIsolated(false);
            e2->setIsolated(false);

            int intCount = 0;

            //    Find intersection near p1
            if (p1p3)
            {
                e1->addIntersection(ai,0);
                e2->addIntersection(bi,0);
                intCount++;
            }
            if (p1p4)
            {
                e1->addIntersection(ai,0);
                e2->addIntersection(bi+1,0);
                intCount++;
            }
            if (p2p3)
            {
                e1->addIntersection(ai+1,0);
                e2->addIntersection(bi,0);
                intCount++;
            }
            if (p2p4)
            {
                e1->addIntersection(ai+1,0);
                e2->addIntersection(bi+1,0);
                intCount++;
            }

            if (Bp1 && !(p1p3||p1p4))
            {
                e1->addIntersection(ai,0);
                e2->addIntersection(bi,1);
                intCount++;
            }
            if (Bp2 && !(p2p3||p2p4))
            {
                e1->addIntersection(ai+1,0);
                e2->addIntersection(bi,1);
                intCount++;
            }
            if (Bp3 && !(p1p3||p2p3))
            {
                e1->addIntersection(ai,1);
                e2->addIntersection(bi,0);
                intCount++;
            }
            if (Bp4 && !(p1p4||p2p4))
            {
                e1->addIntersection(ai,1);
                e2->addIntersection(bi,0);
                intCount++;
            }

        //    Full parallel line, update the labels of the edges on both sides for only this geometry. This will allow
        //    detection of colapse edges, or redundant internal edges, but don't copy location on the edges
            if (intCount > 1 && e1->getGeomArg() == e2->getGeomArg())
            {
                int arg = e1->getGeomArg();
                Location left = (e1->getLeft(arg) == INTERIOR || e2->getLeft(arg) == INTERIOR ? INTERIOR : EXTERIOR);
                Location right = (e1->getRight(arg) == INTERIOR || e2->getRight(arg) == INTERIOR ? INTERIOR : EXTERIOR);
                e1->setLeft(arg,left);
                e2->setLeft(arg,left);
                e1->setRight(arg,right);
                e2->setRight(arg,right);
            }

            return;
        }
        else
        {
            double x31 = p3->X() - p1->X();
            double y31 = p3->Y() - p1->Y();
            double s = (dx2*y31 - x31*dy2)/denom;
            double t = (dx1*y31 - x31*dy1)/denom;

            double x = p1->X() + dx1*s;
            double y = p1->Y() + dy1*s;

            sfa::Point test(p1->X() + dx1*s, p1->Y() + dy1*s);
            if (test.equals(p1))
                s = 0;
            else if (test.equals(p2))
            {
                ai++;
                s = 0;
            }
            if (test.equals(p3))
                t = 0;
            else if (test.equals(p4))
            {
                bi++;
                t = 0;
            }

            if (s < 0 || s > 1 || t < 0 || t > 1)
                return;

            e1->addIntersection(ai,s);
            e2->addIntersection(bi,t);
        }
    }
     
/*
    Brute force for now, will make SL later
*/
    EdgeIntersector::EdgeIntersector(EdgeNodeList A, EdgeNodeList B, bool selfTest)
    {
        for (int ea = 0; ea < int(A.size()); ea++)
        {
            for (int eb = (selfTest ? ea : 0); eb < int(B.size()); eb++)
            {
                for (int i = 0; i < (A[ea])->getNumPoints() - 1; i++)
                {
                    for (int j = 0; j < (B[eb])->getNumPoints() - 1; j++)
                    {
                        addIntersection(A[ea],i,B[eb],j);
                    }
                }
            }
        }
    }

}