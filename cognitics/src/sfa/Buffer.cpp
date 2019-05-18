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
#include <list>
#include "sfa/Buffer.h"
#include "sfa/PointMath.h"
#include "sfa/LineString.h"
#include "sfa/Polygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/GeometryCollection.h"
#include "sfa/RingMath.h"
#include "sfa/PointMath.h"
#include "sfa/SegmentIntersector.h"
#include "sfa/BSP.h"
#include "sfa/File.h"
#include <cmath>
#include <fstream>
#include <boost/foreach.hpp>

#undef min
#undef max

namespace sfa {

    const double Buffer::pi = atan(1.0)*4.0;

    void Buffer::addPoint(const Point& point)
    {
        if (points.empty())
            points.push_back(point);
        else if (points.back() != point)
            points.push_back(point);
    }

    void Buffer::closePoints(void)
    {
        if (points.size() < 3)
            return;
        else if (points.back() != points.front())
            points.push_back(points.front());
    }

    void Buffer::addTurn(const Point& p, const Point& p0, const Point& p1, int direction)
    {
        double x = p.X();
        double y = p.Y();
        double dx0 = p0.X() - p.X();
        double dy0 = p0.Y() - p.Y();
        double dx1 = p1.X() - p.X();
        double dy1 = p1.Y() - p.Y();

        double startAngle = atan2(dy0, dx0);
        double endAngle = atan2(dy1, dx1);
        // Check for bad geometry that creates indeterminates.
        if (startAngle != startAngle)
        {
            return;
        }
        if (endAngle != endAngle)
        {
            return;
        }
        if (direction == COUNTERCLOCKWISE)
        {
            if (startAngle >= endAngle) startAngle -= 2.0*pi;
        }
        else //if (direction == CLOCKWISE)
        {
            if (startAngle <= endAngle) startAngle += 2.0*pi;
        }

        double totalAngle = abs(startAngle - endAngle);
        int nsegs = int((totalAngle / angle) + 0.5);
        //Detect overflow caused by bad math (previous div by 0)
        if (nsegs < (nsegs - 1))
            return;
        addPoint(p0);
        for (int i = 0; i < nsegs - 1; i++)
        {
            //    User rotational matrix on initial vector. This is much faster than computing the angle and transforming every time.
            //    We just need to know which direction to rotate.
            if (direction == COUNTERCLOCKWISE)
            {
                dx1 = cos_angle*dx0 - sin_angle*dy0;
                dy1 = sin_angle*dx0 + cos_angle*dy0;
            }
            else
            {
                dx1 = cos_angle*dx0 + sin_angle*dy0;
                dy1 = -sin_angle*dx0 + cos_angle*dy0;
            }
            addPoint(Point(x + dx1, y + dy1));
            dx0 = dx1;
            dy0 = dy1;
        }
        addPoint(p1);
    }

    void Buffer::computeOffset(const Point& p0, const Point& p1, Point& o0, Point& o1)
    {
        if (p0 == p1)
        {
            o0 = p0;
            o1 = p1;
            return;
        }
        double dx = p1.X() - p0.X();
        double dy = p1.Y() - p0.Y();
        double len = sqrt(dx*dx + dy*dy);
        len = distance / len;
        //    Compute offset vector
        double ux = dy * len;
        double uy = -dx * len;
        //    Offset points
        o0.setX(p0.X() + ux);
        o0.setY(p0.Y() + uy);
        o1.setX(p1.X() + ux);
        o1.setY(p1.Y() + uy);
    }

    void Buffer::addSegment(const Point& p)
    {
        s0 = s1;
        s1 = s2;
        s2 = p;
        offset00 = offset10;
        offset01 = offset11;
        computeOffset(s1, s2, offset10, offset11);

        if (s1 == s2) return;

        int orientation = CrossProduct(s0, s1, s1, s2);

        if (orientation == 1)        // COUNTERCLOCKWISE turn
            addTurn(s1, offset01, offset10, COUNTERCLOCKWISE);
        else if (orientation == -1)    // CLOCKWISE turn
        {
            //    Compute intersection
            Geometry* intersection = NULL;
            SegmentIntersector::Intersection(&offset00, &offset01, &offset10, &offset11, intersection);

            Point* intPoint = dynamic_cast<Point*>(intersection);
            if (intPoint)
                addPoint(*intPoint);
            else
            {
                //    If there is no intersection ,it means the angle is so small and/or the offset so large the segments won't intersect,
                //    so a temporary inner loop must be created which will be fixed later.
                if (offset01.distance(&offset10) < distance / 1000.0)
                    addPoint(offset01);
                else
                {
                    addPoint(offset01);
                    addPoint(s1);
                    addPoint(offset10);
                }
            }

            if (intersection) delete intersection;
        }
        //    else do nothing, the points are collinear
    }

    void Buffer::addLineEnd(const Point& p0, const Point& p1)
    {
        Point right0, right1;
        Point left0, left1;

        computeOffset(p1, p0, left1, left0);
        computeOffset(p0, p1, right0, right1);

        switch (end_policy)
        {
        case ROUND_ENDS:
            addTurn(p1, right1, left1, COUNTERCLOCKWISE);
            break;
        case FLATTEN_ENDS:
            addPoint(right1);
            addPoint(left1);
            break;
            //case SQUARE_ENDS:
            //    TODO
            //    break;
        default:
            break;
        }
    }

#define DEBUG_NODE_WITH_ID 0
#if DEBUG_NODE_WITH_ID
    static int dbgid = 0;
#endif
    class LineDeconvolute
    {
        double position_epsilon;
        class NodeSegment;
        //Multiple nodes may exist with the same point value, but each unique Node is a different position in the linestring
        struct Node
        {
            Point pt;
#if DEBUG_NODE_WITH_ID
            int id;
#endif
            std::set<NodeSegment *> segments;//weak, for quick removal from the BSP since it works on pointers
            std::set<Node *> neighbors;//Weak
            std::set<Node *> reverse_neighbors;//Weak, to allow quick changes
            virtual ~Node()
            {
            }

            Node(Point _pt)
            {
                pt = _pt;
#if DEBUG_NODE_WITH_ID
                id = dbgid++;
#endif
            }

            bool hasNeighbor(Node *neighbor)
            {
                if (this == neighbor)
                    return false;
                std::set<Node *>::iterator iter = neighbors.find(neighbor);
                if (neighbors.end() != iter)
                {
                    return true;
                }
                return false;
            }

            void addNeighbor(Node *neighbor)
            {
                if (neighbor != this)
                {
                    neighbors.insert(neighbor);
                    //Add myself to the neighbors reverse list
                    neighbor->reverse_neighbors.insert(this);
                }
            }
            void removeNeighbor(Node *neighbor)
            {
                if (neighbor == this)
                    return;
                std::set<Node *>::iterator iter = neighbors.find(neighbor);
                if (neighbors.end() != iter)
                {
                    neighbors.erase(iter);
                }
                //Update the reverse map
                iter = neighbor->reverse_neighbors.find(this);
                if (neighbor->reverse_neighbors.end() != iter)
                {
                    neighbor->reverse_neighbors.erase(iter);
                }

            }

            bool alreadyTraversed(const std::set<Node *> &traversed, Node *n)
            {
                return (traversed.find(n) != traversed.end());
            }

            //Returns true if c is to the right of the line between a and b
            bool isRight(Point a, Point b, Point c)
            {
                double cpval = CrossProduct(a, b, a, c);
                if (cpval < 0)
                    return true;
                /*
                double val = (b.X() - a.X())*(c.Y() - a.Y()) - (b.Y() - a.Y())*(c.X() - a.X());
                if (val < 0)
                    return true;
                    */
                return false;
            }

            //Return the neighboring edge that is the most to the right side of all the other edges
            //if no neighbors, return NULL
            Node *findRightMostNeighbor(const std::set<Node *> &traversed)
            {
                std::set<Node *>::iterator iter = neighbors.begin();
                Node *right_most = NULL;
                bool found_right = false;
                while (iter != neighbors.end())
                {
                    if (alreadyTraversed(traversed, *iter))
                    {
                        iter++;
                        continue;
                    }
                    if (!right_most)
                        right_most = *iter;
                    else
                    {
                        double val = (right_most->pt.X() - pt.X())*((*iter)->pt.Y() - pt.Y()) - (right_most->pt.Y() - pt.Y())*((*iter)->pt.X() - pt.X());
                        if (val<0)
                        {
                            right_most = *iter;
                            found_right = true;
                        }
                        else if (val == 0)
                        {
                            double dist = pt.distance2D2(&(*iter)->pt);
                            double right_most_dist = pt.distance2D2(&right_most->pt);
                            if (dist < right_most_dist)
                            {
                                right_most = *iter;
                                found_right = true;
                            }
                        }

                    }
                    iter++;
                }
                
                return right_most;                
            }            
        };

        class NodeSegment : public sfa::LineString
        {
        public:
            Node *first;//weak
            Node *second;//weak
        
            virtual ~NodeSegment()
            {
            }

            NodeSegment(Node *a, Node *b)
            {
                first = a;
                second = b;
                addPoint(a->pt);
                addPoint(b->pt);
            }

        };

        std::set<Node *> frontier;        
        std::vector<LineString> exteriorRings;
        std::vector<LineString> interiorRings;
        
        std::list<NodeSegment *> segments;
        BSP bsp;
        
        bool Intersection(const Point& p1, const Point& p2, const Point& p3, const Point& p4, Point* &result)
        {
            Point u = p2 - p1;
            Point v = p4 - p3;
            Point w = p1 - p3;

            double denom = v.X()*u.Y() - v.Y()*u.X();

            if (abs(denom) < SFA_EPSILON)
                return false;

            double s = (v.Y()*w.X() - v.X()*w.Y()) / denom;
            if (s < -SFA_EPSILON || s > 1 + SFA_EPSILON) 
                return false;
            double t = (u.Y()*w.X() - u.X()*w.Y()) / denom;
            if (t < -SFA_EPSILON || t > 1 + SFA_EPSILON) 
                return false;

            result = new Point(p1 + u*s);
            return true;
        }


        bool isCounterClockwise(LineString *line)
        {
            /**
              * Sum over the edges, (x2 - x1)(y2 + y1). If the result is positive the curve is clockwise, 
              * if it's negative the curve is counter-clockwise. (The result is twice the enclosed area, with a +/- convention.)
            **/
            int numPoints = line->getNumPoints();
            double sum = 0;
            for (int i = 1; i < numPoints; i++)
            {
                Point p1 = line->getPointN(i - 1);
                Point p2 = line->getPointN(i);
                sum += ((p2.X() - p1.X())*(p2.Y() + p1.Y()));
            }
            if (sum <= 0)
                return true;
            return false;
        }
    public:

        sfa::Geometry *getPolygon()
        {
            sfa::Polygon *ret = new sfa::Polygon();
            if (!extractRings())
            {
                return ret;
            }

            std::vector<LineString>::iterator ext_iter = exteriorRings.begin();
            while (ext_iter != exteriorRings.end())
            {
                ret->addRing(*ext_iter++);
            }

            std::vector<LineString>::iterator int_iter = interiorRings.begin();
            while (int_iter != interiorRings.end())
            {
                ret->addRing(*int_iter++);
            }
            return ret;
        }

        

            struct{
                bool operator()(const Node *n1, const Node *n2)
                {
                    return n1->pt.X() < n2->pt.X();
                }
            } customLess;


        bool extractRings()
        {
            std::set<Node *> traversed;
            std::set<Node *> nodes = frontier;
            std::vector<Node *> sorted_nodes;
            sorted_nodes.insert(sorted_nodes.begin(), frontier.begin(), frontier.end());
            std::sort(sorted_nodes.begin(), sorted_nodes.end(), customLess);

            //We need to start with an outside point, so we'll use the left most edge

            //pick some random point, and iterate along the left edge until either:
            //    a) we've iterated over frontier.size() points and found no cycle (wtf?)
            //        -Return an empty LineString
            //  b) we come across a node we've already visited
            //        -Return all the points in the cycle, removing them from the frontier as we collect them

            std::vector<Node *>::iterator iter = sorted_nodes.begin();
            size_t numNodes = sorted_nodes.size();
            while (iter != sorted_nodes.end())
            {
                Node *n = *iter++;
                if (traversed.find(n) != traversed.end())
                    continue;
                std::vector<Node *> path;
                path.push_back(n);
                size_t i = 0;
                bool found_cycle = false;
                while (i < numNodes && !found_cycle)
                {
                    Node *right = n->findRightMostNeighbor(traversed);
                    if (right)
                    {
                        n = right;
                    }
                    else
                    {
                        break;
                    }
                    // Look to see if we've completed a cycle
                    for (size_t j = 0, jc = path.size(); j < jc; j++)
                    {
                        if (path.at(j) == n)
                        {
                            found_cycle = true;
                            //We have a cycle! from j to (jc-1) + n
                            LineString ring;
                            for (size_t k = j; k < jc; k++)
                            {
                                ring.addPoint(path.at(k)->pt);
                                traversed.insert(path.at(k));
                            }
                            ring.addPoint(n->pt);//Close the ring
                            //Only add a true polygon
                            if (ring.getNumPoints() > 3)
                            {
                                bool CCW = isCounterClockwise(&ring);

                                //Add the ring
                                if (CCW)
                                    exteriorRings.push_back(ring);
                                else
                                    interiorRings.push_back(ring);
                            }
                        }
                        if (found_cycle)
                            break;
                    }
                    if (!found_cycle)
                        path.push_back(n);
                    i++;
                }

            }
            return exteriorRings.size() > 0;//No exterior ring means failure
        }

        ~LineDeconvolute()
        {
            std::set<Node *>::iterator iter = frontier.begin();
            while (iter != frontier.end())
            {
                Node *n = *iter++;
                delete n;
            }
            frontier.clear();
            std::list<NodeSegment *>::iterator segiter = segments.begin();
            while (segiter != segments.end())
            {
                delete *segiter++;
            }
            segments.clear();
        }

        typedef std::pair<Node *, Node *> node_pair_t;

        void removePair(std::list<node_pair_t> &plist, node_pair_t &thepair)
        {
            std::list<node_pair_t>::iterator iter = plist.begin();
            while (iter != plist.end())
            {
                if (thepair == *iter)
                {
                    plist.erase(iter);
                    return;
                }
                iter++;
            }
        }

        bool addNeighbor(Node *node, Node *neighbor, std::list<node_pair_t> &segment_pair)
        {
            if (node == neighbor)
                return false;
            if (node->hasNeighbor(neighbor))
                return false;
            //Add 
            node->addNeighbor(neighbor);
            NodeSegment *seg1 = new NodeSegment(node, neighbor);
            node->segments.insert(seg1);
            segments.push_back(seg1);
            //Add the segment to the BSP
            sfa::BSPEditVisitor insertEditor;
            insertEditor.setVisitMode(sfa::INSERT);
            insertEditor.setEditElement(seg1);
            insertEditor.visiting(&bsp);
            segment_pair.push_back(std::make_pair(node, neighbor));
            return true;
        }

        bool removeNeighbor(Node *node, Node *neighbor, std::list<node_pair_t> &segment_pair)
        {
            //Remove the segment from the BSP
            std::set<NodeSegment *>::iterator segment_iter = node->segments.begin();
            while (segment_iter != node->segments.end())
            {
                NodeSegment *remove_seg = *segment_iter++;
                if (remove_seg->second == neighbor)
                {
                    sfa::BSPEditVisitor removeEditor;
                    removeEditor.setVisitMode(sfa::REMOVE);
                    removeEditor.setEditElement(remove_seg);
                    removeEditor.visiting(&bsp);
                }
            }            
            node->removeNeighbor(neighbor);
                        node_pair_t np = std::make_pair(node, neighbor);
            removePair(segment_pair, np);

            return true;
        }

        // If node has neighbor 'oldNeighbor', remove oldNeighbor and add newNeighbor
        bool replaceNeighbor(Node *node, Node *oldNeighbor, Node *newNeighbor, NodeSegment *ns, std::list<node_pair_t> &segment_pair)
        {
            if (node == newNeighbor)
                return false;
            if (node->hasNeighbor(oldNeighbor))
            {
#if DEBUG_NODE_WITH_ID
                printf("Node %d: replacing neighbor of %d with %d\n", node->id, oldNeighbor->id, newNeighbor->id);
#endif
                addNeighbor(node, newNeighbor,segment_pair);
                removeNeighbor(node, oldNeighbor, segment_pair);
                return true;
            }

            return false;

        }

        void consolidateDuplicateNodes(Node *keepNode, Node *delNode, NodeSegment *ns, std::list<node_pair_t> &segment_pair)
        {
#if DEBUG_NODE_WITH_ID
            printf("Consolidating %d into %d\n", delNode->id, keepNode->id);
#endif
                
            //Update any node that points to delNode to instead point to keepNode                                
            std::set<Node *>::iterator f_neighbor_iter = delNode->neighbors.begin();
            while (f_neighbor_iter != delNode->neighbors.end())
            {
                Node *neighbor = *f_neighbor_iter++;
                removeNeighbor(delNode, neighbor, segment_pair);
                addNeighbor(keepNode, neighbor, segment_pair);
            }

            //Update any other node that points to delNode to point to keepNode instead
            std::set<Node *>::iterator front_iter = delNode->reverse_neighbors.begin();
            while (front_iter != delNode->reverse_neighbors.end())
            {
                Node *f = *front_iter++;
                removeNeighbor(f, delNode, segment_pair);//Remove delNode from being a neighbor of f                    
                addNeighbor(f, keepNode, segment_pair);//Add keepNode as a neighbor to f
            }

        }

        bool PointEqualsEpsilon(sfa::Point *p1, sfa::Point *p2, double epsilon = sfa::SFA_EPSILON)
        {
            double deltax = p1->X() - p2->X();
            double deltay = p1->Y() - p2->Y();
            double deltaz = p1->Z() - p2->Z();
            if (deltax == 0 && deltay == 0 )//&& deltaz == 0
                return true;

            double dist2 = (deltax * deltax) + (deltay*deltay);// +(deltaz*deltaz);
            if (dist2 < (epsilon*epsilon))
                return true;
            return false;
        }
        
        LineDeconvolute(std::vector<Point>& points, const sfa::LineString *refLine, double bufferDist)
        {
            //Use a larger epsilon for positional duplication
            position_epsilon = sfa::SFA_EPSILON * 10;
#if DEBUG_NODE_WITH_ID
            dbgid = 0;
#endif
            if (points.size() < 3)
                return;
            if (points.at(points.size() - 1) != points.at(0))
                points.push_back(points.at(0));            
            

            std::list<node_pair_t> segment_pair;
            //Build the initial 'graph'
            std::vector<Point>::const_iterator prev = points.begin();
            if (prev != points.end())
            {
                Node *prevNode = new Node(*prev++);
                Node *firstNode = prevNode;
                frontier.insert(prevNode);
                std::vector<Point>::const_iterator iter = prev;
                while (iter != points.end())
                {
                    Node *n = new Node(*iter);                    
                    frontier.insert(n);
                    prevNode->addNeighbor(n);
                    segment_pair.push_back(std::make_pair(prevNode, n));
                    NodeSegment *seg = new NodeSegment(prevNode, n);
                    segments.push_back(seg);
                    prevNode->segments.insert(seg);
                    bsp.addGeometry(seg);
                    //printf("Node %d connects to %d\n", prevNode->id, n->id);
                    prevNode = n;
                    iter++;
                }
                prevNode->addNeighbor(firstNode);
                segment_pair.push_back(std::make_pair(prevNode, firstNode));
                NodeSegment *seg = new NodeSegment(prevNode, firstNode);
                segments.push_back(seg);
                prevNode->segments.insert(seg);
                bsp.addGeometry(seg);
            }

            {
                
                std::set<Node *> traversed;
                std::set<Node *>::iterator iter = frontier.begin();
                size_t numNodes = frontier.size();
                while (iter != frontier.end())
                {
                    Node *n = *iter++;
                    
                    BSPCollectGeometriesVisitor collector;
                    collector.setBounds(n->pt.X() - position_epsilon, n->pt.Y() - position_epsilon, n->pt.X() + position_epsilon, n->pt.Y() + position_epsilon);
                    collector.visiting(&bsp);
                    std::list<sfa::Geometry *> results = collector.results;
                    std::list<sfa::Geometry *>::iterator isect_iter = results.begin();
                    while (!results.empty())
                    {
                        sfa::Geometry *result = results.back();
                        results.pop_back();
                        NodeSegment *ns = dynamic_cast<NodeSegment*>(result);

                        if (n == ns->first || n == ns->second)
                            continue;
                        if (PointEqualsEpsilon(&n->pt, &ns->first->pt, position_epsilon))
                        {
                            //If we do this we need to update the geometry of n and all of its nodes in the BSP
                            //n->pt = (n->pt + ns->first->pt) / 2;

                            consolidateDuplicateNodes(n, ns->first, ns, segment_pair);
                            
                            //Requery the bsp now
                            collector.results.clear();
                            collector.visiting(&bsp);
                            results = collector.results;
                        }
                        if (PointEqualsEpsilon(&n->pt, &ns->second->pt, position_epsilon))
                        {
                            //If we do this we need to update the geometry of n and all of its nodes in the BSP
                            //n->pt = (n->pt + ns->second->pt) / 2;
                            consolidateDuplicateNodes(n, ns->second, ns, segment_pair);
                            //Requery the bsp now
                            collector.results.clear();
                            collector.visiting(&bsp);
                            results = collector.results;
                        }
                    }
                }

            }

            std::map<Geometry *, LineString *> envelopes;
            bsp.generate(envelopes);
            //When a pair is broken up, remove that pair from the segment_pair list!
            while (!segment_pair.empty())
            {
                node_pair_t pair = segment_pair.back();
                segment_pair.pop_back();
                Node *n = pair.first;
                Node *neighbor = pair.second;                

                NodeSegment seg1(n, neighbor);
                BSPCollectGeometriesVisitor collector;
                sfa::LineString *env = dynamic_cast<sfa::LineString*>(seg1.getEnvelope());
                sfa::Point *lsMin = dynamic_cast<sfa::Point *>(env->getPointN(0));
                sfa::Point *lsMax = dynamic_cast<sfa::Point *>(env->getPointN(1));
                collector.setBounds(lsMin->X(), lsMin->Y(), lsMax->X(), lsMax->Y());
                delete env;
                collector.visiting(&bsp);
                std::list<sfa::Geometry *> results = collector.results;

                std::list<sfa::Geometry *>::iterator isect_iter = results.begin();
                while (!results.empty())
                {
                    sfa::Geometry *result = results.back();
                    results.pop_back();
                    NodeSegment *ns = dynamic_cast<NodeSegment*>(result);
                    Node *f = ns->first;
                    Node *f_neighbor = ns->second;

                    //seg2 is f and f_neighbor
                    if (n == f)
                        continue;
                    if (n == f_neighbor)
                        continue;
                    if (neighbor == f)
                        continue;
                    if (f_neighbor == neighbor)
                        continue;
                    //We don't want to compare f_neighbor with f or neighbor
                    if (f->hasNeighbor(n))
                        continue;
                    if (f_neighbor->hasNeighbor(neighbor))
                        continue;
                    if (f->hasNeighbor(neighbor))
                        continue;
#if DEBUG_NODE_WITH_ID
                    printf("testing %d -> %d against %d -> %d\n", n->id, neighbor->id, f->id, f_neighbor->id);
#endif
                    if (PointEqualsEpsilon(&n->pt, &f_neighbor->pt, position_epsilon))
                    {
                        consolidateDuplicateNodes(n, f_neighbor, ns, segment_pair);
                        //Requery the bsp now
                        collector.results.clear();
                        collector.visiting(&bsp);
                        results = collector.results;
                        continue;
                    }
                    if (PointEqualsEpsilon(&f->pt, &neighbor->pt, position_epsilon))
                    {
                        consolidateDuplicateNodes(f, neighbor, ns, segment_pair);
                        //Neighbor is now f!
                        neighbor = f;
                        
                        //Requery the bsp now
                        collector.results.clear();
                        collector.visiting(&bsp);
                        results = collector.results;
                        continue;
                    }

                    // look for an intersection between seg1 and seg2
                    sfa::Point *ipoint = NULL;
                    if (Intersection(n->pt, neighbor->pt, f->pt, f_neighbor->pt, ipoint))
                    {
                        if (ipoint)
                        {
#if DEBUG_NODE_WITH_ID
                            printf("Intersection between %d<-->%d and %d<-->%d\n", n->id, neighbor->id, f->id, f_neighbor->id);
#endif
                            if (PointEqualsEpsilon(ipoint, &n->pt) || 
                                PointEqualsEpsilon(ipoint, &neighbor->pt) ||
                                PointEqualsEpsilon(ipoint, &f->pt) ||
                                PointEqualsEpsilon(ipoint, &f_neighbor->pt))
                            {
                                delete ipoint;
                                continue;
                            }
                            // Add the intersection point (int_node) as a node
                            Node *int_node = new Node(*ipoint);
                            delete ipoint;
#if DEBUG_NODE_WITH_ID
                            if (int_node->id == 39)
                                printf("");
                            printf("Adding node %d\n", int_node->id);
#endif        
                            //Add it to the master list of nodes
                            frontier.insert(int_node);
                            // Add a neighbor relationship between int_node and (n,neighbor,f,f_neighbor)
                            if (replaceNeighbor(f, f_neighbor, int_node, ns, segment_pair))
                            {
                                //Above we added a node from f -> int_pair, so now we complete it by adding from int_pair to f
                                addNeighbor(int_node, f_neighbor, segment_pair);                                
                            }                                
                            else
                            {
                                replaceNeighbor(f_neighbor, f, int_node, ns, segment_pair);
                                addNeighbor(int_node, f, segment_pair);
                            }

                            if (replaceNeighbor(n, neighbor, int_node, ns, segment_pair))
                            {
                                addNeighbor(int_node, neighbor, segment_pair);                                
                            }
                            else
                            {
                                replaceNeighbor(neighbor, n, int_node, ns, segment_pair);
                                addNeighbor(int_node, n, segment_pair);
                            }
                                
                            //Requery the bsp now
                            collector.results.clear();
                            collector.visiting(&bsp);
                            results = collector.results;
                            break;
                        }
                        else
                        {
                            //Something broke in Intersection
                        }
                    }
                }
            }
            //Iterate over the frontier, and remove any neighbors for any nodes closer than the buffer length from the refLine
            std::set<Node *>::iterator iter = frontier.begin();
            while (iter != frontier.end())
            {
                Node *n = *iter++;
                double dist = n->pt.distance(refLine);
                //This is a magic number (just a number that seems to work reliably), but it's trying to account for the approximation of a circle
                //that we form with facets of a turn
                if (dist < (bufferDist *.8))
                {
#if DEBUG_NODE_WITH_ID
                    printf("Removing node %d\n", n->id);
#endif
                    //This node should be removed from the graph (by breaking the links)
                    //First remove any neighbors I have
                    std::set<Node *>::iterator niter = n->neighbors.begin();
                    while (niter != n->neighbors.end())
                    {
                        Node *neighbor = *niter++;
                        n->removeNeighbor(neighbor);
                    }
                    //Next update any nodes that have me as a neighbor, to not have me as a neighbor
                    niter = n->reverse_neighbors.begin();
                    while (niter != n->reverse_neighbors.end())
                    {
                        Node *neighbor = *niter++;
                        neighbor->removeNeighbor(n);
                    }
                }
            }

#if DEBUG_NODE_WITH_ID
            if (1)
            {
                std::string gvfilename("dot.gv");
                std::remove(gvfilename.c_str());
                std::ofstream of(gvfilename.c_str());
                of << "digraph {\n";
                std::set<Node *> traversed;
                std::set<Node *>::iterator iter = frontier.begin();
                size_t numNodes = frontier.size();
                while (iter != frontier.end())
                {
                    Node *n = *iter++;
                    //of << "\t\"" << n->id << "\"\n";
                    Node *rightmost = n->findRightMostNeighbor(traversed);
                    //if (rightmost)
                    //    of << "\t\"" << n->id << "\" -> \"" << rightmost->id << "\"\n";
                    std::set<Node *>::iterator niter = n->neighbors.begin();
                    while (niter != n->neighbors.end())
                    {
                        Node *neighbor = *niter++;
                        if (neighbor == rightmost)
                        {
                            of << "\t" << n->id << " -> " << neighbor->id << "[color=red,penwidth=3.0]\n";
                        }
                        else
                        {
                            of << "\t" << n->id << " -> " << neighbor->id << "\n";
                        }
                    }
                }
                of << "}\n";
            }


            if (1)
            {
                std::set<Node *> nodes = frontier;
                FeatureList features;
                std::set<Node *> traversed;
                //Write a feature (segment) for each node connection.
                std::set<Node *>::iterator iter = frontier.begin();
                size_t numNodes = frontier.size();
                while (iter != frontier.end())
                {
                    Node *n = *iter++;

                    {
                        sfa::Feature *f = new sfa::Feature;
                        f->geometry = new sfa::Point(n->pt.X(), n->pt.Y(), 0);
                        f->attributes.setAttribute("nodeid", n->id);
                        features.push_back(f);
                    }
                    Node *rightmost = n->findRightMostNeighbor(traversed);
                    if (rightmost)
                    {
                        LineString *seg = new LineString;
                        seg->addPoint(sfa::Point(n->pt.X(), n->pt.Y(), 0));
                        seg->addPoint(sfa::Point(rightmost->pt.X(),rightmost->pt.Y(),0));
                        sfa::Feature *f = new sfa::Feature;
                        f->geometry = seg;
                        f->attributes.setAttribute("nodeid", n->id);
                        f->attributes.setAttribute("rightmost", rightmost->id);
                        features.push_back(f);
                    }                    
                }
                sfa::Feature origFeature;
                origFeature.attributes.setAttribute("name", "original");
                origFeature.geometry = refLine->copy();
                features.push_back(&origFeature);
                sfa::writeSFAFile(features, "features.sfa");
            }
#endif

        }

        


    };

    /*
    DE_CONVOLUTE
    ===============================================================================================
    */
    Geometry* Buffer::de_convolute(const std::vector<Point>& points)
    {
        /*
        We can detect loops in the convolution by running a ray-tracing backwards when we attemp to add a new Point to the result.
        If it intersects a previous edge, then the first intersection point acts as the point to "pinch" the loop closed.
        If the loop is found to be counter-clockwise, the loop is excessive and can be discarded, else if it is clockwise, then we
        should keep the loop and use it as a hole.

        When detecting if an edge intersects the ray, we do not count any intersections that occur at an endpoint of an edge; although,
        we do count intersections that occur at the start point of an edge.

        Use brute force for now, with queries in linear time.
        Later implement MonotonicChain to allow optimal log(n) query time.
        */

        //    Holds the currently processed points to the list
        std::vector<Point> processed;
        std::vector< std::vector<Point> > holes;

        //    Find the first point to start on. This must be on the exterior or else problems may ocur, so we just choose the point further in one direction.
        int start = 0;
        int rev_start = 0;
        for (int i = 0; i < int(points.size()); i++)
        {
            if (points[start].X() < points[i].X())
                start = i;
            if (points[rev_start].X() > points[i].X())
                rev_start = i;
        }

        // we want to use a start that is the furthest distance possible from the endpoint
        int start_dist = std::min(start, int(points.size()) - start);
        int rev_dist = std::min(rev_start, int(points.size()) - rev_start);
        if (rev_dist > start_dist)
            start = rev_start;


        processed.push_back(points[start]);

        /*    Itterate through the points and try to add the next point. Shoot a ray back and try to intersect all the edges already added.
        If there is an intersection, attempt to handle the loop.
        */
        for (size_t offset = 1; offset < int(points.size()); offset++)
        {
            size_t i = (offset + start) % points.size();

            Point next = points[i];
            bool intersection = false;

            //    Shoot ray backwards
            double U[2] = { next.X() - processed.back().X(), next.Y() - processed.back().Y() };

            //    Find s such that next + s*dnext = intersection
            //    find t such that p1 + t*(p2-p1) = intersection
            double s;
            double t;

            //    Index of point after intersection
            int j;
            //    CCW or CW
            bool CCW;

            //    Don't start at the end, start just before the end...we already know that the segment will be intersecting the processed list at the
            //    previous point by definition.
            for (j = int(processed.size() - 2); j > 0; j--)
            {
                Point p1 = processed[j - 1];
                Point p2 = processed[j];

                double V[2] = { p2.X() - p1.X(), p2.Y() - p1.Y() };
                double W[2] = { next.X() - p1.X(), next.Y() - p1.Y() };

                double denom = V[0] * U[1] - V[1] * U[0];

                if (denom == 0)
                    continue;
                else
                {
                    s = (V[1] * W[0] - V[0] * W[1]) / denom;
                    t = (U[1] * W[0] - U[0] * W[1]) / denom;
                }

                if (s > 0 || s < -1)
                    continue;
                else if (t < 0 || t > 1)
                    continue;
                else
                {
                    CCW = (denom < 0);
                    intersection = true;
                    //printf("Intersection at i=%d j=%d\n", i, j);
                    break;
                }
            }

            if (intersection)
            {
                Point intPoint(next.X() + s*U[0], next.Y() + s*U[1]);

                if (!CCW)    //    Save loop as a hole
                {
                    //    Note that hole will NOT be closed yet
                    std::vector<Point> hole;
                    hole.push_back(intPoint);
                    for (int k = j; k < int(processed.size()); k++)
                        hole.push_back(processed[k]);

                    holes.push_back(hole);
                }

                //    Cut away loop from main processed values

                size_t d = processed.size() - j;
                //printf("Cutting %d points from the end\n", d);
                for (size_t k = 0; k < d; k++)
                    processed.pop_back();
                //printf("Pushing back point at %f %f\n", intPoint.X(), intPoint.Y());
                processed.push_back(intPoint);
                offset--;
                continue;

            }
            else processed.push_back(next);
        }

        Polygon* result = new Polygon;

        //    Add boundary
        LineString* boundary = new LineString;
        for (unsigned int i = 0; i < processed.size(); i++)
            boundary->addPoint(processed[i]); // create new pointer

        boundary->addPoint(processed[0]); // close
        result->addRing(boundary);


        //    Add holes
        for (std::vector< std::vector<Point> >::iterator it = holes.begin(); it != holes.end(); ++it)
        {
            LineString* hole = new LineString;
            for (unsigned int i = 0; i < it->size(); i++)
                hole->addPoint((*it)[i]); // create new pointer

            hole->addPoint((*it)[0]);

            //    Check if interior ring hasn't collapsed
            double area = GetRingArea(hole);
            if (area <-SFA_EPSILON)
                result->addRing(hole);
        }

        return result;
    }

    /*
    Point buffer
    ===============================================================================================
    */
    Geometry* Buffer::bufferPoint(const Geometry* point)
    {
        //    You cannot buffer a Point with a negative distance. It results in nothing, so wer return nothing.
        if (distance < 0) return NULL;

        const Point* p = dynamic_cast<const Point*>(point);
        LineString* ring = new LineString;

        double dx0, dy0, dx1, dy1;
        dx0 = distance;
        dy0 = 0;

        for (unsigned int i = 0; i < 4 * quad_segments; i++)
        {
            dx1 = cos_angle*dx0 - sin_angle*dy0;
            dy1 = sin_angle*dx0 + cos_angle*dy0;

            ring->addPoint(Point(p->X() + dx1, p->Y() + dy1));

            dx0 = dx1;
            dy0 = dy1;
        }

        ring->addPoint(*ring->getPointN(0));

        Polygon* poly = new Polygon;
        poly->addRing(ring);
        return poly;
    }



    /*
    LineString buffer
    ===============================================================================================
    */
    Geometry* Buffer::bufferLineString(const Geometry* origLine)
    {
        sfa::Geometry *geomcopy = origLine->copy();
        sfa::LineString *line = dynamic_cast<sfa::LineString *>(geomcopy);
        
        if (!line)
        {
            if (geomcopy)
                delete geomcopy;
            return NULL;
        }
        line->removeColinearPoints(sfa::SFA_EPSILON);

        if (distance < 0)
        {
            if (geomcopy)
                delete geomcopy;
            return NULL;
        }

        points.clear();
        const LineString* linestring = line;

        int num = linestring->getNumPoints();
        if (num == 0)
        {
            if (geomcopy)
                delete geomcopy;
            return NULL;
        }
        else if (num == 1)
            return bufferPoint(linestring->getPointN(0));
        else if (num == 2)
        {
            addLineEnd(*linestring->getPointN(0), *linestring->getPointN(1));
            addLineEnd(*linestring->getPointN(1), *linestring->getPointN(0));
        }
        else
        {
            Point next, first;

            //    Move down the line
            s1 = *linestring->getPointN(0);
            s2 = *linestring->getPointN(1);
            computeOffset(s1, s2, offset10, offset11);
            for (int i = 2; i < num; i++)
                addSegment(*linestring->getPointN(i));

            //    Add end cap
            first = *linestring->getPointN(num - 1);
            for (int i = 2; i <= num; i++)
            {
                next = *linestring->getPointN(num - i);
                if (next != first) break;
                else if (i == num)
                {
                    if (geomcopy)
                        delete geomcopy;
                    //    ERROR!! All points of the line are equal!
                    return NULL;
                }
            }
            addLineEnd(next, first);

            //    Move back up the line
            s1 = *linestring->getPointN(num - 1);
            s2 = *linestring->getPointN(num - 2);
            computeOffset(s1, s2, offset10, offset11);
            for (int i = num - 3; i >= 0; i--)
                addSegment(*linestring->getPointN(i));

            //    Add end cap
            first = *linestring->getPointN(0);
            for (int i = 1; i <= num; i++)
            {
                next = *linestring->getPointN(i);
                if (next != first) break;
                else if (i == num)
                {
                    if (geomcopy)
                        delete geomcopy;
                    //    ERROR!! All point of the line are equal!
                    return NULL;
                }
            }

            addLineEnd(next, first);
        }
    
        LineDeconvolute dc(points, linestring, distance);
        if (geomcopy)
            delete geomcopy;
        sfa::Polygon *ret = dynamic_cast<sfa::Polygon *>(dc.getPolygon());
        //Return NULL on failure rather than an empty polygon
        if (ret->isEmpty())
        {
            delete ret;
            ret = NULL;
        }
        return ret;
    }


    /*
    Polygon buffer
    ===============================================================================================
    */
    Geometry* Buffer::bufferPolygon(const Geometry* poly)
    {
        bool negativeDistance = distance < 0;
        distance = abs(distance);

        //    Only supports rounded end policies
        int backupEndPolicy = end_policy;
        end_policy = ROUND_ENDS;

        const Polygon* polygon = dynamic_cast<const Polygon*>(poly);
        Geometry* result = NULL;

        //    Buffer exterior ring
        Geometry* outerRingBuffer = bufferLineString(polygon->getExteriorRing());

        if (outerRingBuffer)
        {
            if (outerRingBuffer->isEmpty())
                return NULL;
            Polygon* outerRingBufferPolygon = dynamic_cast<Polygon*>(outerRingBuffer);

            if (negativeDistance) // Use interior rings
            {
                int n = outerRingBufferPolygon->getNumInteriorRing();
                if (n == 1) // Result is a single polygon
                {
                    Polygon* polygonResult = new Polygon();
                    polygonResult->addRing(*outerRingBufferPolygon->getInteriorRingN(0));
                    polygonResult->reverse();
                    result = polygonResult;
                }
                else if (n>1) // Result is a multi polygon
                {
                    MultiPolygon* multiPolygonResult = new MultiPolygon();
                    for (int i = 0; i<n; i++)
                    {
                        Polygon* next = new Polygon();
                        next->addRing(*outerRingBufferPolygon->getInteriorRingN(i));
                        next->reverse();
                        multiPolygonResult->addGeometry(next);
                    }
                    result = multiPolygonResult;
                }
            }
            else //    Use exterior rings
            {
                Polygon* polygonResult = new Polygon();
                polygonResult->addRing(*outerRingBufferPolygon->getExteriorRing());
                result = polygonResult;
            }

            delete outerRingBuffer;
        }

        //    Buffer all interior rings and use them as a difference
        for (int i = 0; i<polygon->getNumInteriorRing(); i++)
        {
            if (!result) break;

            Geometry* nextBuffer = bufferLineString(polygon->getInteriorRingN(i));
            if (nextBuffer)
            {
                Polygon* polygonBuffer = dynamic_cast<Polygon*>(nextBuffer);

                if (negativeDistance) // Use exterior ring
                {
                    Geometry* temp = result->difference(polygonBuffer->getExteriorRing());
                    delete result;
                    result = temp;
                }
                else //    Use Interior rings
                {
                    for (int j = 0; j<polygonBuffer->getNumInteriorRing(); j++)
                    {
                        if (!result) break;

                        Polygon ring;
                        ring.addRing(*polygonBuffer->getInteriorRingN(j));
                        ring.reverse();
                        Geometry* temp = result->difference(&ring);
                        delete result;
                        result = temp;
                    }
                }

                delete nextBuffer;
            }
        }

        //    Restore original settings
        end_policy = backupEndPolicy;
        if (negativeDistance) distance *= -1;

        return result;
    }

    /*
    PolyhedralSurface buffer
    ===============================================================================================
    */
    Geometry* Buffer::bufferPolyhedralSurface(const Geometry* surface)
    {
        const PolyhedralSurface* polyhedralSurface = dynamic_cast<const PolyhedralSurface*>(surface);

        if (union_policy == DISJOINT_RESULTS)
        {
            GeometryCollection* result = new GeometryCollection;

            for (int i = 0; i<polyhedralSurface->getNumPatches(); i++)
            {
                Geometry* temp = polyhedralSurface->getPatchN(i)->buffer(distance, end_policy, union_policy, quad_segments);
                if (temp)
                    result->addGeometry(temp);
            }

            return result;
        }
        else
        {
            Geometry* result = NULL;

            for (int i = 0; i<polyhedralSurface->getNumPatches(); i++)
            {
                if (result)
                {
                    Geometry* geometryBuffer = polyhedralSurface->getPatchN(i)->buffer(distance, end_policy, union_policy, quad_segments);
                    Geometry* temp = result->Union(geometryBuffer);
                    delete geometryBuffer;
                    delete result;
                    result = temp;
                }
                else
                    result = polyhedralSurface->getPatchN(i)->buffer(distance, end_policy, union_policy, quad_segments);
            }

            return result;
        }
    }

    /*
    Collection buffer
    ===============================================================================================
    */
    Geometry* Buffer::bufferCollection(const Geometry* collection)
    {
        const GeometryCollection* geometryCollection = dynamic_cast<const GeometryCollection*>(collection);

        if (union_policy == DISJOINT_RESULTS)
        {
            GeometryCollection* result = new GeometryCollection;

            for (int i = 1; i <= geometryCollection->getNumGeometries(); i++)
            {
                Geometry* temp = geometryCollection->getGeometryN(i)->buffer(distance, end_policy, union_policy, quad_segments);
                if (temp)
                    result->addGeometry(temp);
            }

            return result;
        }
        else
        {
            Geometry* result = NULL;

            for (int i = 1; i <= geometryCollection->getNumGeometries(); i++)
            {
                if (result)
                {
                    Geometry* geometryBuffer = geometryCollection->getGeometryN(i)->buffer(distance, end_policy, union_policy, quad_segments);
                    Geometry* temp = result->Union(geometryBuffer);
                    delete geometryBuffer;
                    delete result;
                    result = temp;
                }
                else
                    result = geometryCollection->getGeometryN(i)->buffer(distance, end_policy, union_policy, quad_segments);
            }

            return result;
        }
    }

    /*
    Geometry buffer
    ===============================================================================================
    */
    Geometry* Buffer::bufferGeometry(const Geometry* geom)
    {
        int type = geom->getWKBGeometryType(false, false);

        switch (type)
        {
        case (wkbPoint) :
            return bufferPoint(geom);
            break;
        case (wkbLineString) :
            return bufferLineString(geom);
            break;
        case (wkbPolygon) :
        case (wkbTriangle) :
                           return bufferPolygon(geom);
            break;
        case (wkbMultiPoint) :
        case (wkbMultiLineString) :
        case (wkbMultiPolygon) :
        case (wkbGeometryCollection) :
                                     return bufferCollection(geom);
            break;
        case (wkbPolyhedralSurface) :
        case (wkbTIN) :
                      return bufferPolyhedralSurface(geom);
            break;
        default:
            throw std::runtime_error("Buffer::apply() Unknown Geometry!");
        }
    }

    //    PUBLIC METHODS ================================================================================
    //    ===============================================================================================

    Buffer::Buffer(unsigned int quadrantSegments, int endPolicy, int unionPolicy)
    {
        end_policy = endPolicy;
        union_policy = unionPolicy;

        distance = 0;

        if (quadrantSegments < 1) quadrantSegments = 1;

        //    Compute quadrant segment information
        quad_segments = quadrantSegments;
        angle = (pi / 2) / quad_segments;
        sin_angle = sin(angle);
        cos_angle = cos(angle);
    }

    int Buffer::getEndPolicy(void) const
    {
        return end_policy;
    }

    void Buffer::setEndPolicy(int policy)
    {
        switch (policy)
        {
        case ROUND_ENDS:
        case FLATTEN_ENDS:
            end_policy = policy;
            break;
        default:
            end_policy = ROUND_ENDS;
        }
    }

    int Buffer::getUnionPolicy(void) const
    {
        return union_policy;
    }

    void Buffer::setUnionPolicy(int policy)
    {
        switch (policy)
        {
        case UNION_RESULTS:
        case DISJOINT_RESULTS:
            union_policy = policy;
            break;
        default:
            union_policy = UNION_RESULTS;
        }
    }

    Geometry* Buffer::apply(const Geometry* geom, double d)
    {
        if (!geom) return NULL;
        if (d == 0 || geom->isEmpty()) return geom->copy();

        distance = d;
        return bufferGeometry(geom);
    }

}
