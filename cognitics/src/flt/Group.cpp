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

#include "flt/Group.h"

namespace flt
{
    Group::~Group(void)
    {
    }

    Group::Group(void)
        :
        id(""),
        priority(0),
        RESERVED14(0),
        //flags(0),
        specialEffectID1(0),
        specialEffectID2(0),
        significance(0),
        layerCode(0),
        RESERVED27(0),
        RESERVED28(0),
        loopCount(0),
        loopDuration(0),
        lastFrameDuration(0)
    {
    }

    int Group::getRecordType(void)
    {
        return FLT_GROUP;
    }

    std::string Group::getRecordName(void)
    {
        return "Group";
    }

    void Group::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(priority);
        /*  14 */ bs.bind(RESERVED14);
        /*  16 */ bs.bind(flags);
        /*  20 */ bs.bind(specialEffectID1);
        /*  22 */ bs.bind(specialEffectID2);
        /*  24 */ bs.bind(significance);
        /*  26 */ bs.bind(layerCode);
        /*  27 */ bs.bind(RESERVED27);
        /*  28 */ bs.bind(RESERVED28);

        if(revision < 1580)
            return;

        /*  32 */ bs.bind(loopCount);
        /*  36 */ bs.bind(loopDuration);
        /*  40 */ bs.bind(lastFrameDuration);
    }


}
