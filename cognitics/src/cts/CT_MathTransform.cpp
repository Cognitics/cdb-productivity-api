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

#include "cts/CT_MathTransform.h"
#include "cts/CS_CoordinateSystem.h"

#include <ogrsf_frmts.h>
#include <ogr_api.h>

namespace cts
{
    struct CT_MathTransform::_CT_MathTransform
    {
        OGRSpatialReference sourceSR;
        OGRSpatialReference targetSR;
        OGRCoordinateTransformation *ct;
        _CT_MathTransform(void) : ct(NULL) { }
    };

    CT_MathTransform::~CT_MathTransform(void)
    {
        delete _data->ct;
        delete _data;
    }

    CT_MathTransform::CT_MathTransform(void) : _data(new _CT_MathTransform)
    {
    }

    CT_MathTransform::CT_MathTransform(CS_CoordinateSystem *sourceCS, CS_CoordinateSystem *targetCS) : _data(new _CT_MathTransform)
    {
        if(_data->sourceSR.SetFromUserInput(sourceCS->getWKT().c_str()) != OGRERR_NONE)
            return;
        if(_data->targetSR.SetFromUserInput(targetCS->getWKT().c_str()) != OGRERR_NONE)
            return;
        _data->ct = OGRCreateCoordinateTransformation(&_data->sourceSR, &_data->targetSR);
    }

/*
    std::string CT_MathTransform::getWKT(void)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::getWKT(): not yet implemented");
    }

    std::string CT_MathTransform::getXML(void)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::getXML(): not yet implemented");
    }

    int CT_MathTransform::getDimSource(void)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::getDimSource(): not yet implemented");
    }

    int CT_MathTransform::getDimTarget(void)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::getDimTarget(): not yet implemented");
    }

    bool CT_MathTransform::isIdentity(void)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::isIdentity(): not yet implemented");
    }

    int CT_MathTransform::getDomainFlags(const std::vector<double> &ord)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::getDomainFlags(): not yet implemented");
    }

    std::vector<double> CT_MathTransform::getCodomainConvexHull(const std::vector<double> &ord)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::getCodomainConvexHull(): not yet implemented");
    }
*/

    PT_CoordinatePoint CT_MathTransform::transform(const PT_CoordinatePoint &cp)
    {
        if(!_data->ct)
            return PT_CoordinatePoint();
        double x = (cp.size() > 0) ? cp[0] : 0;
        double y = (cp.size() > 1) ? cp[1] : 0;
        double z = (cp.size() > 2) ? cp[2] : 0;
        if(!_data->ct->Transform(1, &x, &y, &z))
            return PT_CoordinatePoint();
        PT_CoordinatePoint result;
        if(cp.size() > 0)
            result.push_back(x);
        if(cp.size() > 1)
            result.push_back(y);
        if(cp.size() > 2)
            result.push_back(z);
        return result;
    }

/*
    std::vector<double> CT_MathTransform::transformList(const std::vector<double> &ord)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::transformList(): not yet implemented");
    }

    PT_Matrix CT_MathTransform::derivative(const PT_CoordinatePoint &cp)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::derivative(): not yet implemented");
    }

    CT_MathTransform CT_MathTransform::inverse(void)
    {
        // TODO
        throw std::runtime_error("CT_MathTransform::inverse(): not yet implemented");
    }
*/

}