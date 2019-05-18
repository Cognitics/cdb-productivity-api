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

#include "flt/BoundingConvexHull.h"

namespace flt
{
    BoundingConvexHull::~BoundingConvexHull(void)
    {
    }

    BoundingConvexHull::BoundingConvexHull(void)
    {
    }

    int BoundingConvexHull::getRecordType(void)
    {
        return FLT_BOUNDINGCONVEXHULL;
    }

    std::string BoundingConvexHull::getRecordName(void)
    {
        return "BoundingConvexHull";
    }

    void BoundingConvexHull::bind(ccl::BindStream &bs, int length, int revision)
    {
        ccl::BigEndian<ccl::uint32_t> numberOfTriangles = (ccl::uint32_t)triangles.size();    // overwritten if reading

        /*   4 */ bs.bind(numberOfTriangles);

        triangles.resize(numberOfTriangles);
        for(ccl::int32_t i = 0, n = numberOfTriangles; i < n; ++i)
        {
            bs.bind(triangles[i].vertex1X);
            bs.bind(triangles[i].vertex1Y);
            bs.bind(triangles[i].vertex1Z);
            bs.bind(triangles[i].vertex2X);
            bs.bind(triangles[i].vertex2Y);
            bs.bind(triangles[i].vertex2Z);
            bs.bind(triangles[i].vertex3X);
            bs.bind(triangles[i].vertex3Y);
            bs.bind(triangles[i].vertex3Z);
        }
    }


}
