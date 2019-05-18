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
/*! \file cts/CT_CoordinateTransformation.h
\headerfile cts/CT_CoordinateTransformation.h
\brief Provides cts::CT_CoordinateTransformation.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4
*/
#pragma once

#include "CS_CoordinateSystem.h"
#include "CT_MathTransform.h"

namespace cts
{
//! \sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.8
    enum CT_TransformType
    {
        CT_TT_Other                            = 0,
        CT_TT_Converation                    = 1,
        CT_TT_Transformation                = 2,
        CT_TT_ConverstionAndTransformation    = 3
    };

/*! \class cts::CT_CoordinateTransformation CT_CoordinateTransformation.h CT_CoordinateTransformation.h
\brief CT_CoordinateTransformation

\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.1
*/
    class CT_CoordinateTransformation
    {
        friend class CT_CoordinateTransformationFactory;

    private:
        CS_CoordinateSystem *source;
        CS_CoordinateSystem *target;

        CT_CoordinateTransformation(CS_CoordinateSystem *sourceCS, CS_CoordinateSystem *targetCS);

    public:
        virtual ~CT_CoordinateTransformation(void);

        // TODO: std::string getName(void);
        // TODO: std::string getAuthority(void);
        // TODO: std::string getAuthorityCode(void);
        // TODO: std::string getRemarks(void);
        // TODO: std::string getAreaOfUse(void);
        // TODO: CT_TransformType getTransformType(void);
        CS_CoordinateSystem *getSource(void);
        CS_CoordinateSystem *getTarget(void);
        CT_MathTransform *getMathTransform(void);

    };

}

