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

#define XMD_H

// Don't include anything before the jpeglib section or you may get conflicting headers
// that only manifest a problem at runtime when using the library!
#include <stdio.h>
#include <setjmp.h>
#undef FAR
extern "C" {
    #include "jpeglib.h"
}

#include "ip/jpgwrapper.h"


#include <ccl/ccl.h>
#include <fstream>




namespace ip {

    struct my_error_mgr 
    {
        struct jpeg_error_mgr pub;    /* "public" fields */
        jmp_buf setjmp_buffer;    /* for return to caller */
    };

    typedef struct my_error_mgr * my_error_ptr;

    static void my_output_message (j_common_ptr cinfo)
    {
        ccl::ObjLog log;
        log.init("libjpeg exit error handler", 0);

        char buffer[2048];
        (*cinfo->err->format_message)(cinfo,buffer);
        log << ccl::LERR << buffer << log.endl;
    }
    
    /*
    * Here's the routine that will replace the standard error_exit method:
    */

    METHODDEF(void) my_error_exit (j_common_ptr cinfo)
    {
        /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
        my_error_ptr myerr = (my_error_ptr) cinfo->err;

        /* Always display the message. */
        /* We could postpone this until after returning, if we chose. */
        (*cinfo->err->output_message) (cinfo);

        /* Return control to the setjmp point */
        longjmp(myerr->setjmp_buffer, 1);
        return;
    }

    bool GetJPGImageInfo(std::string filename, ip::ImageInfo &info)
    {
        
        ccl::ObjLog log;
        log.init("GetJPGImageInfo", 0);

        FILE * infile;
        struct jpeg_decompress_struct cinfo;
        struct my_error_mgr jerr;
        //int row_stride;
        //JSAMPARRAY buffer;        /* Output row buffer */
        if ((infile = fopen(filename.c_str(), "rb")) == NULL) 
        {
            log << ccl::LERR  << "File could not be opened for reading: " << filename << log.endl;
            return false;
        }

        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = my_error_exit;
        // Establish the setjmp return context for my_error_exit to use.
        if (setjmp(jerr.setjmp_buffer)) 
        {
            //If we get here, the JPEG code has signaled an error.
            // We need to clean up the JPEG object, close the input file, and return.
            jpeg_destroy_decompress(&cinfo);
            fclose(infile);
            return false;
        }

        //Now we can initialize the JPEG compression object.
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);

        jpeg_read_header(&cinfo, TRUE);

        info.width = cinfo.image_width;
        info.height = cinfo.image_height;
        info.depth = cinfo.output_components;
        info.dataType = ip::ImageInfo::UBYTE;
        info.interleaved = true;

        jpeg_destroy_decompress(&cinfo);
        fclose(infile);        

        return true;
    }

    bool GetJPGImagePixels(std::string filename, ip::ImageInfo &info, ccl::binary &outbuffer)
    {
        ccl::ObjLog log;
        log.init("GetJPGImagePixels", 0);

        FILE * infile;
        struct jpeg_decompress_struct cinfo;
        struct my_error_mgr jerr;
        int row_stride;
        JSAMPARRAY buffer;        /* Output row buffer */
        if ((infile = fopen(filename.c_str(), "rb")) == NULL) 
        {
            log << ccl::LERR  << "File could not be opened for reading: " << filename << log.endl;
            return false;
        }

        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = my_error_exit;
        jerr.pub.output_message = my_output_message;
        // Establish the setjmp return context for my_error_exit to use.
        if (setjmp(jerr.setjmp_buffer)) 
        {
            //If we get here, the JPEG code has signaled an error.
            // We need to clean up the JPEG object, close the input file, and return.
            jpeg_destroy_decompress(&cinfo);
            fclose(infile);
            return false;
        }

        //Now we can initialize the JPEG compression object.
        jpeg_create_decompress(&cinfo);
        jpeg_stdio_src(&cinfo, infile);

        jpeg_read_header(&cinfo, TRUE);
        jpeg_start_decompress(&cinfo);
        row_stride = cinfo.output_width * cinfo.output_components;
        buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);
        int row = 0;
        outbuffer.resize(row_stride*cinfo.image_height);
        while (cinfo.output_scanline < cinfo.output_height) 
        {
            jpeg_read_scanlines(&cinfo, buffer, 1);
            unsigned char *rowp = &outbuffer[row_stride*row];
            for(int i=0;i<row_stride;i++)
            {
                rowp[i] = buffer[0][i];
            }
            row++;
        }
        jpeg_finish_decompress(&cinfo);

        info.width = cinfo.image_width;
        info.height = cinfo.image_height;
        info.depth = cinfo.output_components;
        info.dataType = ip::ImageInfo::UBYTE;
        info.interleaved = true;

        jpeg_destroy_decompress(&cinfo);
        fclose(infile);

        return true;
    }


    bool WriteJPG24(const std::string &filename, const ip::ImageInfo &info, const ccl::binary &buffer, int jpeg_quality)
    {
        ccl::ObjLog log;
        log.init("WriteJPG24", 0);
        struct jpeg_compress_struct cinfo;
        struct my_error_mgr jerr;
    
        /* this is a pointer to one row of image data */
        JSAMPROW row_pointer[1];
        FILE *outfile = fopen( filename.c_str(), "wb" );
    
        if ( !outfile )
        {
            log << "Error opening output jpeg file " << filename << log.endl;
            return false;
        }
  
        cinfo.err = jpeg_std_error(&jerr.pub);
        jerr.pub.error_exit = my_error_exit;
        jerr.pub.output_message = my_output_message;
        // Establish the setjmp return context for my_error_exit to use.
        if (setjmp(jerr.setjmp_buffer)) 
        {
            //If we get here, the JPEG code has signaled an error.
            // We need to clean up the JPEG object, close the input file, and return.
            fclose(outfile);
            return false;
        }
        jpeg_create_compress(&cinfo);
        /* Setting the parameters of the output file here */
        cinfo.image_width = info.width;    
        cinfo.image_height = info.height;
        cinfo.input_components = info.depth;
        cinfo.num_components = info.depth;

        cinfo.in_color_space = JCS_RGB;
        
        jpeg_stdio_dest(&cinfo, outfile);
        jpeg_set_quality(&cinfo,jpeg_quality,false);
        jpeg_set_defaults(&cinfo);
        /* Now do the compression .. */
        jpeg_start_compress( &cinfo, TRUE );
        /* like reading a file, this time write one row at a time */
        while( cinfo.next_scanline < cinfo.image_height )
        {
            row_pointer[0] = (unsigned char *)&buffer[ cinfo.next_scanline * cinfo.image_width *  cinfo.input_components];
            jpeg_write_scanlines( &cinfo, row_pointer, 1 );
        }
        /* similar to read file, clean up after we're done compressing */
        jpeg_finish_compress( &cinfo );
        jpeg_destroy_compress( &cinfo );
        fclose( outfile );

        return true;
    }

}
