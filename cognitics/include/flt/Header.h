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
/*! \file flt/Header.h
\headerfile flt/Header.h
\brief Provides flt::Header
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class Header : public Record
    {
    public:
        std::string id;
        ccl::BigEndian<ccl::int32_t> formatRevisionLevel;
        ccl::BigEndian<ccl::int32_t> editRevisionLevel;
        std::string lastRevisionDateTime;
        ccl::BigEndian<ccl::int16_t> nextGroupNodeID;
        ccl::BigEndian<ccl::int16_t> nextLODNodeID;
        ccl::BigEndian<ccl::int16_t> nextObjectNodeID;
        ccl::BigEndian<ccl::int16_t> nextFaceNodeID;
        ccl::BigEndian<ccl::int16_t> unitMultiplier;
        ccl::int8_t vertexCoordinateUnits;    // enum
        ccl::int8_t texWhite;
        ccl::LSBitField<32> flags;
        ccl::BigEndian<ccl::int32_t> RESERVED68;
        ccl::BigEndian<ccl::int32_t> RESERVED72;
        ccl::BigEndian<ccl::int32_t> RESERVED76;
        ccl::BigEndian<ccl::int32_t> RESERVED80;
        ccl::BigEndian<ccl::int32_t> RESERVED84;
        ccl::BigEndian<ccl::int32_t> RESERVED88;
        ccl::BigEndian<ccl::int32_t> projectionType;    // enum
        ccl::BigEndian<ccl::int32_t> RESERVED96;
        ccl::BigEndian<ccl::int32_t> RESERVED100;
        ccl::BigEndian<ccl::int32_t> RESERVED104;
        ccl::BigEndian<ccl::int32_t> RESERVED108;
        ccl::BigEndian<ccl::int32_t> RESERVED112;
        ccl::BigEndian<ccl::int32_t> RESERVED116;
        ccl::BigEndian<ccl::int32_t> RESERVED120;
        ccl::BigEndian<ccl::int16_t> nextDOFNodeID;
        ccl::BigEndian<ccl::int16_t> vertexStorageType;
        ccl::BigEndian<ccl::int32_t> databaseOrigin;    // enum
        ccl::BigEndian<double> southwestX;
        ccl::BigEndian<double> southwestY;
        ccl::BigEndian<double> deltaX;
        ccl::BigEndian<double> deltaY;
        ccl::BigEndian<ccl::int16_t> nextSoundNodeID;
        ccl::BigEndian<ccl::int16_t> nextPathNodeID;
        ccl::BigEndian<ccl::int32_t> RESERVED168;
        ccl::BigEndian<ccl::int32_t> RESERVED172;
        ccl::BigEndian<ccl::int16_t> nextClipNodeID;
        ccl::BigEndian<ccl::int16_t> nextTextNodeID;
        ccl::BigEndian<ccl::int16_t> nextBSPNodeID;
        ccl::BigEndian<ccl::int16_t> nextSwitchNodeID;
        ccl::BigEndian<ccl::int32_t> RESERVED184;
        ccl::BigEndian<double> southwestLatitude;
        ccl::BigEndian<double> southwestLongitude;
        ccl::BigEndian<double> northeastLatitude;
        ccl::BigEndian<double> northeastLongitude;
        ccl::BigEndian<double> originLatitude;
        ccl::BigEndian<double> originLongitude;
        ccl::BigEndian<double> lambertUpperLatitude;
        ccl::BigEndian<double> lambertLowerLatitude;
        ccl::BigEndian<ccl::int16_t> nextLightSourceNodeID;
        ccl::BigEndian<ccl::int16_t> nextLightPointNodeID;
        ccl::BigEndian<ccl::int16_t> nextRoadNodeID;
        ccl::BigEndian<ccl::int16_t> nextCATNodeID;
        ccl::BigEndian<ccl::int16_t> RESERVED260;
        ccl::BigEndian<ccl::int16_t> RESERVED262;
        ccl::BigEndian<ccl::int16_t> RESERVED264;
        ccl::BigEndian<ccl::int16_t> RESERVED266;
        ccl::BigEndian<ccl::int32_t> earthEllipsoidModel;    // enum
        ccl::BigEndian<ccl::int16_t> nextAdaptiveNodeID;
        ccl::BigEndian<ccl::int16_t> nextCurveNodeID;
        ccl::BigEndian<ccl::int16_t> utmZone;                        // 15.8 (reserved previously)
        std::string RESERVED278;
        ccl::BigEndian<double> deltaZ;                                // 15.7
        ccl::BigEndian<double> radius;                                // 15.7
        ccl::BigEndian<ccl::uint16_t> nextMeshNodeID;                // 15.7
        ccl::BigEndian<ccl::uint16_t> nextLightPointSystemID;        // 15.8 (reserved previously)
        ccl::BigEndian<ccl::int32_t> RESERVED304;
        ccl::BigEndian<double> earthMajorAxis;                        // 15.8
        ccl::BigEndian<double> earthMinorAxis;                        // 15.8


        virtual ~Header(void);
        Header(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}





