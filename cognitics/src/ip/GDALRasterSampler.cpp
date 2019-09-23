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

//#pragma optimize("", off)
#ifdef WIN32
#include <io.h>
#endif
#include "ip/GDALRasterSampler.h"

#include <ccl/FileInfo.h>

#define USE_IPP_LIBRARY 1

//#include "ipp_k0.h"
#include "ipp.h"


#include <sfa/File.h>

#include <boost/foreach.hpp>
#include "ip/rasterPoly.h"

GDALRasterSampler::GDALRasterSampler() : bsp(NULL)
{
    log.init("GDALRasterSampler", this);
    log << ccl::LERR;
    gdalsampler::LoadProjDLL();
    geoSRS.SetWellKnownGeogCS("WGS84");
    m_reader.SetDestSRS(geoSRS);
}



IppStatus WarpPerspective_8u_C3R(Ipp8u *pSrc, IppiSize srcSize, Ipp32s srcStep,
    Ipp8u *pDst, IppiSize dstSize,
    Ipp32s dstStep, const double coeffs[3][3], IppiInterpolationType interpolation)
{
    IppiWarpSpec *pSpec = 0;
    Ipp8u *pInitBuf = 0;
    int specSize = 0, initSize = 0, bufSize = 0;
    Ipp8u *pBuffer = 0;
    const Ipp32u numChannels = 3;
    IppiPoint dstOffset = { 0, 0 };
    IppStatus status = ippStsNoErr;
    IppiBorderType borderType = ippBorderTransp;
    IppiWarpDirection direction = ippWarpForward;
    Ipp64f pBorderValue[numChannels];
    Ipp64f valB = 0.0, valC = 0.5;	 /* Catmull-Rom filter coefficients for cubic interpolation */
    IppiRect srcRoi = ippRectInfinite; /* Region of interest is not specified */
    for (int i = 0; i < numChannels; ++i)
        pBorderValue[i] = 255.0;
    /* Spec and init buffer sizes */
    status = ippiWarpPerspectiveGetSize(srcSize, srcRoi, dstSize, ipp8u, coeffs,
        interpolation, direction, borderType,
        &specSize, &initSize);
    if (status != ippStsNoErr)
        return status;
    /* Memory allocation */
    pSpec = (IppiWarpSpec *)ippsMalloc_8u(specSize);
    if (pSpec == NULL)
    {
        return ippStsNoMemErr;
    }
    /* Filter initialization */
    switch (interpolation)
    {
    case ippNearest:
        status = ippiWarpPerspectiveNearestInit(srcSize, srcRoi, dstSize, ipp8u, coeffs,
            direction, numChannels, borderType, pBorderValue, 0, pSpec);
        break;
    case ippLinear:
        status = ippiWarpPerspectiveLinearInit(srcSize, srcRoi, dstSize, ipp8u, coeffs,
            direction, numChannels, borderType, pBorderValue, 0, pSpec);
        break;
    case ippCubic:
        pInitBuf = ippsMalloc_8u(initSize);
        if (pInitBuf == NULL)
        {
            ippsFree(pSpec);
            return ippStsNoMemErr;
        }
        status = ippiWarpPerspectiveCubicInit(srcSize, srcRoi, dstSize, ipp8u, coeffs,
            direction, numChannels, valB, valC, borderType, pBorderValue, 0, pSpec, pInitBuf);
        ippsFree(pInitBuf);
        break;
    default:
        return ippStsInterpolationErr;
    }
    if (status < ippStsNoErr)
    {
        ippsFree(pSpec);
        return status;
    }
    /* work buffer size */
    status = ippiWarpGetBufferSize(pSpec, dstSize, &bufSize);
    if (status < ippStsNoErr)
    {
        ippsFree(pSpec);
        return status;
    }
    pBuffer = ippsMalloc_8u(bufSize);
    if (pBuffer == NULL)
    {
        ippsFree(pSpec);
        return ippStsNoMemErr;
    }
    /* Warp processing */
    switch (interpolation)
    {
    case ippNearest:
        status = ippiWarpPerspectiveNearest_8u_C3R(pSrc, srcStep, pDst, dstStep,
            dstOffset, dstSize, pSpec, pBuffer);
        break;
    case ippLinear:
        status = ippiWarpPerspectiveLinear_8u_C3R(pSrc, srcStep, pDst, dstStep,
            dstOffset, dstSize, pSpec, pBuffer);
        break;
    case ippCubic:
        status = ippiWarpPerspectiveCubic_8u_C3R(pSrc, srcStep, pDst, dstStep, dstOffset,
            dstSize, pSpec, pBuffer);
        break;
    }
    ippsFree(pSpec);
    ippsFree(pBuffer);
    return status;
}

gdalsampler::GDALRasterFileList GDALRasterSampler::GetFilesInAOI(gdalsampler::Quad &aoi)
{
    gdalsampler::GDALRasterFileList ret;
    bspMutex.lock();
    //aoi is in dest coordinates
    sfa::BSPCollectGeometriesVisitor visitor;

    double top = -DBL_MAX;
    double bottom = DBL_MAX;
    double left = DBL_MAX;
    double right = -DBL_MAX;
    aoi.ToMBR(left, right, top, bottom);
    //printf("Searching for files in left: %f right: %f top: %f bottom: %f\n", left, right, top, bottom);

    visitor.setBounds(left, bottom, right, top);
    visitor.visiting(bsp);

    visitor.results;
    std::list<sfa::Geometry *>::iterator geom_iter = visitor.results.begin();
    while (geom_iter != visitor.results.end())
    {
        sfa::Polygon *poly = dynamic_cast<sfa::Polygon *>(*geom_iter);
        geom_iter++;
        if (!poly)
        {
            continue;
        }
        aoimap_t::iterator find_iter = aoiFileMap.find(poly);

        if (find_iter != aoiFileMap.end())
        {
            gdalsampler::GDALRasterFilePtr file = aoiFileMap[poly];
            ret.push_back(file);
        }
    }
    bspMutex.unlock();
    return ret;
}

bool GDALRasterSampler::BuildBSP(bool rebuild)
{
    bspMutex.lock();
    if (!rebuild && bsp)
    {
        bspMutex.unlock();
        return true;
    }
    //Clean up a previous bsp if it exists
    if (bsp)
    {
        delete bsp;
        bsp = NULL;
    }
    bsp = new sfa::BSP;
    aoimap_t::iterator aoi_iter = aoiFileMap.begin();
    while (aoi_iter != aoiFileMap.end())
    {
        delete aoi_iter->first;
        aoi_iter++;
    }
    aoiFileMap.clear();
    gdalsampler::GDALRasterFileList &files = m_reader.GetFiles();
    //printf("Putting %d files in a bsp\n", files.size());
    gdalsampler::GDALRasterFileList::iterator file_iter = files.begin();
    while (file_iter != files.end())
    {
        gdalsampler::GDALRasterFilePtr file = *file_iter++;
        double top = -DBL_MAX;
        double bottom = DBL_MAX;
        double left = DBL_MAX;
        double right = -DBL_MAX;
        file->GetDestMBR(top, bottom, left, right);
        //printf("File coords: left: %f right: %f top: %f bottom: %f\n", left, right, top, bottom);
        sfa::Polygon *aoi_poly = new sfa::Polygon;
        sfa::LineString *ring = new sfa::LineString;
        ring->addPoint(sfa::Point(left, bottom));
        ring->addPoint(sfa::Point(right, bottom));
        ring->addPoint(sfa::Point(right, top));
        ring->addPoint(sfa::Point(left, top));
        ring->addPoint(sfa::Point(left, bottom));
        aoi_poly->addRing(ring);
        //Keep track of the geometry and its associated file
        aoiFileMap[aoi_poly] = file;
        bsp->addGeometry(aoi_poly);
    }
    std::map<sfa::Geometry *, sfa::LineString *> envelopes;
    bsp->generate(envelopes);
    bspMutex.unlock();
    return true;
}

bool GDALRasterSampler::CopyPixelsInsidePoly(u_char *src, u_char *dest, int width, int height, int depth, sfa::Polygon &poly)
{
    ip::rasterPolygonFill(poly,src,dest,height,width,depth);
    return true;
}

// Add the specified file to the list of sources.
bool GDALRasterSampler::AddFile(std::string file)
{
    ccl::FileInfo fi(file);
    std::string fileext = fi.getSuffix();
    std::string ext = ToLower(fileext);
    return m_reader.AddFile(file);
}
// Add all the files in the specified directory that match the specified filter
bool GDALRasterSampler::AddDirectory(std::string dir, std::set<std::string> extensions)
{
    std::vector<std::string> lowerExtensions;
    std::set<std::string>::iterator ext_iter = extensions.begin();
    while (ext_iter != extensions.end())
    {
        std::string ext = *ext_iter++;
        std::string lowerext = ToLower(ext);
        lowerExtensions.push_back(lowerext);
    }

    bool ret = false;
    std::string searchpath = ccl::joinPaths(dir,"*.*");
    log << ccl::LINFO << "Scanning " << dir << " and subdirectories for imagery files..." << log.endl;
    std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles(dir,"*.*",true);
    BOOST_FOREACH(ccl::FileInfo &fi,files)
    {    
        std::string file = fi.getFileName();
        std::string fileext = fi.getSuffix();
        std::vector<std::string>::iterator ext_iter = lowerExtensions.begin();
        while(ext_iter!= lowerExtensions.end())
        {
            std::string lowerext = *ext_iter++;
            if (ToLower(fileext) == lowerext)
            {
                m_reader.AddFile(file);
                ret = true;
            }
        }
    }
    return ret;
}
// Add a coverage file used to specify valid pixels in the source imagery
// If an attribute called 'file' is found features will be matched against
// the imagery file that attribute specifies. If no file attribute is found
// the first feature that contains the center point of an image will be used.
bool GDALRasterSampler::AddCoverageFile(std::string shapefile)
{
    m_reader.ReadCoverageShapes(shapefile);
    return false;
}

bool GDALRasterSampler::Sample(const gdalsampler::GeoExtents &window, u_char *buf)
{
#ifdef USE_IPP_LIBRARY
    return SampleIPP(window,buf);
#else
    return SampleSoftware(window,buf);
#endif

}

int dbg_no = 0;
std::vector<gdalsampler::GeoExtents> GDALRasterSampler::GetFileExtents()
{
    sfa::FeatureList features;
    std::vector<gdalsampler::GeoExtents> extents;
    gdalsampler::GDALRasterFileList &files = m_reader.GetFiles();
    extents.reserve(files.size());
    gdalsampler::GDALRasterFileList::iterator file_iter = files.begin();
    while(file_iter!=files.end())
    {
        const gdalsampler::GDALRasterFilePtr &file = *file_iter++;
        gdalsampler::GeoExtents fileExtents;
        fileExtents.filename = file->GetFilename();
        if(file->GetDestMBR(fileExtents.north,fileExtents.south,fileExtents.west,fileExtents.east))
        {
            fileExtents.width = file->GetWidth();
            fileExtents.height = file->GetHeight();
            extents.push_back(fileExtents);

            sfa::Polygon *p = new sfa::Polygon;
            sfa::LineString *l = new sfa::LineString;
            l->addPoint(sfa::Point(fileExtents.west, fileExtents.south));
            l->addPoint(sfa::Point(fileExtents.east, fileExtents.south));
            l->addPoint(sfa::Point(fileExtents.east, fileExtents.north));
            l->addPoint(sfa::Point(fileExtents.west, fileExtents.north));
            l->addPoint(sfa::Point(fileExtents.west, fileExtents.south));
            p->addRing(l);
            sfa::Feature *f = new sfa::Feature;
            f->setAttribute("filename", file->GetFilename());
            f->geometry = p;
            features.push_back(f);
        }
    }

    sfa::writeSFAFile(features, "feat.dbg");
    return extents;
}

gdalsampler::GeoExtents GDALRasterSampler::GetMinMaxExtents()
{
    gdalsampler::GeoExtents ret;
    ret.south = 90.0f;
    ret.north = -90.0f;
    ret.west = 180.0f;
    ret.east = -180.0f;

    gdalsampler::GDALRasterFileList &files = m_reader.GetFiles();
    gdalsampler::GDALRasterFileList::iterator file_iter = files.begin();
    while(file_iter!=files.end())
    {
        gdalsampler::GDALRasterFilePtr file = *file_iter++;
        double top = -DBL_MAX;
        double bottom = DBL_MAX;
        double left = DBL_MAX;
        double right = -DBL_MAX;
        if(file->GetDestMBR(top,bottom,left,right))
        {
            ret.north = std::max<double>(top,ret.north);
            ret.south = std::min<double>(bottom,ret.south);
            ret.east = std::max<double>(right,ret.east);
            ret.west = std::min<double>(left,ret.west);
        }        
    }

    return ret;
}

double GDALRasterSampler::GetHighestResolutionInArea(const gdalsampler::GeoExtents &window)
{
    double resolution = 0;

    gdalsampler::Quad aoi;
    aoi.ll.setX(window.west);
    aoi.ul.setX(window.west);
    aoi.lr.setX(window.east);
    aoi.ur.setX(window.east);
    aoi.lr.setY(window.south);
    aoi.ll.setY(window.south);    
    aoi.ur.setY(window.north);    
    aoi.ul.setY(window.north);

    gdalsampler::GDALRasterFileList &files = m_reader.GetFiles();
    gdalsampler::GDALRasterFileList::iterator file_iter = files.begin();
    while(file_iter!=files.end())
    {
        gdalsampler::GDALRasterFilePtr file = *file_iter++;
        gdalsampler::CachedRasterBlockList blocks;
        if(file->GetOverlappingBlocks(aoi,blocks))
        {
            if(resolution==0)
                resolution = file->GetResolution();
            else
                resolution = std::min<double>(resolution,file->GetResolution());
        }
    }
    return resolution;
}

double GDALRasterSampler::GetLowestResolutionInArea(const gdalsampler::GeoExtents &window)
{
    double resolution = DBL_MAX;

    gdalsampler::Quad aoi;
    aoi.ll.setX(window.west);
    aoi.ul.setX(window.west);
    aoi.lr.setX(window.east);
    aoi.ur.setX(window.east);
    aoi.lr.setY(window.south);
    aoi.ll.setY(window.south);    
    aoi.ur.setY(window.north);    
    aoi.ul.setY(window.north);

    gdalsampler::GDALRasterFileList &files = m_reader.GetFiles();
    gdalsampler::GDALRasterFileList::iterator file_iter = files.begin();
    while(file_iter!=files.end())
    {
        gdalsampler::GDALRasterFilePtr file = *file_iter++;
        gdalsampler::CachedRasterBlockList blocks;
        if(file->GetOverlappingBlocks(aoi,blocks))
        {
            if(resolution==DBL_MAX)
                resolution = file->GetResolution();
            else
                resolution = std::max<double>(resolution,file->GetResolution());
        }
    }
    return resolution;
}


bool WriteTiffFile(std::string filename, double n, double s, double e, double w, int width, int height, u_char *buf)
{
    GDALDriver* poDriver;

    GDALAllRegister();

    poDriver = GetGDALDriverManager()->GetDriverByName( "GTiff" );
    if( poDriver == NULL ) {
        std::cout << "Unable to load the geotiff driver.\n";
        return false;
    }            
    GDALDataset *poDstDS;       
    char **papszOptions = new char*[2];
    papszOptions[0]= NULL;
    papszOptions[1]= NULL;

    poDstDS = poDriver->Create( filename.c_str(), width, height, 3, GDT_Byte, papszOptions );

    delete[] papszOptions;

    double adfGeoTransform[6];
    adfGeoTransform[0] = w;
    adfGeoTransform[1] = (e - w) / width;
    adfGeoTransform[2] = 0;
    adfGeoTransform[3] = n;
    adfGeoTransform[4] = 0;
    adfGeoTransform[5] = ((n - s)/height) * -1;

    OGRSpatialReference oSRS;
    char *pszSRS_WKT = NULL;
    GDALRasterBand *poBand;

    poDstDS->SetGeoTransform( adfGeoTransform );
    
    oSRS.SetWellKnownGeogCS( "WGS84" );
    oSRS.exportToWkt( &pszSRS_WKT );
    poDstDS->SetProjection( pszSRS_WKT );
    CPLFree( pszSRS_WKT );

    int len = width * height;
    u_char *channels[3];
    u_char *r = channels[0] = new u_char[len];
    u_char *g = channels[1] = new u_char[len];
    u_char *b = channels[2] = new u_char[len];
    int rows = height;
    int cols = width;
    for(int row=rows-1;row>=0;row--) {

        for(int col=0;col<cols;col++) {
            *r++ = buf[(row*width*3)+(col*3)+0];
            *g++ = buf[(row*width*3)+(col*3)+1];
            *b++ = buf[(row*width*3)+(col*3)+2];
        }
    }

    poBand = poDstDS->GetRasterBand(1);
    poBand->RasterIO( GF_Write, 0, 0, width, height, 
                    channels[0], width, height, GDT_Byte, 0, 0 );

    poBand = poDstDS->GetRasterBand(2);
    poBand->RasterIO( GF_Write, 0, 0, width, height, 
                    channels[1], width, height, GDT_Byte, 0, 0 );

    poBand = poDstDS->GetRasterBand(3);
    poBand->RasterIO( GF_Write, 0, 0, width, height, 
                    channels[2], width, height, GDT_Byte, 0, 0 );

    delete[] channels[0];
    delete[] channels[1];
    delete[] channels[2];
    delete poDstDS;
    return true;
}


#define floatround(a) (floor(a+0.5))

bool GDALRasterSampler::SampleIPP(const gdalsampler::GeoExtents &window, u_char *buf)
{
    bool ret = false;
#ifdef USE_IPP_LIBRARY

    if (!bsp)
    {
        BuildBSP(false);
    }

    int scratchlen = window.width*window.height;
    u_char *scratch = new u_char[scratchlen*3];
    
    gdalsampler::CachedRasterBlockList blocks;
    gdalsampler::Quad aoi;
    aoi.ll.setX(window.west);
    aoi.ul.setX(window.west);
    aoi.lr.setX(window.east);
    aoi.ur.setX(window.east);
    aoi.lr.setY(window.south);
    aoi.ll.setY(window.south);    
    aoi.ur.setY(window.north);    
    aoi.ul.setY(window.north);
    

    gdalsampler::GDALRasterFileList files = GetFilesInAOI(aoi);//m_reader.GetFiles();
    //printf("Found %d files in aoi\n", files.size());
    gdalsampler::GDALRasterFileList::iterator file_iter = files.begin();
    while(file_iter!=files.end())
    {
        memset(scratch,0,scratchlen*3);
        gdalsampler::GDALRasterFilePtr file = *file_iter++;
        sfa::Polygon geoarea = file->GetValidArea();
        sfa::Polygon pixelArea;
        
        blocks.clear();
        file->GetOverlappingBlocks(aoi,blocks);
        if(blocks.size()>0)
        {
            if(!geoarea.isEmpty())
            {
                pixelArea = ProjectToPixelSpace(geoarea,window);
            }
            //printf("Destination block intersects with %d tiles from %s.\n",blocks.size(),file->GetFilename().c_str());
        }
        gdalsampler::CachedRasterBlockList::iterator iter = blocks.begin();
        while(iter!=blocks.end())
        {
            gdalsampler::CachedRasterBlockPtr block = *iter++;
            gdalsampler::CacheManager::getInstance()->PageBlock(block);
            u_char *interleavedbuf = new u_char[block->xsize*block->ysize*3];
            block->GetInterleavedPixels(interleavedbuf);

            IppiRect srcroi = { 0,0,block->xsize,block->ysize };
            IppiRect dstroi = { 0,0,window.width,window.height };
            gdalsampler::Quad srcGeoQuad  = block->GetDestCoverage();

            gdalsampler::Quad pixQuad;
            // Get the source quad in dest pixel coordinates
            window.GeoToPixel(srcGeoQuad.ul,pixQuad.ul);
            window.GeoToPixel(srcGeoQuad.ur,pixQuad.ur);
            window.GeoToPixel(srcGeoQuad.lr,pixQuad.lr);
            window.GeoToPixel(srcGeoQuad.ll,pixQuad.ll);

            // Round pixels to keep IPP from skipping 
            pixQuad.ul.setX(floatround(pixQuad.ul.X()));
            pixQuad.ll.setX(floatround(pixQuad.ll.X()));
            pixQuad.ur.setX(floatround(pixQuad.ur.X()));
            pixQuad.lr.setX(floatround(pixQuad.lr.X()));
            pixQuad.ul.setY(floatround(pixQuad.ul.Y()));
            pixQuad.ur.setY(floatround(pixQuad.ur.Y()));
            pixQuad.lr.setY(floatround(pixQuad.lr.Y()));
            pixQuad.ll.setY(floatround(pixQuad.ll.Y()));

            double srcquad[4][2];// source tile quadrangle in destination pixel space.
            srcquad[0][0] = pixQuad.ul.X();
            srcquad[0][1] = pixQuad.ul.Y();

            srcquad[1][0] = pixQuad.ur.X();
            srcquad[1][1] = pixQuad.ur.Y();

            srcquad[2][0] = pixQuad.lr.X();
            srcquad[2][1] = pixQuad.lr.Y();

            srcquad[3][0] = pixQuad.ll.X();
            srcquad[3][1] = pixQuad.ll.Y();

            int srcStep = block->xsize*3;
            IppiSize srcNumPix = {block->xsize,block->ysize};
            int destStep = window.width*3;
            IppiSize destNumPix = { window.width, window.height };
            // Get the transform from the quad
            double coeff[3][3];
            IppStatus istatus = ippiGetPerspectiveTransform(srcroi,srcquad,coeff);
            if(istatus==ippStsNoErr)
            {
                int srcStep = block->xsize*3;
                IppiSize srcNumPix = {block->xsize,block->ysize};
                
                // Now use the coeff to warp the source on to the dest.
                istatus=WarpPerspective_8u_C3R(interleavedbuf,
                                                   srcNumPix,
                                                   srcStep,
                                                   scratch,
                                                   destNumPix,
                                                   destStep,
                                                   coeff,
                                                   ippCubic);
                if(istatus!=ippStsNoErr)
                {
                    //printf("WarpPerspective_8u_C3R returned %d\n",istatus);
                }
            }            
            delete[] interleavedbuf;
        }
        if(blocks.size()>0)
        {
            ret = true;
            //
            if(!pixelArea.isEmpty())
            {
                CopyPixelsInsidePoly(scratch,buf,window.width,window.height,3,pixelArea);
            }
            else
            {
                CopyNonBlackPixels(scratch,buf,scratchlen);
            }

        }
    }
    
    delete[] scratch;
    
#else
    throw std::runtime_error("Attempt to call SampleIPP when IPP support was not compiled in!.");
#endif
    return ret;
}



bool GDALRasterSampler::SampleSoftware(const gdalsampler::GeoExtents &window, u_char *buf)
{
    // Get the blocks spatial extents
    // For each block pixel, sample the source image, and update the blocks.
    double ewPixelConst = (window.east - window.west)/window.width;
    double nsPixelConst = (window.north - window.south)/window.height;
    double pixelArea = ewPixelConst * nsPixelConst;
    int pixelsContributed = 0;
    int voidPixels = 0;
    int ignoredSourcePixels = 0;
    gdalsampler::OverlappingPixels pixels;
    for(int row=0;row<window.height;row++)
    {
        double pw,pe,pn,ps;//boundaries for this pixel
        // Compute the row top/bottom only once per row
        pn = row * nsPixelConst;
        ps = (row+1) * nsPixelConst;
        for(int col=0;col < window.width;col++)
        {
            pixels.Reset();
            double pixelR=0;
            double pixelG=0;
            double pixelB=0;
            
            pw = ewPixelConst * ewPixelConst;
            pe = (ewPixelConst+1) * ewPixelConst;

            gdalsampler::Quad destPixelAOI;
            destPixelAOI.ul.setX(pw);
            destPixelAOI.ll.setX(pw);
            destPixelAOI.ur.setX(pe);
            destPixelAOI.lr.setX(pe);
            destPixelAOI.ul.setY(pn);
            destPixelAOI.ur.setY(pn);
            destPixelAOI.ll.setY(ps);
            destPixelAOI.lr.setY(ps);


            m_reader.GetOverlappingPixels(destPixelAOI,pixels);
            u_int numOverlappingPixels = pixels.GetNumPixels();
                            
            double srcPixelSum = 0;
            for(u_int pi=0;pi<numOverlappingPixels;pi++)
            {
                const gdalsampler::OverlappingPixel &pixel = pixels.GetPixelAt(pi);
                if(pixel.coverageRatio > .01f)
                {
                    srcPixelSum += pixel.coverageRatio;
                    pixelR += (pixel.r*pixel.coverageRatio);
                    pixelG += (pixel.g*pixel.coverageRatio);
                    pixelB += (pixel.b*pixel.coverageRatio);
                }                    
            }
            
            if(srcPixelSum>0)
            {
                pixelR *= (1/srcPixelSum);
                pixelG *= (1/srcPixelSum);
                pixelB *= (1/srcPixelSum);
                pixelsContributed++;
            }
            else
            {
                pixelR = 0;
                pixelG = 0;
                pixelB = 0;
                pixelsContributed++;
            }
            int rgbidx = ((window.width-row)*window.width*3)+(col*3);
            // Now we've got a double value for RGB, we need to down cast them to ints and save them
            buf[rgbidx] = (int)floatround(pixelR);
            buf[rgbidx+1] = (int)floatround(pixelG);
            buf[rgbidx+2] = (int)floatround(pixelB);
        }
    }
    return true;
}

sfa::Polygon GDALRasterSampler::ProjectToPixelSpace(sfa::Polygon geopoly,const gdalsampler::GeoExtents window)
{            
    sfa::Polygon pixelpoly(geopoly);
    if(!geopoly.getExteriorRing())
        return pixelpoly;
    int numExt = geopoly.getExteriorRing()->getNumPoints();
    sfa::LineString *pixring = pixelpoly.getExteriorRing();
    sfa::LineString *georing = geopoly.getExteriorRing();
    for(int i=0;i<numExt;i++)
    {            
        window.GeoToPixel(*(georing->getPointN(i)),*(pixring->getPointN(i)));
    }

    return pixelpoly;

}
