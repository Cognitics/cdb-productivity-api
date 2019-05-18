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
/*! \page endian_page Endian Template Class Hierarchy
\ref ccl_page

\section Description

The Endian template class hierarchy provides an abstraction of byte order dependent data storage while offering host byte ordering for application access.

It includes the following classes:
\li ccl::Endian \copybrief ccl::Endian
\li ccl::BigEndian \copybrief ccl::BigEndian
\li ccl::LittleEndian \copybrief ccl::LittleEndian

\section Usage

The BigEndian and LittleEndian classes identify the respective data storage byte order.
With the exception of stream operators (<< and >>), accessing the objects is done as the normal primitive data types.
Stream operators are used for conversion from host byte order to class byte order.

\code
#include <ccl.h>

ccl::BigEndian<u_int16_t> myShort = 1234;        // initializes a big endian 16-bit unsigned int
ccl::LittleEndian<u_int32_t> myLong = 12345678;    // initializes a little endian 32-bit unsigned int

std::cout << myShort << std::endl;    // provides myShort in big endian order
std::cout << myLong << std::endl;    // provides myLong in little endian order
\endcode

*/
#pragma once

#include "binary.h"
#include <iostream>
#include <vector>

namespace ccl
{
/*! \brief Identify if the machine uses big endian byte order.
\return True if the machine is big endian; false otherwise.
*/
    bool machBigEndian(void);

/*! \brief Identify if the machine uses little endian byte order.
\return True if the machine is little endian; false otherwise.
*/
    bool machLittleEndian(void);

    //! \brief Template base class for byte order transformations.
    template <typename T>
    class Endian
    {
    protected:
        T value;

    public:
        virtual ~Endian(void) { }
        Endian(void) { }
        Endian(const Endian<T> &src) : value(src.value) { }
        Endian(const T &v) : value(v) { }

        operator T(void)
        {
            return value;
        }

        Endian &operator=(const Endian<T> &rhs)
        {
            if(this != &rhs)
                value = rhs.value;
            return *this;
        }

        Endian &operator=(const T &rhs)
        {
            value = rhs;
            return *this;
        }

/*! \brief Get a ccl::binary representation of the value.
\return A ccl::binary representing the value in class order (e.g. big endian for BigEndian).
*/
        virtual binary as_binary(void) = 0;

/*! \brief Retrieve a value from a ccl::binary representation.
\param binary ccl::binary representation.
\return The value from the ccl::binary in class order (e.g. big endian for BigEndian).
*/
        virtual bool from_binary(const binary &binary) = 0;

    };

}