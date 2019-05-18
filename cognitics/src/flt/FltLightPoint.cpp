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

#include "flt/LightPoint.h"

namespace flt
{
    LightPoint::~LightPoint(void)
    {
    }

    LightPoint::LightPoint(void)
    {
    }

    int LightPoint::getRecordType(void)
    {
        return FLT_LIGHTPOINT;
    }

    std::string LightPoint::getRecordName(void)
    {
        return "LightPoint";
    }

    void LightPoint::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(surfaceMaterialCode);
        /*  14 */ bs.bind(featureID);
        /*  16 */ bs.bind(backColor);
        /*  20 */ bs.bind(displayMode);
        /*  24 */ bs.bind(intensity);
        /*  28 */ bs.bind(backIntensity);
        /*  32 */ bs.bind(minimumDefocus);
        /*  36 */ bs.bind(maximumDefocus);
        /*  40 */ bs.bind(fadingMode);
        /*  44 */ bs.bind(fogPunchMode);
        /*  48 */ bs.bind(directionalMode);
        /*  52 */ bs.bind(rangeMode);
        /*  56 */ bs.bind(minPixelSize);
        /*  60 */ bs.bind(maxPixelSize);
        /*  64 */ bs.bind(actualSize);
        /*  68 */ bs.bind(transparentFalloffPixelSize);
        /*  72 */ bs.bind(transparentFalloffExponent);
        /*  76 */ bs.bind(transparentFalloffScalar);
        /*  80 */ bs.bind(transparentFalloffClamp);
        /*  84 */ bs.bind(fogScalar);
        /*  88 */ bs.bind(RESERVED88);
        /*  92 */ bs.bind(sizeDifferenceThreshold);
        /*  96 */ bs.bind(directionality);
        /* 100 */ bs.bind(horizontalLobeAngle);
        /* 104 */ bs.bind(verticalLobeAngle);
        /* 108 */ bs.bind(lobeRollAngle);
        /* 112 */ bs.bind(directionalFalloffExponent);
        /* 116 */ bs.bind(directionalAmbientIntensity);
        /* 120 */ bs.bind(animationPeriod);
        /* 124 */ bs.bind(animationPhaseDelay);
        /* 128 */ bs.bind(animationEnabledPeriod);
        /* 132 */ bs.bind(significance);
        /* 136 */ bs.bind(calligraphicDrawOrder);
        /* 140 */ bs.bind(flags);
        /* 144 */ bs.bind(axisOfRotationI);
        /* 148 */ bs.bind(axisOfRotationJ);
        /* 152 */ bs.bind(axisOfRotationK);
    }

}
