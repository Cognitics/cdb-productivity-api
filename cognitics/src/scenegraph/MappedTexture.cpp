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

#include "scenegraph/MappedTexture.h"
#include <ccl/ObjLog.h>
//#pragma optimize( "", off )
namespace scenegraph
{
    //std::vector<std::string> MappedTexture::s_names;
    std::map<std::string,int> MappedTexture::s_names;
    std::map<int, std::string> MappedTexture::s_textureIDs;
    ccl::mutex MappedTexture::s_namesProt;

    MappedTexture::MappedTexture() 
    {
        textureNameIdx = 65535;
    }

    void MappedTexture::RemapTextureCoords(unsigned int a_idx, unsigned int b_idx, unsigned int c_idx)
    {
        std::vector<sfa::Point> tmp;
        tmp.push_back(uvs.at(a_idx));
        tmp.push_back(uvs.at(b_idx));
        tmp.push_back(uvs.at(c_idx));
        uvs = tmp;
    }

    void MappedTexture::SetTextureName(const std::string &name)
    {
        s_namesProt.lock();
        if(s_names.find(name)==s_names.end())
        {
            int id = s_names.size();
            s_names[name] = id;
            s_textureIDs[id] = name;
        }
        textureNameIdx = s_names[name];
        s_namesProt.unlock();
    }

    std::string MappedTexture::GetTextureName() const
    {
        s_namesProt.lock();
        if(s_textureIDs.find(textureNameIdx)==s_textureIDs.end())
        {
            ccl::ObjLog log;
            log.init("MappedTexture::GetTextureName()");
            log << ccl::LERR << "Unable to find texture for id " << textureNameIdx << log.endl;
            s_namesProt.unlock();
            return "InvalidTextureID";
        }
        std::string ret = s_textureIDs[textureNameIdx];
        s_namesProt.unlock();
        return ret;


    }

}