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

#include "flt/RoadZone.h"

namespace flt
{
    RoadZone::~RoadZone(void)
    {
    }

    RoadZone::RoadZone(void)
    {
    }

    int RoadZone::getRecordType(void)
    {
        return FLT_ROADZONE;
    }

    std::string RoadZone::getRecordName(void)
    {
        return "RoadZone";
    }

    void RoadZone::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(fileName, 120);
        /* 124 */ bs.bind(RESERVED124);
        /* 128 */ bs.bind(lowerLeftX);
        /* 136 */ bs.bind(lowerLeftY);
        /* 144 */ bs.bind(upperRightX);
        /* 152 */ bs.bind(upperRightY);
        /* 160 */ bs.bind(gridInterval);
        /* 168 */ bs.bind(postsAlongX);
        /* 172 */ bs.bind(postsAlongY);
    }


}
