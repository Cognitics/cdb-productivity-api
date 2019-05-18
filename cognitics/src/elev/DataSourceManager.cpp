/****************************************************************************
Copyright 2018 Cognitics, Inc.
****************************************************************************/

//#pragma optimize("", off)

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_spatialref.h>
#pragma warning ( pop )

#include "elev/DataSourceManager.h"
#include "elev/Cache.h"

#include <sstream>

using namespace ccl;
namespace elev
{
    ElevationBSP::~ElevationBSP(void)
    {
        if(minChild)
            delete minChild;
        if(maxChild)
            delete maxChild;
    }

    ElevationBSP::ElevationBSP(size_t depth, bool vertical) : generated(false), depth(depth), vertical(vertical), split(0.0f), minChild(NULL), maxChild(NULL)
    {
    }

    bool ElevationBSP::generate(void)
    {
        if(minChild)
            return false;

        generated = true;

        if(depth > 20)
            return true;

        if(sources.size() < 4)
            return true;

        // average the centroids to determine the split
        double sum = 0.0f;
        for(std::list<DataSource_Raster *>::iterator it = sources.begin(), end = sources.end(); it != end; ++it)
        {
            DataSource_Raster *ds = *it;
            sum += vertical ? (ds->geo_bound_x_low + ((ds->geo_bound_x_high - ds->geo_bound_x_low) / 2)) : (ds->geo_bound_y_high + ((ds->geo_bound_y_low - ds->geo_bound_y_high) / 2));
        }
        split = sum / sources.size();

        // create children
        minChild = new ElevationBSP(depth + 1, !vertical);
        maxChild = new ElevationBSP(depth + 1, !vertical);

        // identify where each source goes
        std::list<DataSource_Raster *> saved_sources;
        for(std::list<DataSource_Raster *>::iterator it = sources.begin(), end = sources.end(); it != end; ++it)
        {
            DataSource_Raster *ds = *it;
            double low = vertical ? ds->geo_bound_x_low : ds->geo_bound_y_high;
            double high = vertical ? ds->geo_bound_x_high : ds->geo_bound_y_low;
            if(low > split)
                maxChild->sources.push_back(ds);
            else if(high < split)
                minChild->sources.push_back(ds);
            else
                saved_sources.push_back(ds);
        }

        // update sources and generate children
        sources = saved_sources;
        minChild->generate();
        maxChild->generate();

        return true;
    }

    void ElevationBSP::getSourcesForPoint(double x, double y, std::list<DataSource_Raster *> &result)
    {
        for(std::list<DataSource_Raster *>::iterator it = sources.begin(), end = sources.end(); it != end; ++it)
        {
            DataSource_Raster *ds = *it;
            double epsilon = abs(vertical ? ds->spacing_x : ds->spacing_y);
            double low = vertical ? ds->geo_bound_x_low : ds->geo_bound_y_low;
            double high = vertical ? ds->geo_bound_x_high : ds->geo_bound_y_high;
            if(low > high)
                std::swap(low, high);
            double v = vertical ? x : y;
            if(low - epsilon > v)
                continue;
            if(high + epsilon < v)
                continue;
            result.push_back(ds);
        }
        if(!minChild)
            return;
        double v = vertical ? x : y;
        if(v <= split)
            minChild->getSourcesForPoint(x, y, result);
        if(v >= split)
            maxChild->getSourcesForPoint(x, y, result);
    }

    DataSourceManager::DataSourceManager(unsigned int cachesize) : generate_debug_features(false)
    {
        log.init("DataSourceManager", this);
#ifdef DEBUG
        log << ccl::LDEBUG << "DataSourceManager(" << cachesize << ")" << log.endl;
#endif
        this->cache = new Cache(cachesize);
        this->reftype = "WGS84";
        GDALAllRegister();
        OGRRegisterAll();
    }

    DataSourceManager::~DataSourceManager()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "~DataSourceManager()" << log.endl;
#endif
        delete cache;
        while(!sources.empty())
        {
            DataSource *ds = sources.back();
            sources.pop_back();
            delete ds;
        }
    }

    void DataSourceManager::SetReferenceType(std::string reftype)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "SetReferenceType(" << reftype << ")" << log.endl;
#endif
        this->reftype = reftype;
        size_t count = sources.size();
        for(size_t i = 0; i < count; i++)
        {
            DataSource *ds = sources.at(i);
            ds->SetReferenceType(this->reftype);
        }
    }

    bool DataSourceManager::AddFile_Raster_GDAL(std::string filename)
    {
        return AddFile_Raster_GDAL_With_Priority(filename, 0);
    }

    bool DataSourceManager::AddFile_Raster_GDAL_With_Priority(std::string filename, int priority)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "AddFile_Raster_GDAL(" << filename << ")" << log.endl;
#endif
        DataSource_Raster_GDAL *ds = new DataSource_Raster_GDAL(filename, this->cache);
        if(!ds) return false;
        if(!ds->IsValid()) 
        { 
            delete ds; 
            return false; 
        }
        ds->SetReferenceType(this->reftype);
        ds->priority = priority;
        sources.push_back(ds);
        return true;
    }

    void DataSourceManager::AddDirectory_Raster_GDAL(const std::string &path, const std::string &mask)
    {
        AddDirectory_Raster_GDAL_With_Priority(path, 0, mask);
    }

    void DataSourceManager::AddDirectory_Raster_GDAL_With_Priority(const std::string &path, int priority, const std::string &mask)
    {
        if(mask == "*.*")
        {
            AddDirectory_Raster_GDAL_With_Priority(path, priority, "*.tif");
            AddDirectory_Raster_GDAL_With_Priority(path, priority, "*.dt0");
            AddDirectory_Raster_GDAL_With_Priority(path, priority, "*.dt1");
            AddDirectory_Raster_GDAL_With_Priority(path, priority, "*.dt2");
            // add other types here
            return;
        }
        std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles(path, mask);
        for(size_t i = 0, c = files.size(); i < c; ++i)
        {
            //log << ccl::LINFO << "Adding elevation file " << files[i].getFileName() << " with priority " << priority << log.endl;
            AddFile_Raster_GDAL_With_Priority(files[i].getFileName(), priority);
        }
    }

    int DataSourceManager::GetPostsForPoint(double x, double y, std::vector<DataPost*> &posts)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "GetPostsForPoint(" << x << ", " << y << ", &)" << log.endl;
#endif
        std::vector<DataSource *> search_sources;
        search_sources.reserve(32);
        if(bsp.generated)
        {
            std::list<DataSource_Raster *> bsp_sources;
            bsp.getSourcesForPoint(x, y, bsp_sources);
            search_sources.insert(search_sources.end(), bsp_sources.begin(), bsp_sources.end());
        }
        else
        {
            search_sources = sources;
        }

        int results = 0;
        size_t count = search_sources.size();
        for(size_t i = 0; i < count; i++)
        {
            DataSource_Raster *ds = (DataSource_Raster *)search_sources.at(i);
            if(ds->GetType() == DATASOURCE_TYPE_RASTER)
                results += ds->GetPostsForPoint(x, y, posts);
        }
        return results;
    }

    void DataSourceManager::generateBSP(void)
    {
        for(size_t i = 0, c = sources.size(); i < c; ++i)
            bsp.sources.push_back(dynamic_cast<DataSource_Raster *>(sources.at(i)));
        bsp.generate();
    }

    bool DataSourceManager::getElevationBounds(double &elevation_min, double &elevation_max)
    {
        bool result = false;
        elevation_min = DBL_MAX;
        elevation_max = -DBL_MAX;
        for(size_t i = 0, c = sources.size(); i < c; ++i)
        {
            DataSource_Raster *dsr = dynamic_cast<DataSource_Raster *>(sources[i]);
            if(dsr && dsr->getElevationBounds(elevation_min, elevation_max))
                result = true;
        }
        return result;
    }

   bool DataSourceManager::getExtents(double &north, double &east, double &south, double &west)
   {
      bool result = false;
      north = -90.0;
      south = 90.0;
      east = -180.0;
      west = 180.0;
      for (size_t i = 0, c = sources.size(); i < c; ++i)
      {
         DataSource_Raster *dsr = dynamic_cast<DataSource_Raster *>(sources[i]);
         if (dsr)
         {
            result = true;
            north = std::max<double>(dsr->geo_bound_y_low, north);
            north = std::max<double>(dsr->geo_bound_y_high, north);
            south = std::min<double>(dsr->geo_bound_y_low, south);
            south = std::min<double>(dsr->geo_bound_y_high, south);
            east = std::max<double>(dsr->geo_bound_x_high, east);
            west = std::min<double>(dsr->geo_bound_x_low, west);
         }
      }
      return result;

   }

    std::vector<DataSource *> DataSourceManager::get_sources(void)
    {
        return sources;
    }

}


