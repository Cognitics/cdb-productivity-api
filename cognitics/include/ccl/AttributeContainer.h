/****************************************************************************
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
/*! \file ccl/AttributeContainer.h
\headerfile ccl/AttributeContainer.h
\brief Container for attributes stored by string key
\author Kevin Bentley kbentley@cognitics.net
*/

#pragma once
#include <float.h>
#include "Variant.h"
#include <ccl/BindStream.h>
#include <ccl/LittleEndian.h>
#include <ccl/binary.h>

namespace ccl
{
    class AttributeContainer
    {
        ccl::VariantMap attributes;
        std::map<std::string,std::string> lowCaseAttributeMap;        
        
    public:
        void clear();
        void updateLowerCaseMap();
        //! Warning, if you modify the variantMap's contents, make sure you call
        // updateLowerCaseMap() so the cached map of keys to lower case versions 
        // gets updated.
        VariantMap *getVariantMap(void);
        void setAttribute(const std::string &key, ccl::Variant var, bool ignoreCase = true);
        bool getAttribute(const std::string &key, ccl::Variant &var, bool ignoreCase = true) const;
        bool hasAttribute(const std::string &key, bool ignoreCase = true) const;
        std::vector<std::string> getKeys() const;

        bool getAttribute(const std::string &key, int &val, bool ignoreCase = true) const;
        bool getAttribute(const std::string &key, bool &val, bool ignoreCase = true) const;
        bool getAttribute(const std::string &key, double &val, bool ignoreCase = true) const;
        bool getAttribute(const std::string &key, std::string &val, bool ignoreCase = true) const;
        
        std::string getAttributeAsString(const std::string &key,bool ignoreCase = true, std::string _default="") const;
        bool getAttributeAsBool(const std::string &key,bool ignoreCase = true, bool _default=false) const;
        int getAttributeAsInt(const std::string &key,bool ignoreCase = true, int _default=-1) const;
        double getAttributeAsDouble(const std::string &key,bool ignoreCase = true, double _default=-DBL_MAX) const;
        ccl::Variant getAttributeAsVariant(const std::string &key,bool ignoreCase = true) const;

        void setAttribute(const std::string &key, int val, bool ignoreCase = true);
        void setAttribute(const std::string &key, bool val, bool ignoreCase = true);
        void setAttribute(const std::string &key, double val, bool ignoreCase = true);
        void setAttribute(const std::string &key, std::string val, bool ignoreCase = true);

        void removeAttribute(const std::string &key,bool ignoreCase=true);
        int getAttributeType(const std::string &key,bool ignoreCase=true);

        void insertAttributes(const ccl::VariantMap &in);
        void insertAttributes(const AttributeContainer &in);

        void bindData(ccl::BindStream bs);
        ccl::binary toBinary();
        static AttributeContainer fromBinary(ccl::binary const &input);

        std::string toString(void);
    };



}
