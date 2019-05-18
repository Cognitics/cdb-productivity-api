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
/*! \file cts/CT_MathTransformFactory.h
\headerfile cts/CT_MathTransformFactory.h
\brief Provides cts::CT_MathTransformFactory.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4
*/
#pragma once

#include "pt.h"
#include "CT_MathTransform.h"
#include <string>

namespace cts
{
//! \sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.7
    struct CT_Parameter
    {
        std::string name;
        double value;
    };

/*! \class cts::CT_MathTransformFactory CT_MathTransformFactory.h CT_MathTransformFactory.h
\brief CT_MathTransformFactory

\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.6
*/
    class CT_MathTransformFactory
    {
    public:
        ~CT_MathTransformFactory(void);
        CT_MathTransformFactory(void);

        // TODO: CT_MathTransform createAffineTransform(const PT_Matrix &matrix);
        // TODO: CT_MathTransform createConcatenatedTransform(const CT_MathTransform &transform1, const CT_MathTransform &transform2);
        // TODO: CT_MathTransform createPassThroughTransform(int firstAffectedOrdinate, const CT_MathTransform &subTransform);
        // TODO: CT_MathTransform createParameterizedTransform(const std::string &classification, const std::vector<CT_Parameter> &parameters);
        // TODO: CT_MathTransform createFromWKT(const std::string &wellKnownText);
        // TODO: CT_MathTransform createFromXML(const std::string &xml);
        // TODO: bool isParameterAngular(const std::string &parameterName);
        // TODO: bool isParameterLinear(const std::string &parameterName);

        // cognitics
        CT_MathTransform *createFromOGR(CS_CoordinateSystem *sourceCS, CS_CoordinateSystem *targetCS);

    };

}
