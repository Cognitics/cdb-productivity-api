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

#include "cts/CS_AngularUnit.h"
#include <sstream>

namespace cts
{
    CS_AngularUnit::~CS_AngularUnit(void)
    {
    }

    CS_AngularUnit::CS_AngularUnit(void) : radiansPerUnit(0.0f)
    {
    }

    void CS_AngularUnit::setRadiansPerUnit(double radiansPerUnit)
    {
        this->radiansPerUnit = radiansPerUnit;
    }

    double CS_AngularUnit::getRadiansPerUnit(void)
    {
        return radiansPerUnit;
    }

    // <angular unit> = <unit>
    // <unit> = UNIT["<name>", <conversion factor> {,<authority>}]
    std::string CS_AngularUnit::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss.precision(8);
        ss << std::fixed;
        ss << "UNIT[\"" << getName() << "\"," << getRadiansPerUnit();
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        return ss.str();
    }

}