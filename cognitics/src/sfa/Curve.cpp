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
#include "sfa/Curve.h"
#include "sfa/LineString.h"
#include "sfa/MultiPoint.h"
#include "sfa/MultiLineString.h"

#include "sfa/PointMath.h"

namespace sfa
{
    Curve::~Curve(void)
    {
        clearPoints();
    }

    Curve::Curve(void)
    {
    }

    Curve::Curve(const Curve& curve)
    {
        setCoordinateSystem(curve.getCoordinateSystem());
        points.resize(curve.points.size());
        for(size_t i=0,ic=curve.points.size();i<ic;i++)
            points[i] = (new Point(*curve.points.at(i)));
    }

    Curve::Curve(const Curve* curve)
    {
        setCoordinateSystem(curve->getCoordinateSystem());
        points.resize(curve->points.size());
        for(size_t i=0,ic=curve->points.size();i<ic;i++)
            points[i] = (new Point(*curve->points.at(i)));
    }

    Curve& Curve::operator=(const Curve& rhs)
    {
        for (PointList::iterator it = points.begin(), end = points.end(); it != end; it++)
            delete (*it);
        points.clear();

        setCoordinateSystem(rhs.getCoordinateSystem());
        points.resize(rhs.points.size());
        for(size_t i=0,ic=rhs.points.size();i<ic;i++)
            points[i] = (new Point(*rhs.points.at(i)));
        return *this;
    }

    void Curve::clearPoints(void)
    {
        for (PointList::iterator it = points.begin(), end = points.end(); it != end; it++)
            delete (*it);
        points.clear();
    }

    void Curve::addPoint(Point* point)
    {
        points.push_back(point);
    }

    void Curve::addPoint(const Point& point)
    {
        addPoint(new Point(point));
    }

    void Curve::insertPoint(int pos, Point* point)
    {
        points.insert(points.begin() + pos, point);
    }

    void Curve::insertPoint(int pos, const Point& point)
    {
        insertPoint(pos,new Point(point));
    }



    void Curve::removePoint(Point* point)
    {
        for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
        {
            if(*it == point)
            {
                delete (*it);
                it = points.erase(it);
                --it;
            }
        }
    }

    void Curve::removePoint(int pos)
    {
        if (pos >= 0 && pos < int(points.size()))
            delete points[pos];
        points.erase(points.begin() + pos);
    }

    void Curve::reverse(void)
    {
        std::reverse(points.begin(),points.end());
    }

    void Curve::setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform)
    {
        if(mathTransform)
        {
            for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
                (*it)->setCoordinateSystem(coordinateSystem, mathTransform);
        }
        Geometry::setCoordinateSystem(coordinateSystem, mathTransform);
    }

    int Curve::getDimension(void) const
    {
        return 1;
    }

    Geometry* Curve::getEnvelope(void) const
    {
        if(isEmpty())
            return NULL;

        double minX, maxX, minY, maxY, minZ, maxZ, minM, maxM;
        minX = maxX = minY = maxY = minZ = maxZ = minM = maxM = 0;
        Point* startPoint = getStartPoint();

        minX = maxX = startPoint->X();
        minY = maxY = startPoint->Y();
        minZ = maxZ = startPoint->Z();
        minM = maxM = startPoint->M();

        for(PointList::const_iterator it = points.begin(), end = points.end(); it != end; ++it)
        {
            Point* point = *it;

            if(point->X() < minX)
                minX = point->X();
            if(point->X() > maxX)
                maxX = point->X();
            if(point->Y() < minY)
                minY = point->Y();
            if(point->Y() > maxY)
                maxY = point->Y();
            if(point->is3D())
            {
                if(point->Z() < minZ)
                    minZ = point->Z();
                if(point->Z() > maxZ)
                    maxZ = point->Z();
            }
            if(point->isMeasured())
            {
                if(point->M() < minM)
                    minM = point->M();
                if(point->M() > maxM)
                    maxM = point->M();
            }
        }

        Point* minPoint = new Point(minX, minY);
        Point* maxPoint = new Point(maxX, maxY);
        if(is3D())
        {
            minPoint->setZ(minZ);
            maxPoint->setZ(maxZ);
        }
        if(isMeasured())
        {
            minPoint->setM(minM);
            maxPoint->setM(maxM);
        }

        LineString* envelope = new LineString;
        envelope->addPoint(minPoint);
        envelope->addPoint(maxPoint);
        return envelope;
    }

    bool Curve::isEmpty(void) const
    {
        return points.empty();
    }

    bool Curve::isSimple(void) const
    {
        //a simple curve is not self intersecting, test all the edges against all other edges
        for (int i = 0; i < int(points.size()-1); i++)
        {
            for (int j = 0; j < int(points.size()-1); j++)
            {
                Point* p1 = points[i];
                Point* p2 = points[i+1];
                Point* p3 = points[j];
                Point* p4 = points[j+1];

                //    Don't bother with trivial intersections
                if (i==j || i==j+1 || i+1==j) continue;
                else if (isClosed())
                {
                    if (i==0 && j==(points.size()-2)) continue;
                    if (j==0 && i==(points.size()-2)) continue;
                }

                bool Bp1 = Between(p3,p4,p1);
                bool Bp3 = Between(p1,p2,p3);
                bool Bp2 = Between(p3,p4,p2);
                bool Bp4 = Between(p1,p2,p4);

                 //if none of the points are between each other, there is no intersection
                if(!(Bp1 || Bp2 || Bp3 || Bp4)) continue;

                double denom = (p1->X()-p2->X())*(p3->Y()-p4->Y()) 
                                    - (p1->Y()-p2->Y())*(p3->X()-p4->X());

                //Segments are parallel
                if (denom == 0.0)
                {
                    if (Collinear(p1,p2,p3)) return false; 
                }

                double a = p1->X()*p2->Y() - p1->Y()*p2->X();
                double b = p3->X()*p4->Y() - p3->Y()*p4->X();
                double x = (a*(p3->X()-p4->X()) - b*(p1->X()-p2->X()))/denom;
                double y = (a*(p3->Y()-p4->Y()) - b*(p1->Y()-p2->Y()))/denom;

                Point test(x,y);

                //test if intersection found is on both segments
                if (Between(p1,p2,&test) && Between(p3,p4,&test)) return false;
            }
        }

        return true;
    }

    bool Curve::is3D(void) const
    {
        for(PointList::const_iterator it = points.begin(), end = points.end(); it != end; ++it)
        {
            if((*it)->is3D())
                return true;
        }
        return false;
    }

    bool Curve::isMeasured(void) const
    {
        for(PointList::const_iterator it = points.begin(), end = points.end(); it != end; ++it)
        {
            if((*it)->isMeasured())
                return true;
        }
        return false;
    }

    Geometry* Curve::getBoundary(void) const
    {
        if(isClosed())
            return NULL;
        MultiPoint* boundary = new MultiPoint();
        boundary->addGeometry(new Point(getStartPoint()));
        boundary->addGeometry(new Point(getEndPoint()));
        return boundary;
    }

    Geometry* Curve::locateBetween(double mStart, double mEnd) const
    {
        if (!isMeasured())
            return new MultiPoint;

        PointList pointCollection;
        LineStringList lineCollection;

        bool open = false;
        LineString* temp = NULL;

        for (int i = 0; i < int(points.size() - 1); i++)
        {
            Point* start = points[i];
            Point* stop = points[i+1];

            if ( (start->M() <= mEnd && start->M() >= mStart) && !(stop->M() <= mEnd && stop->M() >= mStart) )        //Range of m crosses only start point
            {
                double dm = stop->M() - start->M();
                double dx = stop->X() - start->X();
                double dy = stop->Y() - start->Y();
                double dz = stop->Z() - start->Z();

                double mValue = (mEnd >= start->M()) && (mEnd < stop->M()) ? mEnd : mStart;
                double dist = (mValue - start->M())/dm;

                if (dist < SFA_EPSILON)
                {
                    if (open)
                    {
                        temp->addPoint(new Point(start));
                        open = false;
                    }
                    else
                        pointCollection.push_back(new Point(start));
                }
                else
                {
                    Point* interpolatedPoint = new Point(start->X() + dist*dx, start->Y() + dist*dy);
                    if (start->is3D()) interpolatedPoint->setZ(start->Z() + dist*dz);
                    interpolatedPoint->setM(mValue);

                    if (!open) temp = new LineString;
                    temp->addPoint(new Point(start));
                    temp->addPoint(interpolatedPoint);

                    lineCollection.push_back(temp);
                    open = false;
                }
            }
            else if ( (start->M() > mEnd && stop->M() < mStart) || (start->M() < mStart && stop->M() > mEnd)    )    //range of m crosses in the middle
            {
                //interpolate the segment, linear is assumed for this
                double dm = stop->M() - start->M();
                double dx = stop->X() - start->X();
                double dy = stop->Y() - start->Y();
                double dz = stop->Z() - start->Z();
                double startDist = (mStart - start->M())/dm;    //always between 0 and 1
                double stopDist = (mEnd - start->M())/dm;        //always between 0 and 1

                Point interpolation1;
                Point interpolation2;
                if (start->is3D())
                {
                    interpolation1 = Point(start->X() + dx*startDist, start->Y() + dy*startDist, start->Z() + dz*startDist, mStart);
                    interpolation2 = Point(start->X() + dx*stopDist, start->Y() + dy*stopDist, start->Z() + dz*stopDist, mEnd);
                }
                else
                {
                    interpolation1 = Point(start->X() + dx*startDist, start->Y() + dy*startDist);
                    interpolation1.setM(mStart);
                    interpolation2 = Point(start->X() + dx*stopDist, start->Y() + dy*stopDist);
                    interpolation2.setM(mEnd);
                }

                if (interpolation1.equals(&interpolation2))
                    pointCollection.push_back(new Point(interpolation1));
                else
                {
                    temp = new LineString;
                    temp->addPoint(interpolation1);
                    temp->addPoint(interpolation2);
                    lineCollection.push_back(temp);
                }
            }
            else if ( !(start->M() <= mEnd && start->M() >= mStart) && (stop->M() <= mEnd && stop->M() >= mStart))    //range of m crosses only the stop point
            {
                double dm = stop->M() - start->M();
                double dx = stop->X() - start->X();
                double dy = stop->Y() - start->Y();
                double dz = stop->Z() - start->Z();

                double mValue = (mEnd > start->M()) && (mEnd <= stop->M()) ? mEnd : mStart;
                double dist = (mValue - start->M())/dm;

                Point* interpolatedPoint = new Point(start->X() + dist*dx,start->Y() + dist*dy);
                if (start->is3D()) interpolatedPoint->setZ(start->Z() + dist*dz);
                interpolatedPoint->setM(mValue);

                temp = new LineString;
                temp->addPoint(interpolatedPoint);
                if (!interpolatedPoint->equals(stop)) temp->addPoint(new Point(stop));
                open = true;

                if (i == int(points.size() - 2))
                {
                    if (temp->getNumPoints() == 1) 
                    {
                        pointCollection.push_back(new Point(stop));
                        delete temp;
                    }
                    else lineCollection.push_back(temp);
                }
            }
            else if ( (start->M() <= mEnd && stop->M() >= mStart) || (start->M() >= mStart && stop->M() <= mEnd) )    //range of m crosses the entire segment
            {
                if (open)
                {
                    temp->addPoint(new Point(stop));
                }
                else
                {
                    open = true;
                    temp = new LineString;
                    temp->addPoint(new Point(start));
                    temp->addPoint(new Point(stop));
                }

                if (i == int(points.size() - 2)) lineCollection.push_back(temp);
            }
        }

        if (lineCollection.empty() && pointCollection.empty())
            return new MultiPoint;

        GeometryCollection* result = new GeometryCollection;
        for (LineStringList::iterator it = lineCollection.begin(), end = lineCollection.end(); it != end; it++)
            result->addGeometry(*it);
        for (PointList::iterator it = pointCollection.begin(), end = pointCollection.end(); it != end; it++)
            result->addGeometry(*it);

        return result;
    }


    double Curve::getLength(void) const
    {
    //    Should be overloaded in the derived class using that classes interpolation. ie LineString overloaded using linear interpolation
        return 0;
    }


    Point* Curve::getStartPoint(void) const
    {
        return points.empty() ? NULL : points.front();
    }

    Point* Curve::getEndPoint(void) const
    {
        return points.empty() ? NULL : points.back();
    }

    bool Curve::isClosed(void) const
    {
        return (getStartPoint() == NULL) ? false : getStartPoint()->equals(getEndPoint());
    }

    bool Curve::isRing(void) const
    {
        return isSimple() && isClosed();
    }



}