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

#include "cts/WGS84FromFlatEarthMathTransform.h"

namespace cts
{
    WGS84FromFlatEarthMathTransform::~WGS84FromFlatEarthMathTransform(void)
    {
    }

    WGS84FromFlatEarthMathTransform::WGS84FromFlatEarthMathTransform(double originLat, double originLon) : flatEarthProjection(originLat, originLon)
    {
    }

    PT_CoordinatePoint WGS84FromFlatEarthMathTransform::transform(const PT_CoordinatePoint &cp)
    {
        PT_CoordinatePoint result;
        if(cp.size() > 0)
            result.push_back(flatEarthProjection.convertLocalToGeoLon(cp[0]));
        if(cp.size() > 1)
            result.push_back(flatEarthProjection.convertLocalToGeoLat(cp[1]));
        for(int i = 2, c = int(cp.size()); i < c; ++i)
            result.push_back(cp[i]);
        return result;
    }

    void WGS84FromFlatEarthMathTransform::setOrigin(double originLat, double originLon)
    {
        flatEarthProjection.setOrigin(originLat, originLon);
    }


}