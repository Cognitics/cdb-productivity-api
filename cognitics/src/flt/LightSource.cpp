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

#include "flt/LightSource.h"

namespace flt
{
    LightSource::~LightSource(void)
    {
    }

    LightSource::LightSource(void)
    {
    }

    int LightSource::getRecordType(void)
    {
        return FLT_LIGHTSOURCE;
    }

    std::string LightSource::getRecordName(void)
    {
        return "LightSource";
    }

    void LightSource::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(lightPaletteIndex);
        /*  20 */ bs.bind(RESERVED20);
        /*  24 */ bs.bind(flags);
        /*  28 */ bs.bind(RESERVED28);
        /*  32 */ bs.bind(positionX);
        /*  40 */ bs.bind(positionY);
        /*  48 */ bs.bind(positionZ);
        /*  56 */ bs.bind(yaw);
        /*  60 */ bs.bind(pitch);
    }


}
