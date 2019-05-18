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
/*! \file flt/LightPoint.h
\headerfile flt/LightPoint.h
\brief Provides flt::LightPoint
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class LightPoint : public Record
    {
    public:
        std::string id;
        ccl::BigEndian<ccl::int16_t> surfaceMaterialCode;
        ccl::BigEndian<ccl::int16_t> featureID;
        ccl::BigEndian<ccl::uint32_t> backColor;
        ccl::BigEndian<ccl::int32_t> displayMode;
        ccl::BigEndian<float> intensity;
        ccl::BigEndian<float> backIntensity;
        ccl::BigEndian<float> minimumDefocus;
        ccl::BigEndian<float> maximumDefocus;
        ccl::BigEndian<ccl::int32_t> fadingMode;
        ccl::BigEndian<ccl::int32_t> fogPunchMode;
        ccl::BigEndian<ccl::int32_t> directionalMode;
        ccl::BigEndian<ccl::int32_t> rangeMode;
        ccl::BigEndian<float> minPixelSize;
        ccl::BigEndian<float> maxPixelSize;
        ccl::BigEndian<float> actualSize;
        ccl::BigEndian<float> transparentFalloffPixelSize;
        ccl::BigEndian<float> transparentFalloffExponent;
        ccl::BigEndian<float> transparentFalloffScalar;
        ccl::BigEndian<float> transparentFalloffClamp;
        ccl::BigEndian<float> fogScalar;
        ccl::BigEndian<float> RESERVED88;
        ccl::BigEndian<float> sizeDifferenceThreshold;
        ccl::BigEndian<ccl::int32_t> directionality;
        ccl::BigEndian<float> horizontalLobeAngle;
        ccl::BigEndian<float> verticalLobeAngle;
        ccl::BigEndian<float> lobeRollAngle;
        ccl::BigEndian<float> directionalFalloffExponent;
        ccl::BigEndian<float> directionalAmbientIntensity;
        ccl::BigEndian<float> animationPeriod;
        ccl::BigEndian<float> animationPhaseDelay;
        ccl::BigEndian<float> animationEnabledPeriod;
        ccl::BigEndian<float> significance;
        ccl::BigEndian<float> calligraphicDrawOrder;
        ccl::LSBitField<32> flags;
        ccl::BigEndian<float> axisOfRotationI;
        ccl::BigEndian<float> axisOfRotationJ;
        ccl::BigEndian<float> axisOfRotationK;

        virtual ~LightPoint(void);
        LightPoint(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


