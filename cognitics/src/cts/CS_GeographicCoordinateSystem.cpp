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

#include "cts/CS_GeographicCoordinateSystem.h"
#include <sstream>

namespace cts
{
    CS_GeographicCoordinateSystem::~CS_GeographicCoordinateSystem(void)
    {
    }

    CS_GeographicCoordinateSystem::CS_GeographicCoordinateSystem(void)
    {
    }

    void CS_GeographicCoordinateSystem::setAngularUnit(const CS_AngularUnit &angularUnit)
    {
        this->angularUnit = angularUnit;
    }

    CS_AngularUnit CS_GeographicCoordinateSystem::getAngularUnit(void)
    {
        return angularUnit;
    }

    void CS_GeographicCoordinateSystem::setPrimeMeridian(const CS_PrimeMeridian &primeMeridian)
    {
        this->primeMeridian = primeMeridian;
    }

    CS_PrimeMeridian CS_GeographicCoordinateSystem::getPrimeMeridian(void)
    {
        return primeMeridian;
    }

    void CS_GeographicCoordinateSystem::addWGS84ConversionInfo(const CS_WGS84ConversionInfo &wgs84ConversionInfo)
    {
        conversionsToWGS84.push_back(wgs84ConversionInfo);
    }

    int CS_GeographicCoordinateSystem::getNumConversionToWGS84(void)
    {
        return int(conversionsToWGS84.size());
    }

    CS_WGS84ConversionInfo CS_GeographicCoordinateSystem::getWGS84ConversionInfo(int index)
    {
        return conversionsToWGS84.at(index);
    }

    // <geographic cs> = GEOGCS["<name>", <datum>, <prime meridian>, <angular unit> {,<twin axes>} {,<authority>}]
    std::string CS_GeographicCoordinateSystem::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss << "GEOGCS[\"" << getName() << "\"," << getHorizontalDatum().getWKT() << "," << primeMeridian.getWKT() << "," << angularUnit.getWKT();
        for(int i = 0, c = getDimensions(); i < c; ++i)
            ss << "," << getAxisWKT(i);
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }


}