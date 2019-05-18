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
/*! \file cts/CS_CoordinateSystemFactory.h
\headerfile cts/CS_CoordinateSystemFactory.h
\brief Provides cts::CS_CoordinateSystemFactory.
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
#include "CS_PrimeMeridian.h"
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
/*! \brief CS_CoordinateSystemFactory
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3.7
*/
    class CS_CoordinateSystemFactory
    {
    public:
        ~CS_CoordinateSystemFactory(void);
        CS_CoordinateSystemFactory(void);

        // TODO: CS_CoordinateSystem createFromXML(const std::string &xml);
        CS_CoordinateSystem *createFromWKT(const std::string &wellKnownText);
        CS_CoordinateSystem *createFromWKT(const std::string &wellKnownText, bool);
        CS_CompoundCoordinateSystem createCompoundCoordinateSystem(const std::string &name, const CS_CoordinateSystem &head, const CS_CoordinateSystem &tail);
        CS_FittedCoordinateSystem createFittedCoordinateSystem(const std::string &name, const CS_CoordinateSystem &base, const std::string &toBaseWKT, const std::vector<CS_AxisInfo> &arAxes);
        CS_LocalCoordinateSystem createLocalCoordinateSystem(const std::string &name, const CS_LocalDatum &datum, const CS_Unit &unit, const std::vector<CS_AxisInfo> &arAxes);
        CS_Ellipsoid createEllipsoid(const std::string &name, double semiMajorAxis, double semiMinorAxis, const CS_LinearUnit &linearUnit);
        CS_Ellipsoid createFlattenedSphere(const std::string &name, double semiMajorAxis, double inverseFlattening, const CS_LinearUnit &linearUnit);
        CS_ProjectedCoordinateSystem createProjectedCoordinateSystem(const std::string &name, const CS_GeographicCoordinateSystem &gcs, const CS_Projection &projection, const CS_LinearUnit &linearUnit, const CS_AxisInfo &axis0, const CS_AxisInfo &axis1);
        CS_Projection createProjection(const std::string &name, const std::string wktProjectionClass, const std::vector<CS_ProjectionParameter> &parameters);
        CS_HorizontalDatum createHorizontalDatum(const std::string &name, CS_DatumType horizontalDatumType, const CS_Ellipsoid &ellipsoid, const CS_WGS84ConversionInfo &toWGS84);
        CS_PrimeMeridian createPrimeMeridian(const std::string &name, const CS_AngularUnit &angularUnit, double longitude);
        CS_GeographicCoordinateSystem createGeographicCoordinateSystem(const std::string &name, const CS_AngularUnit &angularUnit, const CS_HorizontalDatum &horizontalDatum, const CS_PrimeMeridian &primeMeridian, const CS_AxisInfo &axis0, const CS_AxisInfo &axis1);
        CS_LocalDatum createLocalDatum(const std::string &name, CS_DatumType localDatumType);
        CS_VerticalDatum createVerticalDatum(const std::string &name, CS_DatumType verticalDatumType);
        CS_VerticalCoordinateSystem createVerticalCoordinateSystem(const std::string &name, const CS_VerticalDatum &verticalDatum, const CS_LinearUnit &verticalUnit, const CS_AxisInfo &axis);

    };

}
