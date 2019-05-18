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
#include "sfa/EdgeNode.h"
#include "sfa/EdgeNodeEnd.h"

namespace sfa {

    void EdgeNode::createSplitEdge(double dist0, double dist1)
    {
        int index0 = int(dist0);
        int index1 = int(dist1);
        dist0 = dist0 - index0;
        dist1 = dist1 - index1;

        EdgeNode* result = new EdgeNode(left,on,right,arg);
        result->saveZ = saveZ;

    //    Insert the first point
        if (dist0 == 0)
            result->addPointNode(new PointNode(points[index0]));
        else
        {
            double d[3] = { points[index0+1]->getPoint()->X() - points[index0]->getPoint()->X(), points[index0+1]->getPoint()->Y() - points[index0]->getPoint()->Y(), points[index0+1]->getPoint()->Z() - points[index0]->getPoint()->Z() };
            double x = points[index0]->getPoint()->X() + d[0]*dist0;
            double y = points[index0]->getPoint()->Y() + d[1]*dist0;
            double z = points[index0]->getPoint()->Z() + d[2]*dist0;
            if(points[index0]->getPoint()->is3D())
                result->addPoint(Point(x,y,z));
            else
                result->addPoint(Point(x,y));
        }

        for (int i = index0+1; i <= index1; i++)
        {
            result->addPointNode(new PointNode(points[i]));
        }

    //    Add last point
        if (dist1 != 0)
        {
            double d[3] = { points[index1+1]->getPoint()->X() - points[index1]->getPoint()->X(), points[index1+1]->getPoint()->Y() - points[index1]->getPoint()->Y(), points[index1+1]->getPoint()->Z() - points[index1]->getPoint()->Z() };
            double x = points[index1]->getPoint()->X() + d[0]*dist1;
            double y = points[index1]->getPoint()->Y() + d[1]*dist1;
            double z = points[index1]->getPoint()->Z() + d[2]*dist1;
            if(points[index1]->getPoint()->is3D())
                result->addPoint(Point(x,y,z));
            else
                result->addPoint(Point(x,y));
        }

        result->parent = this;
        children.push_back(result);
    }

    EdgeNode::EdgeNode(void) : arg(0), isolated(true), parent(NULL), start(NULL), end(NULL), saveZ(false)
    {
    }

    EdgeNode::EdgeNode(Label left, Label on, Label right, int geo, bool saveZ): left(left), on(on), right(right), arg(geo), isolated(true), parent(NULL), start(NULL), end(NULL), saveZ(saveZ)
    {

    }
    
    EdgeNode::~EdgeNode(void)
    {
        clearPoints();
    //    Delete all children
        for (int i = 0; i < int(children.size()); i++)
            delete children[i];
    //    Delete ends
        if (start) delete start;
        if (end) delete end;
    }

    int EdgeNode::getGeomArg(void) const
    {
        return arg;
    }

    void EdgeNode::setGeomArg(int geo)
    {
        arg = geo;
    }

    void EdgeNode::addPoint(const Point* p)
    {
        Label label(0);
        label[arg] = on[arg];
        addPointNode(new PointNode(p,label,arg));
    }

    void EdgeNode::addPoint(const Point& p)
    {
        Label label(0);
        label[arg] = on[arg];
        addPointNode(new PointNode(p,label,arg));
    }

    void EdgeNode::addPointNode(PointNode* p)
    {
        points.push_back(p);
    }

    void EdgeNode::clearPoints(void)
    {
        for (int i = 0; i < int(points.size()); i++)
            delete points[i];

        points.clear();
    }

    PointNode* EdgeNode::getStartPoint(void) const
    { 
        return points.front(); 
    }

    PointNode* EdgeNode::getEndPoint(void) const
    { 
        return points.back(); 
    }

    int EdgeNode::getNumPoints(void) const
    {
        return int(points.size());
    }

    PointNode* EdgeNode::getPointN(int n) const
    {
        return points[n];
    }

    Label EdgeNode::getLeft(void) const
    {
        return left;
    }

    Location EdgeNode::getLeft(int n) const
    {
        return left.loc[n];
    }

    void EdgeNode::setLeft(int n, Location l)
    {
        left.loc[n] = l;
    }

    void EdgeNode::setLeft(Label label)
    {
        left = label;
    }

    Label EdgeNode::getOn(void) const
    {
        return on;
    }

    Location EdgeNode::getOn(int n) const
    {
        return on.loc[n];
    }

    void EdgeNode::setOn(int n, Location l)
    {
        on.loc[n] = l;
    }

    void EdgeNode::setOn(Label label)
    {
        on = label;
    }

    Label EdgeNode::getRight(void) const
    {
        return right;
    }

    Location EdgeNode::getRight(int n) const
    {
        return right.loc[n];
    }

    void EdgeNode::setRight(int n, Location l)
    {
        right.loc[n] = l;
    }

    void EdgeNode::setRight(Label label)
    {
        right = label;
    }

    bool EdgeNode::isIsolated(void) const
    {
        return isolated;
    }

    void EdgeNode::setIsolated(bool flag)
    {
        isolated = flag;
    }

    bool EdgeNode::equals(const EdgeNode* other) const
    {
    //    Check forward equals
        if (getNumPoints() != other->getNumPoints()) return false;

        bool forward = true;
        bool reverse = true;
        for (int i = 0; i < getNumPoints(); i++)
        {
            if (!points[i]->equals(other->getPointN(i)))
            {
                forward = false;
                break;
            }
        }
        if (forward) return true;
        for (int i = 0, j = getNumPoints()-1; i < getNumPoints(); i++)
        {
            if (!points[i]->equals(other->getPointN(j))) return false;
            j--;
        }
        return true;
    }

    void EdgeNode::addIntersection(int index, double dist)
    {
        isolated = false;
        /*
        if (!(dist < SFA_EPSILON && dist > -SFA_EPSILON))
        {
            double d = points[index+1]->getPoint()->distance(points[index]->getPoint());
            dist = dist/d;
        }
        */

        double intersection = index+dist;
        intersections.insert(intersection);
    }

    void EdgeNode::createEnds(void)
    {
        if (start) delete start;
        if (end) delete end;
        start = new EdgeNodeEnd(this,true, saveZ);
        end = new EdgeNodeEnd(this,false, saveZ);
        start->setSym(end);
        end->setSym(start);
    }

    EdgeNodeEnd* EdgeNode::getEnd(int i)
    {
        if (i == 0)
        {
            if (!start) createEnds();
            return start;
        }
        else
        {
            if (!end) createEnds();
            return end;
        }
    }

    EdgeNode* EdgeNode::getParent(void) const
    {
        return parent;
    }

    EdgeNodeList EdgeNode::split(void)
    {
        intersections.insert(0);
        intersections.insert(points.size()-1);

    //    Cleanup intersections and compress any close intersections and create array
        std::vector<double> intersection_vector;
        for (std::set<double>::iterator it=intersections.begin(); it!=intersections.end(); ++it)
        {
            if (intersection_vector.empty()) intersection_vector.push_back(*it);
            else if (*it - intersection_vector.back() > SFA_EPSILON) intersection_vector.push_back(*it);
        }

        for (size_t i=1; i < intersection_vector.size(); i++)
            createSplitEdge(intersection_vector[i-1],intersection_vector[i]);

        intersections.clear();
        
        return children;
    }

}