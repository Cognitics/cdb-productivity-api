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
#pragma once

#include <string>

namespace cts
{
    //! Provides conversion between WGS84 geodetic and orthographic coordinates.
    class FlatEarthProjection
    {
    private:
        double lat_origin;
        double lon_origin;
        double convergence;

    public:
        ~FlatEarthProjection(void);
        FlatEarthProjection(void);
        FlatEarthProjection(double originLat, double originLon);

        //! Set the origin for conversions.
        void setOrigin(double lat, double lon);

        //! Get the origin latitude.
        double getOriginLatitude(void) const;

        //! Get the origin longitude.
        double getOriginLongitude(void) const;

        //! Returns true if transformations are valid (valid origin); false otherwise.
        bool isValid(void) const;

        //! Convert the local (flat earth) y-coordinate to geodetic latitude.
        double convertLocalToGeoLat(double y) const;

        //! Convert the local (flat earth) x-coordinate to geodetic longitude.
        double convertLocalToGeoLon(double x) const;

        //! Convert geodetic longitude to local (flat earth) x.
        double convertGeoToLocalX(double lon) const;

        //! Convert geodetic latitude to local (flat earth) y.
        double convertGeoToLocalY(double lat) const;

        //! Write the well-known-text representation of the projection to a file.
        bool writePrj(std::string filename) const;

    };

}