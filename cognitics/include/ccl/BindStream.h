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
/*! \page bindstream_page Stream Binding
\ref ccl_page

\section Description

The BindStream class provides a strategy for bidirectional binding of data to a stream.

\section Usage

\code
#include <ccl.h>

ccl::BigEndian<ccl::uint16_t> myShort = 1234;        // 16-bit unsigned int stored as big endian
ccl::LittleEndian<ccl::uint32_t> myLong = 12345678;    // 32-bit unsigned int stored as little endian
ccl::LittleEndian<double> myDouble = 1234.5678;        // double stored as little endian

void bindData(ccl::BindStream bs)
{
    bs.bind(myShort);
    bs.bind(myLong);
    bs.bind(myDouble);
}

// read
std::ifstream inFile;
inFile.open("myFile.in",std::ios_base::binary);
ccl::BindStream inStream(inFile);
bindData(inStream);
inFile.close();

// write
std::ofstream outfile;
outFile.open("myFile.out",std::ios_base::binary);
ccl::BindStream outStream(outFile);
bindData(outStream);
outFile.close();
\endcode

*/
#pragma once
#pragma warning(disable: 4244)

#include <iostream>

#include "binary.h"

namespace ccl
{
    //! Performs bidirectional binding of data to a stream.
    class BindStream
    {
    private:
        std::istream *is;
        std::ostream *os;

    public:
        ~BindStream(void) { }
        BindStream(void) : is(NULL), os(NULL) { }
        BindStream(std::istream &is) : is(&is), os(NULL) { }
        BindStream(std::ostream &os) : is(NULL), os(&os) { }

        void setStream(std::istream &is) { this->is = &is; os = NULL; }
        void setStream(std::ostream &os) { this->os = &os; is = NULL; }

        //! Returns the stream position.
        int32_t pos(void)
        {
            if(is)
                return is->tellg();
            if(os)
                return os->tellp();
            return 0;
        }

        //! Seeks to the specified stream position.
        void seek(int32_t position)
        {
            if(is)
                is->seekg(position);
            if(os)
                os->seekp(position);
        }

        //! Identifies if the stream is an output stream.
        bool writing(void)
        {
            return (os != NULL);
        }

        //! Bind a variable (read or write the specified item).
        template <typename T>
        void bind(T &var)
        {
            if(is)
                *is >> var;
            if(os)
                *os << var;
        }

        //! Bind specialization for strings.
        void bind(std::string &var, int len)
        {
            if(is)
            {
                var.resize(len, 0);
                is->read((char *)var.data(), len);
            }
            if(os)
            {
                var.resize(len, 0);
                os->write(var.data(), len);
            }
        }

        //! Bind specialization for binary.
        void bind(binary &var, int len)
        {
            if(is)
            {
                var.resize(len, 0);
                is->read((char *)var.data(), len);
            }
            if(os)
            {
                var.resize(len, 0);
                os->write((char *)var.data(), len);
            }
        }

        //! Fill a number of stream characters (in or out).
        void fill(int len, char ch = 0)
        {
            if(is)
            {
                std::streampos pos = is->tellg();
                pos += len;
                is->seekg(pos);
            }
            if(os)
            {
                std::string content = "";
                content.resize(len, ch);
                bind(content, len);
            }
        }

    };

}
