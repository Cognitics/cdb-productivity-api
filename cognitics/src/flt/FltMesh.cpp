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

#include "flt/Mesh.h"

namespace flt
{
    Mesh::~Mesh(void)
    {
    }

    Mesh::Mesh(void)
    {
    }

    int Mesh::getRecordType(void)
    {
        return FLT_MESH;
    }

    std::string Mesh::getRecordName(void)
    {
        return "Mesh";
    }

    void Mesh::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(irColorCode);
        /*  20 */ bs.bind(priority);
        /*  22 */ bs.bind(drawType);
        /*  23 */ bs.bind(texWhite);
        /*  24 */ bs.bind(colorNameIndex);
        /*  26 */ bs.bind(altColorNameIndex);
        /*  28 */ bs.bind(RESERVED28);
        /*  29 */ bs.bind(templateBillboard);
        /*  30 */ bs.bind(detailTexturePatternIndex);
        /*  32 */ bs.bind(texturePatternIndex);
        /*  34 */ bs.bind(materialIndex);
        /*  36 */ bs.bind(surfaceMaterialCode);
        /*  38 */ bs.bind(featureID);
        /*  40 */ bs.bind(irMaterialCode);
        /*  44 */ bs.bind(transparency);
        /*  46 */ bs.bind(lodGenerationControl);
        /*  47 */ bs.bind(lineStyleIndex);
        /*  48 */ bs.bind(flags);
        /*  52 */ bs.bind(lightMode);
        /*  53 */ bs.bind(RESERVED53, 7);
        /*  60 */ bs.bind(primaryPackedColor);
        /*  64 */ bs.bind(altPackedColor);
        /*  68 */ bs.bind(textureMappingIndex);
        /*  70 */ bs.bind(RESERVED70);
        /*  72 */ bs.bind(primaryColorIndex);
        /*  76 */ bs.bind(altColorIndex);
        /*  80 */ bs.bind(RESERVED80);
        /*  82 */ bs.bind(shaderIndex);
    }


}
