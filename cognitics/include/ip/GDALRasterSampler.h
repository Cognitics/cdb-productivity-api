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

#pragma once
#include <sstream>
#include <algorithm>
#include <string>
typedef unsigned char u_char;
typedef unsigned short u_short;
typedef unsigned int u_int;

#include "GDALRasterReader.h"
#include <ccl/ObjLog.h>
#include <ccl/mutex.h>
#include <sfa/BSP.h>

class GDALRasterSampler
{
    ccl::ObjLog log;
    // BSP of all the AOIs as polygon pointers
    sfa::BSP *bsp;
    //Map back to the file objects so when we get BSP hits we can get the associated files
    typedef std::map<sfa::Polygon *, gdalsampler::GDALRasterFilePtr> aoimap_t;
    aoimap_t aoiFileMap;
    ccl::mutex bspMutex;

    bool BuildBSP(bool rebuild);
    gdalsampler::GDALRasterFileList GetFilesInAOI(gdalsampler::Quad &aoi);

    void CopyNonNoDataPixels(float *src, float *dest, int len)
    {
        for (int i = 0; i < len; i++)
        {
            // This avoids copying no-data values, which are typically -32767.
            // IPP tends to sample to values like -32767.002, which causes problems.
            // Yes, we could check for a range around -32767, but nothing should
            // be this low anyway.
            if (src[i] > -30000)
                dest[i] = src[i];
        }

    }

    void CopyNonBlackPixels(u_char *src, u_char *dest, int len)
    {
        for(int i=0;i<len;i++)
        {
            u_char src_r = src[(i*3)+0];
            u_char src_g = src[(i*3)+1];
            u_char src_b = src[(i*3)+2];
            if((src_r!=0)||(src_g!=0)||(src_b!=0))
            {
                dest[(i*3)+0] = src_r;
                dest[(i*3)+1] = src_g;
                dest[(i*3)+2] = src_b;
            }
        }
    }


    std::string ToLower(const std::string &in) const
    {
        std::string ret = in;
        std::transform(ret.begin(), ret.end(), ret.begin(), ::tolower);
        return ret;
    }

    bool StringEndsWith(std::string val,std::string end)
    {
        if(val.size()>=end.size())
        {
            if(val.substr(val.size()-end.size(),end.size())==end)
                return true;
        }
        return false;
    }

    int RoundToInt(double val)
    {
        return int(floor(.5+val));
    }

    bool SolveForX(double x1, double y1, double x2, double y2, double y, double &x)
    {
        if(y1==y2)
            return false;
        if(x1==x2)
        {
            x = x1;
            return true;
        }
        x = ((-x1*y2)+(x1*y)+(y1*x2)-(x2*y))/(y1-y2);
        return true;

    }
    
    bool GetTwoPointsIntersectingQuad(gdalsampler::Quad quad, int y, double &x1, double &x2)
    {
        double min_x = std::min<double>(quad.ll.X(),quad.ul.X());
        double max_x = std::max<double>(quad.ll.X(),quad.ul.X());
        bool first = true;
        bool second = false;
        double pt1_x = min_x;
        double pt2_x = max_x;
        double solved_x = 0;
        if(SolveForX(quad.ul.X(),quad.ul.Y(),quad.ur.X(),quad.ur.Y(),y,solved_x))
        {
            first = false;
            x1 = solved_x;
        }
        if(SolveForX(quad.ur.X(),quad.ur.Y(),quad.lr.X(),quad.lr.Y(),y,solved_x))
        {
            if(first)
            {
                first = false;
                x1 = solved_x;
            }
            else
            {
                second = true;
                x2 = solved_x;
                return true;
            }
        }
        if(SolveForX(quad.lr.X(),quad.lr.Y(),quad.ll.X(),quad.ll.Y(),y,solved_x))
        {
            if(first)
            {
                first = false;
                x1 = solved_x;
            }
            else
            {
                second = true;
                x2 = solved_x;
                return true;
            }
        }
        if(SolveForX(quad.ll.X(),quad.ll.Y(),quad.ul.X(),quad.ul.Y(),y,solved_x))
        {
            if(first)
            {
                return false;
            }
            else
            {
                x2 = solved_x;
                return true;
            }
        }
        return false;
    }

    void CopyPixelsInside(u_char *src, u_char *dest, int width, int height, gdalsampler::Quad &quad)
    {
        const int DEPTH = 3;
        int min_y = std::min<int>(RoundToInt(quad.ul.Y()),RoundToInt(quad.ur.Y()));
        min_y = std::max<int>(0,RoundToInt(min_y));
        int max_y = std::max<int>(RoundToInt(quad.ll.Y()),RoundToInt(quad.lr.Y()));
        max_y = std::min<int>(height-1,RoundToInt(max_y));
        for(int row=min_y;row<max_y;row++)
        {
            double x1,x2;
            GetTwoPointsIntersectingQuad(quad,row,x1,x2);
            int min_x = std::min<int>(RoundToInt(x1),RoundToInt(x2));
            min_x = std::max<int>(min_x,0);
            int max_x = std::max<int>(RoundToInt(x1),RoundToInt(x2));
            max_x = std::min<int>(max_x,width-1);
            int len = ((max_x-min_x)+1)*DEPTH;
            int idx = (row*width*DEPTH)+(min_x*DEPTH);
            memcpy(&src[idx],&dest[idx],len);        
        }

    }

    bool CopyPixelsInsidePoly(u_char *src, u_char *dest, int width, int height, int depth, sfa::Polygon &poly);

    
    gdalsampler::GDALReader m_reader;
    OGRSpatialReference geoSRS;

    bool SampleSoftware(const gdalsampler::GeoExtents &window, u_char *buf);
    bool SampleIPP(const gdalsampler::GeoExtents &window, u_char *buf);
    bool SampleIPP(const gdalsampler::GeoExtents &window, float *buf);

public:
    GDALRasterSampler() ;

    // Add the specified file to the list of sources.
    bool AddFile(std::string file);
    bool RemoveFile(std::string file);
    // Add all the files in the specified directory that match the specified filter
    bool AddDirectory(std::string dir, std::set<std::string> extensions);
    // Add a coverage file used to specify valid pixels in the source imagery
    // If an attribute called 'file' is found features will be matched against
    // the imagery file that attribute specifies. If no file attribute is found
    // the first feature that contains the center point of an image will be used.
    // This should always be called before the first call to AddFile or AddDirectory.
    bool AddCoverageFile(std::string shapefile);

    // Sample all the available files that intersect the specified geographic window
    // into the output buffer with the specified width and height
    bool Sample(const gdalsampler::GeoExtents &window, u_char *buf);
    bool Sample(const gdalsampler::GeoExtents &window, float *buf);

    // Returns a GeoExtents that covers the max extents of all files added to the sampler.
    gdalsampler::GeoExtents GetMinMaxExtents();

    // Returns the resolution of the highest resolution data in the extents specified.
    // Note that resolution is x resolution * y resolution. Smaller is higher resolution.
    // returns 0 if no images overlap the window.
    double GetHighestResolutionInArea(const gdalsampler::GeoExtents &window);

    // Returns the resolution of the highest resolution data in the extents specified.
    // Note that resolution is x resolution * y resolution. Smaller is higher resolution.
    // returns DBL_MAX if no images overlap the window.
    double GetLowestResolutionInArea(const gdalsampler::GeoExtents &window);

    // Returns a list of the extents for all files loaded
    std::vector<gdalsampler::GeoExtents> GetFileExtents();

    sfa::Polygon ProjectToPixelSpace(sfa::Polygon geopoly,const gdalsampler::GeoExtents window);
};

bool WriteTiffFile(std::string filename, double n, double s, double e, double w, int width, int height, u_char *buf);