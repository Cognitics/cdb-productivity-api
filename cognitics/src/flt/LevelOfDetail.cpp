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

#include "flt/LevelOfDetail.h"

namespace flt
{
    LevelOfDetail::~LevelOfDetail(void)
    {
    }

    LevelOfDetail::LevelOfDetail(void)
        : id(""),
        RESERVED12(0),
        switchInDistance(0),
        switchOutDistance(0),
        specialEffectID1(0),
        specialEffectID2(0),
        //flags(0),
        centerX(0),
        centerY(0),
        centerZ(0),
        transitionRange(0)
    {
    }

    int LevelOfDetail::getRecordType(void)
    {
        return FLT_LEVELOFDETAIL;
    }

    std::string LevelOfDetail::getRecordName(void)
    {
        return "LevelOfDetail";
    }

    void LevelOfDetail::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(switchInDistance);
        /*  24 */ bs.bind(switchOutDistance);
        /*  32 */ bs.bind(specialEffectID1);
        /*  34 */ bs.bind(specialEffectID2);
        /*  36 */ bs.bind(flags);
        /*  40 */ bs.bind(centerX);
        /*  48 */ bs.bind(centerY);
        /*  56 */ bs.bind(centerZ);
        /*  64 */ bs.bind(transitionRange);

        if(revision < 1580)
            return;

        /*  72 */ bs.bind(significantSize);
    }


}
