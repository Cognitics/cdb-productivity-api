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

#include "cts/CT_MathTransformFactory.h"

namespace cts
{
    CT_MathTransformFactory::~CT_MathTransformFactory(void)
    {
    }

    CT_MathTransformFactory::CT_MathTransformFactory(void)
    {
    }

/*
    CT_MathTransform CT_MathTransformFactory::createAffineTransform(const PT_Matrix &matrix)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::createAffineTransform(): not yet implemented");
    }

    CT_MathTransform CT_MathTransformFactory::createConcatenatedTransform(const CT_MathTransform &transform1, const CT_MathTransform &transform2)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::createConcatenatedTransform(): not yet implemented");
    }

    CT_MathTransform CT_MathTransformFactory::createPassThroughTransform(int firstAffectedOrdinate, const CT_MathTransform &subTransform)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::createPassThroughTransform(): not yet implemented");
    }

    CT_MathTransform CT_MathTransformFactory::createParameterizedTransform(const std::string &classification, const std::vector<CT_Parameter> &parameters)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::createParameterizedTransform(): not yet implemented");
    }

    CT_MathTransform CT_MathTransformFactory::createFromWKT(const std::string &wellKnownText)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::createFromWKT(): not yet implemented");
    }

    CT_MathTransform CT_MathTransformFactory::createFromXML(const std::string &xml)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::createFromXML(): not yet implemented");
    }

    bool CT_MathTransformFactory::isParameterAngular(const std::string &parameterName)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::isParameterAngular(): not yet implemented");
    }

    bool CT_MathTransformFactory::isParameterLinear(const std::string &parameterName)
    {
        // TODO
        throw std::runtime_error("CT_MathTransformFactory::isParameterLinear(): not yet implemented");
    }
*/

    CT_MathTransform *CT_MathTransformFactory::createFromOGR(CS_CoordinateSystem *sourceCS, CS_CoordinateSystem *targetCS)
    {
        return (sourceCS && targetCS) ? new CT_MathTransform(sourceCS, targetCS) : NULL;
    }

}