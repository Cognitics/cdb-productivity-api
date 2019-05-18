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

#include "cts/CS_FittedCoordinateSystem.h"
#include <sstream>

namespace cts
{
    CS_FittedCoordinateSystem::~CS_FittedCoordinateSystem(void)
    {
    }

    CS_FittedCoordinateSystem::CS_FittedCoordinateSystem(void)
    {
    }

    void CS_FittedCoordinateSystem::setBaseCoordinateSystem(const CS_CoordinateSystem &baseCoordinateSystem)
    {
        this->baseCoordinateSystem = baseCoordinateSystem;
    }

    CS_CoordinateSystem CS_FittedCoordinateSystem::getBaseCoordinateSystem(void)
    {
        return baseCoordinateSystem;
    }

    void CS_FittedCoordinateSystem::setToBase(const std::string &toBaseWKT)
    {
        this->toBaseWKT = toBaseWKT;
    }

    std::string CS_FittedCoordinateSystem::getToBase(void)
    {
        return toBaseWKT;
    }

    // <fitted cs> = FITTED_CS["<name>", <to base>, <base cs>]
    std::string CS_FittedCoordinateSystem::getWKT(void)
    {
        std::stringstream ss;
        ss << "FITTED_CS[\"" << getName() << "\"," << toBaseWKT << "," << baseCoordinateSystem.getWKT() << "]";
        return ss.str();
    }


}