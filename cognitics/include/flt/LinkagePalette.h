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
/*! \file flt/LinkagePalette.h
\headerfile flt/LinkagePalette.h
\brief Provides flt::LinkagePalette
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "KeyTable.h"

namespace flt
{
    struct LinkagePaletteGeneralNode
    {
        ccl::BigEndian<ccl::int32_t> identifier;
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        ccl::BigEndian<ccl::int32_t> nodeType;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<ccl::int32_t> RESERVED16;
        ccl::BigEndian<ccl::int32_t> RESERVED20;
        ccl::BigEndian<ccl::int32_t> RESERVED24;
        ccl::BigEndian<ccl::int32_t> sinks;
        ccl::BigEndian<ccl::int32_t> sources;
        ccl::BigEndian<ccl::int32_t> nextNodeIdentifier;
        ccl::BigEndian<ccl::int32_t> previousNodeIdentifier;
        ccl::BigEndian<ccl::int32_t> arcSourceIdentifer;
        ccl::BigEndian<ccl::int32_t> arcSinkIdentifer;
    };

    struct LinkagePaletteFormulaNode
    {
        ccl::BigEndian<ccl::int32_t> identifier;
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        ccl::BigEndian<ccl::int32_t> nodeType;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<ccl::int32_t> RESERVED16;
        ccl::BigEndian<ccl::int32_t> RESERVED20;
        ccl::BigEndian<ccl::int32_t> RESERVED24;
        ccl::BigEndian<ccl::int32_t> sinks;
        ccl::BigEndian<ccl::int32_t> sources;
        ccl::BigEndian<ccl::int32_t> nextNodeIdentifier;
        ccl::BigEndian<ccl::int32_t> previousNodeIdentifier;
        ccl::BigEndian<ccl::int32_t> arcSourceIdentifer;
        ccl::BigEndian<ccl::int32_t> arcSinkIdentifer;
        ccl::BigEndian<ccl::int32_t> RESERVED52;
        ccl::BigEndian<ccl::int32_t> RESERVED56;
        ccl::BigEndian<ccl::int32_t> RESERVED60;
        ccl::BigEndian<ccl::int32_t> RESERVED64;
        ccl::BigEndian<ccl::int32_t> RESERVED68;
        ccl::BigEndian<ccl::int32_t> RESERVED72;
        ccl::BigEndian<ccl::int32_t> RESERVED76;
        ccl::BigEndian<ccl::int32_t> RESERVED80;
    };

    struct LinkagePaletteDriverNode
    {
        ccl::BigEndian<ccl::int32_t> identifier;
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        ccl::BigEndian<ccl::int32_t> nodeType;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<ccl::int32_t> RESERVED16;
        ccl::BigEndian<ccl::int32_t> RESERVED20;
        ccl::BigEndian<ccl::int32_t> RESERVED24;
        ccl::BigEndian<ccl::int32_t> sinks;
        ccl::BigEndian<ccl::int32_t> sources;
        ccl::BigEndian<ccl::int32_t> nextNodeIdentifier;
        ccl::BigEndian<ccl::int32_t> previousNodeIdentifier;
        ccl::BigEndian<ccl::int32_t> arcSourceIdentifer;
        ccl::BigEndian<ccl::int32_t> arcSinkIdentifer;
        ccl::BigEndian<float> currentValue;
        ccl::BigEndian<float> minAmplitude;
        ccl::BigEndian<float> maxAmplitude;
        ccl::BigEndian<float> waveOffset;
        ccl::BigEndian<float> minTime;
        ccl::BigEndian<float> maxTime;
        ccl::BigEndian<float> timeSteps;
        ccl::BigEndian<ccl::int32_t> RESERVED80;
        ccl::BigEndian<ccl::int32_t> RESERVED84;
        ccl::BigEndian<ccl::int32_t> RESERVED88;
        ccl::BigEndian<ccl::int32_t> RESERVED92;
    };

    struct LinkagePaletteArcData
    {
        ccl::BigEndian<ccl::int32_t> identifier;
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        ccl::BigEndian<ccl::int32_t> nodeType;
        ccl::BigEndian<ccl::int32_t> RESERVED12;
        ccl::BigEndian<ccl::int32_t> RESERVED16;
        ccl::BigEndian<ccl::int32_t> priority;
        ccl::BigEndian<ccl::int32_t> sourceParameter;
        ccl::BigEndian<ccl::int32_t> sinkParameter;
        ccl::BigEndian<ccl::int32_t> RESERVED32;
        ccl::BigEndian<ccl::int32_t> nextSourceIdentifier;
        ccl::BigEndian<ccl::int32_t> nextSinkIdentifer;
        ccl::BigEndian<ccl::int32_t> nodeSourceIdentifier;
        ccl::BigEndian<ccl::int32_t> nodeSinkIdentiifer;
    };

    class LinkagePalette : public KeyTable
    {
    public:
        //std::map<ccl::int32_t, LinkagePaletteGeneralNode> generalNodes;
        //std::map<ccl::int32_t, LinkagePaletteFormulaNode> formulaNodes;
        //std::map<ccl::int32_t, LinkagePaletteDriverNode> driverNodes;
        //std::map<ccl::int32_t, LinkagePaletteArcData> arcData;
        //std::map<ccl::int32_t, std::string> entityNames;

        virtual ~LinkagePalette(void);
        LinkagePalette(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

        //virtual void bindGeneralNode(struct LinkagePaletteGeneralNode &generalNode, ccl::EndianStream &es, int32_t revision = COGNITICS_OPENFLIGHT_FORMAT_REVISION);
        //virtual void bindFormulaNode(struct LinkagePaletteFormulaNode &formulaNode, ccl::EndianStream &es, int32_t revision = COGNITICS_OPENFLIGHT_FORMAT_REVISION);
        //virtual void bindDriverNode(struct LinkagePaletteDriverNode &driverNode, ccl::EndianStream &es, int32_t revision = COGNITICS_OPENFLIGHT_FORMAT_REVISION);
        //virtual void bindArcData(struct LinkagePaletteArcData &arcData, ccl::EndianStream &es, int32_t revision = COGNITICS_OPENFLIGHT_FORMAT_REVISION);

    };

}


