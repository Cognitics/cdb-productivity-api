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

#include "cts/CS_CoordinateSystemFactory.h"

#include <ogr_spatialref.h>
#include <cpl_conv.h>

namespace cts
{
    CS_CoordinateSystemFactory::~CS_CoordinateSystemFactory(void)
    {
    }

    CS_CoordinateSystemFactory::CS_CoordinateSystemFactory(void)
    {
    }

/*
    CS_CoordinateSystem CS_CoordinateSystemFactory::createFromXML(const std::string &xml)
    {
        // TODO
        throw std::runtime_error("CS_CoordinateSystemFactory::createFromXML(): not yet implemented");
    }
*/

    CS_CoordinateSystem *CS_CoordinateSystemFactory::createFromWKT(const std::string &wellKnownText)
    {
        CS_CoordinateSystem *cs = new CS_CoordinateSystem;
        cs->wkt = wellKnownText;

        // accept the WGS84 short name, but get the full wkt from gdal
        if(wellKnownText == "WGS84")
        {
            OGRSpatialReference oSRS;
            char *pszSRS_WKT = NULL;
            oSRS.SetWellKnownGeogCS("WGS84");
            oSRS.exportToWkt(&pszSRS_WKT);
            cs->wkt = std::string(pszSRS_WKT);
            CPLFree(pszSRS_WKT);
        }

        return cs;
    }

    // <coordinate system> = <geographic cs> | <projected cs> | <geocentric cs> | <vert cs> | <compd cs> | <fitted cs> | <local cs>
    CS_CoordinateSystem *CS_CoordinateSystemFactory::createFromWKT(const std::string &wellKnownText, bool)
    {
        return NULL;
        /*
        std::string size_type a = wellKnownText.find_first_not_of(" \t\r\n");
        std::string::size_type bracket = wellKnownText.find('[');
        if(bracket == std::string::npos)
            return CS_CoordinateSystemSP();
        std::string prefix = wellKnownText.substr(0, bracket);
        if(prefix == "
        */
//<geographic cs> = GEOGCS["<name>", <datum>, <prime meridian>, <angular unit> {,<twin axes>} {,<authority>}]
//<projected cs> = PROJCS["<name>", <geographic cs>, <projection>, {<parameter>,}* <linear unit> {,<twin axes>}{,<authority>}]
//<geocentric cs> = GEOCCS["<name>", <datum>, <prime meridian>, <linear unit> {,<axis>, <axis>, <axis>} {,<authority>}]
//<vert cs> = VERT_CS["<name>", <vert datum>, <linear unit>, {<axis>,} {,<authority>}]
//<fitted cs> = FITTED_CS["<name>", <to base>, <base cs>]
//<compd cs> = COMPD_CS["<name>", <head cs>, <tail cs> {,<authority>}]
//<local cs> = LOCAL_CS["

    }

    CS_CompoundCoordinateSystem CS_CoordinateSystemFactory::createCompoundCoordinateSystem(const std::string &name, const CS_CoordinateSystem &head, const CS_CoordinateSystem &tail)
    {
        CS_CompoundCoordinateSystem cs;
        cs.setName(name);
        cs.setHeadCS(head);
        cs.setTailCS(tail);
        return cs;
    }

    CS_FittedCoordinateSystem CS_CoordinateSystemFactory::createFittedCoordinateSystem(const std::string &name, const CS_CoordinateSystem &base, const std::string &toBaseWKT, const std::vector<CS_AxisInfo> &arAxes)
    {
        CS_FittedCoordinateSystem cs;
        cs.setName(name);
        cs.setBaseCoordinateSystem(base);
        cs.setToBase(toBaseWKT);
        for(int i = 0, c = int(arAxes.size()); i < c; ++i)
            cs.setAxis(i, arAxes.at(i));
        return cs;
    }

    CS_LocalCoordinateSystem CS_CoordinateSystemFactory::createLocalCoordinateSystem(const std::string &name, const CS_LocalDatum &datum, const CS_Unit &unit, const std::vector<CS_AxisInfo> &arAxes)
    {
        CS_LocalCoordinateSystem cs;
        cs.setName(name);
        cs.setLocalDatum(datum);
        for(int i = 0, c = int(arAxes.size()); i < c; ++i)
        {
            cs.setUnits(i, unit);
            cs.setAxis(i, arAxes.at(i));
        }
        return cs;
    }

    CS_Ellipsoid CS_CoordinateSystemFactory::createEllipsoid(const std::string &name, double semiMajorAxis, double semiMinorAxis, const CS_LinearUnit &linearUnit)
    {
        CS_Ellipsoid ellipsoid;
        ellipsoid.setName(name);
        ellipsoid.setSemiMajorAxis(semiMajorAxis);
        ellipsoid.setSemiMinorAxis(semiMinorAxis);
        ellipsoid.setAxisUnit(linearUnit);
        return ellipsoid;
    }

    CS_Ellipsoid CS_CoordinateSystemFactory::createFlattenedSphere(const std::string &name, double semiMajorAxis, double inverseFlattening, const CS_LinearUnit &linearUnit)
    {
        CS_Ellipsoid ellipsoid;
        ellipsoid.setName(name);
        ellipsoid.setSemiMajorAxis(semiMajorAxis);
        ellipsoid.setInverseFlattening(inverseFlattening);
        ellipsoid.setAxisUnit(linearUnit);
        return ellipsoid;
    }

    CS_ProjectedCoordinateSystem CS_CoordinateSystemFactory::createProjectedCoordinateSystem(const std::string &name, const CS_GeographicCoordinateSystem &gcs, const CS_Projection &projection, const CS_LinearUnit &linearUnit, const CS_AxisInfo &axis0, const CS_AxisInfo &axis1)
    {
        CS_ProjectedCoordinateSystem cs;
        cs.setName(name);
        cs.setGeographicCoordinateSystem(gcs);
        cs.setProjection(projection);
        cs.setLinearUnit(linearUnit);
        cs.setAxis(0, axis0);
        cs.setAxis(1, axis1);
        return cs;
    }

    CS_Projection CS_CoordinateSystemFactory::createProjection(const std::string &name, const std::string wktProjectionClass, const std::vector<CS_ProjectionParameter> &parameters)
    {
        CS_Projection prj;
        prj.setName(name);
        prj.setClassName(wktProjectionClass);
        for(int i = 0, c = int(parameters.size()); i < c; ++i)
        {
            const CS_ProjectionParameter &param = parameters.at(i);
            prj.setParameter(param.name, param.value);
        }
        return prj;
    }

    CS_HorizontalDatum CS_CoordinateSystemFactory::createHorizontalDatum(const std::string &name, CS_DatumType horizontalDatumType, const CS_Ellipsoid &ellipsoid, const CS_WGS84ConversionInfo &toWGS84)
    {
        CS_HorizontalDatum datum;
        datum.setName(name);
        datum.setDatumType(horizontalDatumType);
        datum.setEllipsoid(ellipsoid);
        datum.setWGS84Parameters(toWGS84);
        return datum;
    }

    CS_PrimeMeridian CS_CoordinateSystemFactory::createPrimeMeridian(const std::string &name, const CS_AngularUnit &angularUnit, double longitude)
    {
        CS_PrimeMeridian pm;
        pm.setName(name);
        pm.setAngularUnit(angularUnit);
        pm.setLongitude(longitude);
        return pm;
    }

    CS_GeographicCoordinateSystem CS_CoordinateSystemFactory::createGeographicCoordinateSystem(const std::string &name, const CS_AngularUnit &angularUnit, const CS_HorizontalDatum &horizontalDatum, const CS_PrimeMeridian &primeMeridian, const CS_AxisInfo &axis0, const CS_AxisInfo &axis1)
    {
        CS_GeographicCoordinateSystem cs;
        cs.setName(name);
        cs.setAngularUnit(angularUnit);
        cs.setHorizontalDatum(horizontalDatum);
        cs.setPrimeMeridian(primeMeridian);
        cs.setAxis(0, axis0);
        cs.setAxis(1, axis1);
        return cs;
    }

    CS_LocalDatum CS_CoordinateSystemFactory::createLocalDatum(const std::string &name, CS_DatumType localDatumType)
    {
        CS_LocalDatum datum;
        datum.setName(name);
        datum.setDatumType(localDatumType);
        return datum;
    }

    CS_VerticalDatum CS_CoordinateSystemFactory::createVerticalDatum(const std::string &name, CS_DatumType verticalDatumType)
    {
        CS_VerticalDatum datum;
        datum.setName(name);
        datum.setDatumType(verticalDatumType);
        return datum;
    }

    CS_VerticalCoordinateSystem CS_CoordinateSystemFactory::createVerticalCoordinateSystem(const std::string &name, const CS_VerticalDatum &verticalDatum, const CS_LinearUnit &verticalUnit, const CS_AxisInfo &axis)
    {
        CS_VerticalCoordinateSystem cs;
        cs.setName(name);
        cs.setVerticalDatum(verticalDatum);
        cs.setVerticalUnit(verticalUnit);
        cs.setAxis(1, axis);
        return cs;
    }


}