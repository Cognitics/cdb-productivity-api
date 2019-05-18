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

#include "cts/CS_Ellipsoid.h"
#include <sstream>

namespace cts
{
    CS_Ellipsoid::~CS_Ellipsoid(void)
    {
    }

    CS_Ellipsoid::CS_Ellipsoid(void) : semiMajorAxis(0.0f), semiMinorAxis(0.0f), inverseFlattening(0.0f), ivfDefinitive(false)
    {
    }

    void CS_Ellipsoid::setSemiMajorAxis(double semiMajorAxis)
    {
        this->semiMajorAxis = semiMajorAxis;
    }

    double CS_Ellipsoid::getSemiMajorAxis(void)
    {
        return semiMajorAxis;
    }

    void CS_Ellipsoid::setSemiMinorAxis(double semiMinorAxis)
    {
        this->semiMinorAxis = semiMinorAxis;
    }

    double CS_Ellipsoid::getSemiMinorAxis(void)
    {
        return semiMinorAxis;
    }

    void CS_Ellipsoid::setInverseFlattening(double inverseFlattening)
    {
        this->inverseFlattening = inverseFlattening;
    }

    double CS_Ellipsoid::getInverseFlattening(void)
    {
        return inverseFlattening;
    }

    void CS_Ellipsoid::setIVFDefinitive(bool ivfDefinitive)
    {
        this->ivfDefinitive = ivfDefinitive;
    }

    bool CS_Ellipsoid::getIVFDefinitive(void)
    {
        return ivfDefinitive;
    }

    void CS_Ellipsoid::setAxisUnit(const CS_LinearUnit &axisUnit)
    {
        this->axisUnit = axisUnit;
    }

    CS_LinearUnit CS_Ellipsoid::getAxisUnit(void)
    {
        return axisUnit;
    }

    // <spheroid> = SPHEROID["<name>", <semi-major axis>, <inverse flattening> {,<authority>}]
    std::string CS_Ellipsoid::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss.precision(8);
        ss << std::fixed;
        ss << "SPHEROID[\"" << getName() << "\"," << semiMajorAxis << "," << inverseFlattening;
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }

}