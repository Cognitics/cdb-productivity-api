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

#include "flt/KeyTable.h"

namespace flt
{
    KeyTable::~KeyTable(void)
    {
    }

    KeyTable::KeyTable(void)
    {
    }

    void KeyTable::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(subtype);
        if((subtype == 1) || (subtype == 3))
        {
            /*   8 */ bs.bind(header.maxNumber);
            /*  12 */ bs.bind(header.actualNumber);
            /*  16 */ bs.bind(header.totalLength);
            /*  20 */ bs.bind(header.RESERVED20);
            /*  24 */ bs.bind(header.RESERVED24);
            /*  28 */ bs.bind(header.RESERVED28);
            for(int i = 0; i < header.actualNumber; ++i)
            {
                KeyTableKey key;
                bs.bind(key.value);
                bs.bind(key.type);
                bs.bind(key.offset);
                keys.push_back(key);
            }
        }

        // TODO: get rid of this
        bs.bind(data, length - 4);
    }

}
