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

#include "cts/CT_CoordinateTransformation.h"
#include "cts/CT_MathTransformFactory.h"

namespace cts
{
    CT_CoordinateTransformation::~CT_CoordinateTransformation(void)
    {
    }

    CT_CoordinateTransformation::CT_CoordinateTransformation(CS_CoordinateSystem *sourceCS, CS_CoordinateSystem *targetCS) : source(sourceCS), target(targetCS)
    {
    }

/*
    std::string CT_CoordinateTransformation::getName(void)
    {
        // TODO
        throw std::runtime_error("CT_CoordinateTransformation::getName(): not yet implemented");
    }

    std::string CT_CoordinateTransformation::getAuthority(void)
    {
        // TODO
        throw std::runtime_error("CT_CoordinateTransformation::getAuthority(): not yet implemented");
    }

    std::string CT_CoordinateTransformation::getAuthorityCode(void)
    {
        // TODO
        throw std::runtime_error("CT_CoordinateTransformation::getAuthorityCode(): not yet implemented");
    }

    std::string CT_CoordinateTransformation::getRemarks(void)
    {
        // TODO
        throw std::runtime_error("CT_CoordinateTransformation::getRemarks(): not yet implemented");
    }

    std::string CT_CoordinateTransformation::getAreaOfUse(void)
    {
        // TODO
        throw std::runtime_error("CT_CoordinateTransformation::getAreaOfUse(): not yet implemented");
    }

    CT_TransformType CT_CoordinateTransformation::getTransformType(void)
    {
        // TODO
        throw std::runtime_error("CT_CoordinateTransformation::getTransformType(): not yet implemented");
    }
*/

    CS_CoordinateSystem *CT_CoordinateTransformation::getSource(void)
    {
        return source;
    }

    CS_CoordinateSystem *CT_CoordinateTransformation::getTarget(void)
    {
        return target;
    }

    CT_MathTransform *CT_CoordinateTransformation::getMathTransform(void)
    {
        CT_MathTransformFactory mtFactory;
        return mtFactory.createFromOGR(source, target);
    }

}