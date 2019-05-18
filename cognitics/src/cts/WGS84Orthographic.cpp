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

#include "cts/WGS84Orthographic.h"
#include <math.h>
#include <fstream>

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

namespace cts
{
    WGS84Orthographic::~WGS84Orthographic(void)
    {
    }

    WGS84Orthographic::WGS84Orthographic(void) : lat_origin(0.0f), lon_origin(0.0f)
    {
    }

    WGS84Orthographic::WGS84Orthographic(double originLat, double originLon)
    {
        setOrigin(originLat, originLon);
    }

    void WGS84Orthographic::setOrigin(double lat, double lon)
    {
        lat_origin = lat * M_PI / 180.0f;       
        lon_origin = lon * M_PI / 180.0f;
    }

    double WGS84Orthographic::getOriginLatitude(void) const
    {
        return lat_origin * 180.0f / M_PI;
    }

    double WGS84Orthographic::getOriginLongitude(void) const
    {
        return lon_origin * 180.0f / M_PI;
    }

    void WGS84Orthographic::convertLocalToGeo(double &x, double &y) const
    {
        double xtmp = x;
        double ytmp = y;
        double r = 6378137.0f;
        double p = sqrt(x * x + y * y);
        double c = asin(p / r);
        x = lon_origin + atan2(xtmp * sin(c), (p * cos(lat_origin) * cos(c)) - (ytmp * sin(lat_origin) * sin(c)));
        y = asin((cos(c) * sin(lat_origin)) + (ytmp * sin(c) * cos(lat_origin) / p));
        x *= 180.0f / M_PI;
        y *= 180.0f / M_PI;
    }

    void WGS84Orthographic::convertGeoToLocal(double &x, double &y) const
    {
        double lon = x * M_PI / 180.0f;
        double lat = y * M_PI / 180.0f;
        double r = 6378137.0f;
        x = r * cos(lat) * sin(lon - lon_origin);
        y = r * ((cos(lat_origin) * sin(lat)) - (sin(lat_origin) * cos(lat) * cos(lon - lon_origin)));
    }

    bool WGS84Orthographic::writePrj(std::string filename) const
    {
        std::ofstream fileStream;
        fileStream.open(filename.c_str(), std::ifstream::out);
        if(fileStream.fail())
            return false;
        fileStream << std::fixed;        
        fileStream.precision(15);

        fileStream << "PROJCS[\"Orthographic\",GEOGCS[\"Geographic Coordinate System\",DATUM[\"WGS84\",";
        fileStream << "SPHEROID[\"WGS84\",6378137,298.257223560493]],PRIMEM[\"Greenwich\",0],";
        fileStream << "UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Orthographic\"],PARAMETER[\"longitude_of_center\",";
        fileStream << getOriginLongitude() << "],PARAMETER[\"latitude_of_center\",";
        fileStream << getOriginLatitude() << "],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
        
        fileStream.close();
        return true;
    }

    
}