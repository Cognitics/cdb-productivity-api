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
/*! \file flt/MultiTexture.h
\headerfile flt/MultiTexture.h
\brief Provides flt::MultiTexture
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct MultiTextureLayer
    {
        ccl::BigEndian<ccl::uint16_t> index;
        ccl::BigEndian<ccl::uint16_t> effect;
        ccl::BigEndian<ccl::uint16_t> mapping;
        ccl::BigEndian<ccl::uint16_t> data;
    };

    class MultiTexture : public Record
    {
    public:
        ccl::LSBitField<32> attributeMask;
        ccl::BigEndian<ccl::uint16_t> index1;
        ccl::BigEndian<ccl::uint16_t> effect1;
        ccl::BigEndian<ccl::uint16_t> mapping1;
        ccl::BigEndian<ccl::uint16_t> data1;
        ccl::BigEndian<ccl::uint16_t> index2;
        ccl::BigEndian<ccl::uint16_t> effect2;
        ccl::BigEndian<ccl::uint16_t> mapping2;
        ccl::BigEndian<ccl::uint16_t> data2;
        ccl::BigEndian<ccl::uint16_t> index3;
        ccl::BigEndian<ccl::uint16_t> effect3;
        ccl::BigEndian<ccl::uint16_t> mapping3;
        ccl::BigEndian<ccl::uint16_t> data3;
        ccl::BigEndian<ccl::uint16_t> index4;
        ccl::BigEndian<ccl::uint16_t> effect4;
        ccl::BigEndian<ccl::uint16_t> mapping4;
        ccl::BigEndian<ccl::uint16_t> data4;
        ccl::BigEndian<ccl::uint16_t> index5;
        ccl::BigEndian<ccl::uint16_t> effect5;
        ccl::BigEndian<ccl::uint16_t> mapping5;
        ccl::BigEndian<ccl::uint16_t> data5;
        ccl::BigEndian<ccl::uint16_t> index6;
        ccl::BigEndian<ccl::uint16_t> effect6;
        ccl::BigEndian<ccl::uint16_t> mapping6;
        ccl::BigEndian<ccl::uint16_t> data6;
        ccl::BigEndian<ccl::uint16_t> index7;
        ccl::BigEndian<ccl::uint16_t> effect7;
        ccl::BigEndian<ccl::uint16_t> mapping7;
        ccl::BigEndian<ccl::uint16_t> data7;

        virtual ~MultiTexture(void);
        MultiTexture(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


