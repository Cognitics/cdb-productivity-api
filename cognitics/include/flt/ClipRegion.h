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
/*! \file flt/ClipRegion.h
\headerfile flt/ClipRegion.h
\brief Provides flt::ClipRegion
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class ClipRegion : public Record
    {
    public:
        std::string id;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<ccl::int16_t> RESERVED16;
        char edgeFlag0;
        char edgeFlag1;
        char edgeFlag2;
        char edgeFlag3;
        char edgeFlag4;
        ccl::int8_t RESERVED23;
        ccl::BigEndian<double> clipRegionCoordinate1X;
        ccl::BigEndian<double> clipRegionCoordinate1Y;
        ccl::BigEndian<double> clipRegionCoordinate1Z;
        ccl::BigEndian<double> clipRegionCoordinate2X;
        ccl::BigEndian<double> clipRegionCoordinate2Y;
        ccl::BigEndian<double> clipRegionCoordinate2Z;
        ccl::BigEndian<double> clipRegionCoordinate3X;
        ccl::BigEndian<double> clipRegionCoordinate3Y;
        ccl::BigEndian<double> clipRegionCoordinate3Z;
        ccl::BigEndian<double> clipRegionCoordinate4X;
        ccl::BigEndian<double> clipRegionCoordinate4Y;
        ccl::BigEndian<double> clipRegionCoordinate4Z;
        ccl::BigEndian<double> planeCoefficientA0;
        ccl::BigEndian<double> planeCoefficientA1;
        ccl::BigEndian<double> planeCoefficientA2;
        ccl::BigEndian<double> planeCoefficientA3;
        ccl::BigEndian<double> planeCoefficientA4;
        ccl::BigEndian<double> planeCoefficientB0;
        ccl::BigEndian<double> planeCoefficientB1;
        ccl::BigEndian<double> planeCoefficientB2;
        ccl::BigEndian<double> planeCoefficientB3;
        ccl::BigEndian<double> planeCoefficientB4;
        ccl::BigEndian<double> planeCoefficientC0;
        ccl::BigEndian<double> planeCoefficientC1;
        ccl::BigEndian<double> planeCoefficientC2;
        ccl::BigEndian<double> planeCoefficientC3;
        ccl::BigEndian<double> planeCoefficientC4;
        ccl::BigEndian<double> planeCoefficientD0;
        ccl::BigEndian<double> planeCoefficientD1;
        ccl::BigEndian<double> planeCoefficientD2;
        ccl::BigEndian<double> planeCoefficientD3;
        ccl::BigEndian<double> planeCoefficientD4;

        virtual ~ClipRegion(void);
        ClipRegion(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };


}


