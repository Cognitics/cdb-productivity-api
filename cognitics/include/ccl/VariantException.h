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
/*! \file ccl/VariantException.h
\headerfile ccl/VariantException.h
\brief Provides ccl::VariantException.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 October 2010
*/
#pragma once

#include <exception>
#include <stdexcept>
#include <string>

namespace ccl
{
/*! Variant Exception

Source type is stored in the source property.
Destination type is stored in the destination property.

The what() call will always return "Invalid Conversion".
*/
    class VariantException : public std::exception
    {
    public:
        std::string source;
        std::string destination;

        VariantException(const std::string &source, const std::string destination) : source(source), destination(destination) { }
        ~VariantException(void) throw() { }

        virtual const char *what() const throw()
        {
            return "Invalid Conversion";
        }
    };

}
