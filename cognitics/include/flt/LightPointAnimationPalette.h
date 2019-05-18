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
/*! \file flt/LightPointAnimationPalette.h
\headerfile flt/LightPointAnimationPalette.h
\brief Provides flt::LightPointAnimationPalette
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct LightPointAnimationPaletteSequence
    {
        ccl::BigEndian<ccl::uint32_t> state;
        ccl::BigEndian<float> duration;
        ccl::BigEndian<ccl::uint32_t> color;
    };

    class LightPointAnimationPalette : public Record
    {
    public:
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        std::string name;
        ccl::BigEndian<ccl::int32_t> index;
        ccl::BigEndian<float> period;
        ccl::BigEndian<float> phaseDelay;
        ccl::BigEndian<float> enabledPeriod;
        ccl::BigEndian<float> rotationI;
        ccl::BigEndian<float> rotationJ;
        ccl::BigEndian<float> rotationK;
        ccl::LSBitField<32> flags;
        ccl::BigEndian<ccl::int32_t> type;
        ccl::BigEndian<ccl::int32_t> morseCodeTiming;
        ccl::BigEndian<ccl::int32_t> wordRate;
        ccl::BigEndian<ccl::int32_t> characterRate;
        std::string morseCodeString;
        std::vector<LightPointAnimationPaletteSequence> sequences;

        virtual ~LightPointAnimationPalette(void);
        LightPointAnimationPalette(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


