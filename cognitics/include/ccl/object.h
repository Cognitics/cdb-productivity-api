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
/*! \file ccl/object.h
\headerfile ccl/object.h
\brief Provides ccl::object
\author Aaron Brinton <abrinton@cognitics.net>
\date 28 March 2011
*/
#pragma once

#include <string>

namespace ccl
{
    class datetime;
    class date;
    class time;

    class object
    {
    public:
        virtual ~object(void);
        object(void);

        //! Returns true if object is an instance or subclass instance of the specified class name.
        bool isKindOf(const std::string &name) const;

        //! Returns true if object is an instance of the specified class name.
        bool isInstanceOf(const std::string &name) const;

        //! Get the name of the class.
        virtual std::string getClassName(void) const;

        //! Get the description of the class.
        virtual std::string getClassDescription(void) const;

        //! Get the name of the superclass.
        virtual std::string getSuperClassName(void) const;

        //! Returns true if the object provides the specified feature.
        virtual bool hasFeature(const std::string &name) const;

        //! Get the specified feature, or NULL if the feature is not provided.
        virtual object *getFeature(const std::string &name) const;

        //! Clone the current object.
        virtual object *clone(void) const;

    };

    /*
    template <typename T>
    T object_cast(T obj)
    {
        return dynamic_cast<T>(obj);
    }
    */


}