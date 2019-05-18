/*************************************************************************
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

#pragma warning ( disable : 4996 )

#include "ccl/md5.h"
#include <cstdio>

namespace ccl
{
	std::string md5(unsigned char *data, unsigned int length)
	{
		MD5_CTX ctx;
		MD5 *m = new MD5();
		m->MD5Init(&ctx);
		m->MD5Update(&ctx, data, length);
		unsigned char rawmd5[16] = "";	
		m->MD5Final(rawmd5, &ctx);
		delete m;

		char hexmd5[33];
		int p = 0;
		for(int i = 0; i < 16; i++)
			sprintf(&hexmd5[i*2], "%02x", rawmd5[i]);
		hexmd5[32] = 0;

		return std::string(hexmd5);
	}

	std::string md5(const std::vector<unsigned char> &data)
	{
		if(data.empty())
			return "";
		return md5((unsigned char *)(&data[0]), data.size());
	}

	std::string md5(const binary &data)
	{
		if(data.empty())
			return "";
		return md5((unsigned char *)(&data[0]), data.size());
	}

	std::string md5(const std::string &str)
	{
		return md5((unsigned char *)str.c_str(), str.size() * sizeof(char));
	}

	std::string md5(const std::wstring &str)
	{
		return md5((unsigned char *)str.c_str(), str.size() * sizeof(wchar_t));
	}
	
}

