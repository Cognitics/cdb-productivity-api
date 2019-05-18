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
/*! \file cts/CS_CoordinateSystemAuthorityFactory.h
\headerfile cts/CS_CoordinateSystemAuthorityFactory.h
\brief Provides cts::CS_CoordinateSystemAuthorityFactory.
\author Aaron Brinton <abrinton@cognitics.net>
\date 10 July 2010
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_Info.h"
#include "CS_Unit.h"
#include "CS_LinearUnit.h"
#include "CS_AngularUnit.h"
#include "CS_CoordinateSystem.h"
#include "CS_Datum.h"
#include "CS_LocalDatum.h"
#include "CS_VerticalDatum.h"
#include "CS_HorizontalDatum.h"
#include "CS_CompoundCoordinateSystem.h"
#include "CS_FittedCoordinateSystem.h"
#include "CS_GeocentricCoordinateSystem.h"
#include "CS_HorizontalCoordinateSystem.h"
#include "CS_VerticalCoordinateSystem.h"
#include "CS_LocalCoordinateSystem.h"
#include "CS_ProjectedCoordinateSystem.h"
#include "CS_GeographicCoordinateSystem.h"

namespace cts
{
/*! \brief CS_CoordinateSystemAuthorityFactory
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3.6
*/
    class CS_CoordinateSystemAuthorityFactory
    {
    public:
        virtual ~CS_CoordinateSystemAuthorityFactory(void) { }
        CS_CoordinateSystemAuthorityFactory(void) { }

        virtual std::string getAuthority(void) = 0;
        virtual CS_AngularUnit createAngularUnit(const std::string &code) = 0;
        virtual CS_CompoundCoordinateSystem createCompoundCoordinateSystem(const std::string &code) = 0;
        virtual CS_Ellipsoid createEllipsoid(const std::string &code) = 0;
        virtual CS_GeographicCoordinateSystem createGeographicCoordinateSystem(const std::string &code) = 0;
        virtual CS_HorizontalCoordinateSystem createHorizontalCoordinateSystem(const std::string &code) = 0;
        virtual CS_HorizontalDatum createHorizontalDatum(const std::string &code) = 0;
        virtual CS_LinearUnit createLinearUnit(const std::string &code) = 0;
        virtual CS_PrimeMeridian createPrimeMeridian(const std::string &code) = 0;
        virtual CS_ProjectedCoordinateSystem createProjectedCoordinateSystem(const std::string &code) = 0;
        virtual CS_VerticalCoordinateSystem createVerticalCoordinateSystem(const std::string &code) = 0;
        virtual CS_VerticalDatum createVerticalDatum(const std::string &code) = 0;
        virtual std::string getDescriptionText(const std::string &code) = 0;
        virtual std::string getGeoidFromWKTName(const std::string &wkt) = 0;
        virtual std::string getWKTGeoidName(int geoid) = 0;

    };

}
