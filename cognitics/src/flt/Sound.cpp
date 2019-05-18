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

#include "flt/Sound.h"

namespace flt
{
    Sound::~Sound(void)
    {
    }

    Sound::Sound(void)
    {
    }

    int Sound::getRecordType(void)
    {
        return FLT_SOUND;
    }

    std::string Sound::getRecordName(void)
    {
        return "Sound";
    }

    void Sound::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(soundPaletteIndex);
        /*  20 */ bs.bind(RESERVED20);
        /*  24 */ bs.bind(offsetX);
        /*  32 */ bs.bind(offsetY);
        /*  40 */ bs.bind(offsetZ);
        /*  48 */ bs.bind(soundDirectionI);
        /*  52 */ bs.bind(soundDirectionJ);
        /*  56 */ bs.bind(soundDirectionK);
        /*  60 */ bs.bind(amplitude);
        /*  64 */ bs.bind(pitchBend);
        /*  68 */ bs.bind(priority);
        /*  72 */ bs.bind(falloff);
        /*  76 */ bs.bind(soundLobeWidth);
        /*  80 */ bs.bind(flags);
        /*  84 */ bs.bind(RESERVED84);
    }

}
