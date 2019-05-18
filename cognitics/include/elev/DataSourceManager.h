/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/

#pragma once

#include "DataSource.h"
#include "DataSource_Raster.h"
#include "DataSource_Raster_GDAL.h"
#include "DataPost.h"

#include <vector>
#include <string>

#include <ccl/Log.h>
#include <elev/Cache.h>


namespace elev
{
    class ElevationBSP
    {
    public:
        bool generated;
        size_t depth;
        bool vertical;
        double split;
        ElevationBSP *minChild;
        ElevationBSP *maxChild;
        std::list<DataSource_Raster *> sources;

        ~ElevationBSP(void);
        ElevationBSP(size_t depth = 0, bool vertical = true);

        bool generate(void);

        void getSourcesForPoint(double x, double y, std::list<DataSource_Raster *> &result);
    };


    class DataSourceManager
    {
        ccl::ObjLog log;                            //!< ObjLog instance
        ccl::mutex cacheMutex;
        Cache *cache;                        //!< Cache instance
        std::string reftype;                //!< application reference (projection) type (default is "WGS84")
        std::vector<DataSource *> sources;    //!< DataSource instance list
        ElevationBSP bsp;

    public:
        bool generate_debug_features;

        //! Instantiate a DataSourceManager instance.
        DataSourceManager(unsigned int cachesize);

        //! Destroy the DataSourceManager instance.
        ~DataSourceManager();

        //! Set the application geospatial reference type (e.g. "WGS84").
        /*! This is passed to all current and new DataSource instances for use in transformations. */
        void SetReferenceType(std::string reftype);
        //! Get the application geospatial reference type.
        std::string GetReferenceType() { return this->reftype; }

        //! Add a GDAL raster file.
        bool AddFile_Raster_GDAL(std::string filename);
        bool AddFile_Raster_GDAL_With_Priority(std::string filename, int priority);

        void AddDirectory_Raster_GDAL(const std::string &path, const std::string &mask = "*.*");
        void AddDirectory_Raster_GDAL_With_Priority(const std::string &path, int priority, const std::string &mask = "*.*");

        //! [RASTER] Generate a list of elev::DataPost instances for data posts (points) that are near the provided application coordinates.
        /*!    Returns the number of elev::DataPost objects created. */
        int GetPostsForPoint(double x, double y, std::vector<DataPost*> &posts);

        void generateBSP(void);

        bool getElevationBounds(double &elevation_min, double &elevation_max);

      // Returns the extents of the union of all the source files in geographic coordinates
      bool getExtents(double &north, double &east, double &south, double &west);

        std::vector<DataSource *> get_sources(void);
    };

}
