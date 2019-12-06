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

//#pragma optimize("", off)

#include "scenegraph/CoordinateTransformVisitor.h"
#include "scenegraph/LOD.h"

namespace scenegraph
{
    CoordinateTransformVisitor::~CoordinateTransformVisitor(void)
    {
    }

    CoordinateTransformVisitor::CoordinateTransformVisitor(cts::CT_MathTransform *ct) : ct(ct)
    {
    }

    void CoordinateTransformVisitor::visiting(Scene *scene)
    {
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            // update face coordinates based on transform
            int numVerts = face.getNumVertices();
            for(int i=0;i<numVerts;i++)
            {
                sfa::Point &pt = face.verts.at(i);
                cts::PT_CoordinatePoint p;
                p.push_back(pt.X());
                p.push_back(pt.Y());
                cts::PT_CoordinatePoint result = ct->transform(p);
                pt.setX(result[0]);
                pt.setY(result[1]);
            }            
        }

        traverse(scene);
    }

    Scene *CoordinateTransformVisitor::transform(Scene *scene)
    {
        visit(scene);
        return scene;
    }


}