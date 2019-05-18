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

#include "ccl/primitive.h"
#include "ccl/datetime.h"

namespace ccl
{
    primitive::~primitive(void)
    {
    }

    primitive::primitive(void)
    {
    }

    std::string primitive::getClassName(void) const
    {
        return "ccl::primitive";
    }

    std::string primitive::getClassDescription(void) const
    {
        return "CCL Primitive Object";
    }

    std::string primitive::getSuperClassName(void) const
    {
        return "ccl::object";
    }

    object *primitive::clone(void) const
    {
        return new primitive(*this);
    }

    int8_t primitive::as_int8_t(void) const
    {
        throw primitive_conversion_exception(this, "int8_t");
    }

    uint8_t primitive::as_uint8_t(void) const
    {
        throw primitive_conversion_exception(this, "uint8_t");
    }

    int16_t primitive::as_int16_t(void) const
    {
        throw primitive_conversion_exception(this, "int16_t");
    }

    uint16_t primitive::as_uint16_t(void) const
    {
        throw primitive_conversion_exception(this, "uint16_t");
    }

    int32_t primitive::as_int32_t(void) const
    {
        throw primitive_conversion_exception(this, "int32_t");
    }

    uint32_t primitive::as_uint32_t(void) const
    {
        throw primitive_conversion_exception(this, "uint32_t");
    }

    int64_t primitive::as_int64_t(void) const
    {
        throw primitive_conversion_exception(this, "int64_t");
    }

    uint64_t primitive::as_uint64_t(void) const
    {
        throw primitive_conversion_exception(this, "uint64_t");
    }

    float primitive::as_float(void) const
    {
        throw primitive_conversion_exception(this, "float");
    }

    double primitive::as_double(void) const
    {
        throw primitive_conversion_exception(this, "double");
    }

    long double primitive::as_long_double(void) const
    {
        throw primitive_conversion_exception(this, "long double");
    }

    uuid primitive::as_uuid(void) const
    {
        throw primitive_conversion_exception(this, "ccl::uuid");
    }

    datetime primitive::as_datetime(void) const
    {
        throw primitive_conversion_exception(this, "ccl::datetime");
    }

    date primitive::as_date(void) const
    {
        throw primitive_conversion_exception(this, "ccl::date");
    }

    time primitive::as_time(void) const
    {
        throw primitive_conversion_exception(this, "ccl::time");
    }

    std::string primitive::as_std_string(void) const
    {
        throw primitive_conversion_exception(this, "std::string");
    }

    std::wstring primitive::as_std_wstring(void) const
    {
        throw primitive_conversion_exception(this, "std::wstring");
    }

    /*
    string primitive::as_string(void) const
    {
        throw primitive_conversion_exception(this, "ccl::string");
    }
    */

    //binary primitive::as_binary(void) const


    primitive_conversion_exception::primitive_conversion_exception(const object * const obj, const std::string &target) : obj(obj), target(target)
    {
    }

    primitive_conversion_exception::~primitive_conversion_exception(void) throw()
    {
    }

    const char *primitive_conversion_exception::what() const throw()
    {
        return "Invalid Conversion";
    }


}