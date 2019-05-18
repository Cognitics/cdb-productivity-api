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

#include "flt/RotateScaleToPoint.h"

namespace flt
{
    RotateScaleToPoint::~RotateScaleToPoint(void)
    {
    }

    RotateScaleToPoint::RotateScaleToPoint(void)
    {
    }

    int RotateScaleToPoint::getRecordType(void)
    {
        return FLT_ROTATESCALETOPOINT;
    }

    std::string RotateScaleToPoint::getRecordName(void)
    {
        return "RotateScaleToPoint";
    }

    void RotateScaleToPoint::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);
        /*   8 */ bs.bind(centerX);
        /*  16 */ bs.bind(centerY);
        /*  24 */ bs.bind(centerZ);
        /*  32 */ bs.bind(referenceX);
        /*  40 */ bs.bind(referenceY);
        /*  48 */ bs.bind(referenceZ);
        /*  56 */ bs.bind(toX);
        /*  64 */ bs.bind(toY);
        /*  72 */ bs.bind(toZ);
        /*  80 */ bs.bind(overallScaleFactor);
        /*  84 */ bs.bind(axisScaleFactor);
        /*  88 */ bs.bind(rotateAngle);
        /*  92 */ bs.bind(RESERVED92);
    }


}
