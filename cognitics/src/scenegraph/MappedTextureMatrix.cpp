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

#include "scenegraph/MappedTextureMatrix.h"

#include <vector>

namespace scenegraph
{

    bool CreateMappedTextureMatrix(const Face& F, const MappedTexture& T, sfa::Matrix& M)
    {
        std::vector<sfa::Point> S1, S2;

        if(F.verts.size()<3 || T.uvs.size()<3)
            return false;

        S1.push_back(F.verts[0]);
        S1.push_back(F.verts[1]);
        S1.push_back(F.verts[2]);
        S1.push_back(F.verts[0] + F.computeNormal());

        S2.push_back(T.uvs[0]);
        S2.push_back(T.uvs[1]);
        S2.push_back(T.uvs[2]);
        S2.push_back(T.uvs[0] + sfa::Point(0,0,1));

        return M.CreateTransform(S1, S2);
    }

}