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

#include "flt/RoadConstruction.h"

namespace flt
{
    RoadConstruction::~RoadConstruction(void)
    {
    }

    RoadConstruction::RoadConstruction(void)
    {
    }

    int RoadConstruction::getRecordType(void)
    {
        return FLT_ROADCONSTRUCTION;
    }

    std::string RoadConstruction::getRecordName(void)
    {
        return "RoadConstruction";
    }

    void RoadConstruction::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(id, 8);
        /*  12 */ bs.bind(RESERVED12);
        /*  16 */ bs.bind(roadType);
        /*  20 */ bs.bind(roadToolsVersion);
        /*  24 */ bs.bind(entryControlPointX);
        /*  32 */ bs.bind(entryControlPointY);
        /*  40 */ bs.bind(entryControlPointZ);
        /*  48 */ bs.bind(alignmentControlPointX);
        /*  56 */ bs.bind(alignmentControlPointY);
        /*  64 */ bs.bind(alignmentControlPointZ);
        /*  72 */ bs.bind(exitControlPointX);
        /*  80 */ bs.bind(exitControlPointY);
        /*  88 */ bs.bind(exitControlPointZ);
        /*  96 */ bs.bind(arcRadius);
        /* 104 */ bs.bind(entrySpiralLength);
        /* 112 */ bs.bind(exitSpiralLength);
        /* 120 */ bs.bind(superelevation);
        /* 128 */ bs.bind(spiralType);
        /* 132 */ bs.bind(additionalVerticalParabolaFlag);
        /* 136 */ bs.bind(verticalCurveLength);
        /* 144 */ bs.bind(minimumCurveLength);
        /* 152 */ bs.bind(entrySlope);
        /* 160 */ bs.bind(exitSlope);
    }

}
