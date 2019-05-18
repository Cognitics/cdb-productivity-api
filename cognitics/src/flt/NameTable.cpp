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

#include "flt/NameTable.h"

namespace flt
{
    NameTable::~NameTable(void)
    {
    }

    NameTable::NameTable(void)
    {
    }

    int NameTable::getRecordType(void)
    {
        return FLT_NAMETABLE;
    }

    std::string NameTable::getRecordName(void)
    {
        return "NameTable";
    }

    void NameTable::bind(ccl::BindStream &bs, int length, int revision)
    {
        ccl::BigEndian<ccl::int32_t> count = ccl::int32_t(names.size());
        /*   4 */ bs.bind(count);

        /*   8 */ bs.bind(nextIndex);

        for(ccl::int32_t i = 0, n = count; i < n; ++i)
        {
            if(names[i].name.size() > 79)
                names[i].name.resize(79);
            ccl::BigEndian<ccl::int32_t> len = ccl::int32_t(names[i].name.size()) + 1;
            bs.bind(len);
            bs.bind(names[i].index);
            bs.bind(names[i].name, len - 1);
            bs.fill(1);    // null termination
        }
    }


}
