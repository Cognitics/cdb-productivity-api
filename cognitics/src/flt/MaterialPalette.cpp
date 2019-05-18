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

#include "flt/MaterialPalette.h"

namespace flt
{
    MaterialPalette::~MaterialPalette(void)
    {
    }

    MaterialPalette::MaterialPalette(void)
        : index(0),
        name(""),
        //flags(0),
        ambientRed(0),
        ambientGreen(0),
        ambientBlue(0),
        diffuseRed(0),
        diffuseGreen(0),
        diffuseBlue(0),
        specularRed(0),
        specularGreen(0),
        specularBlue(0),
        emissiveRed(0),
        emissiveGreen(0),
        emissiveBlue(0),
        shininess(0),
        alpha(0),
        RESERVED80(0)
    {
    }

    int MaterialPalette::getRecordType(void)
    {
        return FLT_MATERIALPALETTE;
    }

    std::string MaterialPalette::getRecordName(void)
    {
        return "MaterialPalette";
    }

    void MaterialPalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(index);
        /*   8 */ bs.bind(name, 12);
        /*  20 */ bs.bind(flags);
        /*  24 */ bs.bind(ambientRed);
        /*  28 */ bs.bind(ambientGreen);
        /*  32 */ bs.bind(ambientBlue);
        /*  36 */ bs.bind(diffuseRed);
        /*  40 */ bs.bind(diffuseGreen);
        /*  44 */ bs.bind(diffuseBlue);
        /*  48 */ bs.bind(specularRed);
        /*  52 */ bs.bind(specularGreen);
        /*  56 */ bs.bind(specularBlue);
        /*  60 */ bs.bind(emissiveRed);
        /*  64 */ bs.bind(emissiveGreen);
        /*  68 */ bs.bind(emissiveBlue);
        /*  72 */ bs.bind(shininess);
        /*  76 */ bs.bind(alpha);
        /*  80 */ bs.bind(RESERVED80);
    }


}
