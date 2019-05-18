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
/*! \file ccl/md5.h
\headerfile ccl/md5.h
\brief MD5 Message Digest Library
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009

\page md5_page MD5 Message Digest Algorithm
\ref ccl_page

\section Description

The MD5 library wraps the RSA MD5 Message-Digest Algorithm C++ implementation.

\section Usage

The library consists of four functions which all accomplish the same task on variations of input data:
\code
std::string MD5a = ccl::md5("C String", 8);        // generate the MD5 of a C string
std::string MD5b = ccl::md5("std:string");        // generate the MD5 of a std::string
std::string MD5c = ccl::md5(L"std:wstring");    // generate the MD5 of a std::wstring

std::vector<unsigned char> data;
std::string MD5d = ccl::md5(data);                // generate the MD5 of unsigned char vector data

ccl::binary bin;
std::string MD5e = ccl::md5(bin);                // generate the MD5 of binary data
\endcode

*/
#pragma once

#define COGNITICS_MD5_VERSION 1.0

#include "binary.h"
#include <vector>

namespace ccl
{
/*! \brief Calculate the MD5 (Message Digest 5) hash using a C string.
\param data data to hash
\param length length of data
\return MD5 hash string
*/
    std::string md5(unsigned char *data, unsigned int length);

/*! \brief Calculate the MD5 (Message Digest 5) hash using a binary unsigned char vector.
\param data data to hash
\return MD5 hash string
*/
    std::string md5(const std::vector<unsigned char> &data);

/*! \brief Calculate the MD5 (Message Digest 5) hash using a ccl::binary.
\param data data to hash
\return MD5 hash string
*/
    std::string md5(const binary &data);

/*! \brief Calculate the MD5 (Message Digest 5) hash using a std::string.
\param str std::string to hash
\return MD5 hash string
*/
    std::string md5(const std::string &str);

/*! \brief Calculate the MD5 (Message Digest 5) hash using a std::wstring.
\param str std::wstring to hash
\return MD5 hash string
\bug This is not cross-platform.
*/
    std::string md5(const std::wstring &str);

}

/*
 *    This is the C++ implementation of the MD5 Message-Digest
 *    Algorithm desrcipted in RFC 1321.
 *    I translated the C code from this RFC to C++.
 *    There is no warranty.
 *
 *    Feb. 12. 2005
 *    Benjamin Gr�delbach
 */

/*
 * Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
 * rights reserved.
 * 
 * License to copy and use this software is granted provided that it
 * is identified as the "RSA Data Security, Inc. MD5 Message-Digest
 * Algorithm" in all material mentioning or referencing this software
 * or this function.
 * 
 * License is also granted to make and use derivative works provided
 * that such works are identified as "derived from the RSA Data
 * Security, Inc. MD5 Message-Digest Algorithm" in all material
 * mentioning or referencing the derived work.
 * 
 * RSA Data Security, Inc. makes no representations concerning either
 * the merchantability of this software or the suitability of this
 * software for any particular purpose. It is provided "as is"
 * without express or implied warranty of any kind.
 * 
 * These notices must be retained in any copies of any part of this
 * documentation and/or software.
 */

//---------------------------------------------------------------------- 
//typedefs
typedef unsigned char *POINTER;

#include "cstdint.h"

/*
 * MD5 context.
 */
typedef struct 
{
    ccl::uint32_t state[4];             /* state (ABCD) */
    ccl::uint32_t count[2];           /* number of bits, modulo 2^64 (lsb first) */
    unsigned char buffer[64];          /* input buffer */
} MD5_CTX;

/*
 * MD5 class
 */
class MD5
{

    private:

        void MD5Transform (ccl::uint32_t state[4], unsigned char block[64]);
        void Encode (unsigned char*, ccl::uint32_t*, unsigned int);
        void Decode (ccl::uint32_t*, unsigned char*, unsigned int);
        void MD5_memcpy (POINTER, POINTER, unsigned int);
        void MD5_memset (POINTER, int, unsigned int);

    public:
    
        void MD5Init (MD5_CTX*);
        void MD5Update (MD5_CTX*, unsigned char*, unsigned int);
        void MD5Final (unsigned char [16], MD5_CTX*);

    MD5(){};
};
