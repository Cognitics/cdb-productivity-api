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

#include "flt/Header.h"

namespace flt
{
    Header::~Header(void)
    {
    }

    Header::Header(void)
        : id("db"),
        formatRevisionLevel(0),
        editRevisionLevel(0),
        lastRevisionDateTime(""),
        nextGroupNodeID(0),
        nextLODNodeID(0),
        nextObjectNodeID(0),
        nextFaceNodeID(0),
        unitMultiplier(1),
        vertexCoordinateUnits(0),
        texWhite(0),
        //flags(0),
        RESERVED68(0),
        RESERVED72(0),
        RESERVED76(0),
        RESERVED80(0),
        RESERVED84(0),
        RESERVED88(0),
        projectionType(0),
        RESERVED96(0),
        RESERVED100(0),
        RESERVED104(0),
        RESERVED108(0),
        RESERVED112(0),
        RESERVED116(0),
        RESERVED120(0),
        nextDOFNodeID(0),
        vertexStorageType(1),
        databaseOrigin(100),
        southwestX(0),
        southwestY(0),
        deltaX(0),
        deltaY(0),
        nextSoundNodeID(0),
        nextPathNodeID(0),
        RESERVED168(0),
        RESERVED172(0),
        nextClipNodeID(0),
        nextTextNodeID(0),
        nextBSPNodeID(0),
        nextSwitchNodeID(0),
        RESERVED184(0),
        southwestLatitude(0),
        southwestLongitude(0),
        northeastLatitude(0),
        northeastLongitude(0),
        originLatitude(0),
        originLongitude(0),
        lambertUpperLatitude(0),
        lambertLowerLatitude(0),
        nextLightSourceNodeID(0),
        nextLightPointNodeID(0),
        nextRoadNodeID(0),
        nextCATNodeID(0),
        RESERVED260(0),
        RESERVED262(0),
        RESERVED264(0),
        RESERVED266(0),
        earthEllipsoidModel(0),
        nextAdaptiveNodeID(0),
        nextCurveNodeID(0),
        utmZone(0),
        RESERVED278(""),
        deltaZ(0),
        radius(0),
        nextMeshNodeID(0),
        nextLightPointSystemID(0),
        earthMajorAxis(0),
        earthMinorAxis(0)
    {
    }

    int Header::getRecordType(void)
    {
        return FLT_HEADER;
    }

    std::string Header::getRecordName(void)
    {
        return "Header";
    }

    void Header::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(formatRevisionLevel);
        /*  16 */ bs.bind(editRevisionLevel);
        /*  20 */ bs.bind(lastRevisionDateTime, 32);
        /*  52 */ bs.bind(nextGroupNodeID);
        /*  54 */ bs.bind(nextLODNodeID);
        /*  56 */ bs.bind(nextObjectNodeID);
        /*  58 */ bs.bind(nextFaceNodeID);
        /*  60 */ bs.bind(unitMultiplier);
        /*  62 */ bs.bind(vertexCoordinateUnits);
        /*  63 */ bs.bind(texWhite);
        /*  64 */ bs.bind(flags);
        /*  68 */ bs.bind(RESERVED68);
        /*  72 */ bs.bind(RESERVED72);
        /*  76 */ bs.bind(RESERVED76);
        /*  80 */ bs.bind(RESERVED80);
        /*  84 */ bs.bind(RESERVED84);
        /*  88 */ bs.bind(RESERVED88);
        /*  92 */ bs.bind(projectionType);
        /*  96 */ bs.bind(RESERVED96);
        /* 100 */ bs.bind(RESERVED100);
        /* 104 */ bs.bind(RESERVED104);
        /* 108 */ bs.bind(RESERVED108);
        /* 112 */ bs.bind(RESERVED112);
        /* 116 */ bs.bind(RESERVED116);
        /* 120 */ bs.bind(RESERVED120);
        /* 124 */ bs.bind(nextDOFNodeID);
        /* 126 */ bs.bind(vertexStorageType);
        /* 128 */ bs.bind(databaseOrigin);
        /* 132 */ bs.bind(southwestX);
        /* 140 */ bs.bind(southwestY);
        /* 148 */ bs.bind(deltaX);
        /* 156 */ bs.bind(deltaY);
        /* 164 */ bs.bind(nextSoundNodeID);
        /* 166 */ bs.bind(nextPathNodeID);
        /* 168 */ bs.bind(RESERVED168);
        /* 172 */ bs.bind(RESERVED172);
        /* 176 */ bs.bind(nextClipNodeID);
        /* 178 */ bs.bind(nextTextNodeID);
        /* 180 */ bs.bind(nextBSPNodeID);
        /* 182 */ bs.bind(nextSwitchNodeID);
        /* 184 */ bs.bind(RESERVED184);
        /* 188 */ bs.bind(southwestLatitude);
        /* 196 */ bs.bind(southwestLongitude);
        /* 204 */ bs.bind(northeastLatitude);
        /* 212 */ bs.bind(northeastLongitude);
        /* 220 */ bs.bind(originLatitude);
        /* 228 */ bs.bind(originLongitude);
        /* 236 */ bs.bind(lambertUpperLatitude);
        /* 244 */ bs.bind(lambertLowerLatitude);
        /* 252 */ bs.bind(nextLightSourceNodeID);
        /* 254 */ bs.bind(nextLightPointNodeID);
        /* 256 */ bs.bind(nextRoadNodeID);
        /* 258 */ bs.bind(nextCATNodeID);
        /* 260 */ bs.bind(RESERVED260);
        /* 262 */ bs.bind(RESERVED262);
        /* 264 */ bs.bind(RESERVED264);
        /* 266 */ bs.bind(RESERVED266);
        /* 268 */ bs.bind(earthEllipsoidModel);
        /* 272 */ bs.bind(nextAdaptiveNodeID);
        /* 274 */ bs.bind(nextCurveNodeID);
        /* 276 */ bs.bind(utmZone);
        /* 278 */ bs.bind(RESERVED278, 6);

        if(formatRevisionLevel < 1570)
            return;

        /* 284 */ bs.bind(deltaZ);
        /* 292 */ bs.bind(radius);
        /* 300 */ bs.bind(nextMeshNodeID);
        /* 302 */ bs.bind(nextLightPointSystemID);

        if(formatRevisionLevel < 1580)
            return;

        /* 304 */ bs.bind(RESERVED304);    // this should be 1580
        /* 308 */ bs.bind(earthMajorAxis);
        /* 316 */ bs.bind(earthMinorAxis);
    }

}
