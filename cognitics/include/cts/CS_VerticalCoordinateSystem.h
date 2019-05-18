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
/*! \file cts/CS_VerticalCoordinateSystem.h
\headerfile cts/CS_VerticalCoordinateSystem.h
\brief Provides cts::CS_VerticalCoordinateSystem.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_CoordinateSystem.h"
#include "CS_VerticalDatum.h"
#include "CS_LinearUnit.h"

namespace cts
{
/*! \brief CS_VerticalCoordinateSystem
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
    class CS_VerticalCoordinateSystem : public CS_CoordinateSystem
    {
    private:
        CS_VerticalDatum verticalDatum;
        CS_LinearUnit verticalUnit;

    public:
        virtual ~CS_VerticalCoordinateSystem(void);
        CS_VerticalCoordinateSystem(void);
        
        virtual void setVerticalDatum(const CS_VerticalDatum &verticalDatum);
        virtual CS_VerticalDatum getVerticalDatum(void);
        virtual void setVerticalUnit(const CS_LinearUnit &verticalUnit);
        virtual CS_LinearUnit getVerticalUnit(void);

        virtual std::string getWKT(void);

    };

}

