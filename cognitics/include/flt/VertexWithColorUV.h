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
/*! \file flt/VertexWithColorUV.h
\headerfile flt/VertexWithColorUV.h
\brief Provides flt::VertexWithColorUV
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class VertexWithColorUV : public Record
    {
    public:
        ccl::BigEndian<ccl::uint16_t> nameIndex;
        ccl::LSBitField<16> flags;
        ccl::BigEndian<double> x;
        ccl::BigEndian<double> y;
        ccl::BigEndian<double> z;
        ccl::BigEndian<float> u;
        ccl::BigEndian<float> v;
        ccl::BigEndian<ccl::int32_t> packedColor;
        ccl::BigEndian<ccl::uint32_t> colorIndex;

        virtual ~VertexWithColorUV(void);
        VertexWithColorUV(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


