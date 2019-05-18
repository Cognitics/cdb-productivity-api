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
/*! \page binary_page Binary Type Definitions
\ref ccl_page

\section Description

This component provides definitions for binary operations.

These definitions include:
\li <tt>binary</tt>
\li <tt>binarystream</tt>
\li <tt>ibinarystream</tt>
\li <tt>obinarystream</tt>
\li <tt>binarybuf</tt>

\section Notes

These binary definitions are simply mappings to the <tt>u8string</tt> definitions from \ref utf_page.
The main <tt>binary</tt> typedef is essentially a vector of <tt>int8_t</tt>.

*/
#pragma once

#include <string>

namespace ccl
{
    typedef std::basic_string<unsigned char> binary;
    typedef std::basic_stringstream<unsigned char> binarystringstream;
    typedef std::basic_istringstream<unsigned char> binaryistringstream;
    typedef std::basic_ostringstream<unsigned char> binaryostringstream;
    typedef std::basic_stringbuf<unsigned char> binarystringbuf;
    typedef std::basic_istream<unsigned char> binaryistream;
    typedef std::basic_ostream<unsigned char> binaryostream;

    //! Translate a string into binary
    binary bin(const std::string &value);

}


