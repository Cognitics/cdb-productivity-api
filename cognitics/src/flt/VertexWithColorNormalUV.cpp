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

#include "flt/VertexWithColorNormalUV.h"

namespace flt
{
    VertexWithColorNormalUV::~VertexWithColorNormalUV(void)
    {
    }

    VertexWithColorNormalUV::VertexWithColorNormalUV(void)
        : nameIndex(0),
        //flags(0),
        x(0),
        y(0),
        z(0),
        i(0),
        j(0),
        k(0),
        u(0),
        v(0),
        packedColor(0),
        colorIndex(-1),
        RESERVED60(0)
    {
    }

    int VertexWithColorNormalUV::getRecordType(void)
    {
        return FLT_VERTEXWITHCOLORNORMALUV;
    }

    std::string VertexWithColorNormalUV::getRecordName(void)
    {
        return "VertexWithColorNormalUV";
    }

    void VertexWithColorNormalUV::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(nameIndex);
        /*   6 */ bs.bind(flags);
        /*   8 */ bs.bind(x);
        /*  16 */ bs.bind(y);
        /*  24 */ bs.bind(z);
        /*  32 */ bs.bind(i);
        /*  36 */ bs.bind(j);
        /*  40 */ bs.bind(k);
        /*  44 */ bs.bind(u);
        /*  48 */ bs.bind(v);
        /*  52 */ bs.bind(packedColor);
        /*  56 */ bs.bind(colorIndex);
        /*  60 */ bs.bind(RESERVED60);
    }

}
