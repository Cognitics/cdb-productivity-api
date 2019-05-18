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

#include "flt/CATData.h"

namespace flt
{
    CATData::~CATData(void)
    {
    }

    CATData::CATData(void)
    {
    }

    int CATData::getRecordType(void)
    {
        return FLT_CATDATA;
    }

    std::string CATData::getRecordName(void)
    {
        return "CATData";
    }

    void CATData::bind(ccl::BindStream &bs, int length, int revision)
    {
        KeyTable::bind(bs, length, revision);
        if(subtype != 2)
            return;

        // TODO
    }

    /*
    void CATData::bindFace(struct CATDataFace &face, ccl::EndianStream &es, int32_t revision)
    {
        es.bind(face.lod);
        es.bind(face.childIndex1);
        es.bind(face.childIndex2);
        es.bind(face.childIndex3);
        es.bind(face.childIndex4);
        ccl::BigEndian<int32_t> idLength = face.id.size() + (face.id.size() % 4);
        es.bind(idLength);
        es.bind(face.id, idLength);
    }

    void CATData::unpack(int32_t revision)
    {
        KeyTable::unpack(revision);

        faces.clear();
        for(std::map<int32_t, KeyTableData>::iterator it = keyData.begin(); it != keyData.end(); ++it)
        {
            std::stringstream ss(it->second.data, std::stringstream::binary | std::stringstream::in);
            ccl::EndianStream es((std::istream &)ss);
            bindFace(faces[it->first], es, revision);
        }

        keyData.clear();
    }

    void CATData::pack(int32_t revision)
    {
        keyData.clear();
        for(std::map<int32_t, CATDataFace>::iterator it = faces.begin(); it != faces.end(); ++it)
        {
            std::stringstream ss(std::stringstream::binary | std::stringstream::out);
            ccl::EndianStream es((std::ostream &)ss);
            bindFace(it->second, es, revision);
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = ss.str();
        }
        KeyTable::pack(revision);
    }
    */


}
