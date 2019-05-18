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

#include "cts/CS_LocalCoordinateSystem.h"
#include <sstream>

namespace cts
{
    CS_LocalCoordinateSystem::~CS_LocalCoordinateSystem(void)
    {
    }

    CS_LocalCoordinateSystem::CS_LocalCoordinateSystem(void)
    {
    }

    void CS_LocalCoordinateSystem::setDimensions(int dimensions)
    {
        // we are just making this public
        CS_CoordinateSystem::setDimensions(dimensions);
    }

    void CS_LocalCoordinateSystem::setLocalDatum(const CS_LocalDatum &localDatum)
    {
        this->localDatum = localDatum;
    }

    CS_LocalDatum CS_LocalCoordinateSystem::getLocalDatum(void)
    {
        return localDatum;
    }

    // <local cs> = LOCAL_CS["<name>", <local datum>, <unit>, <axis>, {,<axis>}* {,<authority>}]
    std::string CS_LocalCoordinateSystem::getWKT(void)
    {
        if(getDimensions() < 1)
            return std::string();
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss << "PROJCS[\"" << getName() << "\"," << localDatum.getWKT() << "," << getUnits(1).getWKT();
        for(int i = 0, c = getDimensions(); i < c; ++i)
            ss << "," << getAxisWKT(i);
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }

}