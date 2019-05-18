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
/*! \file cts/CT_MathTransform.h
\headerfile cts/CT_MathTransform.h
\brief Provides cts::CT_MathTransform.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4
*/
#pragma once

#include "pt.h"
#include <vector>

namespace cts
{
    class CS_CoordinateSystem;

//! \sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.4
    enum CT_DomainFlags
    {
        CT_DF_Inside        = 1,
        CT_DF_Outside        = 2,
        CT_DF_Discontinuous    = 4
    };

/*! \class cts::CT_MathTransform CT_MathTransform.h CT_MathTransform.h
\brief CT_MathTransform

\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.4.5
*/
    class CT_MathTransform
    {
        friend class CT_MathTransformFactory;

    private:
        struct _CT_MathTransform;
        _CT_MathTransform *_data;

    protected:
        CT_MathTransform(void);
        CT_MathTransform(CS_CoordinateSystem *sourceCS, CS_CoordinateSystem *targetCS);

    public:
        virtual ~CT_MathTransform(void);

        // TODO: std::string getWKT(void);
        // TODO: std::string getXML(void);
        // TODO: int getDimSource(void);
        // TODO: int getDimTarget(void);
        // TODO: bool isIdentity(void);

        // TODO: int getDomainFlags(const std::vector<double> &ord);
        // TODO: std::vector<double> getCodomainConvexHull(const std::vector<double> &ord);
        virtual PT_CoordinatePoint transform(const PT_CoordinatePoint &cp);
        // TODO: std::vector<double> transformList(const std::vector<double> &ord);
        // TODO: PT_Matrix derivative(const PT_CoordinatePoint &cp);
        // TODO: CT_MathTransformSP inverse(void);

    };

}

