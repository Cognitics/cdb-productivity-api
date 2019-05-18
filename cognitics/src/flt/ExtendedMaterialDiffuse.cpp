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

#include "flt/ExtendedMaterialDiffuse.h"

namespace flt
{
    ExtendedMaterialDiffuse::~ExtendedMaterialDiffuse(void)
    {
    }

    ExtendedMaterialDiffuse::ExtendedMaterialDiffuse(void)
    {
    }

    int ExtendedMaterialDiffuse::getRecordType(void)
    {
        return FLT_EXTENDEDMATERIALDIFFUSE;
    }

    std::string ExtendedMaterialDiffuse::getRecordName(void)
    {
        return "ExtendedMaterialDiffuse";
    }

    void ExtendedMaterialDiffuse::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(colorR);
        /*   8 */ bs.bind(colorG);
        /*  12 */ bs.bind(colorB);
        /*  16 */ bs.bind(textureIndex0);
        /*  20 */ bs.bind(uvSet0);
        /*  24 */ bs.bind(textureIndex1);
        /*  28 */ bs.bind(uvSet1);
        /*  32 */ bs.bind(textureIndex2);
        /*  36 */ bs.bind(uvSet2);
        /*  40 */ bs.bind(textureIndex3);
        /*  44 */ bs.bind(uvSet3);
    }


}
