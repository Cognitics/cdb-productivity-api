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

#include "flt/DegreeOfFreedom.h"

namespace flt
{
    DegreeOfFreedom::~DegreeOfFreedom(void)
    {
    }

    DegreeOfFreedom::DegreeOfFreedom(void)
    {
    }

    int DegreeOfFreedom::getRecordType(void)
    {
        return FLT_DEGREEOFFREEDOM;
    }

    std::string DegreeOfFreedom::getRecordName(void)
    {
        return "DegreeOfFreedom";
    }

    void DegreeOfFreedom::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(originX);
        /*  24 */ bs.bind(originY);
        /*  32 */ bs.bind(originZ);
        /*  40 */ bs.bind(xAxisPointX);
        /*  48 */ bs.bind(xAxisPointY);
        /*  56 */ bs.bind(xAxisPointZ);
        /*  64 */ bs.bind(xyPlanePointX);
        /*  72 */ bs.bind(xyPlanePointY);
        /*  80 */ bs.bind(xyPlanePointZ);
        /*  88 */ bs.bind(minZValue);
        /*  96 */ bs.bind(maxZValue);
        /* 104 */ bs.bind(currentZValue);
        /* 112 */ bs.bind(zValueIncrement);
        /* 120 */ bs.bind(minYValue);
        /* 128 */ bs.bind(maxYValue);
        /* 136 */ bs.bind(currentYValue);
        /* 144 */ bs.bind(yValueIncrement);
        /* 152 */ bs.bind(minXValue);
        /* 160 */ bs.bind(maxXValue);
        /* 168 */ bs.bind(currentXValue);
        /* 176 */ bs.bind(xValueIncrement);
        /* 184 */ bs.bind(minPitch);
        /* 192 */ bs.bind(maxPitch);
        /* 200 */ bs.bind(currentPitch);
        /* 208 */ bs.bind(pitchIncrement);
        /* 216 */ bs.bind(minRoll);
        /* 224 */ bs.bind(maxRoll);
        /* 232 */ bs.bind(currentRoll);
        /* 240 */ bs.bind(rollIncrement);
        /* 248 */ bs.bind(minYaw);
        /* 256 */ bs.bind(maxYaw);
        /* 264 */ bs.bind(currentYaw);
        /* 272 */ bs.bind(yawIncrement);
        /* 280 */ bs.bind(minZScale);
        /* 288 */ bs.bind(maxZScale);
        /* 296 */ bs.bind(currentZScale);
        /* 304 */ bs.bind(zScaleIncrement);
        /* 312 */ bs.bind(minYScale);
        /* 320 */ bs.bind(maxYScale);
        /* 328 */ bs.bind(currentYScale);
        /* 336 */ bs.bind(yScaleIncrement);
        /* 344 */ bs.bind(minXScale);
        /* 352 */ bs.bind(maxXScale);
        /* 360 */ bs.bind(currentXScale);
        /* 368 */ bs.bind(xScaleIncrement);
        /* 376 */ bs.bind(flags);
        /* 380 */ bs.bind(RESERVED380);
    }


}
