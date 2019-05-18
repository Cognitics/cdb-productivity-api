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

#include <sfa/Matrix.h>
#include <sfa/Polygon.h>
#include "Face.h"
#include "LineString.h"
#include "LightPoint.h"
#include "ExternalReference.h"
#include "MappedTexture.h"
#include "Relationship.h"
#include "Visitor.h"

namespace scenegraph
{
    class Scene
    {
    public:
        Scene *parent;
        UserData *userData;
        std::vector<Scene *> children;
        std::string name;
        std::string fltobject;
        int ID;
        bool sharpEdges;

        /**
         * @brief    The faces.
         */

        FaceList faces;
        std::vector< std::vector<int> > sectionFaceIndices;
        bool hasVertexNormals;
        LineStringList lineStrings;
        LightPointList lightPoints;
        ExternalReferenceList externalReferences;
        std::vector<Node *> nodes;
        sfa::Matrix matrix;            // TODO: this was renamed from transform, so anything using transform needs to be "matrix" now
        BSPNode bsp;
        ccl::AttributeContainer attributes;

        std::vector<std::string> faceTextures;        // texture list for all children
        std::vector<Material> faceMaterials;        // material list for all children

        std::map<std::string,std::vector<Face *> > stateMap;


        virtual ~Scene();
        Scene(Scene *p = NULL);

        virtual std::string type(void) const;
        virtual Scene *copy(void) const;
        virtual void copyTo(Scene *scene) const;

        void visit(Visitor *visitor);

        MappedTextureList getUniqueTextures();        // TODO: delete
        MaterialList getUniqueMaterials();        // TODO: delete

        virtual void setVertexNormals();        // TODO: visitor

        virtual void getUniqueColors(std::set<Color> &colors);        // TODO: visitor
        virtual void getUniqueTextures(std::set<std::string> &textures);        // TODO: visitor
        virtual void getUniqueMaterials(std::set<Material> &materials);            // TODO: visitor
        
        bool sortFaces();            // TODO: visitor

        bool buildStateFaces(void);

        double InterpolateZForXY(const double x, const double y) const            // TODO: visitor
        {
            sfa::Point pt(x,y,0);
            size_t numFaces = faces.size();
            for(size_t i=0;i<numFaces;i++)
            {
                const Face &face = faces.at(i);
                if(face.interpolatePointInFace(pt))
                {
                    return pt.Z();
                }
            }
            return -32767.0f;
        }

        bool getExtents(double &minX, double &maxX, double &minY, double &maxY, double &minZ, double &maxZ);            // TODO: visitor

        std::string gv(void);
        bool gv(const std::string &filename);

        Scene & operator=(Scene const &in);
        Scene(const Scene& scene);

        std::vector<sfa::Polygon> footprints;

        std::vector<sfa::Polygon> terrainHoles;

        virtual int treeSize(bool counting = false) const;

        virtual void addChild(Scene *child);
    };

}
