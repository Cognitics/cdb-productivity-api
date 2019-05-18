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

#include "flt/LightPointAppearancePalette.h"

namespace flt
{
    LightPointAppearancePalette::~LightPointAppearancePalette(void)
    {
    }

    LightPointAppearancePalette::LightPointAppearancePalette(void)
    {
    }

    int LightPointAppearancePalette::getRecordType(void)
    {
        return FLT_LIGHTPOINTAPPEARANCEPALETTE;
    }

    std::string LightPointAppearancePalette::getRecordName(void)
    {
        return "LightPointAppearancePalette";
    }

    void LightPointAppearancePalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);
        /*   8 */ bs.bind(name, 256);
        /* 264 */ bs.bind(index);
        /* 268 */ bs.bind(surfaceMaterialCode);
        /* 270 */ bs.bind(featureID);
        /* 272 */ bs.bind(backColor);
        /* 276 */ bs.bind(displayMode);
        /* 280 */ bs.bind(intensity);
        /* 284 */ bs.bind(backIntensity);
        /* 288 */ bs.bind(minDefocus);
        /* 292 */ bs.bind(maxDefocus);
        /* 296 */ bs.bind(fadingMode);
        /* 300 */ bs.bind(fogPunchMode);
        /* 304 */ bs.bind(directionalMode);
        /* 308 */ bs.bind(rangeMode);
        /* 312 */ bs.bind(minPixelSize);
        /* 316 */ bs.bind(maxPixelSize);
        /* 320 */ bs.bind(actualSize);
        /* 324 */ bs.bind(transparentFalloffExponent);
        /* 328 */ bs.bind(transparentFalloffScalar);
        /* 332 */ bs.bind(transparentFalloffClamp);
        /* 336 */ bs.bind(fogScalar);
        /* 340 */ bs.bind(fogIntensity);
        /* 344 */ bs.bind(sizeDifferenceThreshold);
        /* 352 */ bs.bind(directionality);
        /* 356 */ bs.bind(horizontalLobeAngle);
        /* 360 */ bs.bind(verticalLobeAngle);
        /* 364 */ bs.bind(lobeRollAngle);
        /* 368 */ bs.bind(directionalFalloffExponent);
        /* 372 */ bs.bind(directionalAmbientIntensity);
        /* 376 */ bs.bind(significance);
        /* 380 */ bs.bind(flags);
        /* 384 */ bs.bind(visibilityRange);
        /* 388 */ bs.bind(fadeRangeRatio);
        /* 392 */ bs.bind(fadeInDuration);
        /* 396 */ bs.bind(fadeOutDuration);
        /* 400 */ bs.bind(lodRangeRatio);
        /* 404 */ bs.bind(lodScale);
        /* 408 */ bs.bind(textureIndex);
        /* 410 */ bs.bind(RESERVED410);
    }


}
