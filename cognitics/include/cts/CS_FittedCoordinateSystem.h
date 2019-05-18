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
/*! \file cts/CS_FittedCoordinateSystem.h
\headerfile cts/CS_FittedCoordinateSystem.h
\brief Provides cts::CS_FittedCoordinateSystem.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_CoordinateSystem.h"

#include <string>

namespace cts
{
/*! \brief CS_FittedCoordinateSystem
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
    class CS_FittedCoordinateSystem : public CS_CoordinateSystem
    {
    private:
        CS_CoordinateSystem baseCoordinateSystem;
        std::string toBaseWKT;

    public:
        virtual ~CS_FittedCoordinateSystem(void);
        CS_FittedCoordinateSystem(void);
        
        virtual void setBaseCoordinateSystem(const CS_CoordinateSystem &baseCoordinateSystem);
        virtual CS_CoordinateSystem getBaseCoordinateSystem(void);
        virtual void setToBase(const std::string &toBaseWKT);
        virtual std::string getToBase(void);

        virtual std::string getWKT(void);

    };

}

