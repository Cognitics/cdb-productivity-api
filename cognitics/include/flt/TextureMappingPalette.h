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
/*! \file flt/TextureMappingPalette.h
\headerfile flt/TextureMappingPalette.h
\brief Provides flt::TextureMappingPalette
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct TextureMappingPalettePoint
    {
        ccl::BigEndian<double> x;
        ccl::BigEndian<double> y;
    };

    class TextureMappingPalette : public Record
    {
    public:
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        ccl::BigEndian<ccl::int32_t> index;
        std::string name;
        ccl::BigEndian<ccl::int32_t> type;
        ccl::BigEndian<ccl::int32_t> warped;
        std::vector<std::vector<ccl::BigEndian<double> > > matrix;

        // type = 1
        ccl::BigEndian<ccl::int32_t> type1State;
        ccl::BigEndian<ccl::int32_t> type1ActiveGeometryPoint;
        ccl::BigEndian<double> type1LowerLeftX;
        ccl::BigEndian<double> type1LowerLeftY;
        ccl::BigEndian<double> type1LowerLeftZ;
        ccl::BigEndian<double> type1UpperRightX;
        ccl::BigEndian<double> type1UpperRightY;
        ccl::BigEndian<double> type1UpperRightZ;
        ccl::BigEndian<ccl::int32_t> type1UseRealWorldSize1;
        ccl::BigEndian<ccl::int32_t> type1UseRealWorldSize2;
        ccl::BigEndian<ccl::int32_t> type1UseRealWorldSize3;
        ccl::BigEndian<ccl::int32_t> type1RESERVED236;
        ccl::BigEndian<double> type1TextureOriginX;
        ccl::BigEndian<double> type1TextureOriginY;
        ccl::BigEndian<double> type1TextureOriginZ;
        ccl::BigEndian<double> type1TextureAlignmentX;
        ccl::BigEndian<double> type1TextureAlignmentY;
        ccl::BigEndian<double> type1TextureAlignmentZ;
        ccl::BigEndian<double> type1TextureShearX;
        ccl::BigEndian<double> type1TextureShearY;
        ccl::BigEndian<double> type1TextureShearZ;
        ccl::BigEndian<double> type1GeometryOriginX;
        ccl::BigEndian<double> type1GeometryOriginY;
        ccl::BigEndian<double> type1GeometryOriginZ;
        ccl::BigEndian<double> type1GeometryAlignmentX;
        ccl::BigEndian<double> type1GeometryAlignmentY;
        ccl::BigEndian<double> type1GeometryAlignmentZ;
        ccl::BigEndian<double> type1GeometryShearX;
        ccl::BigEndian<double> type1GeometryShearY;
        ccl::BigEndian<double> type1GeometryShearZ;
        ccl::BigEndian<ccl::int32_t> type1ActiveTexturePoint;
        ccl::BigEndian<ccl::int32_t> type1UVDisplayType;
        ccl::BigEndian<float> type1URepetition;
        ccl::BigEndian<float> type1VRepetition;

        // type = 2
        ccl::BigEndian<ccl::int32_t> type2State;
        ccl::BigEndian<ccl::int32_t> type2ActiveGeometryPoint;
        ccl::BigEndian<double> type2LowerLeftX;
        ccl::BigEndian<double> type2LowerLeftY;
        ccl::BigEndian<double> type2LowerLeftZ;
        ccl::BigEndian<double> type2UpperRightX;
        ccl::BigEndian<double> type2UpperRightY;
        ccl::BigEndian<double> type2UpperRightZ;
        ccl::BigEndian<ccl::int32_t> type2UseRealWorldSize1;
        ccl::BigEndian<ccl::int32_t> type2UseRealWorldSize2;
        ccl::BigEndian<ccl::int32_t> type2UseRealWorldSize3;
        ccl::BigEndian<ccl::int32_t> type2RESERVED236;
        ccl::BigEndian<double> type2TextureOriginX;
        ccl::BigEndian<double> type2TextureOriginY;
        ccl::BigEndian<double> type2TextureOriginZ;
        ccl::BigEndian<double> type2TextureAlignmentX;
        ccl::BigEndian<double> type2TextureAlignmentY;
        ccl::BigEndian<double> type2TextureAlignmentZ;
        ccl::BigEndian<double> type2TextureShearX;
        ccl::BigEndian<double> type2TextureShearY;
        ccl::BigEndian<double> type2TextureShearZ;
        ccl::BigEndian<double> type2TexturePerspectiveX;
        ccl::BigEndian<double> type2TexturePerspectiveY;
        ccl::BigEndian<double> type2TexturePerspectiveZ;
        ccl::BigEndian<double> type2GeometryOriginX;
        ccl::BigEndian<double> type2GeometryOriginY;
        ccl::BigEndian<double> type2GeometryOriginZ;
        ccl::BigEndian<double> type2GeometryAlignmentX;
        ccl::BigEndian<double> type2GeometryAlignmentY;
        ccl::BigEndian<double> type2GeometryAlignmentZ;
        ccl::BigEndian<double> type2GeometryShearX;
        ccl::BigEndian<double> type2GeometryShearY;
        ccl::BigEndian<double> type2GeometryShearZ;
        ccl::BigEndian<double> type2GeometryPerspectiveX;
        ccl::BigEndian<double> type2GeometryPerspectiveY;
        ccl::BigEndian<double> type2GeometryPerspectiveZ;
        ccl::BigEndian<ccl::int32_t> type2ActiveTexturePoint;
        ccl::BigEndian<ccl::int32_t> type2UVDisplayType;
        ccl::BigEndian<float> type2DepthScaleFactor;
        ccl::BigEndian<ccl::int32_t> type2RESERVED444;
        std::vector<std::vector<ccl::BigEndian<double> > > type2Matrix;
        ccl::BigEndian<float> type2URepetition;
        ccl::BigEndian<float> type2VRepetition;

        // type = 4
        ccl::BigEndian<float> type4Scale;
        ccl::BigEndian<ccl::int32_t> type4RESERVED172;
        ccl::BigEndian<double> type4CenterX;
        ccl::BigEndian<double> type4CenterY;
        ccl::BigEndian<double> type4CenterZ;
        ccl::BigEndian<float> type4ScaleDivMaxDimension;
        ccl::BigEndian<float> type4MaxDimension;

        // type = 5
        ccl::BigEndian<ccl::int32_t> type5ActiveGeometryPoint;
        ccl::BigEndian<ccl::int32_t> type5RESERVED172;
        ccl::BigEndian<float> type5RadialScale;
        ccl::BigEndian<float> type5LengthScale;
        std::vector<std::vector<ccl::BigEndian<double> > > type5Matrix;
        ccl::BigEndian<double> type5EndPoint1X;
        ccl::BigEndian<double> type5EndPoint1Y;
        ccl::BigEndian<double> type5EndPoint1Z;
        ccl::BigEndian<double> type5EndPoint2X;
        ccl::BigEndian<double> type5EndPoint2Y;
        ccl::BigEndian<double> type5EndPoint2Z;

        // warped
        ccl::BigEndian<ccl::int32_t> warpedActiveGeometryPoint;
        ccl::BigEndian<ccl::int32_t> warpedToolState;
        ccl::BigEndian<ccl::int32_t> warpedRESERVED8;
        std::vector<TextureMappingPalettePoint> warpedFromPoints;
        std::vector<TextureMappingPalettePoint> warpedToPoints;

        virtual ~TextureMappingPalette(void);
        TextureMappingPalette(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


