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

#include <ip/imageinfo.h>
#include <ip/rgb.h>
#include <ip/attr.h>

namespace ip 
{

    bool GetImagePixels(std::string filename, ip::ImageInfo &info, ccl::binary &buffer);
    bool GetImageInfo(std::string filename, ip::ImageInfo &info);
    void FlipVertically(ip::ImageInfo &info, ccl::binary &buffer);
    bool InterleavePixels(ip::ImageInfo &info, ccl::binary &buffer);
    /**
     * Add the alpha buffer (must be depth 1 with matchin height and width)
     * to the image buffer, increasing depth by 1
     **/
    void AddAlphaChannel(ip::ImageInfo &info, ccl::binary &image, ccl::binary &alpha);
}