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

#include "flt/ClipRegion.h"

namespace flt
{
    ClipRegion::~ClipRegion(void)
    {
    }

    ClipRegion::ClipRegion(void)
    {
    }

    int ClipRegion::getRecordType(void)
    {
        return FLT_CLIPREGION;
    }

    std::string ClipRegion::getRecordName(void)
    {
        return "ClipRegion";
    }

    void ClipRegion::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(RESERVED16);
        /*  18 */ bs.bind(edgeFlag0);
        /*  19 */ bs.bind(edgeFlag1);
        /*  20 */ bs.bind(edgeFlag2);
        /*  21 */ bs.bind(edgeFlag3);
        /*  22 */ bs.bind(edgeFlag4);
        /*  23 */ bs.bind(RESERVED23);
        /*  24 */ bs.bind(clipRegionCoordinate1X);
        /*  32 */ bs.bind(clipRegionCoordinate1Y);
        /*  40 */ bs.bind(clipRegionCoordinate1Z);
        /*  48 */ bs.bind(clipRegionCoordinate2X);
        /*  56 */ bs.bind(clipRegionCoordinate2Y);
        /*  64 */ bs.bind(clipRegionCoordinate2Z);
        /*  72 */ bs.bind(clipRegionCoordinate3X);
        /*  80 */ bs.bind(clipRegionCoordinate3Y);
        /*  88 */ bs.bind(clipRegionCoordinate3Z);
        /*  96 */ bs.bind(clipRegionCoordinate4X);
        /* 104 */ bs.bind(clipRegionCoordinate4Y);
        /* 112 */ bs.bind(clipRegionCoordinate4Z);
        /* 120 */ bs.bind(planeCoefficientA0);
        /* 128 */ bs.bind(planeCoefficientA1);
        /* 136 */ bs.bind(planeCoefficientA2);
        /* 144 */ bs.bind(planeCoefficientA3);
        /* 152 */ bs.bind(planeCoefficientA4);
        /* 160 */ bs.bind(planeCoefficientB0);
        /* 168 */ bs.bind(planeCoefficientB1);
        /* 176 */ bs.bind(planeCoefficientB2);
        /* 184 */ bs.bind(planeCoefficientB3);
        /* 192 */ bs.bind(planeCoefficientB4);
        /* 200 */ bs.bind(planeCoefficientC0);
        /* 208 */ bs.bind(planeCoefficientC1);
        /* 216 */ bs.bind(planeCoefficientC2);
        /* 224 */ bs.bind(planeCoefficientC3);
        /* 232 */ bs.bind(planeCoefficientC4);
        /* 240 */ bs.bind(planeCoefficientD0);
        /* 248 */ bs.bind(planeCoefficientD1);
        /* 256 */ bs.bind(planeCoefficientD2);
        /* 264 */ bs.bind(planeCoefficientD3);
        /* 272 */ bs.bind(planeCoefficientD4);
    }


}
