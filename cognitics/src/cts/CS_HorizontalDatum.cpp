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

#include "cts/CS_HorizontalDatum.h"
#include <sstream>

namespace cts
{
    CS_WGS84ConversionInfo::CS_WGS84ConversionInfo(void) : dx(0.0f), dy(0.0f), dz(0.0f), ex(0.0f), ey(0.0f), ez(0.0f), ppm(0.0f)
    {
    }

    // <to wgs84s> = TOWGS84[<seven param>]
    // <seven param> = <dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>
    std::string CS_WGS84ConversionInfo::getWKT(void)
    {
        std::stringstream ss;
        ss.precision(8);
        ss << "TOWGS84[" << dx << "," << dy << "," << dz << "," << ex << "," << ey << "," << ez << "," << ppm << "]";
        return ss.str();
    }

    CS_HorizontalDatum::~CS_HorizontalDatum(void)
    {
    }

    CS_HorizontalDatum::CS_HorizontalDatum(void) : hasWGS84Parameters(false)
    {
    }

    void CS_HorizontalDatum::setEllipsoid(const CS_Ellipsoid &ellipsoid)
    {
        this->ellipsoid = ellipsoid;
    }

    CS_Ellipsoid CS_HorizontalDatum::getEllipsoid(void)
    {
        return ellipsoid;
    }

    void CS_HorizontalDatum::setWGS84Parameters(const CS_WGS84ConversionInfo &wgs84Parameters)
    {
        this->wgs84Parameters = wgs84Parameters;
        hasWGS84Parameters = true;
    }

    CS_WGS84ConversionInfo CS_HorizontalDatum::getWGS84Parameters(void)
    {
        return wgs84Parameters;
    }

    // <datum> = DATUM["<name>", <spheroid> {,<to wgs84>} {,<authority>}]
    std::string CS_HorizontalDatum::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::string wgs84wkt = wgs84Parameters.getWKT();
        std::stringstream ss;
        ss << "DATUM[\"" << getName() << "\"," << ellipsoid.getWKT();
        if(hasWGS84Parameters)
            ss << "," << wgs84wkt;
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }

}