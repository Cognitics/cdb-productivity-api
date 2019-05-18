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
/*! \file cts/CT_CoordinateTransformationAuthorityFactory.h
\headerfile cts/CT_CoordinateTransformationAuthorityFactory.h
\brief Provides cts::CT_CoordinateTransformationAuthorityFactory.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4
*/
#pragma once

#include "CT_CoordinateTransformation.h"

namespace cts
{
/*! \class cts::CT_CoordinateTransformationAuthorityFactory CT_CoordinateTransformationAuthorityFactory.h CT_CoordinateTransformationAuthorityFactory.h
\brief CT_CoordinateTransformationAuthorityFactory

\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.2
*/
    class CT_CoordinateTransformationAuthorityFactory
    {
    public:
        virtual ~CT_CoordinateTransformationAuthorityFactory(void) { }
        CT_CoordinateTransformationAuthorityFactory(void) { }

        virtual std::string getAuthority(void) = 0;
        virtual CT_CoordinateTransformation createFromTransformationCode(const std::string &code) = 0;
        virtual CT_CoordinateTransformation createFromCoordinateSystemCodes(const std::string &sourceCode, const std::string &targetCode) = 0;

    };

}

