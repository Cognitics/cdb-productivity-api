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

//#pragma optimize( "", off )

#include <float.h>
#include "sfa/LineString.h"

#include "sfa/ConvexHull.h"
#include "sfa/ConvexHull3D.h"
#include "sfa/Distance3D.h"

#include <sstream>
#include <exception>
#include <stdexcept>
#include <math.h>

namespace sfa
{
    LineString::~LineString(void)
    {
    }

    LineString::LineString(void)
    {
    }

    LineString::LineString(const LineString& linestring) : Curve(linestring)
    {
    }

    LineString::LineString(const LineString* linestring) : Curve(linestring)
    {
    }

    LineString& LineString::operator=(const LineString& rhs)
    {
        clearPoints();
        points.resize(rhs.points.size());
        for(size_t i=0,ic=rhs.points.size();i<ic;i++)
            points[i] = (new Point(*rhs.points.at(i)));
        return *this;
    }

    std::string LineString::getGeometryType(void) const
    {
        if(points.size() == 2)
            return "Line";
        if(isSimple() && isClosed()) return "LinearRing";
        // TODO: if closed and simple, return "LinearRing", but is it necessary to do simple every time?
        return "LineString";
    }
    
    int LineString::getNumPoints(void) const
    {
        return (int)points.size();
    }

    Point* LineString::getPointN(int n) const
    {
        if (n >= 0 && n < int(points.size()))
            return points[n];
        throw std::runtime_error("LineString::getPointN(): index out of bounds");
    }

    double LineString::getLength(void) const
    {
        double length = 0;
        for (int i = 0; i < int(points.size()) - 1; i++)
        {
            double v[3] = { points[i+1]->X() - points[i]->X() , points[i+1]->Y() - points[i]->Y() , points[i+1]->Z() - points[i]->Z() };
            length += sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
        }
        return length;
    }

    double LineString::getLength2D(void) const
    {
        double length = 0;
        for (int i = 0; i < int(points.size()) - 1; i++)
        {
            double v[2] = { points[i+1]->X() - points[i]->X() , points[i+1]->Y() - points[i]->Y() };
            length += sqrt(v[0]*v[0] + v[1]*v[1]);
        }
        return length;
    }

    Geometry* LineString::convexHull(void) const
    {
        return MelkmanHull::apply(this);
    }

    Geometry* LineString::convexHull3D(void) const
    {
    //    Must convert into a multiPoint
        MultiPoint mpoint;
        for (PointList::const_iterator it = points.begin(), end = points.end(); it != end; it++)
            mpoint.addGeometry(new Point(*it));
        return ConvexHull3D::apply(&mpoint);
    }

    bool LineString::isValid(void) const
    {
        return true;
    }

    WKBGeometryType LineString::getWKBGeometryType(bool withZ, bool withM) const
    {
        if (withZ && withM && is3D() && isMeasured())
            return wkbLineStringZM;
        if (withZ && is3D())
            return wkbLineStringZ;
        if (withM && isMeasured())
            return wkbLineStringM;
        return wkbLineString;
    }

    void LineString::toText(std::ostream &os, bool tag, bool withZ, bool withM) const
    {
        if(tag)
        {
            Geometry::toText(os, tag, withZ, withM);
            os << " ";
        }
        os << "(";
        if(isEmpty())
        {
            os << "EMPTY";
        }
        else
        {
            for(PointList::const_iterator it = points.begin(), end = points.end(); it != end; ++it)
            {
                if(it != points.begin())
                    os << ",";
                (*it)->toText(os, false, withZ, withM);
            }
        }
        os << ")";
    }

    void LineString::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
    //    Move into Point List
        is.ignore(1024,'(');

    //    Check for empty case
        if (is.peek() == 'E' || is.peek() == 'e')
        {
            is.ignore(1024,')'); // Move out of Point List
            return;
        }

    //    Parse Points, the check for ) will also remove the commas
        while (is.good())
        {
            Point* point = new Point;
            point->fromText(is,false,withZ,withM);
            addPoint(point);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }

    void LineString::toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const
    {
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> numPoints(ccl::uint32_t(points.size()));
            os << numPoints;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> numPoints(ccl::uint32_t(points.size()));
            os << numPoints;
        }
        for(PointList::const_iterator it = points.begin(), end = points.end(); it != end; ++it)
        {
            (*it)->toBinary(os, byteOrder, withZ, withM);
        }
    }

    void LineString::fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM)
    {
        ccl::uint32_t numPoints;
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> tempPoints;
            is >> tempPoints;
            numPoints = tempPoints;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> tempPoints;
            is >> tempPoints;
            numPoints = tempPoints;
        }
        for(ccl::uint32_t i = 0; i < numPoints; ++i)
        {
            Point* point = new Point();
            point->fromBinary(is, byteOrder, withZ, withM);
            addPoint(point);
        }
    }

    bool LineString::isParentOf(const GeometryBase *child) const
    {
        if(!child)
            return false;
        for(int i = 0, c = getNumPoints(); i < c; ++i)
        {
            if(getPointN(i) == child)
                return true;
        }
        return false;
    }

    GeometryBase *LineString::getParentOf(const GeometryBase *child) const
    {
        return isParentOf(child) ? const_cast<LineString *>(this) : NULL;
    }

    int LineString::getNumChildren(void) const
    {
        return getNumPoints();
    }

    bool LineString::interpolateXfromY(Point* p, int segment) const
    {
        if((getNumPoints() < 2) || (segment >= getNumPoints()))
            return false;
        Point* a = getPointN(segment);
        Point* b = getPointN(segment + 1);
        if(b->Y() - a->Y() == 0)
            return false;
        p->setX(a->X() + ((p->Y() - a->Y()) / (b->Y() - a->Y()) * (b->X() - a->X())));
        return true;
    }

    bool LineString::interpolateXfromZ(Point* p, int segment) const
    {
        if((getNumPoints() < 2) || (segment >= getNumPoints()))
            return false;
        Point* a = getPointN(segment);
        Point* b = getPointN(segment + 1);
        if(b->Z() - a->Z() == 0)
            return false;
        p->setX(a->X() + ((p->Z() - a->Z()) / (b->Z() - a->Z()) * (b->X() - a->X())));
        return true;
    }

    bool LineString::interpolateYfromX(Point* p, int segment) const
    {
        if((getNumPoints() < 2) || (segment >= getNumPoints()))
            return false;
        Point* a = getPointN(segment);
        Point* b = getPointN(segment + 1);
        if(b->X() - a->X() == 0)
            return false;
        p->setY(a->Y() + ((p->X() - a->X()) / (b->X() - a->X()) * (b->Y() - a->Y())));
        return true;
    }

    bool LineString::interpolateYfromZ(Point* p, int segment) const
    {
        if((getNumPoints() < 2) || (segment >= getNumPoints()))
            return false;
        Point* a = getPointN(segment);
        Point* b = getPointN(segment + 1);
        if(b->Z() - a->Z() == 0)
            return false;
        p->setY(a->Y() + ((p->Z() - a->Z()) / (b->Z() - a->Z()) * (b->Y() - a->Y())));
        return true;
    }

    bool LineString::interpolateZfromX(Point* p, int segment) const
    {
        if((getNumPoints() < 2) || (segment >= getNumPoints()))
            return false;
        Point* a = getPointN(segment);
        Point* b = getPointN(segment + 1);
        if(b->X() - a->X() == 0)
            return false;
        p->setZ(a->Z() + ((p->X() - a->X()) / (b->X() - a->X()) * (b->Z() - a->Z())));
        return true;
    }

    bool LineString::interpolateZfromY(Point* p, int segment) const
    {
        if((getNumPoints() < 2) || (segment >= getNumPoints()))
            return false;
        Point* a = getPointN(segment);
        Point* b = getPointN(segment + 1);
        if(b->Y() - a->Y() == 0)
            return false;
        p->setZ(a->Z() + ((p->Y() - a->Y()) / (b->Y() - a->Y()) * (b->Z() - a->Z())));
        return true;
    }

    bool LineString::interpolateX(Point *p, int segment) const
    {
        return interpolateXfromY(p, segment) || interpolateXfromZ(p, segment);
    }

    bool LineString::interpolateY(Point *p, int segment) const
    {
        return interpolateYfromX(p, segment) || interpolateYfromZ(p, segment);
    }

    bool LineString::interpolateZ(Point *p, int segment) const
    {
        return interpolateZfromX(p, segment) || interpolateZfromY(p, segment);
    }

    int LineString::addColinearPoints(double maxDist, double tolerance)
    {
        if(maxDist==0)//Sanity check to prevent memory overrun
            return 0;
        //get the total line length
        int num_points_to_add = (this->getLength()/maxDist)-getNumPoints();
        if(num_points_to_add > 10e8)
        {            
            return 0;
        }
        double softDist = maxDist + (maxDist*tolerance);
        double maxDist2 = softDist*softDist;//so we don't need to do sqrts
        sfa::PointList newPoints;
        size_t numPoints = points.size();
        for(size_t i=0;i<numPoints;i++)
        {
            sfa::Point *pt = points.at(i);
            newPoints.push_back(pt);
            if((i+1)<numPoints)
            {
                sfa::Point *nextPt = points.at(i+1);
                sfa::Point vec = *nextPt - *pt;
                while(vec.length2()>maxDist2)
                {
                    vec.normalize();
                    sfa::Point *newPt = new sfa::Point(*pt + (vec*maxDist));
                    newPoints.push_back(newPt);
                    pt = newPt;
                    vec = *nextPt - *pt;
                }
            }
        }
        int delta = int(newPoints.size() - points.size());
        points = newPoints;
        return delta;
    }

    int LineString::addDeviatedColinearPoints(double mean, double stddev, double mindist)
    {
        if((mean == 0) || points.empty())
            return 0;

        sfa::PointList newPoints;
        for(size_t i = 0, c = points.size(); i < c - 1; ++i)
        {
            sfa::Point *p1 = points.at(i);
            sfa::Point *p2 = points.at(i + 1);
            sfa::Point vec = *p2 - *p1;
            newPoints.push_back(p1);
            double dist = p1->distance(p2);
            while(dist > mean)
            {
                vec.normalize();
                double newdist = mean + (((double(rand()) / double(RAND_MAX)) * stddev * 2) - stddev);
                if(newdist > dist - mindist)
                    break;
                p1 = new sfa::Point(*p1 + (vec * newdist));
                newPoints.push_back(p1);
                dist = p1->distance(p2);
                vec = *p2 - *p1;
            }
        }
        newPoints.push_back(points.at(points.size() - 1));
        int delta = int(newPoints.size() - points.size());
        points = newPoints;
        return delta;
    }
    
    int LineString::removeColinearPoints(double epsilon)
    {
        sfa::PointList newPoints;
        size_t numPoints = points.size();
        if(numPoints<3)
            return 0;//too small to remove any points
        double total_error = 0;
        sfa::Point *start = points.at(0);
        newPoints.push_back(start);
        for(size_t i=1;i<(numPoints-1);i++)
        {
            sfa::Point *middle = points.at(i);
            sfa::Point *end = points.at(i+1);
            sfa::LineString seg;
            seg.addPoint(*start);
            seg.addPoint(*end);
            total_error += middle->distance3D(&seg);
            if(total_error>epsilon)
            {
                newPoints.push_back(middle);
                total_error = 0;
                start = middle;
            }
            else
            {
                delete middle;
            }
        }
        newPoints.push_back(points.at(numPoints-1));
        int delta = int(newPoints.size() - points.size());
        points = newPoints;
        return delta;
#if 0
        sfa::PointList newPoints;
        size_t numPoints = points.size();
        for(size_t i=0;i<numPoints;i++)
        {
            sfa::Point *p1 = points.at(i);
            newPoints.push_back(p1);
            if(i<(numPoints - 2))
            {
                sfa::Point *p2 = points.at(i+1);
                sfa::Point *p3 = points.at(i+2);
                sfa::LineString seg;
                seg.addPoint(*p1);
                seg.addPoint(*p3);
                double dist = p2->distance(&seg);
                if(dist>epsilon)
                {
                    newPoints.push_back(p2);
                }
                else
                {
                    //if we skip this point, increment i
                    i++;
                }
            }
        }

        int delta = int(newPoints.size() - points.size());
        points = newPoints;
        return delta;
#endif
    }

//!    Doubly linked list implementation used to efficiently track points in removeColinearPoints implmentation
    class LinkedList {
    public:
        struct Node {
            Point*    point;
            Node*    prev;
            Node*    next;
        };

        Node*    begin;
        Node*    end;

        //! constructs a linked list
        LinkedList(void) 
        {
            begin = NULL;
            end = NULL;
        }
        
        ~LinkedList(void) 
        {
            while (begin)
            {
                Node* tmp = begin->next;
                delete begin;
                begin = tmp;
            }
        }

        //! insert a point at the beginning of the list
        Node* prepend(Point* p) 
        {
            Node* node = new Node;
            node->point = p;
            node->prev = NULL;
            node->next = begin;
            if (begin)
                begin->prev = node;
            else
                end = node;
            begin = node;
            return node;
        }

        //! insert a node at the end of the list
        Node* append(Point* p) 
        {
            Node* node = new Node;
            node->point = p;
            node->next = NULL;
            node->prev = end;
            if (end)
                end->next = node;
            else
                begin = node;
            end =  node;
            return node;
        }

        //! remove a node from the list (deletes the node)
        void remove(Node* n) 
        {
            if (n->prev)
                n->prev->next = n->next;
            else
                begin = n->next;

            if (n->next)
                n->next->prev = n->prev;
            else
                end = n->prev;

            delete n;
        }
    };

    static void getDistances(LinkedList::Node* node, double &distanceMapValue, double &distanceMapValueZ, bool zSpecified)
    {
        Point* curr = node->point;
        Point* prev = (node->prev ? node->prev->point : NULL);
        Point* next = (node->next ? node->next->point : NULL);
        distanceMapValue = DBL_MAX;
        distanceMapValueZ = DBL_MAX;
        if(!prev || !next)
            return;
        Point u = *next - *prev;
        Point v = *curr - *prev;
        if(zSpecified)
        {
            double t = u.dot2D(&v) / u.dot2D(&u);
            Point u2 = u;
            double uvZ = ((next->Z() - prev->Z()) * t) + prev->Z();
            distanceMapValue = (*curr - (*prev + (u2 * t))).length2D();
            distanceMapValueZ = abs(uvZ - curr->Z());
        }
        else
        {
            double t = u.dot(&v) / u.dot(&u);
            distanceMapValue = (*curr - (*prev + (u * t))).length();
            distanceMapValueZ = 0.0f;
        }
    }

    int LineString::removeColinearPoints(double epsilon, double zEpsilon)
    {
        bool zSpecified = (zEpsilon != 0.0f);

        if (points.empty())
            return 0;

        size_t initialSize = points.size();

        // build our working data
        LinkedList list;
        std::map<LinkedList::Node *, double> vertexDistanceMap;
        std::map<LinkedList::Node *, double> vertexDistanceMapZ;
        for (size_t i = 0, n = points.size(); i < n; i++)
        {
            Point* point = points[i];
            LinkedList::Node* node = list.append(point);
        }

        for (LinkedList::Node* n = list.begin; n; n = n->next)
        {
            vertexDistanceMap[n] = DBL_MAX;
            vertexDistanceMapZ[n] = DBL_MAX;
            getDistances(n, vertexDistanceMap[n], vertexDistanceMapZ[n], zSpecified);
        }
        while(true)
        {
            LinkedList::Node* node = NULL;
            double minDistance = DBL_MAX;
            double minZDistance = DBL_MAX;
            for(std::map<LinkedList::Node *, double>::iterator it = vertexDistanceMap.begin(), end = vertexDistanceMap.end(); it != end; ++it)
            {
                // skip stuff that isn't collinear in any way
                if(it->second > epsilon)
                    continue;

                float zDistance = vertexDistanceMapZ[it->first];
                if(zDistance > zEpsilon)
                    continue;

                // update if it's the closest
                if(it->second < minDistance || (it->second == minDistance && zDistance < minZDistance))
                {
                    node = it->first;
                    minDistance = it->second;
                    minZDistance = zDistance;
                }
            }

            // we didn't find any collinear points
            if (!node)
                break;

            // remove the collinear vertex and update the previous and next
            LinkedList::Node* prev = node->prev;
            LinkedList::Node* next = node->next;

            vertexDistanceMap.erase(node);
            vertexDistanceMapZ.erase(node);
            delete node->point;
            list.remove(node);
            
            getDistances(prev, vertexDistanceMap[prev], vertexDistanceMapZ[prev], zSpecified);
            getDistances(next, vertexDistanceMap[next], vertexDistanceMapZ[next], zSpecified);
        }

        points.clear();
        for (LinkedList::Node* n = list.begin; n; n = n->next)
            points.push_back(n->point);

        return int(initialSize - points.size());
    }

    bool calculateDistance2D2(sfa::Point lineA, sfa::Point lineB, sfa::Point pt, sfa::Point &outputPt, double &dist)
    {
        lineA.clearZ();
        lineB.clearZ();
        pt.clearZ();

        sfa::Point lineDelta = lineB - lineA;
        double mag2 = lineDelta.length2();

        double u = ( ( ( pt.X() - lineA.X() ) * ( lineB.X() - lineA.X() ) ) + 
            ( ( pt.Y() - lineA.Y() ) * ( lineB.Y() - lineA.Y() ) ) ) / mag2;

        if(u < 0.0f)
        {
            outputPt = lineA;
        }
        else if(u > 1.0f)
        {
            outputPt = lineB;
        }
        else
        {
            outputPt = (lineA + (lineDelta * u));
        }
        
        // get the distance        
        sfa::Point distvec(pt - outputPt);
        dist = distvec.length2();
        return true; 
    }

    int LineString::getNearestPointOnLine2D(const sfa::Point &searchPoint, sfa::Point &outputPoint, int startPt, int endPt)
    {
        double dist = DBL_MAX;
        sfa::Point intersection;
        int segment = -1;
        size_t numPoints = points.size();
        if(endPt==-1)
            endPt = int(numPoints);
        for(size_t i=startPt;i<(endPt-1);i++)
        {
            sfa::Point thisintersection;
            double thisdist = DBL_MAX;
            sfa::Point *p1 = points.at(i);
            sfa::Point *p2 = points.at(i+1);
            if(calculateDistance2D2(*p1,*p2,searchPoint,thisintersection,thisdist))
            {
                if(thisdist < dist)
                {
                    dist = thisdist;
                    intersection = thisintersection;
                    segment = int(i);
                }
                dist = std::min<double>(dist,thisdist);
            }
        }
        outputPoint = intersection;

        return segment;
    }


    bool calculateDistance2(const sfa::Point &lineA, const sfa::Point &lineB, const sfa::Point &pt, sfa::Point &outputPt, double &dist)
    {
        sfa::Point lineDelta = lineB - lineA;
        double mag2 = lineDelta.length2();

        double u = ( ( ( pt.X() - lineA.X() ) * ( lineB.X() - lineA.X() ) ) + 
            ( ( pt.Y() - lineA.Y() ) * ( lineB.Y() - lineA.Y() ) ) + 
            ( ( pt.Z() - lineA.Z() ) * ( lineB.Z() - lineA.Z() ) ) ) / mag2;

        if(u < 0.0f)
        {
            outputPt = lineA;
        }
        else if(u > 1.0f)
        {
            outputPt = lineB;
        }
        else
        {
            outputPt = (lineA + (lineDelta * u));
        }
        
        // get the distance        
        sfa::Point distvec(pt - outputPt);
        dist = distvec.length2();
        return true; 
    }


    int LineString::getNearestPointOnLine(const sfa::Point &searchPoint, sfa::Point &outputPoint, int startPt, int endPt)
    {
        double dist = DBL_MAX;
        sfa::Point intersection;
        int segment = -1;
        size_t numPoints = points.size();
        if(endPt==-1)
            endPt = int(numPoints);
        for(size_t i=startPt;i<(endPt-1);i++)
        {
            sfa::Point thisintersection;
            double thisdist = DBL_MAX;
            sfa::Point *p1 = points.at(i);
            sfa::Point *p2 = points.at(i+1);
            if(calculateDistance2(*p1,*p2,searchPoint,thisintersection,thisdist))
            {
                if(thisdist < dist)
                {
                    dist = thisdist;
                    intersection = thisintersection;
                    segment = int(i);
                }
                dist = std::min<double>(dist,thisdist);
            }
        }
        outputPoint = intersection;

        return segment;
    }

    void LineString::reverse()
    {
        std::reverse(points.begin(), points.end());
    }

    bool LineString::getPointAtS(double &s, sfa::Point &pt, int &afterIdx, bool use3D)
    {
        if(points.size()<2)
            return false;
        if(s < 0)
            return false;
        double prevPos = 0;
        sfa::Point *prevpt = points.at(0);
        for(size_t i=1,ic=points.size();i<ic;i++)
        {
            sfa::Point *currpt = points.at(i);
            sfa::Point line = *currpt - *prevpt;
            if(!use3D)
            {
                double len = sqrt(line.X() * line.X() + line.Y() * line.Y());
                if((len + prevPos) >= s - SFA_EPSILON)
                {
                    double seg_s = s - prevPos;
                    line *= (seg_s/len);
                    pt = *prevpt + line;
                    afterIdx = int(i - 1);
                    return true;
                }
                prevPos += len;
            }
            else
            {
                double len = line.normalize();
                if((len + prevPos) >= s - SFA_EPSILON)
                {
                    double seg_s = s - prevPos;
                    line *= seg_s;
                    pt = *prevpt + line;
                    afterIdx = int(i - 1);
                    return true;
                }
                prevPos += len;
            }
            prevpt = currpt;
            
        }
        afterIdx = int(points.size() - 1);
        pt = points.at(afterIdx);
        return true;
    }

    bool LineString::trimEnds(double offset0, double offsetN)
    {
        sfa::Point firstPt,lastPt;
        int firstidx = -1;
        int lastidx = (int)points.size();
        if(!this->getPointAtS(offset0,firstPt,firstidx,false))
            return false;
        double beforeLen2D = getLength2D();
        double endOffsetS = beforeLen2D - offsetN;
        if(endOffsetS <= SFA_EPSILON)
            return false;
        if(!getPointAtS(endOffsetS,lastPt,lastidx,false))
            return false;

        sfa::PointList newPoints;
        newPoints.push_back(new sfa::Point(firstPt));
        for(size_t i=0,ic=points.size();i<ic;i++)
        {
            if(((int)i <= firstidx) || ((int)i > lastidx))
                delete points.at(i);
            else
                newPoints.push_back(points.at(i));
        }
        newPoints.push_back(new sfa::Point(lastPt));
        points = newPoints;
        return true;
    }

    void LineString::clean(double minDistance2)
    {
        if(points.size()<3)
            return;//Nothing to clean
        sfa::PointList newPoints;
        newPoints.push_back(points.at(0));
        sfa::Point *prev = points.at(0);
        for(size_t i=1,ic=points.size()-1;i<ic;i++)
        {
            sfa::Point *pt = points.at(i);
            sfa::Point line = *pt - *prev;
            if(line.length2()>minDistance2)
            {
                newPoints.push_back(pt);
                prev = pt;
            }
            else
            {
                delete pt;// delete it
            }
        }
        // Always add the end
        sfa::Point *end = points.at(points.size()-1);
        newPoints.push_back(end);
        points = newPoints;
        newPoints.clear();
        // Now remove points that double back on themselves.
        prev = points.at(0);
        newPoints.push_back(prev);
        for(size_t i=1,ic=points.size()-1;i<ic;i++)
        {
            sfa::Point *pt = points.at(i);
            sfa::Point *next = points.at(i+1);
            sfa::Point line = *next - *prev;
            if(line.length2()>minDistance2)//If the two end points are too close to each other, delete the one in the middle
            {
                newPoints.push_back(pt);
                prev = pt;
            }
            else
            {
                delete pt;
            }
            prev = pt;
        }
        newPoints.push_back(end);
        points = newPoints;
    }

    void LineString::transform(const sfa::Matrix &xform)
    {
        for (size_t i = 0, ic = points.size(); i < ic; i++)
        {
            points.at(i)->transform(xform);
        }
    }
}