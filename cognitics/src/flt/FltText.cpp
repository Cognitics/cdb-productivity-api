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

#include "flt/Text.h"

namespace flt
{
    Text::~Text(void)
    {
    }

    Text::Text(void)
    {
    }

    int Text::getRecordType(void)
    {
        return FLT_TEXT;
    }

    std::string Text::getRecordName(void)
    {
        return "Text";
    }

    void Text::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(RESERVED16);
        /*  20 */ bs.bind(type);
        /*  24 */ bs.bind(drawType);
        /*  28 */ bs.bind(justification);
        /*  32 */ bs.bind(floatingPointValue);
        /*  40 */ bs.bind(integerValue);
        /*  44 */ bs.bind(RESERVED44);
        /*  48 */ bs.bind(RESERVED48);
        /*  52 */ bs.bind(RESERVED52);
        /*  56 */ bs.bind(RESERVED56);
        /*  60 */ bs.bind(RESERVED60);
        /*  64 */ bs.bind(flags);
        /*  68 */ bs.bind(color);
        /*  72 */ bs.bind(color2);
        /*  76 */ bs.bind(material);
        /*  80 */ bs.bind(RESERVED80);
        /*  84 */ bs.bind(maxLines);
        /*  88 */ bs.bind(maxCharacters);
        /*  92 */ bs.bind(lengthOfText);
        /*  96 */ bs.bind(nextLineNumber);
        /* 100 */ bs.bind(topLineNumber);
        /* 104 */ bs.bind(lowIntegerValue);
        /* 108 */ bs.bind(highIntegerValue);
        /* 112 */ bs.bind(lowFloatValue);
        /* 120 */ bs.bind(highFloatValue);
        /* 128 */ bs.bind(lowerLeftX);
        /* 136 */ bs.bind(lowerLeftY);
        /* 144 */ bs.bind(lowerLeftZ);
        /* 152 */ bs.bind(upperRightX);
        /* 160 */ bs.bind(upperRightY);
        /* 168 */ bs.bind(upperRightZ);
        /* 176 */ bs.bind(fontName, 120);
        /* 296 */ bs.bind(drawVertical);
        /* 300 */ bs.bind(drawItalic);
        /* 304 */ bs.bind(drawBold);
        /* 308 */ bs.bind(drawUnderline);
        /* 312 */ bs.bind(lineStyle);
        /* 316 */ bs.bind(RESERVED316);
    }

}
