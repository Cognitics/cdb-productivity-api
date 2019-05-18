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

#include "scenegraph/FlattenVisitor.h"
#include "scenegraph/LOD.h"

namespace scenegraph
{
    FlattenVisitor::~FlattenVisitor(void)
    {
    }

    FlattenVisitor::FlattenVisitor(double distance) : distance(distance), resultScene(NULL)
    {
    }

    void FlattenVisitor::visiting(Scene *scene)
    {
        if(scene->type() == "LOD")
        {
            bool cull = true;
            LOD *lod = dynamic_cast<LOD *>(scene);
            for(size_t i = 0, c = lod->ranges.size(); i < c; ++i)
            {
                if((distance >= lod->ranges[i].min) && (distance <= lod->ranges[i].max))
                    cull = false;
            }
            if(cull)
                return;
        }

        // we need to be able to iterate from the root scene to this scene, so we create a stack
        std::vector<Scene *> groupStack;
        for(Scene *group = scene->parent; group != NULL; group = group->parent)
            groupStack.push_back(group);
        sfa::Matrix matrix;
        for(size_t j = 0, jc = groupStack.size(); j < jc; ++j)
            matrix = matrix * groupStack[j]->matrix;
        matrix = matrix * scene->matrix;
        // matrix is now our current transform from identity

        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face face = scene->faces.at(i);        // copy assignment

            // consolidate attributes
            for(size_t j = 0, jc = groupStack.size(); j < jc; ++j)
            {
                std::vector<std::string> keys = groupStack[j]->attributes.getKeys();
                for(size_t k = 0, kc = keys.size(); k < kc; ++k)
                    face.attributes.setAttribute(keys.at(k), groupStack[j]->attributes.getAttributeAsVariant(keys.at(k)));
            }
            std::vector<std::string> keys = scene->faces.at(i).attributes.getKeys();        // re-copy the face attributes since we may have overwritten them
            for(size_t k = 0, kc = keys.size(); k < kc; ++k)
                face.attributes.setAttribute(keys.at(k), scene->faces.at(i).attributes.getAttributeAsVariant(keys.at(k)));

            
            int numVerts = face.getNumVertices();
            for(int i=0;i<numVerts;i++)
            {
                sfa::Point pt = face.getVertN(i);
                pt = matrix * pt;
                face.setVertN(i,pt);
                sfa::Point n = face.getNormalN(i);
                n = matrix * n;
                face.setNormalN(i,n);
            }
            

            resultScene->faces.push_back(face);
        }

        for(size_t i = 0, c = scene->externalReferences.size(); i < c; ++i)
        {
            ExternalReference externalReference = scene->externalReferences.at(i);        // copy assignment

            // consolidate transform
            sfa::Matrix m;
            m.PushRotate(externalReference.attitude);
            m.PushScale(externalReference.scale);
            m.PushTranslate(externalReference.position);
            sfa::Matrix extMatrix = matrix * m;
            externalReference.attitude = extMatrix.getRotation();
            externalReference.position = extMatrix.getTranslation();
            externalReference.scale = extMatrix.getScale();

            resultScene->externalReferences.push_back(externalReference);
        }

        traverse(scene);
    }

    Scene *FlattenVisitor::getResultScene(void)
    {
        return resultScene;
    }

    Scene *FlattenVisitor::flatten(Scene *scene)
    {
        if(resultScene)
            delete resultScene;
        resultScene = new Scene;        // resultScene will have the identity matrix
        visit(scene);
        return getResultScene();
    }


}