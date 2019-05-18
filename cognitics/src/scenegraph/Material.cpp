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

#include "scenegraph/Material.h"

namespace scenegraph
{

    Material::Material() : shine(0.0), transparency(0.0) 
    {
    }

    Material::~Material()
    {
    }

    bool Material::operator==(const Material &rhs) const
    {
        return (shine == rhs.shine)
            && (transparency == rhs.transparency)
            && (ambient == rhs.ambient)
            && (diffuse == rhs.diffuse)
            && (specular == rhs.specular)
            && (emission == rhs.emission);
    }

    bool Material::operator< (const Material &rhs) const
    {
        if(shine < rhs.shine)
            return true;
        if(shine > rhs.shine)
            return false;
        if(transparency < rhs.transparency)
            return true;
        if(transparency > rhs.transparency)
            return false;
        if(ambient < rhs.ambient)
            return true;
        if(ambient > rhs.ambient)
            return false;
        if(diffuse < rhs.diffuse)
            return true;
        if(diffuse > rhs.diffuse)
            return false;
        if(specular < rhs.specular)
            return true;
        if(specular > rhs.specular)
            return false;
        return emission < rhs.emission;
    }

} //scenegraph : namespace