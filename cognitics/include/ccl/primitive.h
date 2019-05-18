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
/*! \file ccl/primitive.h
\headerfile ccl/primitive.h
\brief Provides ccl::primitive
\author Aaron Brinton <abrinton@cognitics.net>
\date 28 March 2011
*/
#pragma once

#include "object.h"
#include "cstdint.h"
#include "uuid.h"
#include <string>
#include <exception>

namespace ccl
{
    class datetime;
    class date;
    class time;
    //class string;

    class primitive : public object
    {
    public:
        virtual ~primitive(void);
        primitive(void);

        virtual std::string getClassName(void) const;
        virtual std::string getClassDescription(void) const;
        virtual std::string getSuperClassName(void) const;

        virtual object *clone(void) const;

        virtual int8_t as_int8_t(void) const;
        virtual uint8_t as_uint8_t(void) const;
        virtual int16_t as_int16_t(void) const;
        virtual uint16_t as_uint16_t(void) const;
        virtual int32_t as_int32_t(void) const;
        virtual uint32_t as_uint32_t(void) const;
        virtual int64_t as_int64_t(void) const;
        virtual uint64_t as_uint64_t(void) const;
        virtual float as_float(void) const;
        virtual double as_double(void) const;
        virtual long double as_long_double(void) const;
        virtual uuid as_uuid(void) const;
        virtual datetime as_datetime(void) const;
        virtual date as_date(void) const;
        virtual time as_time(void) const;
        virtual std::string as_std_string(void) const;
        virtual std::wstring as_std_wstring(void) const;
        //virtual string as_string(void) const;
        //virtual binary as_binary(void) const;

    };

    /*
    template <typename T>
    T object_cast(T obj)
    {
        return dynamic_cast<T>(obj);
    }
    */

    class primitive_conversion_exception : public std::exception
    {
    public:
        const object *obj;
        std::string target;

        primitive_conversion_exception(const object * const obj, const std::string &target);
        virtual ~primitive_conversion_exception(void) throw();
        virtual const char *what() const throw();
    };


}