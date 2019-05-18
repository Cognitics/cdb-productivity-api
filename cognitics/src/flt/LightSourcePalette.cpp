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

#include "flt/LightSourcePalette.h"

namespace flt
{
    LightSourcePalette::~LightSourcePalette(void)
    {
    }

    LightSourcePalette::LightSourcePalette(void)
        : index(0),
        RESERVED8(0),
        RESERVED12(0),
        name(""),
        RESERVED36(0),
        ambientRed(0),
        ambientGreen(0),
        ambientBlue(0),
        ambientAlpha(0),
        diffuseRed(0),
        diffuseGreen(0),
        diffuseBlue(0),
        diffuseAlpha(0),
        specularRed(0),
        specularGreen(0),
        specularBlue(0),
        specularAlpha(0),
        type(0),
        RESERVED92(0),
        RESERVED96(0),
        RESERVED100(0),
        RESERVED104(0),
        RESERVED108(0),
        RESERVED112(0),
        RESERVED116(0),
        RESERVED120(0),
        RESERVED124(0),
        RESERVED128(0),
        spotExponentialDropoffTerm(0),
        spotCutoffAngle(0),
        yaw(0),
        constantAttenuationCoefficient(0),
        linearAttenuationCoefficient(0),
        quadraticAttenuationCoefficient(0),
        activeDuringModeling(0),
        RESERVED164(0),
        RESERVED168(0),
        RESERVED172(0),
        RESERVED176(0),
        RESERVED180(0),
        RESERVED184(0),
        RESERVED188(0),
        RESERVED192(0),
        RESERVED196(0),
        RESERVED200(0),
        RESERVED204(0),
        RESERVED208(0),
        RESERVED212(0),
        RESERVED216(0),
        RESERVED220(0),
        RESERVED224(0),
        RESERVED228(0),
        RESERVED232(0),
        RESERVED236(0)
    {
    }

    int LightSourcePalette::getRecordType(void)
    {
        return FLT_LIGHTSOURCEPALETTE;
    }

    std::string LightSourcePalette::getRecordName(void)
    {
        return "LightSourcePalette";
    }

    void LightSourcePalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(index);
        /*   8 */ bs.bind(RESERVED8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(name, 20);
        /*  36 */ bs.bind(RESERVED36);
        /*  40 */ bs.bind(ambientRed);
        /*  44 */ bs.bind(ambientGreen);
        /*  48 */ bs.bind(ambientBlue);
        /*  52 */ bs.bind(ambientAlpha);
        /*  56 */ bs.bind(diffuseRed);
        /*  60 */ bs.bind(diffuseGreen);
        /*  64 */ bs.bind(diffuseBlue);
        /*  68 */ bs.bind(diffuseAlpha);
        /*  72 */ bs.bind(specularRed);
        /*  76 */ bs.bind(specularGreen);
        /*  80 */ bs.bind(specularBlue);
        /*  84 */ bs.bind(specularAlpha);
        /*  88 */ bs.bind(type);
        /*  92 */ bs.bind(RESERVED92);
        /*  96 */ bs.bind(RESERVED96);
        /* 100 */ bs.bind(RESERVED100);
        /* 104 */ bs.bind(RESERVED104);
        /* 108 */ bs.bind(RESERVED108);
        /* 112 */ bs.bind(RESERVED112);
        /* 116 */ bs.bind(RESERVED116);
        /* 120 */ bs.bind(RESERVED120);
        /* 124 */ bs.bind(RESERVED124);
        /* 128 */ bs.bind(RESERVED128);
        /* 132 */ bs.bind(spotExponentialDropoffTerm);
        /* 136 */ bs.bind(spotCutoffAngle);
        /* 140 */ bs.bind(yaw);
        /* 144 */ bs.bind(pitch);
        /* 148 */ bs.bind(constantAttenuationCoefficient);
        /* 152 */ bs.bind(linearAttenuationCoefficient);
        /* 156 */ bs.bind(quadraticAttenuationCoefficient);
        /* 160 */ bs.bind(activeDuringModeling);
        /* 164 */ bs.bind(RESERVED164);
        /* 168 */ bs.bind(RESERVED168);
        /* 172 */ bs.bind(RESERVED172);
        /* 176 */ bs.bind(RESERVED176);
        /* 180 */ bs.bind(RESERVED180);
        /* 184 */ bs.bind(RESERVED184);
        /* 188 */ bs.bind(RESERVED188);
        /* 192 */ bs.bind(RESERVED192);
        /* 196 */ bs.bind(RESERVED196);
        /* 200 */ bs.bind(RESERVED200);
        /* 204 */ bs.bind(RESERVED204);
        /* 208 */ bs.bind(RESERVED208);
        /* 212 */ bs.bind(RESERVED212);
        /* 216 */ bs.bind(RESERVED216);
        /* 220 */ bs.bind(RESERVED220);
        /* 224 */ bs.bind(RESERVED224);
        /* 228 */ bs.bind(RESERVED228);
        /* 232 */ bs.bind(RESERVED232);
        /* 236 */ bs.bind(RESERVED236);
    }


}
