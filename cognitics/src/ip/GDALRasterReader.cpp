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

//#pragma optimize( "", off )

#include <float.h>
#include <ccl/StringUtils.h>
#include "ip/GDALRasterReader.h"
#include <sfa_file_factory/sfa_file_factory.h>
#include <cts/FlatEarthProjection.h>

#include <algorithm>

int g_debugMode = 0;
bool g_UseProjDLL = false;

namespace gdalsampler
{

    const int CacheManager::MAX_CACHE_ENTRIES = 5;
    const int CacheManager::CACHE_BLOCK_HEIGHT = 3000;
    const int CacheManager::CACHE_BLOCK_WIDTH = 3000;

    TransformCache *TransformCache::instance = NULL;
    ccl::mutex transform_cache_singleton_mutex;
    TransformCache *TransformCache::getInstance()
    {
        transform_cache_singleton_mutex.lock();
        if (instance == NULL)
        {
            instance = new TransformCache();
            instance->log.init("TransformCache");
            instance->log << ccl::LDEBUG;
        }
        transform_cache_singleton_mutex.unlock();
        return instance;
    }

    TransformSet *TransformCache::getTransforms(std::string filewkt, std::string destwkt)
    {
        sting_pair_t key;
        key.first = filewkt;
        key.second = destwkt;
        cache_mutex.lock();
        std::map<sting_pair_t, TransformSet *>::iterator xform_iter = transform_map.find(key);
        if (xform_iter != transform_map.end())
        {
            TransformSet *ret = xform_iter->second;
            cache_mutex.unlock();
            return ret;
        }

        //Create it if we don't already have it
        TransformSet *xform = new TransformSet();
        xform->_fileSRS = new OGRSpatialReference(filewkt.c_str());
        xform->_destSRS = new OGRSpatialReference(destwkt.c_str());

        xform->_localToDesttransformer = OGRCreateCoordinateTransformation(xform->_fileSRS, xform->_destSRS);
        xform->_destToLocaltransformer = OGRCreateCoordinateTransformation(xform->_destSRS, xform->_fileSRS);

        xform->_fileToDestTransformer = new CoordinateTransformer(xform->_fileSRS, xform->_destSRS);
        xform->_destToFileTransformer = new CoordinateTransformer(xform->_destSRS, xform->_fileSRS);

        transform_map[key] = xform;
        cache_mutex.unlock();
        return xform;
    }

    TransformCache::~TransformCache()
    {

    }

    //CacheManager *CacheManager::theInstance = NULL;
    // Thread local cache
    tl_ptr<CacheManager> CacheManager::theInstance;

    CacheManager *CacheManager::getInstance()
    {
        if(theInstance.get()==NULL)
        {
            //theInstance = new CacheManager();
            theInstance.set(new CacheManager());
        }
        return theInstance.get();
    }

    CacheManager::CacheManager()
    {
        _blockCache.clear();
        _altBlockCache.clear();
    }

    bool CacheManager::GetPixel(GDALRasterFile *file, int row, int col, OverlappingPixel &pixel)
    {
        //_cacheLock.lock();
        // Look in the cache first
        CachedRasterBlockList::iterator block_iter = _blockCache.begin();
        while(block_iter!=_blockCache.end())
        {
            CachedRasterBlockPtr block = *block_iter;
            if(block)
            {
                if((row >= block->yoffset) && (row < block->yoffset+block->ysize) &&
                    (col >= block->xoffset) && (col < block->xoffset+block->xsize))
                {
                    //cout << "Cache hit on " << row << "/" << col << std::endl;
                    int relative_row = row - block->yoffset;
                    int relative_col = col - block->xoffset;
                    int pixel_idx = (relative_row*block->xsize)+relative_col;
                    pixel.r = block->r[pixel_idx];
                    pixel.g = block->g[pixel_idx];
                    pixel.b = block->b[pixel_idx];
                    block->age = 0;
                    //_cacheLock.unlock();
                    return true;
                }
                else
                {
                    block->age++;
                }
            }

            block_iter++;
        }

        //_cacheLock.lock();

        // read the block and add it to the cache.        
        int xoffset = (col / CACHE_BLOCK_WIDTH)*CACHE_BLOCK_WIDTH;
        int yoffset = (row / CACHE_BLOCK_HEIGHT)*CACHE_BLOCK_HEIGHT;
        int xsize = std::min<int>(CACHE_BLOCK_WIDTH,file->GetWidth()-xoffset);
        int ysize = std::min<int>(CACHE_BLOCK_HEIGHT,file->GetHeight()-yoffset);
        CachedRasterBlockPtr block = CachedRasterBlockPtr(new CachedRasterBlock(xoffset,yoffset,xsize,ysize,file->GetFilename(),file));
        block->ReadBlock();

        if(_blockCache.size()==0)
        {
            for(int i=0;i<MAX_CACHE_ENTRIES;i++)
            {
                CachedRasterBlockPtr ptr = CachedRasterBlockPtr();
                _blockCache.push_back(ptr);
            }
        }

        CachedRasterBlockList::iterator iter = _blockCache.begin();
        int max_age = 0;
        CachedRasterBlockList::iterator insert_position = _blockCache.end();
        //printf("Reading cached block for %s %d/%d\n",file->GetFilename().c_str(),col,row);
        while(iter!=_blockCache.end())
        {
            CachedRasterBlockPtr ptr = *iter;
            if(!ptr)
            {
                // This is an empty slot
                insert_position = iter;
                break;
            }        
            else
            {
                if(ptr->age > max_age)
                    max_age = ptr->age;
            }
            iter++;

        }
        // If the cache is full, kick out the oldest entry
        if(insert_position==_blockCache.end())
        {
            iter = _blockCache.begin();
            while(iter!=_blockCache.end())
            {
                CachedRasterBlockPtr ptr = *iter;
                if(ptr->age>=max_age)
                {
                    //printf("Kicking out cached block for %s %d/%d\n",file->GetFilename().c_str(),ptr->xoffset,ptr->yoffset);
                    insert_position = iter;
                    break;
                }
                iter++;
            }
        }
        if(insert_position!=_blockCache.end())
        {
            *insert_position = block;
        }

        int relative_row = row - block->yoffset;
        int relative_col = col - block->xoffset;
        int pixel_idx = (relative_row*block->xsize)+relative_col;
        pixel.r = block->r[pixel_idx];
        pixel.g = block->g[pixel_idx];
        pixel.b = block->b[pixel_idx];
        block->age = 0;
        //_cacheLock.unlock();
        return false;


    }


    



    bool GetMBR(const sfa::PointList &poly, double &left, double &right, double &top, double &bottom)
    {
        size_t numPointsInPoly = poly.size();
        if(numPointsInPoly < 1)
            return false;
        const sfa::Point &firstPt = poly[0];
        
        for (size_t pti = 0; pti<numPointsInPoly; pti++)
        {
            const sfa::Point &thePt = poly[pti];
            if(thePt.X() < left)
                left = thePt.X();
            if(thePt.X() > right)
                right = thePt.X();
            if(thePt.Y() < bottom)
                bottom = thePt.Y();
            if(thePt.Y() > top)
                top = thePt.Y();
        }

        return true;
    }

    

    bool TransformPoly(const PointList &in,PointList &out, OGRCoordinateTransformation *transformer)
    {
        out.clear();// remove any previous entries
        size_t numPointsInPoly = in.size();
        for (size_t pti = 0; pti<numPointsInPoly; pti++)
        {
            sfa::Point outPt = in[pti];// copy to a new point            
            if(transformer)
            {
                double x = outPt.X();
                double y = outPt.Y();
                transformer->TransformEx(1,&x,&y,0);
                outPt.setX(x);
                outPt.setY(y);
            }
            else
            {
                //printf("!!");// for debugging
            }
            out.push_back(outPt);
        }
        return true;
    }


    

    bool GDALReader::IsPointCovered(double x, double y)
    {
        GDALRasterFileList::iterator iter = _files.begin();
        while(iter!=_files.end())
        {
            GDALRasterFilePtr file = *iter++;
            // test if the point is inside one of the files.
            double top = -DBL_MAX;
            double bottom = DBL_MAX;
            double left = DBL_MAX;
            double right = -DBL_MAX;
            // Get the MBR for this file
            file->GetDestMBR(top,bottom,left,right);
            if((x >= left) && (x <= right) && (y >= bottom) && (y <= top))
            {
                return true;
            }
        }
        return false;
    }

    GDALReader::GDALReader() :okToSort(false)
    {
        sortLock.unlock();
        log.init("GDALReader");
        log << ccl::LINFO;
    }

    GDALReader::~GDALReader()
    {
    }

    bool GDALRasterFile::OpenGDALFile(OGRSpatialReference &destsrs, std::string filename)
    {        
        GDALAllRegister();
        //Is this a pseudo filename ending with the table name?
        std::string tableName;
        std::string strippedFilename = filename;
        ccl::GetFilenameAndTable(filename, strippedFilename, tableName);
        if (!tableName.empty())
        {
            char** papszOptions = NULL;
            std::string tableStr = "TABLE=" + tableName;
            papszOptions = CSLAddString(papszOptions, tableStr.c_str());

            poDataset = (GDALDataset*)GDALOpenEx(strippedFilename.c_str(),
                GDAL_OF_READONLY, NULL, papszOptions, NULL);
        }
        else
        {
            poDataset = (GDALDataset *)GDALOpen(filename.c_str(), GA_ReadOnly);
        }
        if( poDataset == NULL )
        {
            log << ccl::LERR << "Unable to open " << filename << log.endl;        
            return false;
        }

        if (!xformset)
        {
            //Get the source and dest SRS, and look them up from the cache
            char *psrfstr = new char[4096];
            char *srfstr = psrfstr;
            strncpy(srfstr, poDataset->GetProjectionRef(), 4096);
            srfstr[4095] = '\0';//make sure it'm_bottom NULL terminated on an overflow
            std::string filesrswkt(psrfstr);            

            char *pszSRS_WKT = NULL;
            destsrs.exportToWkt(&pszSRS_WKT);
            std::string destsrswkt(pszSRS_WKT);            
            
            //Get from the command line parameter            
            xformset = TransformCache::getInstance()->getTransforms(filesrswkt, destsrswkt);
            delete[] psrfstr;
            CPLFree(pszSRS_WKT);
        }

        auto area_or_point_ptr = poDataset->GetMetadataItem("AREA_OR_POINT");
        if(area_or_point_ptr != nullptr)
        {
            auto area_or_point = std::string(poDataset->GetMetadataItem("AREA_OR_POINT"));
            std::transform(area_or_point.begin(), area_or_point.end(), area_or_point.begin(), ::toupper);
            pixel_is_point = (area_or_point == "POINT");
        }

        // Get the resolution and normalize it (somehow)
        double adfGeoTransform[6];
        if( poDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
        {
            OGRSpatialReference oOrtho;        
            oOrtho.SetWellKnownGeogCS( "WGS84" );        

            _fileWidth = poDataset->GetRasterXSize();
            _fileHeight = poDataset->GetRasterYSize();
            _top = adfGeoTransform[3];
            _left = adfGeoTransform[0];
            _bottom = _top + (adfGeoTransform[5] * poDataset->GetRasterYSize());
            _right = _left + (adfGeoTransform[1] * poDataset->GetRasterXSize());

            _origEWConst = adfGeoTransform[1];
            _origNSConst = adfGeoTransform[5];
            adfTransform4 = adfGeoTransform[4];
            adfTransform2 = adfGeoTransform[2];
            _determinate = (_origEWConst * _origNSConst - adfTransform2 * adfTransform4);
        }
        else
        {
            return false;
        }

        return true;
    }

    
    GDALRasterFile::GDALRasterFile(OGRSpatialReference destSRS, std::string filename)
        : _filename(filename)
    {
        isRPF =  false;
        referenceCount = 0;
        references = 0;
        log.init("GDALRasterFile");
        log << ccl::LINFO;
        _age = 0;
        xformset = NULL;

        // Open the file, get the fileSRS
        _isValid = OpenGDALFile(destSRS, _filename);
        if (!_isValid)
        {
            xformset = NULL;
            return;
        }
        Quad localCoverage;
        PixelToLocalPoint(0,0,localCoverage.ul);
        PixelToLocalPoint(0,_fileWidth,localCoverage.ur);
        PixelToLocalPoint(_fileHeight,_fileWidth,localCoverage.lr);
        PixelToLocalPoint(_fileHeight,0,localCoverage.ll);    
        SetGeoCoverage(localCoverage);
        // get the geo center
        double orig_lat = (_coverage.ll.Y() + _coverage.ul.Y())/2;
        double orig_lon = (_coverage.ll.X() + _coverage.lr.X())/2;
        cts::FlatEarthProjection flatEarthProjection(orig_lat,orig_lon);
        double widthm = flatEarthProjection.convertGeoToLocalX(_coverage.lr.X()) - flatEarthProjection.convertGeoToLocalX(_coverage.ll.X());
        double heightm = flatEarthProjection.convertGeoToLocalX(_coverage.ul.Y()) - flatEarthProjection.convertGeoToLocalX(_coverage.ll.Y());

        double resX = widthm / _fileWidth;
        double resY = heightm / _fileHeight;
        x_resolution = resX;
        y_resolution = resY;
        resolution = resX * resY;
        Close();//Free up the file handle
    }

    GDALRasterFile::~GDALRasterFile()
    {    
        if (poDataset)
            GDALClose(poDataset);
    }

    // Query all the files for overlapping pixels
    bool GDALReader::GetOverlappingPixels(Quad aoi, OverlappingPixels &pixels)
    {
        GDALRasterFileList::iterator iter = _files.begin();
        while(iter!=_files.end())
        {
            GDALRasterFilePtr file = *iter++;
            // Get overlapping pixels and add the results in
            file->GetOverlappingPixels(aoi,pixels);
        }
        return true;
    }

    void GDALRasterFile::SetGeoCoverage(const Quad &coverage)
    {
        //_coverage = coverage.Transform(_localToDesttransformer);
        _localCoverage = coverage;
        _coverage = coverage.Transform(GetFileToDestTransformer());//_fileToDestTransformer);
    }


    // Query this file (potentially one or more blocks) for overlapping pixels
    bool GDALRasterFile::GetOverlappingPixels(Quad aoi, OverlappingPixels &pixels)
    {
        CacheManager *cachemgr = CacheManager::getInstance();
        _age++;
        bool ret = true;
        double aoi_top = -DBL_MAX;
        double aoi_bottom = DBL_MAX;
        double aoi_left = DBL_MAX;
        double aoi_right = -DBL_MAX;

        double ext_top = aoi_top;
        double ext_bottom = aoi_bottom;
        double ext_left = aoi_left;
        double ext_right = aoi_right;

        Quad localaoi = aoi.Transform(GetDestToFileTransformer());
        Quad validaoi;
        // Get the part of the aoi that intersects with the valid
        // area as specified by the shapefile.
        if(!_validArea.isEmpty())
        {
            bool intersects = localaoi.IntersectWith(_localCoverage,validaoi);
            if(!intersects)
            {
                return false;
            }
        }
        else
        {
            validaoi = localaoi;
        }
        validaoi.ToMBR(aoi_left,aoi_right,aoi_top,aoi_bottom);

        double local_row = 0;
        double local_col = 0;
        int pixel_row = 0;
        int pixel_col = 0;
        int minrow = _fileHeight;
        int maxrow = 0;
        int mincol = _fileWidth;
        int maxcol = 0;
        
        // Repeat for each of the four corners.
        LocalToPixelPoint(pixel_row,pixel_col,aoi_top,aoi_left);
        ExpandExtents(pixel_row,pixel_col,minrow,maxrow,mincol,maxcol);
        
        LocalToPixelPoint(pixel_row,pixel_col,aoi_top,aoi_right);
        ExpandExtents(pixel_row,pixel_col,minrow,maxrow,mincol,maxcol);

        LocalToPixelPoint(pixel_row,pixel_col,aoi_bottom,aoi_right);
        ExpandExtents(pixel_row,pixel_col,minrow,maxrow,mincol,maxcol);

        LocalToPixelPoint(pixel_row,pixel_col,aoi_bottom,aoi_left);
        ExpandExtents(pixel_row,pixel_col,minrow,maxrow,mincol,maxcol);

        bool somePixel = false;

        // Now we have the local coordinates for the rows and columns that overlap.
        for(int row = minrow;row<=maxrow;row++)
        {
            for(int col = mincol;col<=maxcol;col++)
            {
                _age = 0;
                somePixel = true;
                OverlappingPixel &thePixel = pixels.AllocatePixel();;
                thePixel.queriedArea = aoi;
                // Get the pixel RGB value
                //GetPixel(row,col,thePixel);
                cachemgr->GetPixel(this,row,col,thePixel);

                Quad localPixelCoverage;
                PixelToLocalPoint(row,col,localPixelCoverage.ul);
                PixelToLocalPoint(row,col+1,localPixelCoverage.ur);
                PixelToLocalPoint(row+1,col+1,localPixelCoverage.lr);
                PixelToLocalPoint(row+1,col,localPixelCoverage.ll);
                
                // Compute the intersection between this pixel and the queried value (in dest coordinates)
                Quad localCoverage;
                if(localaoi.IntersectWith(localPixelCoverage,localCoverage))
                {                    
                    // transform the overlap into geo coverage
                    //thePixel.coveredArea = localCoverage.Transform(_localToDesttransformer);
                    thePixel.coveredArea = localCoverage.Transform(GetFileToDestTransformer());//_fileToDestTransformer);
                    // Now see if each corner is inside the file areal (this specifies the area for which the image is valid).
                    double valid_area_weight = 1;
                    // This is a cheat to approximate edges where the pixel is partially outside the boundary of the file's valid area
                    if(!_validArea.isEmpty())
                    {
                        if(!_validArea.intersects(&thePixel.coveredArea.ul))
                        {
                            valid_area_weight -= .25;
                        }
                        if(!_validArea.intersects(&thePixel.coveredArea.ur))
                        {
                            valid_area_weight -= .25;
                        }
                        if(!_validArea.intersects(&thePixel.coveredArea.lr))
                        {
                            valid_area_weight -= .25;
                        }
                        if(!_validArea.intersects(&thePixel.coveredArea.ll))
                        {
                            valid_area_weight -= .25;
                        }
                    }
                    double pixel_area_covered = localCoverage.CalculateArea();
                    if(pixel_area_covered && (valid_area_weight>0))
                    {
                        double aoi_area = localaoi.CalculateArea();
                        // compute the percentage they overlap (query area)/(this pixel area)
                        thePixel.coverageRatio = valid_area_weight * std::min<double>(1,pixel_area_covered/aoi_area);

                        // Add to the output
                        thePixel.isValid = true;
                    }
                    else
                    {
                        pixels.DeallocateLastPixel();
                    }
                }
                else
                {
                    pixels.DeallocateLastPixel();
                }
            }
        }

        return ret;
    }

    Quad CachedRasterBlock::GetDestCoverage()
    {
        Quad ret;
        Quad localquad;
        int left_pix = xoffset;
        int right_pix = xoffset + xsize;
        int top_pix = yoffset;
        int bottom_pix = yoffset + ysize;

        m_file->PixelToLocalPoint(top_pix,left_pix,localquad.ul);
        m_file->PixelToLocalPoint(top_pix,right_pix,localquad.ur);
        m_file->PixelToLocalPoint(bottom_pix,right_pix,localquad.lr);
        m_file->PixelToLocalPoint(bottom_pix,left_pix,localquad.ll);

        CoordinateTransformer * toDestTransform = m_file->GetFileToDestTransformer();
        ret = localquad.Transform(toDestTransform);

        return ret;
    }

    CachedRasterBlock::CachedRasterBlock(int xoffset, int yoffset, int width, int height, std::string filename, GDALRasterFile *file, bool lazyAlloc)
    {
        m_ready = false;
        this->m_file = file;
        this->m_filename = filename;
        this->xoffset = xoffset;
        this->yoffset = yoffset;
        this->xsize = width;
        this->ysize = height;
        this->age = 0;
        if(!lazyAlloc)
        {
            r = new u_char[width*height];
            memset(r,0,width*height);
            g = new u_char[width*height];
            memset(g,0,width*height);
            b = new u_char[width*height];
            memset(b,0,width*height);
        }
        else
        {
            r = g = b = NULL;
        }
    }

    CachedRasterBlock::~CachedRasterBlock()
    {
        //printf("~CachedRasterBlock() for %s\n",m_filename.c_str());
        delete[] r;
        delete[] g;
        delete[] b;
        delete[] elev;

    }

    bool CacheManager::MakeCacheRoom(int size)
    {

        while((GetMemoryInUse()+size) > CACHE_MAX_MEMORY)
        {
            int max_age = 0;
            CachedRasterBlockList::iterator block_iter = _altBlockCache.begin();
            while(block_iter!=_altBlockCache.end())
            {
                CachedRasterBlockPtr ptr = *block_iter++;
                if(ptr->IsReady())
                {
                    if(ptr->age > max_age)
                    {
                        max_age = ptr->age;
                    }
                }                
            }
            
            block_iter = _altBlockCache.begin();
            while(block_iter!=_altBlockCache.end())
            {
                CachedRasterBlockPtr ptr = *block_iter++;
                if(ptr->IsReady()&&(ptr->age>=max_age))
                {
                    //printf("Kicking out cached block for %s %d/%d\n", ptr->m_filename.c_str(),ptr->xoffset,ptr->yoffset);
                    ptr->UnloadBlock();
                    break;
                }
            }
        }

        return true;
    }

    bool CacheManager::PageBlock(CachedRasterBlockPtr &block)
    {
        if(block->IsReady())
            return true;

        int blocksize = (block->xsize * block->ysize * 3);
        CachedRasterBlockList::iterator block_iter = _altBlockCache.begin();
        while(block_iter!=_altBlockCache.end())
        {
            CachedRasterBlockPtr ptr = *block_iter++;
            if(ptr->IsSameBlock(block.get()))
            {
                ptr->age = 0;
                if(ptr->IsReady())
                {
                    block = ptr;
                    return true;
                }
                else
                {                        
                    // make room for this block
                    MakeCacheRoom(blocksize);
                    block->ReadBlock();
                    return true;
                }
            }
            else
            {
                ptr->age++;
            }
        }
        MakeCacheRoom(blocksize);
        _altBlockCache.push_back(block);
        block->ReadBlock();
        return true;
    }

    bool CachedRasterBlock::GetInterleavedPixels(u_char *buf)
    {
        int len = xsize * ysize;
        for(int i=0;i<len;i++)
        {
            buf[(i*3)+0] = r[i];
            buf[(i*3)+1] = g[i];
            buf[(i*3)+2] = b[i];
        }

        return true;
    }
    bool CachedRasterBlock::ReadBlock()
    {
        GDALDataset *poDataset = m_file->Reference();
        if (!poDataset)
            return false;

        if(poDataset->GetRasterCount() == 1)
        {
            auto band = poDataset->GetRasterBand(1);
            if(band->GetRasterDataType() == GDT_Float32)
            {
                if(r)
                {
                    delete r;
                    r = nullptr;
                    delete g;
                    g = nullptr;
                    delete b;
                    b = nullptr;
                }
                elev = new float[xsize * ysize];
                g_GDALProtMutex.lock();

                m_ready = (CE_None == band->RasterIO(GF_Read, xoffset, yoffset, xsize, ysize, elev, xsize, ysize, GDT_Float32, 0, 0));
                if(!m_ready)
                    printf("Error: Failed float read x/y: %d/%d w/h: %d/%d\n", xoffset, yoffset, xsize, ysize);
                g_GDALProtMutex.unlock();
                return m_ready;
            }
        }

        if(!r)
        {
            r = new u_char[xsize*ysize];
            memset(r,0,xsize*ysize);
            g = new u_char[xsize*ysize];
            memset(g,0,xsize*ysize);
            b = new u_char[xsize*ysize];
            memset(b,0,xsize*ysize);
        }
        bool hasRGB = poDataset->GetRasterCount() >= 3; 
        //bool hasAlpha = poDataset->GetRasterCount() >= 4; 
        bool hasColorTable = poDataset->GetRasterCount() >= 1 && poDataset->GetRasterBand(1)->GetColorTable(); 
        bool hasGreyScale = poDataset->GetRasterCount() == 1; 

        if(hasRGB)
        {
            g_GDALProtMutex.lock();
            //printf("RGB Read x/y: %d/%d w/h: %d/%d\n",xoffset,yoffset,xsize,ysize);
            
            GDALRasterBand *poBand;
            poBand = poDataset->GetRasterBand(1);// Red...
            if(CE_None!=poBand->RasterIO( GF_Read, xoffset, yoffset, xsize, ysize, 
                            r, xsize,ysize, GDT_Byte, 
                            0, 0 ))
            {
                printf("Error: Failed RGB Read x/y: %d/%d w/h: %d/%d\n",xoffset,yoffset,xsize,ysize);
                //log << ccl::LERR << "Error: Failed RGB Read x/y: " << xoffset << "/" << yoffset << "w/h: " << xsize << "/" << ysize << l
            }
            poBand = poDataset->GetRasterBand(2);// Green...
            if (CE_None != poBand->RasterIO(GF_Read, xoffset, yoffset, xsize, ysize,
                g, xsize, ysize, GDT_Byte,
                0, 0))
            {
                printf("Error: Failed RGB Read x/y: %d/%d w/h: %d/%d\n", xoffset, yoffset, xsize, ysize);
            }
            poBand = poDataset->GetRasterBand(3);// Blue...
            if (CE_None != poBand->RasterIO(GF_Read, xoffset, yoffset, xsize, ysize,
                b, xsize, ysize, GDT_Byte,
                0, 0))
            {
                printf("Error: Failed RGB Read x/y: %d/%d w/h: %d/%d\n", xoffset, yoffset, xsize, ysize);
            }
            g_GDALProtMutex.unlock();
        }
        else if(hasColorTable)
        {
            g_GDALProtMutex.lock();
            //printf("Color Table Read x/y: %d/%d w/h: %d/%d\n",xoffset,yoffset,xsize,ysize);
            int len = xsize*ysize;
            u_char *idxs = new u_char[len];
            
            poDataset->GetRasterBand(1)->GetColorTable();
            GDALRasterBand *poBand;
            poBand = poDataset->GetRasterBand(1);

            poBand->RasterIO( GF_Read, xoffset, yoffset, xsize, ysize, 
                            idxs, xsize,ysize, GDT_Byte, 
                            0, 0 );

            GDALColorTable *ct;
            ct = poBand->GetColorTable();

            for(int i=0;i<len;i++)
            {
                GDALColorEntry sEntry;
                
                sEntry.c1 = idxs[i];
                sEntry.c2 = idxs[i];
                sEntry.c3 = idxs[i];

                ct->GetColorEntryAsRGB( idxs[i], &sEntry );

                // Apply RGB back over destination image.  
                 r[i] = (u_char)sEntry.c1;  
                 g[i] = (u_char)sEntry.c2;  
                 b[i] = (u_char)sEntry.c3;
            }
            delete[] idxs;
            g_GDALProtMutex.unlock();
        }
        else if(hasGreyScale)
        {
            g_GDALProtMutex.lock();
            int len = xsize*ysize;
            u_char *idxs = new u_char[len];
            
            poDataset->GetRasterBand(1)->GetColorTable();
            GDALRasterBand *poBand;
            poBand = poDataset->GetRasterBand(1);

            poBand->RasterIO( GF_Read, xoffset, yoffset, xsize, ysize, 
                            idxs, xsize,ysize, GDT_Byte, 
                            0, 0 );

            for(int i=0;i<len;i++)
            {
                // Apply RGB back over destination image.  
                 r[i] = idxs[i];  
                 g[i] = idxs[i];  
                 b[i] = idxs[i];
            }
            delete[] idxs;
            g_GDALProtMutex.unlock();
        }
        m_ready = true;
        return true;
    }

    bool GDALReader::AddFile(std::string filename)
    {
        ccl::scoped_mutex m(&addmutex);
        GDALRasterFilePtr gdalfile(new GDALRasterFile(_destSRS,filename));
        if(gdalfile->IsValid())
        {
            double top = -DBL_MAX;
            double bottom = DBL_MAX;
            double left = DBL_MAX;
            double right = -DBL_MAX;
            // Get the MBR for this file
            gdalfile->GetDestMBR(top,bottom,left,right);
            // compute the center
            double centerx = (left + right)/2;
            double centery = (top+bottom)/2;
            
            // look through our coverageShapes for the feature that the center of this file is in
            sfa::Polygon feature = GetShapeForFileOrPoint(filename,centerx,centery);
            if(!feature.isEmpty())
            {
                gdalfile->SetValidArea(feature);
            }
            // set the file to use that shape
            _files.push_back(gdalfile);
            okToSort = true;
            return true;
        }
        return false;
    }

    sfa::Polygon GDALReader::GetShapeForFileOrPoint(std::string filename, double x, double y)
    {
        sfa::Polygon ret;
        std::vector<sfa::Feature>::iterator iter = coverageShapes.begin();
        while(iter!=coverageShapes.end())
        {
            sfa::Feature &feat = *iter++;
            sfa::Polygon *areal = dynamic_cast<sfa::Polygon*>(feat.geometry);
            if(areal)
            {
                if(feat.hasAttribute("file"))
                {
                    std::string vec_file = feat.attributes.getAttributeAsString("file");
                    if(ccl::stringCompareNoCase(vec_file.c_str(),filename.c_str())==0)
                    {
                        return areal;
                    }
                }
                else
                {
                    sfa::Point pt(x,y);
                    if(areal->intersects(&pt))
                    {
                        return areal;
                    }
                }
            }
        }        
        return ret;
    }

    bool GDALReader::GetOverlappingBlocks(Quad aoi, CachedRasterBlockList &blocks)
    {
        GDALRasterFileList::iterator file_iter = _files.begin();
        while(file_iter!=_files.end())
        {
            GDALRasterFilePtr file = *file_iter++;
            file->GetOverlappingBlocks(aoi,blocks);            
        }
        return true;
    }

    bool GDALRasterFile::GetOverlappingBlocks(Quad aoi, CachedRasterBlockList &blocks)
    {
        Quad localaoi = aoi.Transform(GetDestToFileTransformer());
        Quad validaoi;
        Quad intersect;
        // If this file doesn't intersect with the AOI, just return.
        if(!_coverage.IntersectWith(aoi,intersect))
        {
            return false;
        }
        // Get the part of the aoi that intersects with the valid
        // area as specified by the shapefile.
        if(!_validArea.isEmpty())
        {
            bool intersects = localaoi.IntersectWith(_localCoverage,validaoi);
            if(!intersects)
            {
                return false;
            }
        }
        const int MAX_TILE_WIDTH = 1536;
        const int MAX_TILE_HEIGHT = 1536;
        double dnum_cols = this->_fileWidth / MAX_TILE_WIDTH;
        double dnum_rows = this->_fileHeight / MAX_TILE_HEIGHT;
        int num_cols = int(ceil(dnum_cols));
        int num_rows = int(ceil(dnum_rows));
        // Generate the virtual grid of blocks
        for(int row=0;row<=num_rows;row++)
        {
            for(int col=0;col<=num_cols;col++)
            {
                Quad tilequad;
                Quad intersectquad;
                int left = (col*MAX_TILE_WIDTH);
                int right = std::min<int>(((col+1)*MAX_TILE_WIDTH)-1,this->_fileWidth-1);
                int top = (row*MAX_TILE_HEIGHT);
                int bottom = std::min<int>(((row+1)*MAX_TILE_HEIGHT)-1,this->_fileHeight-1);
                PixelToLocalPoint(top,left,tilequad.ul);
                PixelToLocalPoint(top,right,tilequad.ur);
                PixelToLocalPoint(bottom,right,tilequad.lr);
                PixelToLocalPoint(bottom,left,tilequad.ll);

                // convert the tile to destination coordinates.
                //Quad destQuad = tilequad.Transform(this->GetFileToDestTransformer());
                CachedRasterBlockPtr block(new CachedRasterBlock(left,top,(right-left)+1,(bottom-top)+1,this->_filename,this,true));
                Quad destQuad = block->GetDestCoverage();
                // intersect the quad of each block with the AOI, and add any
                // intersecting blocks to the hit list.
                if(destQuad.IntersectWith(aoi,intersectquad))
                {                    
                    blocks.push_back(block);
                }
            }
        }
        return blocks.size()>0;
    }

    void GDALRasterFile::SetValidArea(sfa::Polygon validarea) 
    {
        _validArea = validarea;        
        sfa::Geometry *envelope = _validArea.getEnvelope();
        sfa::LineString *ls = dynamic_cast<sfa::LineString *>(envelope);
        if(ls && ls->getNumPoints()>1)
        {
            sfa::Point *p1 = ls->getPointN(0);
            sfa::Point *p2 = ls->getPointN(1);
            double shape_top = std::max<double>(p1->Y(),p2->Y());
            double shape_bottom = std::min<double>(p1->Y(),p2->Y());;
            double shape_left = std::min<double>(p1->X(),p2->X());;
            double shape_right = std::max<double>(p1->X(),p2->X());;
            
            Quad validAreaQuad;

            validAreaQuad.ul.setX(shape_left);
            validAreaQuad.ul.setY(shape_top);

            validAreaQuad.ll.setX(shape_left);
            validAreaQuad.ll.setY(shape_bottom);

            validAreaQuad.ur.setX(shape_right);
            validAreaQuad.ur.setY(shape_top);

            validAreaQuad.lr.setX(shape_right);
            validAreaQuad.lr.setY(shape_bottom);

            Quad clippedCoverage; 
            Quad validAreaLocalQuad;

            validAreaLocalQuad = validAreaQuad.Transform(GetDestToFileTransformer());
            _localCoverage.IntersectWith(validAreaLocalQuad,clippedCoverage);
            _localCoverage = clippedCoverage;
            _coverage = _localCoverage.Transform(GetFileToDestTransformer());
        }
        if(envelope)
            delete envelope;
    }



    bool GDALRasterFile::GetDestMBR(double &top, double &bottom, double &left, double &right)
    {
        if(!_validArea.isEmpty())
        {
            // the _coverage.ToMBR call doesn't overwrite values if they are larger than the existing, so
            // we'll use local values and expand the MBR to have the same behavior.
            sfa::Geometry *envelope = _validArea.getEnvelope();
            sfa::LineString *ls = dynamic_cast<sfa::LineString *>(envelope);
            if(ls && ls->getNumPoints()>1)
            {
                sfa::Point *p1 = ls->getPointN(0);
                sfa::Point *p2 = ls->getPointN(1);
                double shape_top = std::max<double>(p1->Y(),p2->Y());
                double shape_bottom = std::min<double>(p1->Y(),p2->Y());
                double shape_left = std::min<double>(p1->X(),p2->X());
                double shape_right = std::max<double>(p1->X(),p2->X());
                top = std::max<double>(top,shape_top);
                right = std::max<double>(right,shape_right);
                bottom = std::min<double>(bottom,shape_bottom);
                left = std::min<double>(left,shape_left);
            }
            if(envelope)
                delete envelope;
        }
        else
        {
            _coverage.ToMBR(left,right,top,bottom);
        }
        return true;
    }

    bool GDALReader::GetDestMBR(double &top, double &bottom, double &left, double &right)
    {
        // Initialize the MBR, then let each file expand as needed.
        top = -DBL_MAX;
        bottom = DBL_MAX;
        left = DBL_MAX;
        right = -DBL_MAX;
        GDALRasterFileList::iterator iter = _files.begin();
        while(iter!=_files.end())
        {
            GDALRasterFilePtr file = *iter++;
            // Get overlapping pixels and add the results in
            file->GetDestMBR(top,bottom,left,right);
        }
        return true;
    }

    bool GDALReader::ReadCoverageShapes(std::string filename)
    {
        bool ret = false;
        sfa::File *file = sfa::FileRegistry::instance()->getFile(filename);
        sfa::LayerList layers = file->getLayers();
        for(size_t i = 0; i < layers.size(); ++i)
        {
            sfa::Layer *layer = layers.at(i);
            while(sfa::Feature *feature = layer->getNextFeature())
            {
                if(feature && feature->geometry)
                {
                    if(feature->getWKBGeometryType()==sfa::wkbPolygon || 
                        feature->getWKBGeometryType()==sfa::wkbPolygonZ || 
                        feature->getWKBGeometryType()==sfa::wkbPolygonM || 
                        feature->getWKBGeometryType()==sfa::wkbPolygonZM)
                    coverageShapes.push_back(*feature);
                }
            }
        }

        log << "Read " << (int)coverageShapes.size() << " shapes from " << filename << log.endl;
        return ret;
    }

    bool CachedRasterBlock::UnloadBlock()
    {
        m_ready = false;
        age = 0;
        delete[] r;
        delete[] g;
        delete[] b;
        r = g = b = NULL;
        m_file->DeReference();
        return true;
    }


    bool LoadProjDLL() 
    { 
        //if (g_ProjDLL!=NULL)
        {
            return false;
        }
#if 0
        g_ProjDLL = LoadLibrary("proj470.dll"); 
        if (g_ProjDLL==NULL)
        {
            return false;
        }

        pj_init = (projPJ (*)(int, char**)) GetProcAddress( g_ProjDLL, "pj_init"); 
        if( pj_init == NULL )
            return false; 

        pj_init_plus = (projPJ (*)(const char *))GetProcAddress(g_ProjDLL, "pj_init_plus"); 
        pj_fwd = (projUV (*)(projUV,projPJ))GetProcAddress(g_ProjDLL, "pj_fwd");
        pj_inv = (projUV (*)(projUV,projPJ))GetProcAddress(g_ProjDLL, "pj_inv"); 
        pj_free = (void (*)(projPJ))GetProcAddress(g_ProjDLL, "pj_free"); 
        pj_transform = (int (*)(projPJ,projPJ,long,int,double*,double*,double*))GetProcAddress(g_ProjDLL, "pj_transform");
        pj_get_errno_ref = (int *(*)(void))GetProcAddress(g_ProjDLL, "pj_get_errno_ref");
        pj_strerrno = (char *(*)(int))GetProcAddress(g_ProjDLL,"pj_strerrno");
        pj_get_def = (char *(*)(projPJ,int))GetProcAddress(g_ProjDLL, "pj_get_def"); 
        pj_dalloc = (void (*)(void*))GetProcAddress( g_ProjDLL,"pj_dalloc");
#endif
        return true; 
    }

    projPJ (*pj_init_plus)(const char *) = NULL; 
    projPJ (*pj_init)(int, char**) = NULL; 
    projUV (*pj_fwd)(projUV, projPJ) = NULL; 
    projUV (*pj_inv)(projUV, projPJ) = NULL; 
    void (*pj_free)(projPJ) = NULL; 
    int  (*pj_transform)(projPJ, projPJ, long, int, 
                                        double *, double *, double * ) = NULL; 
    int *(*pj_get_errno_ref)(void) = NULL; 
    char *(*pj_strerrno)(int) = NULL; 
    char *(*pj_get_def)(projPJ,int) = NULL; 
    void (*pj_dalloc)(void *) = NULL; 

}

ccl::mutex g_GDALProtMutex;
