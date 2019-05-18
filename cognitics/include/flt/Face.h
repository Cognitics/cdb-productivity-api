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
/*! \file flt/Face.h
\headerfile flt/Face.h
\brief Provides flt::Face
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class Face : public Record
    {
    public:
        std::string id;
        ccl::BigEndian<ccl::int32_t> irColorCode;
        ccl::BigEndian<ccl::int16_t> priority;
        ccl::int8_t drawType;
        ccl::int8_t texWhite;
        ccl::BigEndian<ccl::uint16_t> colorNameIndex;
        ccl::BigEndian<ccl::uint16_t> altColorNameIndex;
        ccl::int8_t RESERVED24;
        ccl::int8_t templateBillboard;
        ccl::BigEndian<ccl::int16_t> detailTexturePatternIndex;
        ccl::BigEndian<ccl::int16_t> texturePatternIndex;
        ccl::BigEndian<ccl::int16_t> materialIndex;
        ccl::BigEndian<ccl::int16_t> surfaceMaterialCode;
        ccl::BigEndian<ccl::int16_t> featureID;
        ccl::BigEndian<ccl::int32_t> irMaterialCode;
        ccl::BigEndian<ccl::uint16_t> transparency;
        ccl::uint8_t lodGenerationControl;
        ccl::uint8_t lineStyleIndex;
        ccl::LSBitField<32> flags;
        ccl::uint8_t lightMode;
        std::string RESERVED49;
        ccl::BigEndian<ccl::uint32_t> primaryPackedColor;
        ccl::BigEndian<ccl::uint32_t> altPackedColor;
        ccl::BigEndian<ccl::int16_t> textureMappingIndex;
        ccl::BigEndian<ccl::int16_t> RESERVED66;
        ccl::BigEndian<ccl::uint32_t> primaryColorIndex;
        ccl::BigEndian<ccl::uint32_t> altColorIndex;
        ccl::BigEndian<ccl::int16_t> RESERVED76;
        ccl::BigEndian<ccl::int16_t> shaderIndex;

        virtual ~Face(void);
        Face(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


