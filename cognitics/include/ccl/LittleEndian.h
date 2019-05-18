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
/*! \file ccl/LittleEndian.h
\headerfile ccl/LittleEndian.h
\brief Provides ccl::LittleEndian.
\author Aaron Brinton <abrinton@cognitics.net>
\date 05 February 2010
*/
#pragma once

#include "Endian.h"

namespace ccl
{
    //! Template Endian subclass providing transformation between host byte order and little endian byte order.
    template <typename T>
    class LittleEndian : public Endian<T>
    {
    public:
        virtual ~LittleEndian(void) { }
        LittleEndian(void) { }
        LittleEndian(const LittleEndian<T> &src) : Endian<T>(src) { }
        LittleEndian(const T &v) : Endian<T>(v) { }

        //! Get a little endian value from an input stream.
        friend std::istream &operator>>(std::istream &is, LittleEndian<T> &obj) 
        {
            unsigned char *p = (unsigned char *)&obj.value;
            size_t n = sizeof(T);
            if(machLittleEndian())
            {
                for(size_t i = 0; i < n; ++i)
                    p[i] = is.get();
            }
            else
            {
                for(size_t i = 0; i < n; ++i)
                    p[n - i - 1] = is.get();
            }
            return is;
        }

        //! Put a little endian value into an output stream.
        friend std::ostream &operator<<(std::ostream &os, LittleEndian<T> &obj) 
        {
            unsigned char *p = (unsigned char *)&obj.value;
            size_t n = sizeof(T);
            if(machLittleEndian())
            {
                for(size_t i = 0; i < n; ++i)
                    os.put(p[i]);
            }
            else
            {
                for(size_t i = 0; i < n; ++i)
                    os.put(p[n - i - 1]);
            }
            return os;
        }

        virtual binary as_binary(void)
        {
            unsigned char *p = (unsigned char *)&this->value;
            binary result;
            size_t n = sizeof(T);
            if(machLittleEndian())
            {
                for(size_t i = 0; i < n; ++i)
                    result.push_back(p[i]);
            }
            else
            {
                for(size_t i = n; i > 0; --i)
                    result.push_back(p[i - 1]);
            }
            return result;
        }

        virtual bool from_binary(const binary &binary)
        {
            unsigned char *p = (unsigned char *)&this->value;
            size_t n = sizeof(T);
            if(binary.size() < n)
                return false;
            if(machLittleEndian())
            {
                for(size_t i = 0; i < n; ++i)
                    p[i] = binary[i];
            }
            else
            {
                for(size_t i = 0; i < n; ++i)
                    p[n - i - 1] = binary[i];
            }
            return true;
        }

    };

}