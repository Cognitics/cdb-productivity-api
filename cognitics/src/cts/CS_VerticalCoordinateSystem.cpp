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

#include "cts/CS_VerticalCoordinateSystem.h"
#include <sstream>

namespace cts
{
    CS_VerticalCoordinateSystem::~CS_VerticalCoordinateSystem(void)
    {
    }

    CS_VerticalCoordinateSystem::CS_VerticalCoordinateSystem(void) : CS_CoordinateSystem(1)
    {
    }

    void CS_VerticalCoordinateSystem::setVerticalDatum(const CS_VerticalDatum &verticalDatum)
    {
        this->verticalDatum = verticalDatum;
    }

    CS_VerticalDatum CS_VerticalCoordinateSystem::getVerticalDatum(void)
    {
        return verticalDatum;
    }

    void CS_VerticalCoordinateSystem::setVerticalUnit(const CS_LinearUnit &verticalUnit)
    {
        this->verticalUnit = verticalUnit;
    }

    CS_LinearUnit CS_VerticalCoordinateSystem::getVerticalUnit(void)
    {
        return verticalUnit;
    }

    // <vert cs> = VERT_CS["<name>", <vert datum>, <linear unit>, {<axis>,} {,<authority>}]
    std::string CS_VerticalCoordinateSystem::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss << "VERT_CS[\"" << getName() << "\"," << verticalDatum.getWKT() << "," << verticalUnit.getWKT();
        for(int i = 0, c = getDimensions(); i < c; ++i)
            ss << "," << getAxisWKT(i);
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }


}