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
/*! \file flt/KeyTable.h
\headerfile flt/KeyTable.h
\brief Provides flt::KeyTable
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct KeyTableHeader
    {
        ccl::BigEndian<ccl::int32_t> maxNumber;
        ccl::BigEndian<ccl::int32_t> actualNumber;
        ccl::BigEndian<ccl::int32_t> totalLength;
        ccl::BigEndian<ccl::int32_t> RESERVED20;
        ccl::BigEndian<ccl::int32_t> RESERVED24;
        ccl::BigEndian<ccl::int32_t> RESERVED28;
    };

    struct KeyTableKey
    {
        ccl::BigEndian<ccl::int32_t> value;
        ccl::BigEndian<ccl::int32_t> type;
        ccl::BigEndian<ccl::int32_t> offset;
    };

    class KeyTable : public Record
    {
    public:
        ccl::BigEndian<ccl::int32_t> subtype;
        KeyTableHeader header;
        std::vector<KeyTableKey> keys;
        ccl::binary data;    // TODO: replace this will implementations

        virtual ~KeyTable(void);
        KeyTable(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


