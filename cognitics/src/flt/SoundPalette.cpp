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

#include "flt/SoundPalette.h"

namespace flt
{
    SoundPalette::~SoundPalette(void)
    {
    }

    SoundPalette::SoundPalette(void)
    {
    }

    int SoundPalette::getRecordType(void)
    {
        return FLT_SOUNDPALETTE;
    }

    std::string SoundPalette::getRecordName(void)
    {
        return "SoundPalette";
    }

    void SoundPalette::bind(ccl::BindStream &bs, int length, int revision)
    {
        KeyTable::bind(bs, length, revision);
        if(subtype != 2)
            return;

        // TODO
    }


/*
    void SoundPalette::unpack(int32_t revision)
    {
        KeyTable::unpack(revision);
        soundFiles.clear();
        for(std::map<int32_t, KeyTableData>::iterator it = keyData.begin(); it != keyData.end(); ++it)
            soundFiles[it->first] = it->second.data;
        keyData.clear();
    }

    void SoundPalette::pack(int32_t revision)
    {
        keyData.clear();
        for(std::map<int32_t, std::string>::iterator it = soundFiles.begin(); it != soundFiles.end(); ++it)
        {
            keyData[it->first].type = 0;
            keyData[it->first].offset = 0;
            keyData[it->first].data = it->second;
        }
        KeyTable::pack(revision);
    }
*/

}
