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

#include "flt/Face.h"

namespace flt
{
    Face::~Face(void)
    {
    }

    Face::Face(void)
        : id(""),
        irColorCode(0),
        priority(0),
        drawType(0),
        texWhite(0),
        colorNameIndex(0),
        altColorNameIndex(0),
        RESERVED24(0),
        templateBillboard(0),
        detailTexturePatternIndex(-1),
        texturePatternIndex(-1),
        materialIndex(-1),
        surfaceMaterialCode(0),
        featureID(0),
        irMaterialCode(0),
        transparency(0),
        lodGenerationControl(0),
        lineStyleIndex(0),
        //flags(0),
        lightMode(3),
        RESERVED49(""),
        primaryPackedColor(0xFFFFFFFF),
        altPackedColor(0xFFFFFFFF),
        textureMappingIndex(-1),
        RESERVED66(0),
        primaryColorIndex(-1),
        altColorIndex(-1),
        RESERVED76(0),
        shaderIndex(-1)
    {
    }

    int Face::getRecordType(void)
    {
        return FLT_FACE;
    }

    std::string Face::getRecordName(void)
    {
        return "Face";
    }

    void Face::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(irColorCode);
        /*  16 */ bs.bind(priority);
        /*  18 */ bs.bind(drawType);
        /*  19 */ bs.bind(texWhite);
        /*  20 */ bs.bind(colorNameIndex);
        /*  22 */ bs.bind(altColorNameIndex);
        /*  24 */ bs.bind(RESERVED24);
        /*  25 */ bs.bind(templateBillboard);
        /*  26 */ bs.bind(detailTexturePatternIndex);
        /*  28 */ bs.bind(texturePatternIndex);
        /*  30 */ bs.bind(materialIndex);
        /*  32 */ bs.bind(surfaceMaterialCode);
        /*  34 */ bs.bind(featureID);
        /*  36 */ bs.bind(irMaterialCode);
        /*  40 */ bs.bind(transparency);
        /*  42 */ bs.bind(lodGenerationControl);
        /*  43 */ bs.bind(lineStyleIndex);
        /*  44 */ bs.bind(flags);
        /*  48 */ bs.bind(lightMode);
        /*  49 */ bs.bind(RESERVED49, 7);
        /*  56 */ bs.bind(primaryPackedColor);
        /*  60 */ bs.bind(altPackedColor);
        /*  64 */ bs.bind(textureMappingIndex);
        /*  66 */ bs.bind(RESERVED66);
        /*  68 */ bs.bind(primaryColorIndex);
        /*  72 */ bs.bind(altColorIndex);
        /*  76 */ bs.bind(RESERVED76);
        /*  78 */ bs.bind(shaderIndex);
    }

}
