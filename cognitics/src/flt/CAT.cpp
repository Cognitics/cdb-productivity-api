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

#include "flt/CAT.h"

namespace flt
{
    CAT::~CAT(void)
    {
    }

    CAT::CAT(void)
    {
    }

    int CAT::getRecordType(void)
    {
        return FLT_CAT;
    }

    std::string CAT::getRecordName(void)
    {
        return "CAT";
    }

    void CAT::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(irColorCode);
        /*  20 */ bs.bind(drawType);
        /*  21 */ bs.bind(texWhite);
        /*  22 */ bs.bind(RESERVED22);
        /*  24 */ bs.bind(colorNameIndex);
        /*  26 */ bs.bind(altColorNameIndex);
        /*  28 */ bs.bind(detailTexturePatternIndex);
        /*  30 */ bs.bind(texturePatternIndex);
        /*  32 */ bs.bind(materialIndex);
        /*  34 */ bs.bind(surfaceMaterial);
        /*  36 */ bs.bind(irMaterialCode);
        /*  40 */ bs.bind(RESERVED40);
        /*  44 */ bs.bind(RESERVED44);
        /*  48 */ bs.bind(textureMappingIndex);
        /*  50 */ bs.bind(RESERVED50);
        /*  52 */ bs.bind(primaryColorIndex);
        /*  56 */ bs.bind(altColorIndex);
        /*  60 */ bs.bind(RESERVED60);
        /*  64 */ bs.bind(RESERVED64);
        /*  72 */ bs.bind(flags);
        /*  76 */ bs.bind(RESERVED76);
    }


}
