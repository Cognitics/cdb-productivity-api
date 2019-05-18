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

#include "cts/CS_GeocentricCoordinateSystem.h"
#include <sstream>

namespace cts
{
    CS_GeocentricCoordinateSystem::~CS_GeocentricCoordinateSystem(void)
    {
    }

    CS_GeocentricCoordinateSystem::CS_GeocentricCoordinateSystem(void) : CS_CoordinateSystem(3)
    {
    }

    void CS_GeocentricCoordinateSystem::setHorizontalDatum(const CS_HorizontalDatum &horizontalDatum)
    {
        this->horizontalDatum = horizontalDatum;
    }

    CS_HorizontalDatum CS_GeocentricCoordinateSystem::getHorizontalDatum(void)
    {
        return horizontalDatum;
    }

    void CS_GeocentricCoordinateSystem::setLinearUnit(const CS_LinearUnit &linearUnit)
    {
        this->linearUnit = linearUnit;
    }

    CS_LinearUnit CS_GeocentricCoordinateSystem::getLinearUnit(void)
    {
        return linearUnit;
    }

    void CS_GeocentricCoordinateSystem::setPrimeMeridian(const CS_PrimeMeridian &primeMeridian)
    {
        this->primeMeridian = primeMeridian;
    }

    CS_PrimeMeridian CS_GeocentricCoordinateSystem::getPrimeMeridian(void)
    {
        return primeMeridian;
    }

    // <geocentric cs> = GEOCCS["<name>", <datum>, <prime meridian>, <linear unit> {,<axis>, <axis>, <axis>} {,<authority>}]
    std::string CS_GeocentricCoordinateSystem::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss << "GEOCCS[\"" << getName() << "\"," << horizontalDatum.getWKT() << "," << primeMeridian.getWKT() << "," << linearUnit.getWKT();
        for(int i = 0, c = getDimensions(); i < c; ++i)
            ss << "," << getAxisWKT(i);
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }


}