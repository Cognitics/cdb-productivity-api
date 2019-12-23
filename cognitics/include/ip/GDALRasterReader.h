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

#ifndef WIN32
#include <pthread.h>
#endif

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#include <float.h>
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <sfa/sfa.h>
#include <ccl/mutex.h>
#include <ccl/cstdint.h>
#include <ccl/FileInfo.h>
#include <boost/shared_ptr.hpp>

typedef unsigned char u_char;
typedef unsigned int u_int;

extern int g_debugMode;


#define floatround(a) (floor(a+0.5))
#define RAD_TO_DEG      57.29577951308232
#define DEG_TO_RAD      .0174532925199432958
extern bool g_UseProjDLL;

namespace gdalsampler
{

    class GeoExtents
    {
    public:
        double north;
        double east;
        double south;
        double west;

        int width;
        int height;

        double ns_pix_per_degree;
        double ew_pix_per_degree;

        std::string filename;

        GeoExtents()
        {
            north = -90.0f;
            south = 90.0f;
            west = 180.0f;
            east = -180.0f;
            height = 0;
            width = 0;
            filename = "";
            ns_pix_per_degree = 0;
            ew_pix_per_degree = 0;
        }

        bool GeoToPixel(const double geolat,const double geolon, double &ypix, double &xpix) const
        {
            double ewres = (east - west)/width;
            double nsres = (north - south)/height;
            double lonofs = geolon - west;
            double latofs = geolat - south;
            xpix = (lonofs / ewres);
            ypix = (latofs / nsres);

            return true;
        }

        bool GeoToPixel(const sfa::Point geoPt, sfa::Point &pixPt) const
        {
            double ewres = (east - west)/width;
            double nsres = (north - south)/height;
            double lonofs = geoPt.X() - west;
            double latofs = geoPt.Y() - south;
            pixPt.setX(lonofs / ewres);
            pixPt.setY(latofs / nsres);
            return true;
        }

        bool IsValid()
        {
            return((north>south)&&(east>west));
        }
    };

    class GeographicTile : public GeoExtents
    {
        
    public:
        GeographicTile() : GeoExtents()
        {
            used = false;
        }
        //std::vector<GeoExtents> children;
        bool used;
        int multiplier;

    };

    #define projPJ void *
    typedef struct { double u, v; } projUV; 

    extern projPJ (*pj_init_plus)(const char *); 
    extern projPJ (*pj_init)(int, char**); 
    extern projUV (*pj_fwd)(projUV, projPJ); 
    extern projUV (*pj_inv)(projUV, projPJ); 
    extern void (*pj_free)(projPJ); 
    extern int  (*pj_transform)(projPJ, projPJ, long, int, 
                                        double *, double *, double * ); 
    extern int *(*pj_get_errno_ref)(void); 
    extern char *(*pj_strerrno)(int); 
    extern char *(*pj_get_def)(projPJ,int); 
    extern void (*pj_dalloc)(void *); 

    bool LoadProjDLL();


    /**
     * Simple coordinate transformer so we can use proj.4 directly instead of
     * going through gdal. Most of this was borrowed from gdal.
     **/
    class CoordinateTransformer
    {
        OGRSpatialReference *m_gdalSource;
        OGRSpatialReference *m_gdalDest;
        char *m_sourceDescr;
        char *m_destDescr;
        void *m_sourceHandle;
        void *m_destHandle;
        OGRCoordinateTransformation *gdalTransformer;

        
        int bSourceLatLong;
        double dfSourceToRadians;
        double dfSourceFromRadians;
        int bSourceWrap;
        double dfSourceWrapLong;
        int bTargetLatLong;
        double dfTargetToRadians;
        double dfTargetFromRadians;
        int bTargetWrap;
        double dfTargetWrapLong;

        
    public:

        CoordinateTransformer(const OGRSpatialReference *src,const OGRSpatialReference *dest)
        {
            m_gdalSource = src->Clone();
            m_gdalDest = dest->Clone();
            gdalTransformer = OGRCreateCoordinateTransformation( m_gdalSource, m_gdalDest );            
        }

        ~CoordinateTransformer()
        {
            
        }

        bool TransformForward(int count, double *x, double *y, double *z=0)
        {
            if(!gdalTransformer)
                return false;
            return gdalTransformer->TransformEx(count,x,y,z)!=0;
        }

    };

    

    typedef std::vector<sfa::Point> PointList;


    class Quad
    {
        inline void ExpandExtents(const sfa::Point &pt,double &left, double &right, double &top, double &bottom)
        {
            if(left > pt.X())
                left = pt.X();
            if(right < pt.X())
                right = pt.X();
            if(top < pt.Y())
                top = pt.Y();
            if(bottom > pt.Y())
                bottom = pt.Y();
        }

    public:
        sfa::Point ul;
        sfa::Point ur;
        sfa::Point lr;
        sfa::Point ll;

        void ToMBR(double &left, double &right, double &top, double &bottom)
        {
            ExpandExtents(ul,left,right,top,bottom);
            ExpandExtents(ur,left,right,top,bottom);
            ExpandExtents(lr,left,right,top,bottom);
            ExpandExtents(ll,left,right,top,bottom);
        }

        double CalculateArea()
        {
            double left,right,top,bottom;
            top = -DBL_MAX;
            bottom = DBL_MAX;
            left = DBL_MAX;
            right = -DBL_MAX;
            ToMBR(left,right,top,bottom);
            double width = right - left;
            double height = top - bottom;
            return width * height;
        }

        Quad Transform(CoordinateTransformer *transformer) const 
        {
            Quad ret;
            // The call to TransformEx is very costly because GDAL protects it with
            // a Mutex, so we'll do all 4 transforms at once. Transform is even worse
            // because it also allocates space on the heap for 4 bytes (the return code from proj.4)
            double x[4];
            double y[4];
            x[0] = ul.X();
            y[0] = ul.Y();
            x[1] = ur.X();
            y[1] = ur.Y();
            x[2] = lr.X();
            y[2] = lr.Y();
            x[3] = ll.X();
            y[3] = ll.Y();
            transformer->TransformForward(4,x,y,0);
            ret.ul.setX(x[0]);
            ret.ul.setY(y[0]);
            ret.ur.setX(x[1]);
            ret.ur.setY(y[1]);
            ret.lr.setX(x[2]);
            ret.lr.setY(y[2]);
            ret.ll.setX(x[3]);
            ret.ll.setY(y[3]);
            
            return ret;
        }
        

        Quad Transform(OGRCoordinateTransformation *transformer) const 
        {
            Quad ret;

            // The call to TransformEx is very costly because GDAL protects it with
            // a Mutex, so we'll do all 4 transforms at once. Transform is even worse
            // because it also allocates space on the heap for 4 bytes (the return code from proj.4)
            double x[4];
            double y[4];
            x[0] = ul.X();
            y[0] = ul.Y();
            x[1] = ur.X();
            y[1] = ur.Y();
            x[2] = lr.X();
            y[2] = lr.Y();
            x[3] = ll.X();
            y[3] = ll.Y();
            transformer->TransformEx(4,x,y,0);
            ret.ul.setX(x[0]);
            ret.ul.setY(y[0]);
            ret.ur.setX(x[1]);
            ret.ur.setY(y[1]);
            ret.lr.setX(x[2]);
            ret.lr.setY(y[2]);
            ret.ll.setX(x[3]);
            ret.ll.setY(y[3]);
            
            return ret;
        }

        bool IntersectWith(Quad &other, Quad &intersect)
        {
            Quad test;
            bool ret = false;
            double our_top = -DBL_MAX;
            double our_bottom = DBL_MAX;
            double our_left = DBL_MAX;
            double our_right = -DBL_MAX;
            ToMBR(our_left,our_right,our_top,our_bottom);

            double other_top = -DBL_MAX;
            double other_bottom = DBL_MAX;
            double other_left = DBL_MAX;
            double other_right = -DBL_MAX;
            other.ToMBR(other_left,other_right,other_top,other_bottom);

            if((our_left < other_right) &&
                (our_right > other_left) &&
                (our_top > other_bottom) &&
                (our_bottom < other_top))
            {

                test.ul.setX(std::max<double>(ul.X(),other.ul.X()));
                test.ul.setY(std::min<double>(ul.Y(),other.ul.Y()));

                test.ur.setX(std::min<double>(ur.X(),other.ur.X()));
                test.ur.setY(std::min<double>(ur.Y(),other.ur.Y()));

                test.lr.setX(std::min<double>(lr.X(),other.lr.X()));
                test.lr.setY(std::max<double>(lr.Y(),other.lr.Y()));

                test.ll.setX(std::max<double>(ll.X(),other.ll.X()));
                test.ll.setY(std::max<double>(ll.Y(),other.ll.Y()));

                test.ul.setX(std::min<double>(test.ul.X(),test.ur.X()));
                test.ll.setX(std::min<double>(test.ll.X(),test.lr.X()));
                test.ll.setY(std::min<double>(test.ll.Y(),test.ul.Y()));
                test.lr.setY(std::min<double>(test.lr.Y(),test.ur.Y()));

                if( (test.ll.Y() <= test.ul.Y()) &&
                    (test.ll.X() <= test.lr.X()))
                {
                    ret = true;
                    intersect = test;
                }
            }

            return ret;
        }
    };

    
    class OverlappingPixel
    {
    public:
        OverlappingPixel()
        {
            Reset();
        }
        Quad coveredArea;
        Quad queriedArea;
        u_char r;
        u_char g;
        u_char b;
        double coverageRatio;    
        bool isValid;

        void Reset()
        {
            r = g = b = 0;
            coverageRatio = 0;
            isValid = false;
        }
    };    
    //typedef std::vector<OverlappingPixel> OverlappingPixels;

    class OverlappingPixels
    {
        std::vector<OverlappingPixel> _pixels;
        unsigned int _numPixels;// we don't use _pixels.size because we are going to reuse the vector

    public:
        OverlappingPixels()
        {
            _pixels.reserve(16);
        }

        unsigned int GetNumPixels() { return _numPixels; }
        void Reset() { _numPixels = 0; }

        OverlappingPixel &AllocatePixel()
        {
            if(_numPixels >= _pixels.size())
            {
                for(int i=0;i<4;i++)//always add 4 at a time (fewer reallocations)
                {
                    OverlappingPixel pix;
                    _pixels.push_back(pix);
                }
            }
            return _pixels.at(_numPixels++);
        }

        void DeallocateLastPixel()
        {
            if(_numPixels>0)
            {
                _numPixels--;
                _pixels[_numPixels].Reset();
            }
        }

        OverlappingPixel &GetPixelAt(u_int idx)
        {
            if(idx < _pixels.size())
            {
                return _pixels.at(idx);
            }
            throw std::runtime_error("Pixel index out of bounds.");
        }
    };

    
    class GDALRasterFile;
    class CachedRasterBlock
    {

    public:

        int xoffset; //!< The x offset of this block relative to the source file.
        int yoffset; //!< The x offset of this block relative to the source file.
        int xsize; //!< The x size of the raster block in pixels.
        int ysize; //!< The x size of the raster block in pixels.
        u_char *r; //!< An array of red pixels for this block.
        u_char *g; //!< An array of green pixels for this block. 
        u_char *b; //!< An array of blue pixels for this block. 
        float *elev { nullptr };
        int age; //!<  Each time this isn't the block we're looking for, we increment this. Then we can delete the oldest to make room in the cache.        
        GDALRasterFile *m_file;
        std::string m_filename;
        bool m_ready;
        sfa::Feature m_validArea;


        CachedRasterBlock(int xoffset, int yoffset, int width, int height, std::string filename, GDALRasterFile *file, bool lazyAlloc=false);
        ~CachedRasterBlock();

        bool ReadBlock();
        bool UnloadBlock();

        bool IsReady()
        {
            return m_ready;
        }

        bool IsSameBlock(CachedRasterBlock *other)
        {
            if((other->m_filename == this->m_filename) &&
               (other->xoffset == this->xoffset) &&
               (other->yoffset == this->yoffset) &&
               (other->xsize == this->xsize) &&
               (other->ysize == this->ysize))
            {
                return true;
            }
            return false;
        }

        Quad GetDestCoverage();
        bool GetInterleavedPixels(u_char *buf);
    };
    typedef boost::shared_ptr<CachedRasterBlock> CachedRasterBlockPtr;
    typedef std::list<CachedRasterBlockPtr> CachedRasterBlockList;


    inline void DumpQuad(const Quad &quad, std::string text)
    {
        std::cout.precision(12);
        std::cout << "Dump of quad " << text.c_str() << std::endl;
        std::cout << "\tUL: " << quad.ul.X() << " / " << quad.ul.Y() << std::endl;
        std::cout << "\tUR: " << quad.ur.X() << " / " << quad.ur.Y() << std::endl;
        std::cout << "\tLR: " << quad.lr.X() << " / " << quad.lr.Y() << std::endl;
        std::cout << "\tLL: " << quad.ll.X() << " / " << quad.ll.Y() << std::endl;
    }

    /**
     * Template to automate the use of thread local storage. 
     **/
#ifdef WIN32
    template<typename T>
    class tl_ptr {
    protected:
        DWORD index;
    public:
        tl_ptr(void) : index(TlsAlloc()){
            assert(index != TLS_OUT_OF_INDEXES);
            set(NULL);
        }
        void set(T* ptr){
            TlsSetValue(index,(LPVOID) ptr);
        }
        T* get(void)const {
            return (T*) TlsGetValue(index);
        }
        tl_ptr& operator=(T* ptr){
            set(ptr);
            return *this;
        }
        tl_ptr& operator=(const tl_ptr& other){
            set(other.get());
            return *this;
        }
        T& operator*(void)const{
            return *get();
        }
        T* operator->(void)const{
            return get();
        }
        ~tl_ptr(){
            TlsFree(index);
        }
    };
#else
    template<typename T>
    class tl_ptr {
 protected:
        pthread_key_t index;
        
 public:
        tl_ptr(void) 
          {
            pthread_key_create(&index, NULL);
            set(NULL);
          }
        void set(T* ptr){
          pthread_setspecific(index,(void *) ptr);
        }
        T* get(void)const {
          return (T*) pthread_getspecific(index);
        }
        tl_ptr& operator=(T* ptr){
          set(ptr);
          return *this;
        }
        tl_ptr& operator=(const tl_ptr& other){
          set(other.get());
          return *this;
        }
        T& operator*(void)const{
          return *get();
        }
        T* operator->(void)const{
          return get();
        }
        ~tl_ptr(){
          pthread_key_delete(index);
        }
    };
#endif


    class GDALRasterFile;
    class CacheManager
    {        
        ccl::mutex _cacheLock;
        static const int MAX_CACHE_ENTRIES;
        static const int CACHE_BLOCK_HEIGHT;
        static const int CACHE_BLOCK_WIDTH;
        CachedRasterBlockList _blockCache; // This block cache uses the pointers to manage the cache

        static tl_ptr<CacheManager> theInstance;
        //static CacheManager *theInstance;

        static const int CACHE_MAX_MEMORY = 100 * 1024 * 1024;
        CachedRasterBlockList _altBlockCache; // this block cache pages in and out entire blocks but keeps the pointer around.

        bool MakeCacheRoom(int size);
        int GetMemoryInUse()
        {
            int memory_in_use = 0;
            CachedRasterBlockList::iterator block_iter = _altBlockCache.begin();
            while(block_iter!=_altBlockCache.end())
            {
                CachedRasterBlockPtr ptr = *block_iter++;
                if(ptr->IsReady())
                {
                    memory_in_use += (ptr->xsize * ptr->ysize * 3);
                }
            }
            return memory_in_use;
        }
    public:
        CacheManager();

        void Unload()
        {
            CachedRasterBlockList::iterator block_iter = _blockCache.begin();
            while(block_iter!=_blockCache.end())
            {
                *block_iter = CachedRasterBlockPtr();
                block_iter++;
            }
            _blockCache.clear();
            block_iter = _altBlockCache.begin();
            while(block_iter!=_altBlockCache.end())
            {
                CachedRasterBlockPtr block = *block_iter++;
                block->UnloadBlock();
                
            }
            _altBlockCache.clear();
        }
        
        bool PageBlock(CachedRasterBlockPtr &block);
        static CacheManager *getInstance();
        bool GetPixel(GDALRasterFile *file, int row, int col, OverlappingPixel &pixel);
    };

    
    class TransformSet
    {
    public:
        OGRSpatialReference *_fileSRS; // SRS for this file
        OGRSpatialReference *_destSRS; // SRS for this file
        OGRCoordinateTransformation *_localToDesttransformer;
        OGRCoordinateTransformation *_destToLocaltransformer;
        CoordinateTransformer *_fileToDestTransformer;
        CoordinateTransformer *_destToFileTransformer;

    };

    
    class TransformCache
    {
        ccl::ObjLog log;
        typedef std::pair<std::string, std::string> sting_pair_t;
        std::map<sting_pair_t, TransformSet *> transform_map;
        static TransformCache *instance;
        ccl::mutex cache_mutex;
    public:
        TransformSet *getTransforms(std::string filewkt, std::string destwkt);
        static TransformCache *getInstance();
        ~TransformCache();
    };


    class GDALRasterFile
    {
        ccl::ObjLog log;
        int referenceCount;
        ccl::mutex refMutex;
        GDALDataset *poDataset;
        TransformSet *xformset;

        std::string _filename;
        sfa::Polygon _validArea;
        int _fileWidth;
        int _fileHeight;
        double _origEWConst;
        double _origNSConst;
        double _determinate;
        double adfTransform4;
        double adfTransform2;
        double _top;
        double _bottom;
        double _left;
        double _right;
        Quad _coverage; // poly in dest coordinates that represents the area of valid pixels for this file.
        Quad _localCoverage;
        bool _isValid;
        bool OpenGDALFile(OGRSpatialReference &destsrs, std::string filename);
        ccl::int64_t _age;
        double resolution;// area (x resolution * y resolution)
        double x_resolution;
        double y_resolution;
        int references;
        bool isRPF;
        inline void ExpandExtents(int rowval, int colval, int &minrow, int &maxrow, int &mincol, int &maxcol)
        {
            if(minrow > rowval)
                minrow = std::max<int>(0,rowval);
            if(maxrow < rowval)
                maxrow = std::min<int>(_fileHeight-1,rowval);

            if(mincol > colval)
                mincol = std::max<int>(0,colval);
            if(maxcol < colval)
                maxcol = std::min<int>(_fileWidth-1,colval);
        }


    public:
        bool pixel_is_point { false };

        CoordinateTransformer *GetFileToDestTransformer()
        {
            if (xformset)
                return xformset->_fileToDestTransformer;
            else
                return NULL;
        }

        CoordinateTransformer *GetDestToFileTransformer()
        {        
            if (xformset)
                return xformset->_destToFileTransformer;
            else
                return NULL;
        }
        void SetValidArea(sfa::Polygon validarea);
        sfa::Polygon GetValidArea()
        {
            return _validArea;
        }
        bool IsValid() { return _isValid; }
        GDALRasterFile(OGRSpatialReference dest, std::string filename);
        ~GDALRasterFile();


        GDALDataset *GetDataset()
        {
            if (!poDataset)
            {
                std::string tableName;
                std::string filename = _filename;
                ccl::GetFilenameAndTable(_filename, filename, tableName);
                if (!tableName.empty())
                {
                    char** papszOptions = NULL;
                    std::string tableStr = "TABLE=" + tableName;
                    papszOptions = CSLAddString(papszOptions, tableStr.c_str());

                    poDataset = (GDALDataset*)GDALOpenEx(filename.c_str(),
                        GDAL_OF_READONLY, NULL, papszOptions, NULL);
                }
                else
                {
                    poDataset = (GDALDataset *)GDALOpen(_filename.c_str(), GA_ReadOnly);
                }
            }
            return poDataset;
        }

        GDALDataset *Reference()
        {
            GDALDataset *ret = NULL;
            refMutex.lock();
            if (poDataset == NULL)
            {
                ret = GetDataset();
            }
            else
            {
                ret = poDataset;
            }
            referenceCount++;            
            refMutex.unlock();
            return ret;
        }
        void DeReference()
        {
            refMutex.lock();
            if (referenceCount == 0)
            {
                log << ccl::LERR << "Error: negative reference count reached in GDALRasterFile." << log.endl;
                //exit(1);
            }
            referenceCount--;
            if (referenceCount == 0)
            {
                Close();
            }
            refMutex.unlock();
        }

        void Close()
        {
            GDALClose(poDataset);
            poDataset = NULL;
        }

        int GetWidth()
        {
            return _fileWidth;
        }

        int GetHeight()
        {
            return _fileHeight;
        }

        ccl::int64_t GetAge()
        {
            return _age;
        }

        std::string GetFilename()
        {
            return _filename;
        }
        double GetResolution()
        {
            return resolution;
        }

        double GetXResolution()
        {
            return x_resolution;
        }

        double GetYResolution()
        {
            return y_resolution;
        }

        void SetGeoCoverage(const Quad &coverage);
        // return a list of all the pixels that overlap with the specified aoi
        bool GetOverlappingPixels(Quad aoi, OverlappingPixels &pixels);

        bool GetOverlappingBlocks(Quad aoi, CachedRasterBlockList &blocks);

        bool GetPixel(int row, int col, OverlappingPixel &pixel);

        bool GetDestMBR(double &top, double &bottom, double &left, double &right);

        inline void PixelToLocalPoint(int pixel_row, int pixel_col, double &local_row, double &local_col)
        {
            local_col = _left + pixel_col * _origEWConst + pixel_row * adfTransform2;
            local_row = _top + pixel_col * adfTransform4 + pixel_row * _origNSConst;
        }

        inline void PixelToLocalPoint(int pixel_row, int pixel_col, sfa::Point &localPt)
        {
            localPt.setX(_left + pixel_col * _origEWConst + pixel_row * adfTransform2);
            localPt.setY(_top + pixel_col * adfTransform4 + pixel_row * _origNSConst);
        }

        inline void PixelToLocalPoint(double pixel_row, double pixel_col, sfa::Point &localPt)
        {
            localPt.setX(_left + pixel_col * _origEWConst + pixel_row * adfTransform2);
            localPt.setY(_top + pixel_col * adfTransform4 + pixel_row * _origNSConst);
        }


        inline void LocalToPixelPoint(int &pixel_row, int &pixel_col, double local_row, double local_col)
        {
            // My brain exploded when I wrote this. I derived it, so maybe there's a faster way?
            pixel_col = (int)floatround(-((_left - local_col) * _origNSConst - adfTransform2 * _top + adfTransform2 * local_row)/_determinate);
            pixel_row = (int)floatround(-((_top - local_row) * _origEWConst - adfTransform4 * _left + adfTransform4 * local_col)/_determinate);
        }
        //static DWORD TLIndexFileToDest;
        //static DWORD TLIndexDestToFile;
    };

    typedef boost::shared_ptr<GDALRasterFile> GDALRasterFilePtr;
    typedef std::vector<GDALRasterFilePtr> GDALRasterFileList;

    // : public std::binary_function<GDALRasterFilePtr, GDALRasterFilePtr, bool>
    struct CompareResolution
    {
        bool operator()(GDALRasterFilePtr x, GDALRasterFilePtr y) const
        {   
            return y->GetResolution() < x->GetResolution();
        }
    };


    class GDALReader
    {
        ccl::ObjLog log;
        OGRSpatialReference _destSRS;
        GDALRasterFileList _files;
        ccl::mutex sortLock;

        // returns the first areal that contains the specified point.
        sfa::Polygon GetShapeForFileOrPoint(std::string filename,double x, double y);
        std::vector<sfa::Feature> coverageShapes;
    public:

        GDALRasterFileList &GetFiles()
        {
            // Sort files by resolution
            CompareResolution cmp;
            sortLock.lock();
            if (okToSort)
                std::sort(_files.begin(), _files.end(), cmp);

            okToSort = false;
            sortLock.unlock();
            return _files;
        }

        bool ReadCoverageShapes(std::string filename);
        // make sure to call this before adding files.
        void SetDestSRS(OGRSpatialReference srs) { _destSRS = srs; }
        bool AddFile(std::string filename);
        // return a list of all the pixels that overlap with the specified aoi
        bool GetOverlappingPixels(Quad aoi, OverlappingPixels &pixels);

        bool GetOverlappingBlocks(Quad aoi, CachedRasterBlockList &blocks);

        /**
         * Check that the specified point is inside the valid extents of
         * at least one file.
         */
        bool IsPointCovered(double x, double y);
        bool okToSort;


        GDALReader();
        ~GDALReader();

        bool GetDestMBR(double &top, double &bottom, double &left, double &right);
        // free up memory -- this resets the reader to have no files!
        void Unload()
        {
            CacheManager::getInstance()->Unload();
            coverageShapes.clear();
            _files.clear();
        }

        bool IsBlockTouchingSourceData(GeoExtents block)
        {
            bool ret = false;
            GDALRasterFileList::iterator iter = _files.begin();
            while(iter!=_files.end())
            {
                GDALRasterFilePtr file = *iter++;
                // Get overlapping pixels and add the results in
                double top = -DBL_MAX;
                double bottom = DBL_MAX;
                double left = DBL_MAX;
                double right = -DBL_MAX;
                file->GetDestMBR(top,bottom,left,right);
                if((block.north > bottom) &&
                    (block.south < top) &&
                    (block.east > left) &&
                    (block.west < right))
                {
                    return true;
                }
            }

            return ret;
        }

    };

    bool GetMBR(const sfa::PointList &poly, double &left, double &right, double &top, double &bottom);

}

extern ccl::mutex g_GDALProtMutex;
