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

#include "flt/ShaderPalette.h"

namespace flt
{
    ShaderPalette::~ShaderPalette(void)
    {
    }

    ShaderPalette::ShaderPalette(void)
    {
    }

    int ShaderPalette::getRecordType(void)
    {
        return FLT_SHADERPALETTE;
    }

    std::string ShaderPalette::getRecordName(void)
    {
        return "ShaderPalette";
    }

    void ShaderPalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        /*   4 */ bs.bind(index);
        /*   8 */ bs.bind(type);
        /*  12 */ bs.bind(name, 1024);

        if(type == 0)
        {
            /* 1036 */ bs.bind(vertexProgramFileName, 1024);
            /* 2060 */ bs.bind(fragmentProgramFileName, 1024);
            /* 3084 */ bs.bind(vertexProgramProfile);
            /* 3088 */ bs.bind(fragmentProgramProfile);
            /* 3092 */ bs.bind(vertexProgramEntryPoint, 256);
            /* 3348 */ bs.bind(fragmentProgramEntryPoint, 256);
        }

        if(type == 2)
        {
            ccl::BigEndian<ccl::int32_t> vertexProgramCount = ccl::int32_t(vertexProgramFiles.size());
            /* 1036 */ bs.bind(vertexProgramCount);

            ccl::BigEndian<ccl::int32_t> fragmentProgramCount = ccl::int32_t(vertexProgramFiles.size());
            /* 1040 */ bs.bind(fragmentProgramCount);

            for(int i = 0, n = vertexProgramCount; i < n; ++i)
                bs.bind(vertexProgramFiles[i]);

            for(int i = 0, n = fragmentProgramCount; i < n; ++i)
                bs.bind(fragmentProgramFiles[i]);
        }
    }

}
