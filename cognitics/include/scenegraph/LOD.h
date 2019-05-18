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

#include "Scene.h"
#include <sfa/Point.h>

namespace scenegraph
{
    struct LODRange 
    {
        double min;
        double max;
        LODRange(double min = 0.0f, double max = 0.0f) { this->min = min; this->max = max; }
    };

    typedef std::vector<LODRange> LODRangeList;

    class LOD : public Scene
    {
    public:
        LODRangeList ranges;
        sfa::Point center;

        LOD(Scene *p = NULL);
        LOD(const LOD &rhs);
        LOD &operator=(const LOD &rhs);
        virtual ~LOD(void);

        virtual LOD *copy(void) const;
        virtual void copyTo(LOD *lod) const;
        virtual std::string type(void) const;
        virtual Scene* AddChild(double min, double max);
        // Add a child and create the LOD range entry for it
        virtual void addChild(Scene *child);
    };

}
