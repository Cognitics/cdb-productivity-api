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
#pragma once
#include <float.h>
#include <math.h>
#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include <sfa/Point.h>
#include <sfa/Polygon.h>

#include <scenegraph/MappedTexture.h>
#include <scenegraph/UserData.h>
#include "Material.h"
#include <ccl/AttributeContainer.h>

namespace scenegraph
{

    class FaceNormals
    {
    public:
        sfa::Point p1Normal;
        sfa::Point p2Normal;
        sfa::Point p3Normal;
        sfa::Point normal;

    };

    class Face
    {        
    public:
        UserData *userData;
        
        std::vector<sfa::Point> verts;
        std::vector<sfa::Point> vertexNormals;
        std::vector<MappedTexture> textures;
        std::vector<Material> materials;
        int groupID;                // TODO: get rid of this
        std::string groupName;                // TODO: get rid of this
        bool clipped;
        double area;
        ccl::AttributeContainer attributes;
        Color primaryColor;
        Color alternateColor;
        Face(void);
        Face(const Face &in);
        int featureID;
        int smc;
        double transparency;
        std::string id;
        bool drawBothSides;
        ~Face();
        Face & operator=(Face const &in);
        std::string legacyComment;
        sfa::Point GetFaceNormal() const;

        /**
         * @fn    bool Face::hasVertex(const sfa::Point &pt);
         *
         * @brief    Query if the face has the vertex pt.

         * @param    pt    The vertex.
         *
         * @return    true if the vertex is found in the face, false if not.
         */
        bool hasVertex(const sfa::Point &pt);

        /**
         * @fn    int Face::getNumVertices() const;
         *
         * @brief    Gets the number of vertexs in the face.
         *
         * @return    The number of vertexs.
         */
        int getNumVertices() const;

        /**
         * @fn    sfa::Point Face::getVertN(const int &n) const;
         *
         * @brief    Gets vertex n.
         *
         * @param    n    The index of the vertex to return. If n is greater than the number of vertices, an empty sfa::Point {0,0,0} will be returned.
         *
         * @return    The vertex at index n.
         */
        sfa::Point getVertN(const int &n) const;

        /**
         * @fn    void Face::setVertN(const int &n,const sfa::Point &pt);
         *
         * @brief    Sets vertex n. If the face has fewer than n vertices, the method will return false. 
         *
         * @param    n     The index of the vertex to set
         * @param    pt    The vertex.
         *                 
         * @return    true on success, false on error
         */
        bool setVertN(const int &n,const sfa::Point &pt);

        /**
         * @fn    int Face::addVert(const sfa::Point &pt);
         *
         * @brief    Adds a vertex.
         *
         * @param    pt    The vertex.
         *
         * @return    The index of the vertex added.
         */

        int addVert(const sfa::Point &pt);

        /**
         * @fn    MappedTexture Face::getMappedTextureN(const int &n) const;
         *
         * @brief    Gets mapped texture at index n.
         *
         * @param    n    The index of the mapped texture to get
         *
         * @return    The mapped texture at n.
         */
        MappedTexture getMappedTextureN(const int &n) const;

        /**
         * @fn    int Face::addMappedTexture(const MappedTexture &mt);
         *
         * @brief    Adds a mapped texture.
         *
         * @param    mt    The mapped texture to add.
         *
         * @return    The index of the added mapped texture
         */
        int addMappedTexture(const MappedTexture &mt);

        /**
         * @fn    void Face::setMappedTextureN(const int &n, const MappedTexture &mt);
         *
         * @brief    Sets mapped texture n.
         *
         * @param    n     The index of the mapped texture to set. If n is greater than the number of existing mapped textures, the method will return false.
         * @param    mt    The m mapped texture.
         *
         * @return    True on success, false on failure
         */
        bool setMappedTextureN(const int &n, const MappedTexture &mt);

        /**
         * @fn    sfa::Point Face::getNormalN(const int &n) const;
         *
         * @brief    Gets the normal for the vertex at index n.
         *
         * @param    n    The index of the point to return. If n is greater than the number of points, an empty point {0,0,0} will be returned.
         *
         * @return    The normal for vertex n.
         */
        sfa::Point getNormalN(const int &n) const;

        /**
         * @fn    void Face::setNormalN(const int &n, const sfa::Point &norm);
         *
         * @brief    Sets normal n.
         *
         * @param    n       The index of the vertex to set the normal for. 
         * @param    norm    The normal to set.
         *                     
         */
        void setNormalN(const int &n, const sfa::Point &norm);

        void getBoundingBox(double& minX, double& maxX, double& minY, double& maxY) const;
        void getBoundingBox(sfa::Point &minPt, sfa::Point &maxPt) const;

        void SetFaceNormal(const sfa::Point &faceNormal);


        sfa::Point getUVForVertex(const sfa::Point &newp, unsigned int tnum) const;
        sfa::Point computeNormal() const;
    
        std::string getGroupAndTextureKey() const;
        void RemapTextureCoords(unsigned int a_idx, unsigned int b_idx, unsigned int c_idx);

        double getArea();

        //! Returns true and sets the Z value of p if the X and Y coordinates
        //! of p are inside the x and y of the face.
        bool interpolatePointInFace(sfa::Point &p) const;
        ccl::AttributeContainer &getAttributes(void);
        const ccl::AttributeContainer &getAttributes(void) const;
        void setAttributes(const ccl::AttributeContainer &_attributes);

        sfa::Polygon getPolygon() const;
    };

    typedef std::vector<Face> FaceList;

    class BSPNode
    {    
    public:
        static const int MAX_DEPTH = 6;
        std::vector<BSPNode *> children;
        std::vector<Face> *faces; // Just a reference!
        std::set<unsigned int> face_indices;
        int axis;
        double bsp_min;
        double bsp_max;

    public:
        BSPNode()
        {
            faces = NULL;
            bsp_max = 0;
            bsp_min = DBL_MAX;
            axis = -1;
        }
        ~BSPNode() 
        {
            BOOST_FOREACH(BSPNode *child, children)
            {
                delete child;
            }        
        }

        void getBoundingBox(double& minX, double& maxX, double& minY, double& maxY)
        {
            minX = minY = DBL_MAX;
            maxX = maxY = -DBL_MAX;

            BOOST_FOREACH(unsigned int idx, face_indices)
            {
                Face &face = faces->at(idx);
                face.getBoundingBox(minX,maxX,minY,maxY);                
            }

        }

        void printTree(int depth = 0)
        {
            //for(int i=0;i<depth;i++)
            //    std::cout << " ";
            if(children.size()==0)
            {
                std::cout << "Node at depth " << depth << " has " << face_indices.size() << " face_indices.\n";
                return;
            }
            //std::cout << "Node at depth " << depth << " has " << children.size() << " children.\n";
            BOOST_FOREACH(BSPNode *child, children)
            {
                child->printTree(depth+1);
            }
        }

        bool sort(int depth=0, int axis=0)
        {    
            bsp_min = DBL_MAX;
            bsp_max = 0;
            this->axis = axis;
            if(axis==0)
            {
                // Get the min/max for all children
                BOOST_FOREACH(unsigned int idx, face_indices)
                {
                    Face &face = faces->at(idx);
                    sfa::Point minPt,maxPt;
                    face.getBoundingBox(minPt,maxPt);
                    bsp_min = std::min<double>(bsp_min,minPt.X());
                    bsp_max = std::max<double>(bsp_max,maxPt.X());
                }
            }
            else if(axis==1)
            {
                BOOST_FOREACH(unsigned int idx, face_indices)
                {
                    Face &face = faces->at(idx);
                    sfa::Point minPt,maxPt;
                    face.getBoundingBox(minPt,maxPt);
                    bsp_min = std::min<double>(bsp_min,minPt.Y());
                    bsp_max = std::max<double>(bsp_max,maxPt.Y());
                }
            }
            if(axis==2)
            {
                BOOST_FOREACH(unsigned int idx, face_indices)
                {
                    Face &face = faces->at(idx);
                    sfa::Point minPt,maxPt;
                    face.getBoundingBox(minPt,maxPt);
                    bsp_min = std::min<double>(bsp_min,minPt.Z());
                    bsp_max = std::max<double>(bsp_max,maxPt.Z());
                }
            }

            // split the difference
            double split = (bsp_min + bsp_max)/2;
            std::set<unsigned int> new_face_indices;
            if(axis==0)
            {
                BSPNode *left = new BSPNode();
                left->setFaces(faces);
                BSPNode *right = new BSPNode();
                right->setFaces(faces);
                // Is it on the left or right?
                BOOST_FOREACH(unsigned int idx, face_indices)
                {
                    Face &face = faces->at(idx);
                    sfa::Point minPt,maxPt;
                    face.getBoundingBox(minPt,maxPt);

                    bool isLeft = (minPt.X() <= split);
                    bool isRight = (maxPt.X() >= split);

                    if (isLeft && !isRight)
                        left->face_indices.insert(idx);
                    else if (isRight && !isLeft)
                        right->face_indices.insert(idx);
                    else
                        new_face_indices.insert(idx);
                }
                children.push_back(left);
                children.push_back(right);
            }
            else if(axis==1)
            {
                // Is it on top, or bottom?
                BSPNode *top = new BSPNode();
                top->setFaces(faces);
                BSPNode *bottom = new BSPNode();
                bottom->setFaces(faces);
                BOOST_FOREACH(unsigned int idx, face_indices)
                {
                    Face &face = faces->at(idx);
                    sfa::Point minPt,maxPt;
                    face.getBoundingBox(minPt,maxPt);

                    bool isTop = (minPt.Y() <= split);
                    bool isBottom = (maxPt.Y() >= split);

                    if (isTop && !isBottom)
                        top->face_indices.insert(idx);
                    else if (isBottom && !isTop)
                        bottom->face_indices.insert(idx);
                    else
                        new_face_indices.insert(idx);
                }
                children.push_back(top);
                children.push_back(bottom);

            }
            else if(axis==2)
            {
                // Is it on top, or bottom?
                BSPNode *top = new BSPNode();
                top->setFaces(faces);
                BSPNode *bottom = new BSPNode();
                bottom->setFaces(faces);
                BOOST_FOREACH(unsigned int idx, face_indices)
                {
                    Face &face = faces->at(idx);

                    sfa::Point minPt,maxPt;
                    face.getBoundingBox(minPt,maxPt);

                    bool isTop = (minPt.Z() <= split);
                    bool isBottom = (maxPt.Z() >= split);

                    if (isTop && !isBottom)
                        top->face_indices.insert(idx);
                    else if (isBottom && !isTop)
                        bottom->face_indices.insert(idx);
                    else
                        new_face_indices.insert(idx);
                }
                children.push_back(top);
                children.push_back(bottom);

            }
            face_indices.clear();
            face_indices.swap(new_face_indices);

            int new_depth = depth+1;
            if(new_depth < MAX_DEPTH)
            {
                // recurse into all children until someone returns >0
                BOOST_FOREACH(BSPNode *child, children)
                {
                    if(child->face_indices.size()>4)
                    {
                        child->sort(new_depth,(axis+1)%2);// Only split along x/y
                    }
                }
            }
            return true;
        }

        void findFacesWithVert(const sfa::Point &vert, std::vector<int> &indices)
        {
            
            if(axis==0)
            {
                if((bsp_min > vert.X()) ||
                    (bsp_max < vert.X()))
                    return;
            }
            else if(axis==1)
            {
                if((bsp_min > vert.Y()) ||
                    (bsp_max < vert.Y()))
                    return;
            }
            
            BOOST_FOREACH(unsigned int idx, face_indices)
            {
                Face &face = faces->at(idx);
                // test for shared vert
                if(face.hasVertex(vert))
                    indices.push_back(idx);
            }
            // recurse into all children until someone returns >0
            BOOST_FOREACH(BSPNode *child, children)
            {
                child->findFacesWithVert(vert,indices);
            }
            
            return;
        }

        int findFace(double x, double y)
        {

            if(axis==0)
            {
                if((bsp_min > x) ||
                    (bsp_max < x))
                    return -1;
            }
            else if(axis==1)
            {
                if((bsp_min > y) ||
                    (bsp_max < y))
                    return -1;
            }
            sfa::Point pt(x,y);
            BOOST_FOREACH(unsigned int idx, face_indices)
            {
                Face &face = faces->at(idx);
                if(face.interpolatePointInFace(pt))
                {
                    return idx;
                }
            }
            // recurse into all children until someone returns >0
            BOOST_FOREACH(BSPNode *child, children)
            {
                int ret = child->findFace(x,y);
                if(ret>=0)
                    return ret;
            }
            
            return -1;

        }

        void setFaces(std::vector<scenegraph::Face> *face_ref, bool addIndices=false)
        {
            faces = face_ref;
            BOOST_FOREACH(BSPNode *child, children)
            {
                delete child;
            }            
            children.clear();
            face_indices.clear();

            if(addIndices)
            {
                size_t numChildren = face_ref->size();
                for(size_t i=0;i<numChildren;i++)
                    face_indices.insert((int)i);
            }
        }

    };



}