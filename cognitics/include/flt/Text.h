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
/*! \file flt/Text.h
\headerfile flt/Text.h
\brief Provides flt::Text
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class Text : public Record
    {
    public:
        std::string id;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<ccl::int32_t> RESERVED16;
        ccl::BigEndian<ccl::int32_t> type;
        ccl::BigEndian<ccl::int32_t> drawType;
        ccl::BigEndian<ccl::int32_t> justification;
        ccl::BigEndian<double> floatingPointValue;
        ccl::BigEndian<ccl::int32_t> integerValue;
        ccl::BigEndian<ccl::int32_t> RESERVED44;
        ccl::BigEndian<ccl::int32_t> RESERVED48;
        ccl::BigEndian<ccl::int32_t> RESERVED52;
        ccl::BigEndian<ccl::int32_t> RESERVED56;
        ccl::BigEndian<ccl::int32_t> RESERVED60;
        ccl::LSBitField<32> flags;
        ccl::BigEndian<ccl::int32_t> color;
        ccl::BigEndian<ccl::int32_t> color2;
        ccl::BigEndian<ccl::int32_t> material;
        ccl::BigEndian<ccl::int32_t> RESERVED80;
        ccl::BigEndian<ccl::int32_t> maxLines;
        ccl::BigEndian<ccl::int32_t> maxCharacters;
        ccl::BigEndian<ccl::int32_t> lengthOfText;
        ccl::BigEndian<ccl::int32_t> nextLineNumber;
        ccl::BigEndian<ccl::int32_t> topLineNumber;
        ccl::BigEndian<ccl::int32_t> lowIntegerValue;
        ccl::BigEndian<ccl::int32_t> highIntegerValue;
        ccl::BigEndian<double> lowFloatValue;
        ccl::BigEndian<double> highFloatValue;
        ccl::BigEndian<double> lowerLeftX;
        ccl::BigEndian<double> lowerLeftY;
        ccl::BigEndian<double> lowerLeftZ;
        ccl::BigEndian<double> upperRightX;
        ccl::BigEndian<double> upperRightY;
        ccl::BigEndian<double> upperRightZ;
        std::string fontName;
        ccl::BigEndian<ccl::int32_t> drawVertical;
        ccl::BigEndian<ccl::int32_t> drawItalic;
        ccl::BigEndian<ccl::int32_t> drawBold;
        ccl::BigEndian<ccl::int32_t> drawUnderline;
        ccl::BigEndian<ccl::int32_t> lineStyle;
        ccl::BigEndian<ccl::int32_t> RESERVED316;

        virtual ~Text(void);
        Text(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


