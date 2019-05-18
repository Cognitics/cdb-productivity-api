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

#include "flt/Put.h"

namespace flt
{
    Put::~Put(void)
    {
    }

    Put::Put(void)
    {
    }

    int Put::getRecordType(void)
    {
        return FLT_PUT;
    }

    std::string Put::getRecordName(void)
    {
        return "Put";
    }

    void Put::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);
        /*   8 */ bs.bind(fromOriginX);
        /*  16 */ bs.bind(fromOriginY);
        /*  24 */ bs.bind(fromOriginZ);
        /*  32 */ bs.bind(fromAlignX);
        /*  40 */ bs.bind(fromAlignY);
        /*  48 */ bs.bind(fromAlignZ);
        /*  56 */ bs.bind(fromTrackX);
        /*  64 */ bs.bind(fromTrackY);
        /*  72 */ bs.bind(fromTrackZ);
        /*  80 */ bs.bind(toOriginX);
        /*  88 */ bs.bind(toOriginY);
        /*  96 */ bs.bind(toOriginZ);
        /* 104 */ bs.bind(toAlignX);
        /* 112 */ bs.bind(toAlignY);
        /* 120 */ bs.bind(toAlignZ);
        /* 128 */ bs.bind(toTrackX);
        /* 136 */ bs.bind(toTrackY);
        /* 144 */ bs.bind(toTrackZ);
    }

}
