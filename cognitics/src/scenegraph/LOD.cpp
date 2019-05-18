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

#include "scenegraph/LOD.h"

namespace scenegraph
{
    LOD::LOD(Scene* p) : Scene(p)
    {
    }

    LOD::LOD(const LOD &rhs)
    {
        rhs.copyTo(this);
    }

    LOD &LOD::operator=(const LOD &rhs)
    {
        if(this != &rhs)
            rhs.copyTo(this);
        return *this;
    }

    LOD::~LOD(void)
    {
    }

    LOD *LOD::copy(void) const
    {
        LOD *lod = new LOD;
        copyTo(lod);
        return lod;
    }

    void LOD::copyTo(LOD *lod) const
    {
        Scene::copyTo(lod);
        lod->ranges = ranges;
        lod->center = center;
    }

    std::string LOD::type(void) const
    {
        return "LOD";
    }

    Scene* LOD::AddChild(double min, double max)
    {
        //TODO: group is created, and a range is created, but group is
        // never added as a child in the Scene::children member. This
        // whole function is confusing, and the way it is used in the modelgenerator
        // is very confusing. It appreas it creates the range without actually
        // adding a child.
        Scene* group = new Scene(this);
        LODRange r;
        r.min = min;
        r.max = max;
        ranges.push_back(r);
        group->parent = this;
        return group;
    }
    
    void LOD::addChild(Scene *child)
    {
        child->parent = this;
        LODRange r;
        r.min = 0;
        r.max = DBL_MAX;
        ranges.push_back(r);
        children.push_back(child);
    }
}
