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

#include "flt/MultiTexture.h"

namespace flt
{
    MultiTexture::~MultiTexture(void)
    {
    }

    MultiTexture::MultiTexture(void)
    {
    }

    int MultiTexture::getRecordType(void)
    {
        return FLT_MULTITEXTURE;
    }

    std::string MultiTexture::getRecordName(void)
    {
        return "MultiTexture";
    }

    void MultiTexture::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(attributeMask);
        if(attributeMask[0])
        {
            bs.bind(index1);
            bs.bind(effect1);
            bs.bind(mapping1);
            bs.bind(data1);
        }
        if(attributeMask[1])
        {
            bs.bind(index2);
            bs.bind(effect2);
            bs.bind(mapping2);
            bs.bind(data2);
        }
        if(attributeMask[2])
        {
            bs.bind(index3);
            bs.bind(effect3);
            bs.bind(mapping3);
            bs.bind(data3);
        }
        if(attributeMask[3])
        {
            bs.bind(index4);
            bs.bind(effect4);
            bs.bind(mapping4);
            bs.bind(data4);
        }
        if(attributeMask[4])
        {
            bs.bind(index5);
            bs.bind(effect5);
            bs.bind(mapping5);
            bs.bind(data5);
        }
        if(attributeMask[5])
        {
            bs.bind(index6);
            bs.bind(effect6);
            bs.bind(mapping6);
            bs.bind(data6);
        }
        if(attributeMask[6])
        {
            bs.bind(index7);
            bs.bind(effect7);
            bs.bind(mapping7);
            bs.bind(data7);
        }
    }


}
