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
/*! \file flt/DegreeOfFreedom.h
\headerfile flt/DegreeOfFreedom.h
\brief Provides flt::DegreeOfFreedom
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class DegreeOfFreedom : public Record
    {
    public:
        std::string id;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<double> originX;
        ccl::BigEndian<double> originY;
        ccl::BigEndian<double> originZ;
        ccl::BigEndian<double> xAxisPointX;
        ccl::BigEndian<double> xAxisPointY;
        ccl::BigEndian<double> xAxisPointZ;
        ccl::BigEndian<double> xyPlanePointX;
        ccl::BigEndian<double> xyPlanePointY;
        ccl::BigEndian<double> xyPlanePointZ;
        ccl::BigEndian<double> minZValue;
        ccl::BigEndian<double> maxZValue;
        ccl::BigEndian<double> currentZValue;
        ccl::BigEndian<double> zValueIncrement;
        ccl::BigEndian<double> minYValue;
        ccl::BigEndian<double> maxYValue;
        ccl::BigEndian<double> currentYValue;
        ccl::BigEndian<double> yValueIncrement;
        ccl::BigEndian<double> minXValue;
        ccl::BigEndian<double> maxXValue;
        ccl::BigEndian<double> currentXValue;
        ccl::BigEndian<double> xValueIncrement;
        ccl::BigEndian<double> minPitch;
        ccl::BigEndian<double> maxPitch;
        ccl::BigEndian<double> currentPitch;
        ccl::BigEndian<double> pitchIncrement;
        ccl::BigEndian<double> minRoll;
        ccl::BigEndian<double> maxRoll;
        ccl::BigEndian<double> currentRoll;
        ccl::BigEndian<double> rollIncrement;
        ccl::BigEndian<double> minYaw;
        ccl::BigEndian<double> maxYaw;
        ccl::BigEndian<double> currentYaw;
        ccl::BigEndian<double> yawIncrement;
        ccl::BigEndian<double> minZScale;
        ccl::BigEndian<double> maxZScale;
        ccl::BigEndian<double> currentZScale;
        ccl::BigEndian<double> zScaleIncrement;
        ccl::BigEndian<double> minYScale;
        ccl::BigEndian<double> maxYScale;
        ccl::BigEndian<double> currentYScale;
        ccl::BigEndian<double> yScaleIncrement;
        ccl::BigEndian<double> minXScale;
        ccl::BigEndian<double> maxXScale;
        ccl::BigEndian<double> currentXScale;
        ccl::BigEndian<double> xScaleIncrement;
        ccl::LSBitField<32> flags;
        ccl::BigEndian<ccl::int32_t> RESERVED380;

        virtual ~DegreeOfFreedom(void);
        DegreeOfFreedom(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);


    };

}


