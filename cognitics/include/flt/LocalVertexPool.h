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
/*! \file flt/LocalVertexPool.h
\headerfile flt/LocalVertexPool.h
\brief Provides flt::LocalVertexPool
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct LocalVertexPoolVertex
    {
        ccl::BigEndian<double> coordinateX;
        ccl::BigEndian<double> coordinateY;
        ccl::BigEndian<double> coordinateZ;
        ccl::BigEndian<ccl::uint32_t> color;
        ccl::BigEndian<float> normalI;
        ccl::BigEndian<float> normalJ;
        ccl::BigEndian<float> normalK;
        ccl::BigEndian<float> uvBaseU;
        ccl::BigEndian<float> uvBaseV;
        ccl::BigEndian<float> uv1U;
        ccl::BigEndian<float> uv1V;
        ccl::BigEndian<float> uv2U;
        ccl::BigEndian<float> uv2V;
        ccl::BigEndian<float> uv3U;
        ccl::BigEndian<float> uv3V;
        ccl::BigEndian<float> uv4U;
        ccl::BigEndian<float> uv4V;
        ccl::BigEndian<float> uv5U;
        ccl::BigEndian<float> uv5V;
        ccl::BigEndian<float> uv6U;
        ccl::BigEndian<float> uv6V;
        ccl::BigEndian<float> uv7U;
        ccl::BigEndian<float> uv7V;
    };

    class LocalVertexPool : public Record
    {
    public:
        ccl::LSBitField<32> attributeMask;
        std::vector<LocalVertexPoolVertex> vertices;

        virtual ~LocalVertexPool(void);
        LocalVertexPool(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


