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

#include "flt/ExtendedMaterialSpecular.h"

namespace flt
{
    ExtendedMaterialSpecular::~ExtendedMaterialSpecular(void)
    {
    }

    ExtendedMaterialSpecular::ExtendedMaterialSpecular(void)
    {
    }

    int ExtendedMaterialSpecular::getRecordType(void)
    {
        return FLT_EXTENDEDMATERIALSPECULAR;
    }

    std::string ExtendedMaterialSpecular::getRecordName(void)
    {
        return "ExtendedMaterialSpecular";
    }

    void ExtendedMaterialSpecular::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(shininess);
        /*   8 */ bs.bind(colorR);
        /*  12 */ bs.bind(colorG);
        /*  16 */ bs.bind(colorB);
        /*  20 */ bs.bind(textureIndex0);
        /*  24 */ bs.bind(uvSet0);
        /*  28 */ bs.bind(textureIndex1);
        /*  32 */ bs.bind(uvSet1);
        /*  36 */ bs.bind(textureIndex2);
        /*  40 */ bs.bind(uvSet2);
        /*  44 */ bs.bind(textureIndex3);
        /*  48 */ bs.bind(uvSet3);
    }


}
