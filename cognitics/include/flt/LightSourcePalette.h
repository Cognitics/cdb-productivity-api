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
/*! \file flt/LightSourcePalette.h
\headerfile flt/LightSourcePalette.h
\brief Provides flt::LightSourcePalette
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class LightSourcePalette : public Record
    {
    public:
        ccl::BigEndian<ccl::int32_t> index;
        ccl::BigEndian<ccl::int32_t> RESERVED8;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        std::string name;
        ccl::BigEndian<ccl::int32_t> RESERVED36;
        ccl::BigEndian<float> ambientRed;
        ccl::BigEndian<float> ambientGreen;
        ccl::BigEndian<float> ambientBlue;
        ccl::BigEndian<float> ambientAlpha;
        ccl::BigEndian<float> diffuseRed;
        ccl::BigEndian<float> diffuseGreen;
        ccl::BigEndian<float> diffuseBlue;
        ccl::BigEndian<float> diffuseAlpha;
        ccl::BigEndian<float> specularRed;
        ccl::BigEndian<float> specularGreen;
        ccl::BigEndian<float> specularBlue;
        ccl::BigEndian<float> specularAlpha;
        ccl::BigEndian<ccl::int32_t> type;
        ccl::BigEndian<ccl::int32_t> RESERVED92;
        ccl::BigEndian<ccl::int32_t> RESERVED96;
        ccl::BigEndian<ccl::int32_t> RESERVED100;
        ccl::BigEndian<ccl::int32_t> RESERVED104;
        ccl::BigEndian<ccl::int32_t> RESERVED108;
        ccl::BigEndian<ccl::int32_t> RESERVED112;
        ccl::BigEndian<ccl::int32_t> RESERVED116;
        ccl::BigEndian<ccl::int32_t> RESERVED120;
        ccl::BigEndian<ccl::int32_t> RESERVED124;
        ccl::BigEndian<ccl::int32_t> RESERVED128;
        ccl::BigEndian<float> spotExponentialDropoffTerm;
        ccl::BigEndian<float> spotCutoffAngle;
        ccl::BigEndian<float> yaw;
        ccl::BigEndian<float> pitch;
        ccl::BigEndian<float> constantAttenuationCoefficient;
        ccl::BigEndian<float> linearAttenuationCoefficient;
        ccl::BigEndian<float> quadraticAttenuationCoefficient;
        ccl::BigEndian<ccl::int32_t> activeDuringModeling;
        ccl::BigEndian<ccl::int32_t> RESERVED164;
        ccl::BigEndian<ccl::int32_t> RESERVED168;
        ccl::BigEndian<ccl::int32_t> RESERVED172;
        ccl::BigEndian<ccl::int32_t> RESERVED176;
        ccl::BigEndian<ccl::int32_t> RESERVED180;
        ccl::BigEndian<ccl::int32_t> RESERVED184;
        ccl::BigEndian<ccl::int32_t> RESERVED188;
        ccl::BigEndian<ccl::int32_t> RESERVED192;
        ccl::BigEndian<ccl::int32_t> RESERVED196;
        ccl::BigEndian<ccl::int32_t> RESERVED200;
        ccl::BigEndian<ccl::int32_t> RESERVED204;
        ccl::BigEndian<ccl::int32_t> RESERVED208;
        ccl::BigEndian<ccl::int32_t> RESERVED212;
        ccl::BigEndian<ccl::int32_t> RESERVED216;
        ccl::BigEndian<ccl::int32_t> RESERVED220;
        ccl::BigEndian<ccl::int32_t> RESERVED224;
        ccl::BigEndian<ccl::int32_t> RESERVED228;
        ccl::BigEndian<ccl::int32_t> RESERVED232;
        ccl::BigEndian<ccl::int32_t> RESERVED236;

        virtual ~LightSourcePalette(void);
        LightSourcePalette(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


