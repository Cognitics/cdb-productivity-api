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

#include "ip/rgb.h"
#include <ccl/ccl.h>
#include <fstream>

namespace ip {

    unsigned char getbyte(std::ifstream& input)
    {
        unsigned char val;
        input.read((char *)&val, sizeof(val));
        return val;
    }
    void putbyte(std::ofstream& output, char value)
    {
        output.write(&value, sizeof(value));
    }

    void putshort(std::ofstream& output, ccl::uint16_t value)
    {
        ccl::BigEndian<ccl::uint16_t> temp16 = value;
        output << temp16;
    }

    unsigned short getshort(std::ifstream& input)
    {
        ccl::BigEndian<ccl::uint16_t> val;
        input >> val;
        return val;
    }

    void putlong(std::ofstream& output, ccl::uint32_t value)
    {
        ccl::BigEndian<ccl::uint32_t> temp16 = value;
        output << temp16;
    }

    unsigned long getlong(std::ifstream& input)
    {
        ccl::BigEndian<ccl::uint32_t> val;
        input >> val;
        return val;
    }

    bool WriteRGB
    (
        const std::string    &filename,
        unsigned char        *r,
        unsigned char        *g,
        unsigned char        *b,
        int                    size_x,
        int                    size_y
    )
    {
        std::string ext = filename.substr(filename.find_last_of(".") + 1);

        if (
            ext.compare("rgb") != 0 &&
            ext.compare("rgba") != 0 &&
            ext.compare("sgi") != 0 &&
            ext.compare("int") != 0 &&
            ext.compare("inta") != 0 &&
            ext.compare("bw") != 0
        ) return false;

        std::ofstream out;
        out.open(filename.c_str(), std::ios::out | std::ios::binary );

        if (!out.is_open()) return false;

        putshort(out,474);
        putbyte(out,0);
        putbyte(out,1);
        putshort(out,3);
        putshort(out,size_x);
        putshort(out,size_y);
        putshort(out,3);
        putlong(out,0);
        putlong(out,255);
        for (int i = 0; i < 4; i++) putbyte(out,0);
        
        if (filename.size() > 79)
        {
            for (unsigned int i = 0; i < 79; i++) out << filename[i];
            out << '\0';
        }
        else
        {
            out << filename;
            for (unsigned int i = 0; i < (80 - filename.size()); i++) out << '\0';
        }

        putlong(out,0);
        for (int i = 0; i < 404; i++) putbyte(out,0);

        int data_size = size_x*size_y;

        out.write( (char*)r , data_size );
        out.write( (char*)g , data_size );
        out.write( (char*)b , data_size );

        out.close();

        return true;
    }

    bool GetRGBImageInfo(std::string filename, ip::ImageInfo &info)
    {
        ccl::ObjLog log;
        log.init("GetRGBImageInfo", 0);
        std::ifstream in;
        in.open(filename.c_str(), std::ios::in | std::ios::binary );

        if (!in.is_open()) 
        {
            log << ccl::LERR  << "File could not be opened for reading: " << filename << log.endl;
            return false;
        }

        getshort(in);
        getbyte(in);
        int bpp = getbyte(in);//bytes per pixel
        getshort(in);//dimensions
        info.width = getshort(in);
        info.height = getshort(in);
        info.depth = getshort(in);

        info.interleaved = false;
        
        if(bpp==1)
            info.dataType = ImageInfo::UBYTE;
        if(bpp==2)
            info.dataType = ImageInfo::USHORT;
        if(bpp==4)
            info.dataType = ImageInfo::UINT32;
        return true;
    }

    bool GetRGBImagePixels(std::string filename, ip::ImageInfo &info, ccl::binary &buffer)
    {
        ccl::ObjLog log;
        log.init("GetRGBImagePixels", 0);
        std::ifstream in;
        in.open(filename.c_str(), std::ios::in | std::ios::binary );

        if (!in.is_open()) 
        {
            log << ccl::LERR  << "File could not be opened for reading: " << filename << log.endl;
            return false;
        }

        short magic = getshort(in);//magic
        unsigned char storage = getbyte(in);
        int bpp = getbyte(in);//bytes per pixel
        short dim = getshort(in);//dimensions
        info.width = getshort(in);
        info.height = getshort(in);
        info.depth = getshort(in);
        unsigned int minpix = getlong(in);
        unsigned int maxpix = getlong(in);

        if(storage!=0)
        {
            return false;//no RLE compression support yet
        }

        for(int i=0;i<4;i++)
            getbyte(in);//dummy
        char imagename[80];
        for(int i=0;i<80;i++)
            imagename[i] = getbyte(in);//image name
        unsigned int colormap = getlong(in);

        for(int i=0;i<404;i++)
            getbyte(in);//unused
        info.interleaved = false;
        int datasize = 1;

        if(bpp==1)
        {
            info.dataType = ImageInfo::UBYTE;
            int pixsize = info.height*info.width*info.depth;
            buffer.resize(pixsize);
            for (int i = 0; i < pixsize; i++)
            {
                buffer[i] = getbyte(in);
            }
        }
        if(bpp==2)
        {
            info.dataType = ImageInfo::USHORT;
            int pixsize = info.height*info.width*info.depth;
            buffer.resize(pixsize);
            unsigned short *bufp = (unsigned short *)&buffer[0];
            for(int i=0;i<pixsize;i++)
                bufp[i] = getshort(in);
        }
        if(bpp==4)
        {
            info.dataType = ImageInfo::UINT32;
            int pixsize = info.height*info.width*info.depth;
            buffer.resize(pixsize);
            unsigned long *bufp = (unsigned long *)&buffer[0];
            for(int i=0;i<pixsize;i++)
                bufp[i] = getlong(in);
        }
        
        
        return true;
    }

}