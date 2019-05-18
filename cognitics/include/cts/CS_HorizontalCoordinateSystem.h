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
/*! \file cts/CS_HorizontalCoordinateSystem.h
\headerfile cts/CS_HorizontalCoordinateSystem.h
\brief Provides cts::CS_HorizontalCoordinateSystem.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_CoordinateSystem.h"
#include "CS_HorizontalDatum.h"

namespace cts
{
/*! \brief CS_HorizontalCoordinateSystem
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
    class CS_HorizontalCoordinateSystem : public CS_CoordinateSystem
    {
    private:
        CS_HorizontalDatum horizontalDatum;

    public:
        virtual ~CS_HorizontalCoordinateSystem(void);
        CS_HorizontalCoordinateSystem(void);

        virtual void setHorizontalDatum(const CS_HorizontalDatum &horizontalDatum);
        virtual CS_HorizontalDatum getHorizontalDatum(void);

    };

}

