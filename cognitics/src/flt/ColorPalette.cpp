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

#include "flt/ColorPalette.h"

namespace flt
{
    ColorPaletteEntry::ColorPaletteEntry(void) : RESERVED2(0), index(0), RESERVED6(0), name("")
    {
    }

    ColorPalette::~ColorPalette(void)
    {
    }

    ColorPalette::ColorPalette(void) : RESERVED4("")
    {
        brightestRGB.resize(1024, 0);
    }

    int ColorPalette::getRecordType(void)
    {
        return FLT_COLORPALETTE;
    }

    std::string ColorPalette::getRecordName(void)
    {
        return "ColorPalette";
    }

    void ColorPalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4, 128);
        brightestRGB.resize(1024);
        for(size_t i = 0; i < 1024; ++i)
            /* 132+(N*4) */ bs.bind(brightestRGB[i]);

        if(bs.writing())
        {
            if(!entries.size())
                return;
        }
        else
        {
            if(length <= 4228)
                return;
        }

        ccl::BigEndian<ccl::int16_t> count = ccl::int16_t(entries.size());    // overwritten if reading
        /* 4228 */ bs.bind(count);
        entries.resize(count);
        for(int i = 0, n = count; i < n; ++i)
        {
            ccl::BigEndian<ccl::int16_t> len;
            if(bs.writing())
            {
                if(entries[i].name.size() > 79)
                    entries[i].name.resize(79);
                len = ccl::int16_t(entries[i].name.size()) + 1;
            }
            bs.bind(len);
            bs.bind(entries[i].RESERVED2);
            bs.bind(entries[i].index);
            bs.bind(entries[i].RESERVED6);
            bs.bind(entries[i].name, len - 8 - 1);
            bs.fill(1);        // null termination
        }
    }


}
