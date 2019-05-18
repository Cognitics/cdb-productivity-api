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

#include "flt/LinkagePalette.h"

namespace flt
{
    LinkagePalette::~LinkagePalette(void)
    {
    }

    LinkagePalette::LinkagePalette(void)
    {
    }

    int LinkagePalette::getRecordType(void)
    {
        return FLT_LINKAGEPALETTE;
    }

    std::string LinkagePalette::getRecordName(void)
    {
        return "LinkagePalette";
    }

    void LinkagePalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        KeyTable::bind(bs, length, revision);
        if(subtype != 2)
            return;

        // TODO
    }


//    void LinkagePalette::bindGeneralNode(struct LinkagePaletteGeneralNode &generalNode, ccl::EndianStream &es, int32_t revision)
//    {
//        /*   0 */ es.bind(generalNode.identifier);
//        /*   4 */ es.bind(generalNode.RESERVED4);
//        /*   8 */ es.bind(generalNode.nodeType);
//        /*  12 */ es.bind(generalNode.RESERVED12);
//        /*  16 */ es.bind(generalNode.RESERVED16);
//        /*  20 */ es.bind(generalNode.RESERVED20);
//        /*  24 */ es.bind(generalNode.RESERVED24);
//        /*  28 */ es.bind(generalNode.sinks);
//        /*  32 */ es.bind(generalNode.sources);
//        /*  36 */ es.bind(generalNode.nextNodeIdentifier);
//        /*  40 */ es.bind(generalNode.previousNodeIdentifier);
//        /*  44 */ es.bind(generalNode.arcSourceIdentifer);
//        /*  48 */ es.bind(generalNode.arcSinkIdentifer);
//    }
//
//    void LinkagePalette::bindFormulaNode(struct LinkagePaletteFormulaNode &formulaNode, ccl::EndianStream &es, int32_t revision)
//    {
//        /*   0 */ es.bind(formulaNode.identifier);
//        /*   4 */ es.bind(formulaNode.RESERVED4);
//        /*   8 */ es.bind(formulaNode.nodeType);
//        /*  12 */ es.bind(formulaNode.RESERVED12);
//        /*  16 */ es.bind(formulaNode.RESERVED16);
//        /*  20 */ es.bind(formulaNode.RESERVED20);
//        /*  24 */ es.bind(formulaNode.RESERVED24);
//        /*  28 */ es.bind(formulaNode.sinks);
//        /*  32 */ es.bind(formulaNode.sources);
//        /*  36 */ es.bind(formulaNode.nextNodeIdentifier);
//        /*  40 */ es.bind(formulaNode.previousNodeIdentifier);
//        /*  44 */ es.bind(formulaNode.arcSourceIdentifer);
//        /*  48 */ es.bind(formulaNode.arcSinkIdentifer);
//        /*  52 */ es.bind(formulaNode.RESERVED52);
//        /*  56 */ es.bind(formulaNode.RESERVED56);
//        /*  60 */ es.bind(formulaNode.RESERVED60);
//        /*  64 */ es.bind(formulaNode.RESERVED64);
//        /*  68 */ es.bind(formulaNode.RESERVED68);
//        /*  72 */ es.bind(formulaNode.RESERVED72);
//        /*  76 */ es.bind(formulaNode.RESERVED76);
//        /*  80 */ es.bind(formulaNode.RESERVED80);
//    }
//
//    void LinkagePalette::bindDriverNode(struct LinkagePaletteDriverNode &driverNode, ccl::EndianStream &es, int32_t revision)
//    {
//        /*   0 */ es.bind(driverNode.identifier);
//        /*   4 */ es.bind(driverNode.RESERVED4);
//        /*   8 */ es.bind(driverNode.nodeType);
//        /*  12 */ es.bind(driverNode.RESERVED12);
//        /*  16 */ es.bind(driverNode.RESERVED16);
//        /*  20 */ es.bind(driverNode.RESERVED20);
//        /*  24 */ es.bind(driverNode.RESERVED24);
//        /*  28 */ es.bind(driverNode.sinks);
//        /*  32 */ es.bind(driverNode.sources);
//        /*  36 */ es.bind(driverNode.nextNodeIdentifier);
//        /*  40 */ es.bind(driverNode.previousNodeIdentifier);
//        /*  44 */ es.bind(driverNode.arcSourceIdentifer);
//        /*  48 */ es.bind(driverNode.arcSinkIdentifer);
//        /*  52 */ es.bind(driverNode.currentValue);
//        /*  56 */ es.bind(driverNode.minAmplitude);
//        /*  60 */ es.bind(driverNode.maxAmplitude);
//        /*  64 */ es.bind(driverNode.waveOffset);
//        /*  68 */ es.bind(driverNode.minTime);
//        /*  72 */ es.bind(driverNode.maxTime);
//        /*  76 */ es.bind(driverNode.timeSteps);
//        /*  80 */ es.bind(driverNode.RESERVED80);
//        /*  84 */ es.bind(driverNode.RESERVED84);
//        /*  88 */ es.bind(driverNode.RESERVED88);
//        /*  92 */ es.bind(driverNode.RESERVED92);
//    }
//
//    void LinkagePalette::bindArcData(struct LinkagePaletteArcData &arcData, ccl::EndianStream &es, int32_t revision)
//    {
//        /*   0 */ es.bind(arcData.identifier);
//        /*   4 */ es.bind(arcData.RESERVED4);
//        /*   8 */ es.bind(arcData.nodeType);
//        /*  12 */ es.bind(arcData.RESERVED12);
//        /*  16 */ es.bind(arcData.RESERVED16);
//        /*  20 */ es.bind(arcData.priority);
//        /*  24 */ es.bind(arcData.sourceParameter);
//        /*  28 */ es.bind(arcData.sinkParameter);
//        /*  32 */ es.bind(arcData.RESERVED32);
//        /*  36 */ es.bind(arcData.nextSourceIdentifier);
//        /*  40 */ es.bind(arcData.nextSinkIdentifer);
//        /*  44 */ es.bind(arcData.nodeSourceIdentifier);
//        /*  48 */ es.bind(arcData.nodeSinkIdentiifer);
//    }
/*
    void LinkagePalette::unpack(int32_t revision)
    {
        KeyTable::unpack(revision);
        generalNodes.clear();
        formulaNodes.clear();
        driverNodes.clear();
        arcData.clear();
        entityNames.clear();
        for(std::map<int32_t, KeyTableData>::iterator it = keyData.begin(); it != keyData.end(); ++it)
        {
            std::stringstream ss(it->second.data, std::stringstream::binary | std::stringstream::in);
            ccl::EndianStream es((std::istream &)ss);
            es.seek(8);
            ccl::BigEndian<int32_t> nodeType;
            es.bind(nodeType);
            es.seek(0);
            if((nodeType == 0x12120003) || (nodeType == 0x12120005))
            {
                LinkagePaletteGeneralNode generalNode;
                bindGeneralNode(generalNode, es, revision);
                generalNodes[it->first] = generalNode;
            }
            else if(nodeType == 0x12150000)
            {
                LinkagePaletteFormulaNode formulaNode;
                bindFormulaNode(formulaNode, es, revision);
                formulaNodes[it->first] = formulaNode;
            }
            else if((nodeType == 0x12140001) || (nodeType == 0x12140004) || (nodeType == 0x12140005))
            {
                LinkagePaletteDriverNode driverNode;
                bindDriverNode(driverNode, es, revision);
                driverNodes[it->first] = driverNode;
            }
            else if(nodeType && 0x12120002)
            {
                LinkagePaletteArcData arcdata;
                bindArcData(arcdata, es, revision);
                arcData[it->first] = arcdata;
            }
            else
            {
                // assume it's an entity name if there isn't a valid nodeType
                entityNames[it->first] = it->second.data;
            }
        }

        keyData.clear();
    }

    void LinkagePalette::pack(int32_t revision)
    {
        keyData.clear();
        for(std::map<int32_t, LinkagePaletteFormulaNode>::iterator it = formulaNodes.begin(); it != formulaNodes.end(); ++it)
        {
            std::stringstream ss(std::stringstream::binary | std::stringstream::out);
            ccl::EndianStream es((std::ostream &)ss);
            bindFormulaNode(it->second, es, revision);
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = ss.str();
        }
        for(std::map<int32_t, LinkagePaletteFormulaNode>::iterator it = formulaNodes.begin(); it != formulaNodes.end(); ++it)
        {
            std::stringstream ss(std::stringstream::binary | std::stringstream::out);
            ccl::EndianStream es((std::ostream &)ss);
            bindFormulaNode(it->second, es, revision);
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = ss.str();
        }
        for(std::map<int32_t, LinkagePaletteDriverNode>::iterator it = driverNodes.begin(); it != driverNodes.end(); ++it)
        {
            std::stringstream ss(std::stringstream::binary | std::stringstream::out);
            ccl::EndianStream es((std::ostream &)ss);
            bindDriverNode(it->second, es, revision);
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = ss.str();
        }
        for(std::map<int32_t, LinkagePaletteArcData>::iterator it = arcData.begin(); it != arcData.end(); ++it)
        {
            std::stringstream ss(std::stringstream::binary | std::stringstream::out);
            ccl::EndianStream es((std::ostream &)ss);
            bindArcData(it->second, es, revision);
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = ss.str();
        }
        for(std::map<int32_t, std::string>::iterator it = entityNames.begin(); it != entityNames.end(); ++it)
        {
            std::stringstream ss(std::stringstream::binary | std::stringstream::out);
            ccl::EndianStream es((std::ostream &)ss);
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = std::string(it->second.c_str());
            keyData[it->first].data.resize(keyData[it->first].data.size() + 1);        // null termination
        }
        KeyTable::pack(revision);
    }
*/

}
