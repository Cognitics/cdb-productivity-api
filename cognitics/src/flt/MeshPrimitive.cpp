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

#include "flt/MeshPrimitive.h"

namespace flt
{
    MeshPrimitive::~MeshPrimitive(void)
    {
    }

    MeshPrimitive::MeshPrimitive(void)
    {
    }

    int MeshPrimitive::getRecordType(void)
    {
        return FLT_MESHPRIMITIVE;
    }

    std::string MeshPrimitive::getRecordName(void)
    {
        return "MeshPrimitive";
    }

    void MeshPrimitive::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(primitiveType);

        ccl::BigEndian<ccl::uint32_t> vertexCount;
        if(primitiveType == 1)
        {
            vertexCount = ccl::int32_t(vertices1.size());    // overwritten if reading
            bs.bind(vertexCount);
            for(ccl::int32_t i = 0, n = vertexCount; i < n; ++i)
                bs.bind(vertices1[i]);
        }
        else if(primitiveType == 2)
        {
            vertexCount = ccl::int32_t(vertices2.size());    // overwritten if reading
            bs.bind(vertexCount);
            for(ccl::int32_t i = 0, n = vertexCount; i < n; ++i)
                bs.bind(vertices2[i]);
        }
        else if(primitiveType == 4)
        {
            vertexCount = ccl::int32_t(vertices4.size());    // overwritten if reading
            bs.bind(vertexCount);
            for(ccl::int32_t i = 0, n = vertexCount; i < n; ++i)
                bs.bind(vertices4[i]);
        }
    }


}
