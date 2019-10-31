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
#pragma once

#include <string>
#include <vector>
#include <boost/shared_ptr.hpp>
#include <ccl/mutex.h>
#include <sfa/Point.h>

namespace scenegraph
{
    class MappedTexture
    {
        unsigned short textureNameIdx;
        //static std::vector<std::string> s_names;
        static std::map<std::string, int> s_names;
        static std::map<int, std::string> s_textureIDs;
        static ccl::mutex s_namesProt;
    public:
        MappedTexture();
        //std::string textureName;        
        std::vector<sfa::Point> uvs;

        void RemapTextureCoords(unsigned int a_idx, unsigned int b_idx, unsigned int c_idx);    
        void SetTextureName(const std::string &name);
        std::string GetTextureName() const;
    
    };

    typedef std::vector<MappedTexture> MappedTextureList;



}