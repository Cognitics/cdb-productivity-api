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

#include "ip/ip.h"

#include "ip/rgb.h"
#include "ip/jpgwrapper.h"
#include "ip/pngwrapper.h"
#include <sfa/Point.h>

#include <ccl/ccl.h>
#include <fstream>

#define M_PI 3.1415926535897932385

namespace ip {

    bool InterleavePixels(ip::ImageInfo &info, ccl::binary &buffer)
    {
        ccl::binary buffer2 = buffer;
        size_t numpix = info.width * info.height;
        for (size_t i = 0; i < info.depth; ++i)
        {
            for (size_t a = 0; a < numpix; ++a)
            {
                size_t src_pos = (numpix * i) + a;
                size_t dest_pos = (a*info.depth) + i;
                buffer[dest_pos] = buffer2[src_pos];
            }
        }

        info.interleaved = true;
        return true;
    }

    
    bool GetImagePixels(std::string filename, ip::ImageInfo &info, ccl::binary &buffer)
    {
        std::string ext = filename.substr(filename.find_last_of(".") + 1);

        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); 
        if(ext.compare("rgb")==0)
        {
            if (GetRGBImagePixels(filename, info, buffer))
            {
                InterleavePixels(info, buffer);
                FlipVertically(info, buffer);
                return true;
            }
        }
        else if(ext.compare("png")==0)
        {
            return GetPNGImagePixels(filename,info,buffer);
        }
        else if((ext.compare("jpg")==0)||(ext.compare("jpeg")==0))
        {
            return GetJPGImagePixels(filename,info,buffer);
        }

        return false;
    }
    
    bool GetImageInfo(std::string filename, ip::ImageInfo &info)
    {
        std::string ext = filename.substr(filename.find_last_of(".") + 1);

        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower); 
        if(ext.compare("rgb")==0)
        {
            return GetRGBImageInfo(filename,info);
        }
        else if(ext.compare("png")==0)
        {
            return GetPNGImageInfo(filename,info);
        }
        else if((ext.compare("jpg")==0)||(ext.compare("jpeg")==0))
        {
            return GetJPGImageInfo(filename,info);
        }

        return false;
    }

    void FlipVertically(ip::ImageInfo &info, ccl::binary &buffer)
    {
        ccl::binary copy(buffer);
        
        for(int i=0;i<info.height;i++)
        {
            int len = info.width * info.depth;
            switch(info.dataType)
            {
            case ImageInfo::USHORT:
                len *= 2;
                break;
            case ImageInfo::UINT32:
            case ImageInfo::FLOAT:
                len *= 4;
                break;
            }
            int ii = ((info.height-1) - i);
            //unsigned char *src = &buffer[(i*len)];
            //unsigned char *dest = &copy[(ii*len)];
            //memcpy(dest, src, len);
            // copy the row
            
            for(int j=0;j<len;j++)
            {
                buffer[(i*len)+j] = copy[(ii*len)+j];
            }
            
        }
    }

    void AddAlphaChannel(ip::ImageInfo &info, ccl::binary &image, ccl::binary &alpha)
    {
        ccl::binary copy(image);
        image.clear();
        for(int row=0;row<info.height;row++)
        {
            for(int col=0;col<info.width;col++)
            {
                for(int depth=0;depth<info.depth;depth++)
                {
                    image.push_back(copy.at( (row*info.width*info.depth)+(col*info.depth)+depth));
                }
                image.push_back(alpha.at((row*info.width)+col));
            }
        }
        info.depth++;
    }

    void MakeInterleaved(ip::ImageInfo &image)
    {
        
        if(image.interleaved)
        {
            ccl::binary interleaved_pix = image.pixels;
            for(int row=0;row<image.height;row++)
            {
                for(int col=0;col<image.width;col++)
                {
                    

                    for(int depth=0;depth<image.depth;depth++)
                    {
                        int interleaved_pos = (row*image.width*image.depth)+(col*image.depth);
                        int non_interleaved_pos = ((row*image.width) + col) + (depth*image.width*image.height);
                        interleaved_pix[interleaved_pos] = image.pixels.at(non_interleaved_pos);
                    }
                }
            }
            image.pixels = interleaved_pix;
            image.interleaved = true;
        }        
    }

    void MakeNonInterleaved(ip::ImageInfo &image)
    {
        if(!image.interleaved)
        {
            ccl::binary noninterleaved_pix = image.pixels;
            for(int row=0;row<image.height;row++)
            {
                for(int col=0;col<image.width;col++)
                {
                    for(int depth=0;depth<image.depth;depth++)
                    {
                        int interleaved_pos = (row*image.width*image.depth)+(col*image.depth);
                        int non_interleaved_pos = ((row*image.width) + col) + (depth*image.width*image.height);
                        noninterleaved_pix[non_interleaved_pos] = image.pixels.at(interleaved_pos);
                    }
                }
            }
            image.pixels = noninterleaved_pix;
            image.interleaved = false;
        }

    }

    // Performance warning, the image should be interleaved for best performance
    // TODO: use the alpha channel to specify 'blank' parts of the image where no pixels were available, or specify a blank color?
    void RotateImage(double theta_deg,const ip::ImageInfo &source_image, ip::ImageInfo &dest_image, double center_x, double center_y, bool sample = true)
    {
        double theta_rad = (theta_deg/180) * M_PI;
        ip::ImageInfo working_source_image = source_image;
        bool original_interleaved = source_image.interleaved;
        if(!working_source_image.interleaved)
            MakeInterleaved(working_source_image);
        if(sample)
        {
            for(int row=0;row<dest_image.height;row++)
            {
                for(int col=0;col<dest_image.width;col++)
                {
                    for(int depth=0;depth<dest_image.depth;depth++)
                    {
                        // For each pixel, translate the origin to the specified center and counter rotate the destination pixel to the source
                        // Then get the surrounding source pixels using the floor and ceiling of the rotated point
                        // Then weight the colors of each source pixel based on the distance between the rotated point and the floor/ceil pixels
                        // TODO: accumulate all the pixels based on the computed x and y scale, 
                        //       so if the source image pixels are smaller we sample more pixels and vice versa
                    }
                }
            }
        }
        else
        {
            double scale_x = 1.0;//TODO: calculate the x and y scale based on the source/dest pixels
            double scale_y = 1.0;//TODO: calculate the x and y scale based on the source/dest pixels
            double du_col = (double)sin(-theta_rad) * (1.0f / scale_x);
            double dv_col = (double)cos(-theta_rad) * (1.0f / scale_y);
            double du_row = dv_col;
            double dv_row = -du_col;
 
            double startingu = center_x - (center_x * dv_col + center_y * du_col);
            double startingv = center_y - (center_x * dv_row + center_y * du_row);
 
            double rowu = startingu;
            double rowv = startingv;
  
            for(int y = 0; y < dest_image.height; y++)
            {
                float u = rowu;
                float v = rowv;     
        
                for(int x = 0; x < dest_image.width ; x++)
                {            
                    int dstPos = ((x*4)+(y*dest_image.width*4));//TODO: this assumes 4 byte depth
                    int srcPos = ((int)u*4) + ((int)v * working_source_image.width * 4);//TODO: this assumes 4 byte depth
                    //char dbgmsg[1024];
                    //sprintf(dbgmsg,"Dest %d/%d from src %f/%f\n",y,x,v,u);
                    //OutputDebugStringA(dbgmsg);
                    if(u>0 && v>0 && u<working_source_image.width && v<working_source_image.height)
                    {
                        //dest[dstPos+0] = src[srcPos+0];
                        //pDstBase[dstPos+1] = pSrcBase[srcPos+1];
                        //pDstBase[dstPos+2] = pSrcBase[srcPos+2];
                        //pDstBase[dstPos+3] = pSrcBase[srcPos+3];
                    }
                    else
                    {
                        //pDstBase[dstPos+0] = 0;
                        //pDstBase[dstPos+1] = 0;
                        //pDstBase[dstPos+2] = 0;
                        //pDstBase[dstPos+3] = 0;
                    }
 
                    u += du_row;
                    v += dv_row;
                }
 
                rowu += du_col;
                rowv += dv_col;
            }
        }

    }

}