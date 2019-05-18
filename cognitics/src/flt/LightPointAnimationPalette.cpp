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

#include "flt/LightPointAnimationPalette.h"

namespace flt
{
    LightPointAnimationPalette::~LightPointAnimationPalette(void)
    {
    }

    LightPointAnimationPalette::LightPointAnimationPalette(void)
    {
    }

    int LightPointAnimationPalette::getRecordType(void)
    {
        return FLT_LIGHTPOINTANIMATIONPALETTE;
    }

    std::string LightPointAnimationPalette::getRecordName(void)
    {
        return "LightPointAnimationPalette";
    }

    void LightPointAnimationPalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);
        /*   8 */ bs.bind(name, 256);
        /* 264 */ bs.bind(index);
        /* 268 */ bs.bind(period);
        /* 272 */ bs.bind(phaseDelay);
        /* 276 */ bs.bind(enabledPeriod);
        /* 280 */ bs.bind(rotationI);
        /* 284 */ bs.bind(rotationJ);
        /* 288 */ bs.bind(rotationK);
        /* 292 */ bs.bind(flags);
        /* 296 */ bs.bind(type);
        /* 300 */ bs.bind(morseCodeTiming);
        /* 304 */ bs.bind(wordRate);
        /* 308 */ bs.bind(characterRate);
        /* 312 */ bs.bind(morseCodeString, 1024);

        ccl::BigEndian<ccl::int32_t> numberOfSequences = ccl::int32_t(sequences.size());
        /* 1336 */ bs.bind(numberOfSequences);

        for(ccl::int32_t i = 0, n = numberOfSequences; i < n; ++i)
        {
            /* 1340+(N*12) */ bs.bind(sequences[i].state);
            /* 1344+(N*12) */ bs.bind(sequences[i].duration);
            /* 1348+(N*12) */ bs.bind(sequences[i].color);
        }
    }


}
