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

#include "sfa/Feature.h"

namespace sfa
{
    Feature::~Feature(void)
    {
        if (geometry) delete geometry;
    }

    Feature::Feature(void) : geometry(NULL)
    {
    }

    Feature::Feature(const Feature &copy)
    {
        attributes = copy.attributes;
        geometry = copy.geometry ? copy.geometry->copy() : NULL;
    }

    Feature & Feature::operator=(const Feature& in)
    {
        attributes = in.attributes;
        geometry = in.geometry ? in.geometry->copy() : NULL;
        return *this;
    }

    std::string Feature::getGeometryType(void) const
    {
        return "Feature";
    }

    WKBGeometryType Feature::getWKBGeometryType(bool withZ, bool withM) const
    {
        return wkbUnknownSFA;
    }

    bool Feature::isParentOf(const GeometryBase *child) const
    {
        return (child && geometry && (geometry == child));
    }

    GeometryBase *Feature::getParentOf(const GeometryBase *child) const
    {
        return NULL;
    }

    int Feature::getNumChildren(void) const
    {
        return (geometry == NULL) ? 0 : 1;
    }

    void Feature::setAttribute(const std::string &key, ccl::Variant var)
    {
        attributes.setAttribute(key,var);
    }

    bool Feature::getAttributeByName(const std::string &key, ccl::Variant &var, bool ignoreCase) const
    {
        return attributes.getAttribute(key,var,ignoreCase);        
    }
    bool Feature::hasAttribute(const std::string &key, bool ignoreCase) const
    {
        return attributes.hasAttribute(key,ignoreCase);        
    }

}
