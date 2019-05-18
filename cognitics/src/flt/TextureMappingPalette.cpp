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

#include "flt/TextureMappingPalette.h"

namespace flt
{
    TextureMappingPalette::~TextureMappingPalette(void)
    {
    }

    TextureMappingPalette::TextureMappingPalette(void)
    {
    }

    int TextureMappingPalette::getRecordType(void)
    {
        return FLT_TEXTUREMAPPINGPALETTE;
    }

    std::string TextureMappingPalette::getRecordName(void)
    {
        return "TextureMappingPalette";
    }

    void TextureMappingPalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(RESERVED4);
        /*   8 */ bs.bind(index);
        /*  12 */ bs.bind(name, 20);
        /*  32 */ bs.bind(type);
        /*  36 */ bs.bind(warped);

        /*  40 */
        matrix.resize(4);
        for(int i = 0; i < 4; ++i)
        {
            matrix[i].resize(4);
            for(int j = 0; j < 4; ++j)
                bs.bind(matrix[i][j]);
        }

        if(type == 1)
        {
            /* 168 */ bs.bind(type1State);
            /* 172 */ bs.bind(type1ActiveGeometryPoint);
            /* 176 */ bs.bind(type1LowerLeftX);
            /* 184 */ bs.bind(type1LowerLeftY);
            /* 192 */ bs.bind(type1LowerLeftZ);
            /* 200 */ bs.bind(type1UpperRightX);
            /* 208 */ bs.bind(type1UpperRightY);
            /* 216 */ bs.bind(type1UpperRightZ);
            /* 224 */ bs.bind(type1UseRealWorldSize1);
            /* 228 */ bs.bind(type1UseRealWorldSize2);
            /* 232 */ bs.bind(type1UseRealWorldSize3);
            /* 236 */ bs.bind(type1RESERVED236);
            /* 240 */ bs.bind(type1TextureOriginX);
            /* 248 */ bs.bind(type1TextureOriginY);
            /* 256 */ bs.bind(type1TextureOriginZ);
            /* 264 */ bs.bind(type1TextureAlignmentX);
            /* 272 */ bs.bind(type1TextureAlignmentY);
            /* 280 */ bs.bind(type1TextureAlignmentZ);
            /* 288 */ bs.bind(type1TextureShearX);
            /* 296 */ bs.bind(type1TextureShearY);
            /* 304 */ bs.bind(type1TextureShearZ);
            /* 312 */ bs.bind(type1GeometryOriginX);
            /* 320 */ bs.bind(type1GeometryOriginY);
            /* 328 */ bs.bind(type1GeometryOriginZ);
            /* 336 */ bs.bind(type1GeometryAlignmentX);
            /* 344 */ bs.bind(type1GeometryAlignmentY);
            /* 352 */ bs.bind(type1GeometryAlignmentZ);
            /* 360 */ bs.bind(type1GeometryShearX);
            /* 368 */ bs.bind(type1GeometryShearY);
            /* 376 */ bs.bind(type1GeometryShearZ);
            /* 384 */ bs.bind(type1ActiveTexturePoint);
            /* 388 */ bs.bind(type1UVDisplayType);
            /* 392 */ bs.bind(type1URepetition);
            /* 396 */ bs.bind(type1VRepetition);
        }

        if(type == 2)
        {
            /* 168 */ bs.bind(type2State);
            /* 172 */ bs.bind(type2ActiveGeometryPoint);
            /* 176 */ bs.bind(type2LowerLeftX);
            /* 184 */ bs.bind(type2LowerLeftY);
            /* 192 */ bs.bind(type2LowerLeftZ);
            /* 200 */ bs.bind(type2UpperRightX);
            /* 208 */ bs.bind(type2UpperRightY);
            /* 216 */ bs.bind(type2UpperRightZ);
            /* 224 */ bs.bind(type2UseRealWorldSize1);
            /* 228 */ bs.bind(type2UseRealWorldSize2);
            /* 232 */ bs.bind(type2UseRealWorldSize3);
            /* 236 */ bs.bind(type2RESERVED236);
            /* 240 */ bs.bind(type2TextureOriginX);
            /* 248 */ bs.bind(type2TextureOriginY);
            /* 256 */ bs.bind(type2TextureOriginZ);
            /* 264 */ bs.bind(type2TextureAlignmentX);
            /* 272 */ bs.bind(type2TextureAlignmentY);
            /* 280 */ bs.bind(type2TextureAlignmentZ);
            /* 288 */ bs.bind(type2TextureShearX);
            /* 296 */ bs.bind(type2TextureShearY);
            /* 304 */ bs.bind(type2TextureShearZ);
            /* 312 */ bs.bind(type2TexturePerspectiveX);
            /* 320 */ bs.bind(type2TexturePerspectiveY);
            /* 328 */ bs.bind(type2TexturePerspectiveZ);
            /* 336 */ bs.bind(type2GeometryOriginX);
            /* 344 */ bs.bind(type2GeometryOriginY);
            /* 352 */ bs.bind(type2GeometryOriginZ);
            /* 360 */ bs.bind(type2GeometryAlignmentX);
            /* 368 */ bs.bind(type2GeometryAlignmentY);
            /* 376 */ bs.bind(type2GeometryAlignmentZ);
            /* 384 */ bs.bind(type2GeometryShearX);
            /* 392 */ bs.bind(type2GeometryShearY);
            /* 400 */ bs.bind(type2GeometryShearZ);
            /* 408 */ bs.bind(type2GeometryPerspectiveX);
            /* 416 */ bs.bind(type2GeometryPerspectiveY);
            /* 424 */ bs.bind(type2GeometryPerspectiveZ);
            /* 432 */ bs.bind(type2ActiveTexturePoint);
            /* 436 */ bs.bind(type2UVDisplayType);
            /* 440 */ bs.bind(type2DepthScaleFactor);
            /* 444 */ bs.bind(type2RESERVED444);

            /* 448 */ 
            type2Matrix.resize(4);
            for(int i = 0; i < 4; ++i)
            {
                type2Matrix[i].resize(4);
                for(int j = 0; j < 4; ++j)
                    bs.bind(type2Matrix[i][j]);
            }

            /* 576 */ bs.bind(type2URepetition);
            /* 580 */ bs.bind(type2VRepetition);
        }

        if(type == 4)
        {
            /* 168 */ bs.bind(type4Scale);
            /* 172 */ bs.bind(type4RESERVED172);
            /* 176 */ bs.bind(type4CenterX);
            /* 184 */ bs.bind(type4CenterY);
            /* 192 */ bs.bind(type4CenterZ);
            /* 200 */ bs.bind(type4ScaleDivMaxDimension);
            /* 204 */ bs.bind(type4MaxDimension);
        }

        if(type == 5)
        {
            /* 168 */ bs.bind(type5ActiveGeometryPoint);
            /* 172 */ bs.bind(type5RESERVED172);
            /* 176 */ bs.bind(type5RadialScale);
            /* 180 */ bs.bind(type5LengthScale);

            /* 184 */ 
            type5Matrix.resize(4);
            for(int i = 0; i < 4; ++i)
            {
                type5Matrix[i].resize(4);
                for(int j = 0; j < 4; ++j)
                    bs.bind(type5Matrix[i][j]);
            }

            /* 312 */ bs.bind(type5EndPoint1X);
            /* 320 */ bs.bind(type5EndPoint1Y);
            /* 328 */ bs.bind(type5EndPoint1Z);
            /* 336 */ bs.bind(type5EndPoint2X);
            /* 344 */ bs.bind(type5EndPoint2Y);
            /* 352 */ bs.bind(type5EndPoint2Z);
        }

        if(warped != 0)
        {
            /* X+0 */ bs.bind(warpedActiveGeometryPoint);
            /* X+4 */ bs.bind(warpedToolState);
            /* X+8 */ bs.bind(warpedRESERVED8);

            /* X+16 */ 
            warpedFromPoints.resize(8);
            for(int i = 0; i < 8; ++i)
            {
                bs.bind(warpedFromPoints[i].x);
                bs.bind(warpedFromPoints[i].y);
            }

            /* X+144 */ 
            std::vector<std::vector<ccl::BigEndian<double> > > warpedToMatrix;
            warpedToPoints.resize(8);
            for(int i = 0; i < 8; ++i)
            {
                bs.bind(warpedToPoints[i].x);
                bs.bind(warpedToPoints[i].y);
            }
        }
    }


}
