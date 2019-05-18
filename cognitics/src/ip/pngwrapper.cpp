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
#include "ip/pngwrapper.h"
#include <ccl/ccl.h>
#include <fstream>

#define PNG_DEBUG 3
#include <png.h>

namespace ip {

    ccl::ObjLog pnglog("pngwrapper",0);
    
    void png_user_error_fn(png_structp png_ptr,png_const_charp error_msg)
    {
        pnglog << ccl::LERR << "LIBPNG Error:" << error_msg << pnglog.endl;
    }

    void png_user_warning_fn(png_structp png_ptr,png_const_charp warning_msg)
    {
        pnglog << ccl::LWARNING << "LIBPNG Warning: " << warning_msg << pnglog.endl;
    }

    bool GetPNGImageInfo(std::string filename, ip::ImageInfo &info)
    {
        //Based on an example from http://zarb.org/~gc/html/libpng.html
        ccl::ObjLog log;

        png_structp png_ptr = NULL;
        png_infop info_ptr;
        png_set_error_fn(png_ptr,0, png_user_error_fn, png_user_warning_fn);
        log.init("GetPNGImageInfo", 0);

        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(filename.c_str(), "rb");
        if (!fp)
        {
            log << ccl::LERR  << "File could not be opened for reading: " << filename << log.endl;
            return false;
        }

        fread(header, 1, 8, fp);
        if (png_sig_cmp((png_bytep)header, 0, 8))
        {
            log << ccl::LERR  << "File is not recognized as a PNG file: " << filename << log.endl;
            return false;
        }

        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_user_error_fn, png_user_warning_fn);

        if (!png_ptr)
        {
            //abort_("[read_png_file] png_create_read_struct failed");
            return false;
        }

        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            log << ccl::LERR  << "png_create_info_struct" << log.endl;
            return false;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            log << ccl::LERR  << "Error during init_io" << log.endl;
            return false;
        }

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        info.width = png_get_image_width(png_ptr, info_ptr);
        info.height = png_get_image_height(png_ptr, info_ptr);
        png_byte color_type = png_get_color_type(png_ptr, info_ptr);
        png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        info.interleaved = true;
        //TODO: need more robust format types
        switch(color_type)
        {
        case PNG_COLOR_TYPE_RGB:            
            info.depth = 3;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            info.depth = 4;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            info.depth = 2;
            break;
        default:
            info.depth = 1;
        }
        
        switch(bit_depth)
        {
        case 8:
            info.dataType = ip::ImageInfo::UBYTE;
            break;
        case 16:
            info.dataType = ip::ImageInfo::USHORT;
            break;
        case 32:
            info.dataType = ip::ImageInfo::UINT32;
            break;
        default:
            info.dataType = ip::ImageInfo::UBYTE;//throw an error maybe?
        }

        fclose(fp);

        return true;
    }

    bool GetPNGImagePixels(std::string filename, ip::ImageInfo &info, ccl::binary &buffer)
    {
        //Based on an example from http://zarb.org/~gc/html/libpng.html
        ccl::ObjLog log;

        png_structp png_ptr;
        png_infop info_ptr;
        
        log.init("GetPNGImagePixels", 0);

        char header[8];    // 8 is the maximum size that can be checked

        /* open file and test for it being a png */
        FILE *fp = fopen(filename.c_str(), "rb");
        if (!fp)
        {
            log << ccl::LERR  << "File could not be opened for reading: " << filename << log.endl;
            return false;
        }

        fread(header, 1, 8, fp);
        if (png_sig_cmp((png_bytep)header, 0, 8))
        {
            log << ccl::LERR  << "File is not recognized as a PNG file: " << filename << log.endl;
            return false;
        }

        /* initialize stuff */
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, png_user_error_fn, png_user_warning_fn);

        if (!png_ptr)
        {
            //abort_("[read_png_file] png_create_read_struct failed");
            return false;
        }
        png_set_error_fn(png_ptr,0, png_user_error_fn, png_user_warning_fn);
        info_ptr = png_create_info_struct(png_ptr);
        if (!info_ptr)
        {
            log << ccl::LERR  << "png_create_info_struct" << log.endl;
            return false;
        }

        if (setjmp(png_jmpbuf(png_ptr)))
        {
            log << ccl::LERR  << "Error during init_io" << log.endl;
            return false;
        }

        png_init_io(png_ptr, fp);
        png_set_sig_bytes(png_ptr, 8);

        png_read_info(png_ptr, info_ptr);

        info.width = png_get_image_width(png_ptr, info_ptr);
        info.height = png_get_image_height(png_ptr, info_ptr);
        png_byte color_type = png_get_color_type(png_ptr, info_ptr);
        png_byte bit_depth = png_get_bit_depth(png_ptr, info_ptr);

        info.depth = int(bit_depth);

        buffer.resize(info.height * png_get_rowbytes(png_ptr,info_ptr));

        png_bytep buffptr = (png_bytep)&buffer[0];

        png_bytep *row_pointers = new png_bytep[info.height];

        for (int y=0; y<info.height; y++)
               row_pointers[y] = (buffptr + (png_get_rowbytes(png_ptr,info_ptr)*y));
        
        png_read_image(png_ptr, row_pointers);

        info.interleaved = true;
        //TODO: need more robust format types
        switch(color_type)
        {
        case PNG_COLOR_TYPE_RGB:            
            info.depth = 3;
            break;
        case PNG_COLOR_TYPE_RGB_ALPHA:
            info.depth = 4;
            break;
        case PNG_COLOR_TYPE_GRAY_ALPHA:
            info.depth = 2;
            break;
        default:
            info.depth = 1;
        }
        
        switch(bit_depth)
        {
        case 8:
            info.dataType = ip::ImageInfo::UBYTE;
            break;
        case 16:
            info.dataType = ip::ImageInfo::USHORT;
            break;
        case 32:
            info.dataType = ip::ImageInfo::UINT32;
            break;
        default:
            info.dataType = ip::ImageInfo::UBYTE;//throw an error maybe?
        }

        fclose(fp);

        return true;
    }

bool WritePNG24(const std::string &filename, const ip::ImageInfo &info, const ccl::binary &buffer)
{
    if(info.depth!=3)
        return false;
    if(!info.interleaved)
        return false;
    png_byte color_type = PNG_COLOR_TYPE_RGB;
    png_byte bit_depth = 8;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    
    int y=0;
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * info.height);
    for (y=0; y<info.height; y++)
    {
        row_pointers[y] = (png_byte*) malloc(info.width*3);
        unsigned char *p = (unsigned char *)&buffer[(y*info.width*3)];
        memcpy(row_pointers[y],p,info.width*3);
    }
    
    /* create file */
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
            //"[write_png_file] File %s could not be opened for writing", file_name);
            return false;
    }

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, png_user_error_fn, png_user_warning_fn);

    if (!png_ptr)
    {
        //abort_("[write_png_file] png_create_write_struct failed");
        return false;
    }
    png_set_error_fn(png_ptr,0, png_user_error_fn, png_user_warning_fn);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        //abort_("[write_png_file] png_create_info_struct failed");
        return false;
        }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during init_io");
        return false;
    }

    png_init_io(png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during writing header");
        return false;
    }

    png_set_IHDR(png_ptr, info_ptr, info.width, info.height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during writing bytes");
        return false;
    }

    png_write_image(png_ptr, row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during end of write");
        return false;
    }

    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
    for (y=0; y<info.height; y++)
            free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);
    return true;
}


bool WritePNGRGBA(const std::string &filename, const ip::ImageInfo &info, const ccl::binary &buffer)
{
    if(info.depth!=4)
        return false;
    if(!info.interleaved)
        return false;
    

    png_byte color_type = PNG_COLOR_TYPE_RGB_ALPHA;
    png_byte bit_depth = 8;
    png_structp png_ptr;
    png_infop info_ptr;
    png_bytep * row_pointers;
    
    
    int y=0;
    row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * info.height);
    for (y=0; y<info.height; y++)
    {
        row_pointers[y] = (png_byte*) malloc(info.width*4);
        png_bytep row = row_pointers[y];
        for(int p=0;p<info.width;p++)
        {
            int rowstart = (y*info.width*4);
            row[(p*4)+0] = buffer[(rowstart+(p*4)+0)];
            row[(p*4)+1] = buffer[(rowstart+(p*4)+1)];
            row[(p*4)+2] = buffer[(rowstart+(p*4)+2)];
            row[(p*4)+3] = buffer[(rowstart+(p*4)+3)];    
        }
    }
    
    /* create file */
    FILE *fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
            //"[write_png_file] File %s could not be opened for writing", file_name);
            return false;
    }

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr)
    {
        //abort_("[write_png_file] png_create_write_struct failed");
        return false;
    }
    png_set_error_fn(png_ptr,0, png_user_error_fn, png_user_warning_fn);
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr)
    {
        //abort_("[write_png_file] png_create_info_struct failed");
        return false;
        }

    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during init_io");
        return false;
    }

    png_init_io(png_ptr, fp);


    /* write header */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during writing header");
        return false;
    }

    png_set_IHDR(png_ptr, info_ptr, info.width, info.height,
                 bit_depth, color_type, PNG_INTERLACE_NONE,
                 PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info(png_ptr, info_ptr);


    /* write bytes */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during writing bytes");
        return false;
    }

    png_write_image(png_ptr, row_pointers);


    /* end write */
    if (setjmp(png_jmpbuf(png_ptr)))
    {
        //abort_("[write_png_file] Error during end of write");
        return false;
    }

    png_write_end(png_ptr, NULL);

    /* cleanup heap allocation */
    for (y=0; y<info.height; y++)
            free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);
    return true;
}


}
