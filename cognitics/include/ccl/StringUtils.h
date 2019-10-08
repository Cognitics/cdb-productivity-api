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
#pragma once

#include <string.h>
#include <string>
#include <vector>
#include <algorithm>
namespace ccl
{

    inline std::string trim_string(const std::string &orig)
    {
        std::string newstr = "";
        size_t origlen = orig.length();
        for(size_t i=0;i<origlen;i++)
        {
            if((orig[i]==' ') ||
                (orig[i]=='\t')||
                (orig[i]==0x0a)||
                (orig[i]==0x0d))
            {
                // skip it
            }
            else {
                newstr = orig.substr(i,origlen-i);
                break;
            }
        }
        // now do the end
        for(int i=(int)(origlen-1);i>=0;i--)
        {
            if((orig[i]==' ') ||
                (orig[i]=='\t')||
                (orig[i]==0x0a)||
                (orig[i]==0x0d))
            {
                // skip it
            }
            else {
                newstr = orig.substr(0,i+1);
                break;
            }
        }
        return newstr;

    }

    inline std::vector<std::string> splitString(const std::string &str, const std::string &delimiters)
    {
        std::vector<std::string> tokens;
        // Skip delimiters at beginning.
        std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
        // Find first "non-delimiter".
        std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

        while (std::string::npos != pos || std::string::npos != lastPos)
        {
            // Found a token, add it to the vector.
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of(delimiters, pos);
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiters, lastPos);
        }
        return tokens;
    }

    inline bool stringEndsWith(const std::string &str, const std::string &compareStr, bool caseSensitive=true)
    {
        if(compareStr.length()>str.length())
            return false;
        std::string sub = str.substr((str.length()-compareStr.length()),str.length());
        if(!caseSensitive)
        {
            std::string lstr(sub);
            std::string lcomp(compareStr);
            std::transform(lstr.begin(), lstr.end(), lstr.begin(), ::tolower);
            std::transform(lcomp.begin(), lcomp.end(), lcomp.begin(), ::tolower);
            if(lcomp==lstr)
                return true;
            return false;
        }
        if(sub==compareStr)
            return true;
        return false;
    }

    inline bool stringStartsWith(const std::string &str, const std::string &compareStr, bool caseSensitive = true)
    {
        if (compareStr.length() > str.length())
            return false;
        std::string sub = str.substr(0, compareStr.length());
        if (!caseSensitive)
        {
            std::string lstr(sub);
            std::string lcomp(compareStr);
            std::transform(lstr.begin(), lstr.end(), lstr.begin(), ::tolower);
            std::transform(lcomp.begin(), lcomp.end(), lcomp.begin(), ::tolower);
            if (lcomp == lstr)
                return true;
            return false;
        }
        if (sub == compareStr)
            return true;
        return false;
    }

        inline int stringCompareNoCase(const std::string &a, const std::string &b)
        {
#ifdef WIN32
          return _strcmpi(a.c_str(),b.c_str());
#else
          return strcasecmp(a.c_str(),b.c_str());
#endif
          
        }
}
