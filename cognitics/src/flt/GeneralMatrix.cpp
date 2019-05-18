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

#include "flt/GeneralMatrix.h"

namespace flt
{
    GeneralMatrix::~GeneralMatrix(void)
    {
    }

    GeneralMatrix::GeneralMatrix(void)
    {
        matrix.resize(16, 0);
        matrix[0] = 1.0f;
        matrix[5] = 1.0f;
        matrix[10] = 1.0f;
        matrix[15] = 1.0f;
    }

    int GeneralMatrix::getRecordType(void)
    {
        return FLT_GENERALMATRIX;
    }

    std::string GeneralMatrix::getRecordName(void)
    {
        return "GeneralMatrix";
    }

    void GeneralMatrix::bind(ccl::BindStream &bs, int length, int revision)
    {
        if(matrix.size() < 16)
            matrix.resize(16);
        for(int i = 0; i < 16; ++i)
            bs.bind(matrix[i]);
    }

}
