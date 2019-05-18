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

#include "scenegraph/TransformVisitor.h"
#include "scenegraph/LOD.h"

namespace scenegraph
{
    TransformVisitor::~TransformVisitor(void)
    {
    }

    TransformVisitor::TransformVisitor(sfa::Matrix _transform) : mat(_transform)
    {
    }

    void TransformVisitor::visiting(Scene *scene)
    {
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            // update face coordinates based on transform
            int numVerts = face.getNumVertices();
            for(int i=0;i<numVerts;i++)
            {
                sfa::Point pt = face.getVertN(i);
                pt = mat * pt;
                face.setVertN(i,pt);
                sfa::Point n = face.getNormalN(i);
                n = mat * n;
                face.setNormalN(i,n);
            }            
        }

        for(size_t i = 0, c = scene->externalReferences.size(); i < c; ++i)
        {
            ExternalReference &externalReference = scene->externalReferences.at(i);

            // consolidate transform
            sfa::Matrix m;
            m.PushRotate(externalReference.attitude);
            m.PushScale(externalReference.scale);
            m.PushTranslate(externalReference.position);
            sfa::Matrix extMatrix = mat * m;
            externalReference.attitude = extMatrix.getRotation();
            externalReference.position = extMatrix.getTranslation();
            externalReference.scale = extMatrix.getScale();
        }
        traverse(scene);
    }

    Scene *TransformVisitor::transform(Scene *scene)
    {
        visit(scene);
        return scene;
    }


}