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
/*! \page cstdint_page C99 Standard Integers
\ref ccl_page

\section Description

This component provides normalized integer types based on the C99 (ISO/IEC 9899:1999) specification.

Cognitics libraries will use these for most, if not all, integer definitions.

This component includes the following basic types:
\li <tt>ccl::int8_t</tt> - 8-bit signed integer
\li <tt>ccl::uint8_t</tt> - 8-bit unsigned integer
\li <tt>ccl::int16_t</tt> - 16-bit signed integer
\li <tt>ccl::uint16_t</tt> - 16-bit unsigned integer
\li <tt>ccl::int32_t</tt> - 32-bit signed integer
\li <tt>ccl::uint32_t</tt> - 32-bit unsigned integer
\li <tt>ccl::int64_t</tt> - 64-bit signed integer
\li <tt>ccl::uint64_t</tt> - 64-bit unsigned integer

\section Notes

This component is currently just a mapping to the definitions from boost/cstdint.hpp.

*/
#pragma once
#include <cstdint>

namespace ccl
{
    typedef int8_t int8_t;
    typedef int_least8_t int_least8_t;
    typedef int_fast8_t int_fast8_t;

    typedef uint8_t uint8_t;
    typedef uint_least8_t uint_least8_t;
    typedef uint_fast8_t uint_fast8_t;

    typedef int16_t int16_t;
    typedef int_least16_t int_least16_t;
    typedef int_fast16_t int_fast16_t;

    typedef uint16_t uint16_t;
    typedef uint_least16_t uint_least16_t;
    typedef uint_fast16_t uint_fast16_t;

    typedef int32_t int32_t;
    typedef int_least32_t int_least32_t;
    typedef int_fast32_t int_fast32_t;

    typedef uint32_t uint32_t;
    typedef uint_least32_t uint_least32_t;
    typedef uint_fast32_t uint_fast32_t;

    typedef int64_t int64_t;
    typedef int_least64_t int_least64_t;
    typedef int_fast64_t int_fast64_t;

    typedef uint64_t uint64_t;
    typedef uint_least64_t uint_least64_t;
    typedef uint_fast64_t uint_fast64_t;

    typedef intmax_t intmax_t;
    typedef uintmax_t uintmax_t;

}
