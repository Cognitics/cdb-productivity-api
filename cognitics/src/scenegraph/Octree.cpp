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
#include <scenegraph/Octree.h>

namespace scenegraph {

    class IOctTreeLeadNode
    {

    public:
        IOctTreeLeadNode() {}
        bool GetExtents(double &minx, double &maxx, double &miny, double &maxy, double &minz, double &maxz)
        {
            return true;
        }

    };


    void testme()
    {
        IOctTreeLeadNode test2;
        OctTree<IOctTreeLeadNode> test1(1000,10);
    
    
        test1.AddLeaf(&test2);
        test1.Sort();
    }


    struct Octree::OctreeNode
    {
    //    Bit locations of coordiantes
        static const size_t INVALID = 0xFFFFFFFF;
        static const size_t X = 0x00000001;
        static const size_t Y = 0x00000002;
        static const size_t Z = 0x00000004;

        double                        center[3];
        std::vector<OctreeNode*>    children;
        std::set<Face*>                faces;

        OctreeNode(void)
        {
            center[0] = center[1] = center[2] = 0;
        }

        ~OctreeNode(void)
        {
            for (size_t i=0, n=children.size(); i<n; i++)
                if (children[i])
                    delete children[i];
        }

        void Sort(size_t depth, size_t maxDepth, size_t bucketSize)
        {
        //    Does this node need to be broken up
            if (faces.size() < bucketSize)
                return;

        //    Can this node be broken up
            if (depth == maxDepth)
                return;

        //    Find center
            size_t np = 0;
            for (std::set<Face*>::iterator it=faces.begin(), end=faces.end(); it!=end; ++it)
                for (size_t i=0, n=(*it)->verts.size(); i<n; i++)
                {
                    center[0] += (*it)->verts[i].X();
                    center[1] += (*it)->verts[i].Y();
                    center[2] += (*it)->verts[i].Z();
                    np++;
                }
            center[0] /= np;
            center[1] /= np;
            center[2] /= np;

        //    Create children
            children.resize(8, NULL);

        //    Insert faces into children
            std::set<Face*> temp;
            for (std::set<Face*>::iterator it=faces.begin(), end=faces.end(); it!=end; ++it)
            {
                double lb[3], ub[3];
                lb[0] = lb[1] = lb[2] = DBL_MAX;
                ub[0] = ub[1] = ub[2] = -DBL_MAX;
                for (size_t i=0, n=(*it)->verts.size(); i<n; i++)
                {
                    lb[0] = std::min<double>((*it)->verts[i].X(), lb[0]);
                    lb[1] = std::min<double>((*it)->verts[i].Y(), lb[1]);
                    lb[2] = std::min<double>((*it)->verts[i].Z(), lb[2]);
                    ub[0] = std::max<double>((*it)->verts[i].X(), ub[0]);
                    ub[1] = std::max<double>((*it)->verts[i].Y(), ub[1]);
                    ub[2] = std::max<double>((*it)->verts[i].Z(), ub[2]);
                }
                
                size_t index = 0;
                for (int i=0; i<3; i++)
                {
                    if (ub[i] <= center[i])
                        index |= (0 << i);
                    else if (lb[i] >= center[i])
                        index |= (1 << i);
                    else
                        index |= INVALID;
                }
                if (index == INVALID)
                    temp.insert(*it);
                else
                {
                    if (!children[index]) children[index] = new OctreeNode;
                    children[index]->faces.insert(*it);
                }
            }

        //    Clear faces
            faces = temp;

        //    Sort children
            for (int i=0; i<8; i++)
                if (children[i])
                    children[i]->Sort(depth+1, maxDepth, bucketSize);
        }

        void Search(const sfa::Point& p, double searchDist, std::vector<Face*>& results)
        {
        //    Insert all triangles in this node
            results.insert(results.end(), faces.begin(), faces.end());

            if (children.empty()) return;

        //    Search children if they are within range
            for (size_t i=0; i<2; i++)
                for (size_t j=0; j<2; j++)
                    for (size_t k=0; k<2; k++)
                    {
                        size_t index = k<<2 | j<<1 | i;
                        if ((i ? p.X()-searchDist < center[0] : p.X()+searchDist > center[0]) &&
                            (j ? p.Y()-searchDist < center[1] : p.Y()+searchDist > center[1]) &&
                            (k ? p.Z()-searchDist < center[2] : p.Z()+searchDist > center[2]))
                        {
                            if (children[index])
                                children[index]->Search(p, searchDist, results);
                        }
                    }
        }

        void Print(int depth)
        {
            for (int i=0; i<depth; i++)
                std::cout << "\t";
            std::cout << "Node (d=" << depth << ",n=" << faces.size() << ")\n";
            for (size_t i=0, n=children.size(); i<n; i++)
                if (children[i])
                    children[i]->Print(depth+1);
        }

    };

    Octree::Octree(Scene& scene, size_t maxDepth, size_t bucketSize, double searchDist)
    {
        root = new OctreeNode;
        this->maxDepth = maxDepth;
        this->bucketSize = bucketSize;
        this->searchDist = searchDist;
        for (size_t i=0, n=scene.faces.size(); i<n; i++)
            root->faces.insert(&scene.faces[i]);
        root->Sort(0, maxDepth, bucketSize);
    }

    Octree::~Octree(void)
    {
        delete root;
    }

    void Octree::Search(const sfa::Point& p, double radius, std::vector<Face*>& results)
    {
        root->Search(p, radius, results);
    }

    void Octree::Print(void)
    {
        root->Print(0);
    }

}