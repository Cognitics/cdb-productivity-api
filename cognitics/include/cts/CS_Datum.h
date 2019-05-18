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
/*! \file cts/CS_Datum.h
\headerfile cts/CS_Datum.h
\brief Provides cts::CS_Datum.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_Info.h"

namespace cts
{
    enum CS_DatumType
    {
        HD_Min                    = 1000,
        HD_Other                = 1000,
        HD_Classic                = 1001,
        HD_Geocentric            = 1002,
        HD_Max                    = 1999,
        VD_Min                    = 2000,
        VD_Other                = 2000,
        VD_Orthometric            = 2001,
        VD_Ellipsoidal            = 2002,
        VD_AltitudeBarometric    = 2003,
        VD_Normal                = 2004,
        VD_GeoidModelDerived    = 2005,
        VD_Depth                = 2006,
        VD_Max                    = 2999,
        LD_Min                    = 10000,
        LD_Max                    = 32767
    };

/*! \brief CS_Datum
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
    class CS_Datum : public CS_Info
    {
    private:
        CS_DatumType datumType;

    public:
        virtual ~CS_Datum(void);
        CS_Datum(void);
        
        virtual void setDatumType(CS_DatumType datumType);
        virtual CS_DatumType getDatumType(void);

    };

}

