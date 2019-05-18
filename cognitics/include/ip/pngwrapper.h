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

#include <ccl/ccl.h>
#include <ip/imageinfo.h>
#include <ip/ip.h>

namespace ip {

    bool GetPNGImageInfo(std::string filename, ip::ImageInfo &info);
    bool GetPNGImagePixels(std::string filename, ip::ImageInfo &info, ccl::binary &buffer);

    /**
     * @fn    bool WritePNG24(const std::string &filename, const ip::ImageInfo &info,
     *  const ccl::binary &buffer);
     *
     * @brief    Writes a PNG file in RGB format. Requires an input image with interleaved pixels and a depth of 3
     *
     * @author    Kbentley
     * @date    4/17/2013
     *
     * @param    filename    Filename of the file.
     * @param    info        The information.
     * @param    buffer      The input buffer. The size must be info.width * info.height * info.depth, and the pixels must be interleaved.
     *
     * @return    true if it succeeds, false if it fails.
     */
    bool WritePNG24(const std::string &filename, const ip::ImageInfo &info, const ccl::binary &buffer);

    /**
     * @fn    bool WritePNGRGBA(const std::string &filename, const ip::ImageInfo &info,
     *  const ccl::binary &buffer);
     *
     * @brief    Writes a PNG file in RGBA format. Requires an input image with interleaved pixels and a depth of 4
     *
     * @author    Kbentley
     * @date    4/17/2013
     *
     * @param    filename    Filename of the file.
     * @param    info        The image metadata
     * @param    buffer      The input buffer. The size must be info.width * info.height * info.depth, and the pixels must be interleaved.
     *
     * @return    true if it succeeds, false if it fails.
     */
    bool WritePNGRGBA(const std::string &filename, const ip::ImageInfo &info, const ccl::binary &buffer);

}