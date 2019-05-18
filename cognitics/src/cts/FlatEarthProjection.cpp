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

#include "cts/FlatEarthProjection.h"
#include <math.h>
#include <fstream>

#ifndef M_PI
#define M_PI 3.1415926535897932385
#endif

namespace cts
{
    FlatEarthProjection::~FlatEarthProjection(void)
    {
    }

    FlatEarthProjection::FlatEarthProjection(void) : lat_origin(0.0f), lon_origin(0.0f), convergence(0.0f)
    {
    }

    FlatEarthProjection::FlatEarthProjection(double originLat, double originLon)
    {
        setOrigin(originLat, originLon);
    }

    void FlatEarthProjection::setOrigin(double lat, double lon)
    {
        lat_origin = lat;       
        lon_origin = lon;
        convergence = cos(lat_origin * (M_PI / 180.0));
    }

    double FlatEarthProjection::getOriginLatitude(void) const
    {
        return lat_origin;
    }

    double FlatEarthProjection::getOriginLongitude(void) const
    {
        return lon_origin;
    }

    bool FlatEarthProjection::isValid(void) const
    {
        return (convergence != 0.0f);
    }

    double FlatEarthProjection::convertLocalToGeoLat(double x) const
    {
        return (x / 111120.0) + lat_origin;
    }

    double FlatEarthProjection::convertLocalToGeoLon(double y) const
    {
        return ((y / (convergence  * 111120.0)) + lon_origin);
    }

    double FlatEarthProjection::convertGeoToLocalX(double lon) const
    {
        return (lon - lon_origin) * convergence  * 111120.0;
    }

    double FlatEarthProjection::convertGeoToLocalY(double lat) const
    {
        return (lat - lat_origin) * 111120.0;
    }

    bool FlatEarthProjection::writePrj(std::string filename) const
    {
        std::ofstream fileStream;
        fileStream.open(filename.c_str(), std::ifstream::out);
        if(fileStream.fail())
            return false;
        fileStream << std::fixed;        
        fileStream.precision(15);

        // TODO: this is for orthographic; we need a flat earth version

        fileStream << "PROJCS[\"Orthographic\",GEOGCS[\"Geographic Coordinate System\",DATUM[\"WGS84\",";
        fileStream << "SPHEROID[\"WGS84\",6378137,298.257223560493]],PRIMEM[\"Greenwich\",0],";
        fileStream << "UNIT[\"degree\",0.0174532925199433]],PROJECTION[\"Orthographic\"],PARAMETER[\"longitude_of_center\",";
        fileStream << lon_origin << "],PARAMETER[\"latitude_of_center\",";
        fileStream << lat_origin << "],PARAMETER[\"false_easting\",0],PARAMETER[\"false_northing\",0],UNIT[\"Meter\",1]]";
        
        fileStream.close();
        return true;
    }

    
}