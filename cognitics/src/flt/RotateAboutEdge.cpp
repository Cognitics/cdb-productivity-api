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

#include "flt/RotateAboutEdge.h"

namespace flt
{
    RotateAboutEdge::~RotateAboutEdge(void)
    {
    }

    RotateAboutEdge::RotateAboutEdge(void)
    {
    }

    int RotateAboutEdge::getRecordType(void)
    {
        return FLT_ROTATEABOUTEDGE;
    }

    std::string RotateAboutEdge::getRecordName(void)
    {
        return "RotateAboutEdge";
    }

    void RotateAboutEdge::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);
        /*   8 */ bs.bind(firstX);
        /*  16 */ bs.bind(firstY);
        /*  24 */ bs.bind(firstZ);
        /*  32 */ bs.bind(secondX);
        /*  40 */ bs.bind(secondY);
        /*  48 */ bs.bind(secondZ);
        /*  56 */ bs.bind(rotateAngle);
        /*  60 */ bs.bind(RESERVED60);
    }

}
