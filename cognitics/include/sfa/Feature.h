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
/*! \brief Provides sfa::Feature.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
*/
#pragma once

#include <ccl/AttributeContainer.h>
#include "Geometry.h"

namespace sfa
{
/*! \brief Feature class

The Feature class is an extension to SFA to provide a generic object representing a geometry with attributes and a coordinate system.
*/
    class Feature : public GeometryBase
    {
    public:
        ccl::AttributeContainer attributes;
        Geometry *geometry;

        virtual ~Feature(void);
        Feature(void);
        Feature(const Feature &copy);
        Feature & operator=(const Feature& in);

        virtual std::string getGeometryType(void) const;
        virtual WKBGeometryType getWKBGeometryType(bool withZ = false, bool withM = false) const;
        virtual bool isParentOf(const GeometryBase *child) const;
        virtual GeometryBase *getParentOf(const GeometryBase *child) const;
        virtual int getNumChildren(void) const;

        void setAttribute(const std::string &key, ccl::Variant var);
        bool getAttributeByName(const std::string &key, ccl::Variant &var, bool ignoreCase = false) const;
        bool hasAttribute(const std::string &key, bool ignoreCase = false) const;

    };

    typedef std::shared_ptr<Feature> FeatureSP;
    typedef std::vector<Feature *> FeatureList;

}
