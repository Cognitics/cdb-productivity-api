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

#include "scenegraph/ExtentsVisitor.h"

namespace scenegraph
{
    ExtentsVisitor::~ExtentsVisitor(void)
    {
    }

    ExtentsVisitor::ExtentsVisitor(void) : result(false), minX(DBL_MAX), maxX(-DBL_MAX), minY(DBL_MAX), maxY(-DBL_MAX), minZ(DBL_MAX), maxZ(-DBL_MAX)
    {
    }

    void ExtentsVisitor::visiting(Scene *scene)
    {
        result = true;
        for(size_t i = 0, c = scene->faces.size(); i < c; ++i)
        {
            sfa::Point min, max;
            scene->faces[i].getBoundingBox(min, max);
            minX = std::min<double>(minX, min.X());
            maxX = std::max<double>(maxX, max.X());
            minY = std::min<double>(minY, min.Y());
            maxY = std::max<double>(maxY, max.Y());
            minZ = std::min<double>(minZ, min.Z());
            maxZ = std::max<double>(maxZ, max.Z());
        }
        traverse(scene);
    }

    bool ExtentsVisitor::getExtents(double &minX, double &maxX, double &minY, double &maxY, double &minZ, double &maxZ)
    {
        minX = this->minX;
        maxX = this->maxX;
        minY = this->minY;
        maxY = this->maxY;
        minZ = this->minZ;
        maxZ = this->maxZ;
        return result;
    }

}