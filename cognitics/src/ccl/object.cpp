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

#include "ccl/object.h"

namespace ccl
{
    object::~object(void)
    {
    }

    object::object(void)
    {
    }

    bool object::isKindOf(const std::string &name) const
    {
        if(name == getClassName())
            return true;
        return false;
        //registry::iterator it = registry::instance()->find(getSuperClassName());
        //return (it == registry::instance()->end()) ? false : it->second->isKindOf(name);
    }

    bool object::isInstanceOf(const std::string &name) const
    {
        return (name == getClassName());
    }

    std::string object::getClassName(void) const
    {
        return "ccl::object";
    }

    std::string object::getClassDescription(void) const
    {
        return "CCL Base Object";
    }

    std::string object::getSuperClassName(void) const
    {
        return std::string();
    }

    bool object::hasFeature(const std::string &name) const
    {
        return false;
    }

    object *object::getFeature(const std::string &name) const
    {
        return NULL;
    }

    object *object::clone(void) const
    {
        return new object(*this);
    }


}