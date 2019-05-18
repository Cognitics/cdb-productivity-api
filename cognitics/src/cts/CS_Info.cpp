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

#include "cts/CS_Info.h"

namespace cts
{

/*
<coordinate system> = <geographic cs> | <projected cs> | <geocentric cs> | <vert cs> | <compd cs> | <fitted cs> | <local cs>
<geographic cs> = GEOGCS["<name>", <datum>, <prime meridian>, <angular unit> {,<twin axes>} {,<authority>}]
<projected cs> = PROJCS["<name>", <geographic cs>, <projection>, {<parameter>,}* <linear unit> {,<twin axes>}{,<authority>}]
<geocentric cs> = GEOCCS["<name>", <datum>, <prime meridian>, <linear unit> {,<axis>, <axis>, <axis>} {,<authority>}]
<vert cs> = VERT_CS["<name>", <vert datum>, <linear unit>, {<axis>,} {,<authority>}]
<fitted cs> = FITTED_CS["<name>", <to base>, <base cs>]
<compd cs> = COMPD_CS["<name>", <head cs>, <tail cs> {,<authority>}]
<local cs> = LOCAL_CS["<name>", <local datum>, <unit>, <axis>, {,<axis>}* {,<authority>}]

<projection> = PROJECTION["<name>" {,<authority>}]
<datum> = DATUM["<name>", <spheroid> {,<to wgs84>} {,<authority>}]
(ellipsoid) <spheroid> = SPHEROID["<name>", <semi-major axis>, <inverse flattening> {,<authority>}]
<prime meridian> = PRIMEM["<name>", <longitude> {,<authority>}]
<angular unit> = <unit>
<linear unit> = <unit>
<unit> = UNIT["<name>", <conversion factor> {,<authority>}]
<authority> = AUTHORITY["<name>", "<code>"]
<vert datum> = VERT_DATUM["<name>", <datum type> {,<authority>}]
<head cs> = <coordinate system>
<tail cs> = <coordinate system>
<twin axes> = <axis>, <axis>
<axis> = AXIS["<name>", NORTH | SOUTH | EAST | WEST | UP | DOWN | OTHER]
<to wgs84s> = TOWGS84[<seven param>]
<seven param> = <dx>, <dy>, <dz>, <ex>, <ey>, <ez>, <ppm>
<to base> = <math transform>
<base cs> = <coordinate system>
<local datum> = LOCAL_DATUM["<name>", <datum type> {,<authority>}]

<datum type> = <number>
<dx> = <number>
<dy> = <number>
<dz> = <number>
<ex> = <number>
<ey> = <number>
<ez> = <number>
<ppm> = <number>
<longitude> = <number>
<semi-major axis> = <number>
<inverse flattening> = <number>
<conversion factor> = <number>

*/

    // <authority> = AUTHORITY["<name>", "<code>"]
    std::string CS_Info::getAuthorityWKT(void)
    {
        if(authority.empty() && authorityCode.empty())
            return std::string();
        return "AUTHORITY[\"" + authority + ",\"" + authorityCode + "\"]";
    }

    CS_Info::~CS_Info(void)
    {
    }

    CS_Info::CS_Info(void)
    {
    }

    void CS_Info::setName(const std::string &name)
    {
        this->name = name;
    }

    std::string CS_Info::getName(void)
    {
        return name;
    }

    void CS_Info::setAuthority(const std::string &authority)
    {
        this->authority = authority;
    }

    std::string CS_Info::getAuthority(void)
    {
        return authority;
    }

    void CS_Info::setAuthorityCode(const std::string &authorityCode)
    {
        this->authorityCode = authorityCode;
    }

    std::string CS_Info::getAuthorityCode(void)
    {
        return authorityCode;
    }

    void CS_Info::setAlias(const std::string &alias)
    {
        this->alias = alias;
    }

    std::string CS_Info::getAlias(void)
    {
        return alias;
    }

    void CS_Info::setAbbreviation(const std::string &abbreviation)
    {
        this->abbreviation = abbreviation;
    }

    std::string CS_Info::getAbbreviation(void)
    {
        return abbreviation;
    }

    void CS_Info::setRemarks(const std::string &remarks)
    {
        this->remarks = remarks;
    }

    std::string CS_Info::getRemarks(void)
    {
        return remarks;
    }

    std::string CS_Info::getWKT(void)
    {
        return wkt;        // temporary
        return std::string();
    }

/*
    std::string CS_Info::getXML(void)
    {
        return std::string();
    }
*/

}
