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

#include "flt/LocalVertexPool.h"

namespace flt
{
    LocalVertexPool::~LocalVertexPool(void)
    {
    }

    LocalVertexPool::LocalVertexPool(void)
    {
    }

    int LocalVertexPool::getRecordType(void)
    {
        return FLT_LOCALVERTEXPOOL;
    }

    std::string LocalVertexPool::getRecordName(void)
    {
        return "LocalVertexPool";
    }

    void LocalVertexPool::bind(ccl::BindStream &bs, int length, int revision)
    {
        ccl::BigEndian<ccl::uint32_t> numberOfVertices = ccl::int32_t(vertices.size());    // overwritten if reading
        /*   4 */ bs.bind(numberOfVertices);

        /*   8 */ bs.bind(attributeMask);

        vertices.resize(numberOfVertices);
        for(ccl::int32_t i = 0, n = numberOfVertices; i < n; ++i)
        {
            if(attributeMask[0])
            {
                bs.bind(vertices[i].coordinateX);
                bs.bind(vertices[i].coordinateY);
                bs.bind(vertices[i].coordinateZ);
            }
            if(attributeMask[1] || attributeMask[2])
            {
                bs.bind(vertices[i].color);
            }
            if(attributeMask[3])
            {
                bs.bind(vertices[i].normalI);
                bs.bind(vertices[i].normalJ);
                bs.bind(vertices[i].normalK);
            }
            if(attributeMask[4])
            {
                bs.bind(vertices[i].uvBaseU);
                bs.bind(vertices[i].uvBaseV);
            }
            if(attributeMask[5])
            {
                bs.bind(vertices[i].uv1U);
                bs.bind(vertices[i].uv1V);
            }
            if(attributeMask[6])
            {
                bs.bind(vertices[i].uv2U);
                bs.bind(vertices[i].uv2V);
            }
            if(attributeMask[7])
            {
                bs.bind(vertices[i].uv3U);
                bs.bind(vertices[i].uv3V);
            }
            if(attributeMask[8])
            {
                bs.bind(vertices[i].uv4U);
                bs.bind(vertices[i].uv4V);
            }
            if(attributeMask[9])
            {
                bs.bind(vertices[i].uv5U);
                bs.bind(vertices[i].uv5V);
            }
            if(attributeMask[10])
            {
                bs.bind(vertices[i].uv6U);
                bs.bind(vertices[i].uv6V);
            }
            if(attributeMask[11])
            {
                bs.bind(vertices[i].uv7U);
                bs.bind(vertices[i].uv7V);
            }
        }
    }


}
