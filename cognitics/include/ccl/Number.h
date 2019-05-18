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
/*! \page number_page Number
\ref ccl_page

*/
#pragma once

#include "cstdint.h"
#include "binary.h"
#include <string>
#include <typeinfo>

namespace ccl
{
    class NumberType;

    //! Number variant class
    class Number
    {
    private:
        NumberType *content;
        NumberType *clone(void) const;

    public:
        ~Number(void);
        Number(void);

        static const int TYPE_EMPTY            = 0;
        static const int TYPE_INT8            = TYPE_EMPTY + 1;
        static const int TYPE_UINT8            = TYPE_EMPTY + 2;
        static const int TYPE_INT16            = TYPE_EMPTY + 3;
        static const int TYPE_UINT16        = TYPE_EMPTY + 4;
        static const int TYPE_INT32            = TYPE_EMPTY + 5;
        static const int TYPE_UINT32        = TYPE_EMPTY + 6;
        static const int TYPE_INT64            = TYPE_EMPTY + 7;
        static const int TYPE_UINT64        = TYPE_EMPTY + 8;
        static const int TYPE_FLOAT            = TYPE_EMPTY + 9;
        static const int TYPE_DOUBLE        = TYPE_EMPTY + 10;
        static const int TYPE_LONGDOUBLE    = TYPE_EMPTY + 11;
        static const int TYPE_MAX            = TYPE_EMPTY + 100;

        static const int ENDIAN_LITTLE        = 0;
        static const int ENDIAN_BIG            = 1;
        static const int ENDIAN_HOST        = 2;

        Number(const Number &value);
        Number(const int8_t &value);
        Number(const uint8_t &value);
        Number(const int16_t &value);
        Number(const uint16_t &value);
        Number(const int32_t &value);
        Number(const uint32_t &value);
        Number(const int64_t &value);
        Number(const uint64_t &value);
        Number(const float &value);
        Number(const double &value);
        Number(const long double &value);

        // assignment operator
        Number &operator=(const Number &value);
        Number &operator=(const int8_t &value);
        Number &operator=(const uint8_t &value);
        Number &operator=(const int16_t &value);
        Number &operator=(const uint16_t &value);
        Number &operator=(const int32_t &value);
        Number &operator=(const uint32_t &value);
        Number &operator=(const int64_t &value);
        Number &operator=(const uint64_t &value);
        Number &operator=(const float &value);
        Number &operator=(const double &value);
        Number &operator=(const long double &value);

        // prefix increment operator
        void operator++(int);

        // postfix increment operator
        void operator++(void);

        // prefix increment operator
        void operator--(int);

        // postfix increment operator
        void operator--(void);

        //! Get the enumerated data type for the stored value.
        uint8_t type(void) const;

        //! Get the RTTI type information for the stored value.
        const std::type_info &typeinfo(void) const;

        //! Get a pointer to the stored value.
        void *ptr(void) const;

        //! Returns true if the variant has no content (is NULL).
        bool empty(void) const;

        //! Get the stored value as an 8-bit signed integer.
        int8_t as_int8(void) const;

        //! Get the stored value as an 8-bit unsigned integer.
        uint8_t as_uint8(void) const;

        //! Get the stored value as a 16-bit signed integer.
        int16_t as_int16(void) const;

        //! Get the stored value as a 16-bit unsigned integer.
        uint16_t as_uint16(void) const;

        //! Get the stored value as a 32-bit signed integer.
        int32_t as_int32(void) const;

        //! Get the stored value as a 32-bit unsigned integer.
        uint32_t as_uint32(void) const;

        //! Get the stored value as a 64-bit signed integer.
        int64_t as_int64(void) const;

        //! Get the stored value as a 64-bit unsigned integer.
        uint64_t as_uint64(void) const;

        //! Get the stored value as a float.
        float as_float(void) const;

        //! Get the stored value as a double.
        double as_double(void) const;

        //! Get the stored value as a long double.
        long double as_long_double(void) const;

        //! Get the stored value as a string.
        std::string as_string(void) const;

        //! Get the stored value as binary using the specified byte encoding.
        binary as_binary(uint8_t byteOrder = ENDIAN_LITTLE) const;

/*! \brief Encode the number for storage.

This encodes the type (uint8_t) and content.
It is intended for use in storing multiple Numbers into a single binary value.

\sa Number::decode()
*/
        binary encode(uint8_t byteOrder = ENDIAN_LITTLE) const;

/*! \brief Decode the next Number in a binary value.

This generates a Number based on a type and content.
It is intended for decoding binary values created with encode().

The iterator parameter is updated to reflect the next binary position.
If parsing failed, an empty Number is returned and the iterator is reset.

\sa Number::encode()
\todo This is not yet implemented.
*/
        static Number decode(binary::iterator &it, uint8_t byteOrder = ENDIAN_LITTLE);

    };

}
