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
/*! \page bitfield_page BitField Template Class Hierarchy
\ref ccl_page

\section Description

The BitField template class hierarchy provides an abstraction of bitwise data storage for different bit orders.

It includes the following classes:
\li ccl::BitField \copybrief ccl::BitField
\li ccl::MSBitField \copybrief ccl::MSBitField
\li ccl::LSBitField \copybrief ccl::LSBitField

\section Usage

The MSBitField and LSBitField classes provide bitwise data storage for most-significant bit first and least-significant bit first bit orders respectively.
The stream operators (<< and >>) provide input and output in the appropriate bit order.

\code
#include <ccl.h>

ccl::LSBitField<16> lsBitField16;
ccl::MSBitField<32> msBitField32;

// set some bits in the 16-bit LSBitField
lsBitField16[1] = true;
lsBitField16[3] = true;
lsBitField16[8] = true;

// set some bits in the 32-bit MSBitField
msBitField32[2] = true;
msBitField32[11] = true;
msBitField32[28] = true;

std::cout << lsBitField16 << std::endl;        // output the data in least-significant bit first
std::cout << msBitField32 << std::endl;        // output the data in most-significant bit first
\endcode

\section Notes

The stream implementation is compatible with bind operations using ccl::EndianStream from the Endian library.

*/
#pragma once

#include <iostream>
#include <bitset>

namespace ccl
{
    //! \brief Template base class for bit order transformations.
    template <size_t B>
    class BitField
    {
    protected:
        std::bitset<B> bits;

    public:
        ~BitField(void) { }
        BitField(void) { }

        //! Reference class for value accessors.
        class ref
        {
            friend class BitField<B>;
        private:
            BitField<B> *bf;
            size_t index;
            ref(BitField<B> &bf, size_t index) : bf(&bf), index(index) { }
        public:
            operator bool(void) { return bf->get(index); }
            bool operator~() { return !bf->get(index); }
            ref &operator=(bool value) { bf->set(index, value); return *this; }
            ref &operator=(const ref &value) { bf->set(index, bool(value)); return *this; }
        };

        //! Set the value of a specific bit in the bitfield.
        void set(size_t index, bool value)
        {
            bits.set(index, value);
        }

        //! Get the value of a specific bit in the bitfield.
        bool get(size_t index)
        {
            return bits.test(index);
        }

        //! Index operator for getting and setting bit values.
        ref operator[](unsigned short index)
        {
            return (ref(*this, index));
        }

    };

}

