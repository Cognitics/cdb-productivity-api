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
/*! \file flt/EyepointTrackplanePalette.h
\headerfile flt/EyepointTrackplanePalette.h
\brief Provides flt::EyepointTrackplanePalette
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct Eyepoint
    {
        ccl::BigEndian<double> rotationCenterX;
        ccl::BigEndian<double> rotationCenterY;
        ccl::BigEndian<double> rotationCenterZ;
        ccl::BigEndian<float> yaw;
        ccl::BigEndian<float> pitch;
        ccl::BigEndian<float> roll;
        std::vector<std::vector<ccl::BigEndian<float> > > rotationMatrix;
        ccl::BigEndian<float> fieldOfView;
        ccl::BigEndian<float> scale;
        ccl::BigEndian<float> nearClippingPlane;
        ccl::BigEndian<float> farClippingPlane;
        std::vector<std::vector<ccl::BigEndian<float> > > flyThroughMatrix;
        ccl::BigEndian<float> positionX;
        ccl::BigEndian<float> positionY;
        ccl::BigEndian<float> positionZ;
        ccl::BigEndian<float> flyThroughYaw;
        ccl::BigEndian<float> flyThroughPitch;
        ccl::BigEndian<float> directionVectorX;
        ccl::BigEndian<float> directionVectorY;
        ccl::BigEndian<float> directionVectorZ;
        ccl::BigEndian<ccl::int32_t> noFlyThrough;
        ccl::BigEndian<ccl::int32_t> orthoView;
        ccl::BigEndian<ccl::int32_t> valid;
        ccl::BigEndian<ccl::int32_t> imageOffsetX;
        ccl::BigEndian<ccl::int32_t> imageOffsetY;
        ccl::BigEndian<ccl::int32_t> imageZoom;
        ccl::BigEndian<ccl::int32_t> RESERVED244;
        ccl::BigEndian<ccl::int32_t> RESERVED248;
        ccl::BigEndian<ccl::int32_t> RESERVED252;
        ccl::BigEndian<ccl::int32_t> RESERVED256;
        ccl::BigEndian<ccl::int32_t> RESERVED260;
        ccl::BigEndian<ccl::int32_t> RESERVED264;
        ccl::BigEndian<ccl::int32_t> RESERVED268;
        ccl::BigEndian<ccl::int32_t> RESERVED272;
        ccl::BigEndian<ccl::int32_t> RESERVED276;
    };

    struct Trackplane
    {
        ccl::BigEndian<ccl::int32_t> valid;
        ccl::BigEndian<ccl::int32_t> RESERVED2732;
        ccl::BigEndian<double> originX;
        ccl::BigEndian<double> originY;
        ccl::BigEndian<double> originZ;
        ccl::BigEndian<double> alignmentX;
        ccl::BigEndian<double> alignmentY;
        ccl::BigEndian<double> alignmentZ;
        ccl::BigEndian<double> planeX;
        ccl::BigEndian<double> planeY;
        ccl::BigEndian<double> planeZ;
        ccl::int8_t gridVisible;
        ccl::int8_t gridType;
        ccl::int8_t gridUnder;
        ccl::int8_t RESERVED2811;
        ccl::BigEndian<float> gridAngle;
        ccl::BigEndian<double> gridSpacingX;
        ccl::BigEndian<double> gridSpacingY;
        ccl::int8_t radialGridSpacingControl;
        ccl::int8_t rectangularGridSpacingControl;
        ccl::int8_t snapToGrid;
        ccl::int8_t RESERVED2835;
        ccl::BigEndian<ccl::int32_t> RESERVED2836;
        ccl::BigEndian<double> gridSize;
        ccl::BigEndian<ccl::int32_t> visibilityMask;
        ccl::BigEndian<ccl::int32_t> RESERVED2852;
    };

    class EyepointTrackplanePalette : public Record
    {
    public:
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        std::vector<Eyepoint> eyepoints;
        std::vector<Trackplane> trackplanes;

        virtual ~EyepointTrackplanePalette(void);
        EyepointTrackplanePalette(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


