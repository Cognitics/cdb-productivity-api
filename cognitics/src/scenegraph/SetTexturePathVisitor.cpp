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

#include "scenegraph/SetTexturePathVisitor.h"
#include "scenegraph/Scene.h"

namespace scenegraph
{
    SetTexturePathVisitor::~SetTexturePathVisitor(void)
    {
    }

    SetTexturePathVisitor::SetTexturePathVisitor(const std::string &path) : texturePath(path)
    {
    }

    void SetTexturePathVisitor::visiting(Scene *scene)
    {
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            Face &face = scene->faces.at(i);
            for(size_t j = 0, jc = face.textures.size(); j < jc; ++j)
            {
                ccl::FileInfo textureFI = ccl::FileInfo(face.textures[j].GetTextureName());
                face.textures[j].SetTextureName(texturePath + "/" + textureFI.getBaseName());
            }
        }

        traverse(scene);
    }

    void SetTexturePathVisitor::setTexturePath(Scene *scene, const std::string &path)
    {
        if(!path.empty())
            texturePath = path;
        visit(scene);
    }


}