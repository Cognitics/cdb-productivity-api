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

#include "flt/UVList.h"

namespace flt
{
    UVList::~UVList(void)
    {
    }

    UVList::UVList(void)
    {
    }

    int UVList::getRecordType(void)
    {
        return FLT_UVLIST;
    }

    std::string UVList::getRecordName(void)
    {
        return "UVList";
    }

    void UVList::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(attributeMask);
        int entrySize = 0;
        for(int i = 0; i < 8; ++i)
        {
            if(attributeMask[i])
                entrySize += 8;
        }
        ccl::int32_t entryCount =  (length - 4) / entrySize;
        entries.resize(entryCount);
        for(int i = 0; i < entryCount; ++i)
        {
            entries[i].resize(8);
            for(int j = 0; j < 8; ++j)
            {
                if(attributeMask[j])
                {
                    bs.bind(entries[i][j].u);
                    bs.bind(entries[i][j].v);
                }
            }
        }
    }

}
