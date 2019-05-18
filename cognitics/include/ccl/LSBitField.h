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
/*! \file ccl/LSBitField.h
\headerfile ccl/LSBitField.h
\brief Provides ccl::LSBitField.
\author Aaron Brinton <abrinton@cognitics.net>
\date 05 February 2010
*/
#pragma once

#include "BitField.h"

namespace ccl
{
    //! Template BitField subclass providing least-significant bit first ordering.
    template <size_t B>
    class LSBitField : public BitField<B>
    {
    public:
        ~LSBitField(void) { }
        LSBitField(void) { }
        LSBitField(const LSBitField<B> &src) : BitField<B>(src) { }

        //! Get a bitfield value from an input stream.
        friend std::istream &operator>>(std::istream &is, LSBitField<B> &obj) 
        {
            size_t bytes = B / 8;
            if(B % 8)
                ++bytes;
            for(size_t i = 0; i < bytes; ++i)
            {
                unsigned char data = is.get();
                for(size_t j = 0; j < 8; ++j)
                {
                    size_t index = (i * 8) + j;
                    if(index < B)
                        obj.bits[index] = (data & (1 << (7 - j))) ? true : false;
                }
            }
            return is;
        }

        //! Put a bitfield value into an output stream.
        friend std::ostream &operator<<(std::ostream &os, LSBitField<B> &obj) 
        {
            size_t bytes = B / 8;
            if(B % 8)
                ++bytes;
            for(size_t i = 0; i < bytes; ++i)
            {
                unsigned char data = 0;
                for(size_t j = 0; j < 8; ++j)
                {
                    size_t index = (i * 8) + j;
                    if(index < B)
                        data |= (obj.bits[index]) ? (1 << (7 - j)) : 0;
                }
                os.put(data);
            }
            return os;
        }

    };

}
