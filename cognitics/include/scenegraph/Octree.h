/****************************************************************************
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
#pragma once

#include <vector>
#include <scenegraph/Scene.h>
#include <deque>

namespace scenegraph {

    

    template <class T>
    class OctTreeNode 
    {
        sfa::Point minPoint;
        sfa::Point maxPoint;
        sfa::Point center;
        std::deque<T *> leafnodes;
        std::deque<OctTreeNode<T> *> children;
        int depth;

        bool isInside(T *leaf)
        {
            double maxx=-DBL_MAX;
            double minx = DBL_MAX;
            double maxy=-DBL_MAX;
            double miny = DBL_MAX;
            double maxz=-DBL_MAX;
            double minz = DBL_MAX;
            if(!leaf->GetExtents(minx,maxx,miny,maxy,minz,maxz))
                return false;
            if (minx >= minPoint.X() &&
                maxx <= maxPoint.X() &&
                miny >= minPoint.Y() &&
                maxy <= maxPoint.Y() &&
                minz >= minPoint.Z() &&
                maxz <= maxPoint.Z())
            {
                return true;
            }
            return false;
        }
        void updateBounds()
        {
            double maxx=-DBL_MAX;
            double minx = DBL_MAX;
            double maxy=-DBL_MAX;
            double miny = DBL_MAX;
            double maxz=-DBL_MAX;
            double minz = DBL_MAX;

            typedef typename std::deque<OctTreeNode<T> *>::iterator iterator;
            iterator iter;
            iter = children.begin();
            while(iter!=children.end())
            {
                if(iter->GetExtents(minx,maxx,miny,maxy,minz,maxz))
                {
                    if(minPoint.X() > minx)
                        minPoint.setX(minx);
                    if(minPoint.Y() > miny)
                        minPoint.setY(miny);
                    if(minPoint.Z() > minz)
                        minPoint.setZ(minz);
                    if(maxPoint.X() < maxx)
                        maxPoint.setX(maxx);
                    if(maxPoint.Y() < maxy)
                        maxPoint.setY(maxy);
                    if(maxPoint.Z() < maxz)
                        maxPoint.setZ(maxz);
                }
                iter++;
            }
            typedef typename std::deque<T *>::iterator iteratorleaf;
            iteratorleaf iterleaf = leafnodes.begin();
            while(iterleaf!=leafnodes.end())
            {
                if(iterleaf->GetExtents(minx,maxx,miny,maxy,minz,maxz))
                {
                    if(minPoint.X() > minx)
                        minPoint.setX(minx);
                    if(minPoint.Y() > miny)
                        minPoint.setY(miny);
                    if(minPoint.Z() > minz)
                        minPoint.setZ(minz);
                    if(maxPoint.X() < maxx)
                        maxPoint.setX(maxx);
                    if(maxPoint.Y() < maxy)
                        maxPoint.setY(maxy);
                    if(maxPoint.Z() < maxz)
                        maxPoint.setZ(maxz);
                }
                iterleaf++;
            }

        }

        void createChildren()
        {    
            OctTreeNode<T> *childllb = new OctTreeNode<T>();//lower left bottom
            childllb->minPoint = minPoint;
            childllb->maxPoint = center;
            childllb->center = (childllb->minPoint + childllb->maxPoint)/2;
            children.push_back(childllb);

            OctTreeNode<T> *childulb = new OctTreeNode<T>();//upper left bottom
            childulb->minPoint = sfa::Point(minPoint.X(),center.Y(),minPoint.Z());
            childulb->maxPoint = sfa::Point(center.X(),maxPoint.Y(),center.Z());
            childulb->center = (childulb->minPoint + childulb->maxPoint)/2;
            children.push_back(childulb);

            OctTreeNode<T> *childlrb = new OctTreeNode<T>();//lower right bottom
            childlrb->minPoint = sfa::Point(center.X(),minPoint.Y(),minPoint.Z());
            childlrb->maxPoint = sfa::Point(maxPoint.X(),center.Y(),center.Z());
            childlrb->center = (childlrb->minPoint + childlrb->maxPoint)/2;
            children.push_back(childlrb);

            OctTreeNode<T> *childurb = new OctTreeNode<T>();//upper right bottom
            childurb->minPoint = sfa::Point(center.X(),center.Y(),minPoint.Z());
            childurb->maxPoint = sfa::Point(maxPoint.X(),maxPoint.Y(),center.Z());
            childurb->center = (childurb->minPoint + childurb->maxPoint)/2;
            children.push_back(childurb);

            OctTreeNode<T> *childllt = new OctTreeNode<T>();//lower left top
            childllt->minPoint = sfa::Point(minPoint.X(),minPoint.Y(),center.Z());
            childllt->maxPoint = sfa::Point(center.X(),center.Y(),maxPoint.Z());
            childllt->center = (childllt->minPoint + childllt->maxPoint)/2;
            children.push_back(childllt);

            OctTreeNode<T> *childult = new OctTreeNode<T>();//upper left top
            childult->minPoint = sfa::Point(minPoint.X(),center.Y(),center.Z());
            childult->maxPoint = sfa::Point(center.X(),maxPoint.Y(),maxPoint.Z());
            childult->center = (childult->minPoint + childult->maxPoint)/2;
            children.push_back(childult);

            OctTreeNode<T> *childlrt = new OctTreeNode<T>();//lower right top
            childlrt->minPoint = sfa::Point(center.X(),minPoint.Y(),center.Z());
            childlrt->maxPoint = sfa::Point(maxPoint.X(),center.Y(),maxPoint.Z());
            childlrt->center = (childlrt->minPoint + childlrt->maxPoint)/2;
            children.push_back(childlrt);

            OctTreeNode<T> *childurt = new OctTreeNode<T>();//upper right top
            childurt->minPoint = center;
            childurt->maxPoint = sfa::Point(maxPoint.X(),maxPoint.Y(),maxPoint.Z());
            childurt->center = (childurt->minPoint + childurt->maxPoint)/2;
            children.push_back(childurt);
        }
    public:
        ~OctTreeNode<T>()
        {
                         typedef typename std::deque<OctTreeNode<T> *>::iterator iterator;
                iterator iter = children.begin();
            while(iter!=children.end())
            {
                delete *iter;
                iter++;
            }
        }

        bool GetExtents(double &minx, double &maxx, double &miny, double &maxy, double &minz, double &maxz)
        {
            updateBounds();
            minx = minPoint.X();
            miny = minPoint.Y();
            minz = minPoint.Z();

            maxx = maxPoint.X();
            maxy = maxPoint.Y();
            maxz = maxPoint.Z();
            return true;
        }
        bool AddLeaf(T *leaf)
        {
            leafnodes.push_back(leaf);
            return true;
        }

        int GetNumLeaves()
        {
            return (int)leafnodes.size();
        }

        T *GetLeafN(int n)
        {
            return leafnodes.at(n);
        }

        void Sort()
        {
            if(leafnodes.size()==0)
                return;
            createChildren();
            std::deque<T *> remaining_leaves;
            typedef typename std::deque<T *>::iterator iteratorleaf;
            iteratorleaf iterleaf = leafnodes.begin();
            while(iterleaf!=leafnodes.end())
            {
                bool found_child = false;
                double maxx=-DBL_MAX;
                double minx = DBL_MAX;
                double maxy=-DBL_MAX;
                double miny = DBL_MAX;
                double maxz=-DBL_MAX;
                double minz = DBL_MAX;
                if(!(*iterleaf)->GetExtents(minx,maxx,miny,maxy,minz,maxz))
                    continue;

                // Find a child that each leaf fits in completely
                typedef typename std::deque<OctTreeNode<T> *>::iterator iterator;
                iterator iter = children.begin();
                while(iter!=children.end())
                {
                    if((*iter)->isInside(*iterleaf))
                    {
                        found_child = true;
                        // add it to the child
                        (*iter)->AddLeaf(*iterleaf);
                    }
                    iter++;
                }

                if(!found_child)
                    remaining_leaves.push_back(*iterleaf);
                iterleaf++;
            }
            leafnodes = remaining_leaves;
        }

        void Search(const sfa::Point& p, double radius, std::vector<T *>& results)
        {

        }

    };

    template <class T>
    class OctTree
    {
        int maxLeavesPerNode;
        int maxDepth;
        OctTreeNode<T> *root;
    public:
        OctTree(int _maxLeavesPerNode=1000, int _maxDepth=10) :
          maxLeavesPerNode(_maxLeavesPerNode), maxDepth(_maxDepth)
        {
            root = new OctTreeNode<T>();
        }

        bool AddLeaf(T *leaf)
        {
            return root->AddLeaf(leaf);
        }

        bool Sort()
        {
            root->Sort();
            return true;
        }

        void Search(const sfa::Point& p, double radius, std::vector<T *>& results)
        {
            root->Search(p,radius,results);
        }

    };

    class Octree
    {
    protected:

        struct OctreeNode;

        OctreeNode* root;
        size_t        maxDepth;
        size_t        bucketSize;
        double        searchDist;

    public:
        Octree(Scene& scene, size_t maxDepth = 6, size_t bucketSize = 32, double searchDist = 1);
        ~Octree(void);

        void    Search(const sfa::Point& p, double radius, std::vector<Face*>& results);

        void    Print(void);
        void    BuildScene(Scene& scene);
    };

}
