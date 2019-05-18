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
/*! \file cts/CS_HorizontalDatum.h
\headerfile cts/CS_HorizontalDatum.h
\brief Provides cts::CS_HorizontalDatum.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_Datum.h"
#include "CS_LinearUnit.h"
#include "CS_Ellipsoid.h"
#include <string>

namespace cts
{
    struct CS_WGS84ConversionInfo
    {
        double dx;
        double dy;
        double dz;
        double ex;
        double ey;
        double ez;
        double ppm;
        std::string areaOfUse;

        CS_WGS84ConversionInfo(void);
        std::string getWKT(void);
    };

/*! \brief CS_HorizontalDatum
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
    class CS_HorizontalDatum : public CS_Datum
    {
    private:
        CS_Ellipsoid ellipsoid;
        CS_WGS84ConversionInfo wgs84Parameters;
        bool hasWGS84Parameters;

    public:
        virtual ~CS_HorizontalDatum(void);
        CS_HorizontalDatum(void);
        
        virtual void setEllipsoid(const CS_Ellipsoid &ellipsoid);
        virtual CS_Ellipsoid getEllipsoid(void);
        virtual void setWGS84Parameters(const CS_WGS84ConversionInfo &wgs84Parameters);
        virtual CS_WGS84ConversionInfo getWGS84Parameters(void);

        virtual std::string getWKT(void);

    };

}

