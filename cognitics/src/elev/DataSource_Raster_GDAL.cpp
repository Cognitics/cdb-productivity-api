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
//! \file DataSource_Raster_GDAL.cpp

//#pragma optimize("", off)

#pragma warning ( disable : 4996 )

#include "elev/DataSource_Raster_GDAL.h"

#include <sstream>
#include <ccl/Log.h>
#include <cts/FlatEarthProjection.h>
#include <elev/Cache.h>
#include <ccl/Profile.h>

using namespace ccl;
namespace elev
{
    DataSource_Raster_GDAL::DataSource_Raster_GDAL(std::string filename, Cache *cache) : DataSource_Raster(filename, cache), value_min(DBL_MAX), value_max(-DBL_MAX)
    {
        log.init("DataSource_Raster_GDAL", this);
#ifdef DEBUG
        log << ccl::LDEBUG << "DataSource_Raster_GDAL(" << filename << ", *)" << log.endl;
#endif
        this->valid = this->Open();
    }

    DataSource_Raster_GDAL::~DataSource_Raster_GDAL()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "~DataSource_Raster_GDAL()" << log.endl;
#endif
        if(gdal_dataset) 
            GDALClose((GDALDatasetH)gdal_dataset);
    }

    bool DataSource_Raster_GDAL::Open()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "Open()" << log.endl;
#endif
        std::string tableName;
        std::string strippedFilename = filename;
        ccl::GetFilenameAndTable(filename, strippedFilename, tableName);
        if (!tableName.empty())
        {
            //This is a geopackage file with the tablename included
            char** papszOptions = NULL;
            std::string tableStr = "TABLE=" + tableName;
            papszOptions = CSLAddString(papszOptions, tableStr.c_str());
            gdal_dataset = (GDALDataset*)GDALOpenEx(strippedFilename.c_str(),
                GDAL_OF_READONLY, NULL, papszOptions, NULL);
        }
        else
        {
            gdal_dataset = (GDALDataset *)GDALOpen(this->filename.c_str(), GA_ReadOnly);
        }
        if(!gdal_dataset)
            return false;

        if(gdal_dataset->GetProjectionRef() == NULL)
        {
            log << ccl::LERR << "Open(): GetProjectionRef() failed." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }
        pixel_is_point = true;
        const char *szaop = gdal_dataset->GetMetadataItem("AREA_OR_POINT");
#ifndef WIN32
        if (szaop && (strcasecmp("POINT", szaop) != 0))
#else
        if (szaop && (strcmpi("POINT", szaop) != 0))
#endif
            pixel_is_point = false;
        width = gdal_dataset->GetRasterXSize();
        height = gdal_dataset->GetRasterYSize();
        depth = gdal_dataset->GetRasterCount();
#ifdef DEBUG
        log << ccl::LDEBUG << "Open(): width = " << width << log.endl;
        log << ccl::LDEBUG << "Open(): height = " << height << log.endl;
        log << ccl::LDEBUG << "Open(): depth = " << depth << log.endl;
#endif

        char *wkt = new char[8192];
        strncpy(wkt, gdal_dataset->GetProjectionRef(), 8192);
        wkt[8191] = 0;
        if(!strlen(wkt))
        {
            std::string prj_filename = filename.substr(0, filename.size() - 3) + "prj";
            FILE *PRJ = fopen(prj_filename.c_str(), "r");
            if(PRJ)
            {
                fgets(wkt, 8192, PRJ);
                fclose(PRJ);
            }
        }
        wkt[8191] = 0;

        if (!strlen(wkt))
        {
            OGRSpatialReference oSRS;
            oSRS.SetWellKnownGeogCS("WGS84");
            char *wktp = NULL;
            oSRS.exportToWkt(&wktp);
            strncpy(wkt, wktp, 8191);
            CPLFree(wktp);
            log << ccl::LERR << "Open(): missing projection in " << filename << " assuming geographic." << log.endl;

        }
        if(!strlen(wkt))
        {
            log << ccl::LERR << "Open(): missing projection wkt." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            delete [] wkt;
            return false;
        }

        char *pwkt = wkt;
        file_srs = new OGRSpatialReference;
        OGRErr err = file_srs->importFromWkt((char **)&wkt);
        delete [] pwkt;
        if(err != OGRERR_NONE)
        {
            log << ccl::LERR << "Open(): unable to parse projection information." << log.endl;
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        double geotransform[6];
        if(gdal_dataset->GetGeoTransform(geotransform) != CE_None)
        {
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
            return false;
        }

        this->BuildCoordinateTransformations();

        //dfGeoX = adfGeoTransform[0] + adfGeoTransform[1] * x + adfGeoTransform[2] * y;
        //dfGeoY = adfGeoTransform[3] + adfGeoTransform[4] * x + adfGeoTransform[5] * y;

        spacing_x = geotransform[1];
        bound_x_low = geotransform[0];        
        rotation_x = geotransform[2];
        
        spacing_y = geotransform[5];
        bound_y_low = geotransform[3];
        rotation_y = geotransform[4];

        // a post is in the middle of the space, regardless of area_or_point
        bound_x_low = bound_x_low + (spacing_x / 2);
        bound_y_low = bound_y_low + (spacing_y / 2);
        bound_x_high = bound_x_low + (geotransform[1] * (width - 1));
        bound_y_high = bound_y_low + (geotransform[5] * (height - 1));

        // make geographic versions of the bounds
        geo_bound_x_low = bound_x_low;
        geo_bound_y_low = bound_y_low;
        geo_bound_x_high = bound_x_high;
        geo_bound_y_high = bound_y_high;
        if(app_ct)
        {
            app_ct->Transform(1, &geo_bound_x_low, &geo_bound_y_low);
            app_ct->Transform(1, &geo_bound_x_high, &geo_bound_y_high);
        }
        
#ifdef DEBUG
        log << ccl::LDEBUG << "Open(): geotransform = [" << geotransform[0] << ", "
            << geotransform[1] << ", " << geotransform[2] << ", "
            << geotransform[3] << ", " << geotransform[4] << ", "
            << geotransform[5] << ", " << "]" << log.endl;
        log << ccl::LDEBUG << "Open(): spacing_x = " << spacing_x << log.endl;
        log << ccl::LDEBUG << "Open(): spacing_y = " << spacing_y << log.endl;
        log << ccl::LDEBUG << "Open(): bound_x_low = " << bound_x_low << log.endl;
        log << ccl::LDEBUG << "Open(): bound_x_high = " << bound_x_high << log.endl;
        log << ccl::LDEBUG << "Open(): bound_y_low = " << bound_y_low << log.endl;
        log << ccl::LDEBUG << "Open(): bound_y_high = " << bound_y_high << log.endl;
#endif

        // Calculate the nominal x and y spacing by getting the total width in meters and dividing by number of pixels (similar for height)
        
        // First, get the four middle points (middle left/right and middle top/bottom) in geographic
        double middle_left_y = (bound_y_high + bound_y_low)/2;
        double middle_right_y = (bound_y_high + bound_y_low)/2;
        double left = bound_x_low;
        double right = bound_x_high;
        if(app_ct)
        {
            app_ct->Transform(1,&left,&middle_left_y);
            app_ct->Transform(1,&right,&middle_right_y);
        }

        double middle_top_x = (bound_x_high + bound_x_low)/2;
        double middle_bottom_x = (bound_x_high + bound_x_low)/2;
        double bottom = bound_y_high;
        double top = bound_y_low;
        if(app_ct)
        {
            app_ct->Transform(1,&bottom,&middle_bottom_x);
            app_ct->Transform(1,&top,&middle_top_x);
        }
        double flatEarth_origin_y = (top + bottom)/2;
        double flatEarth_origin_x = (right + left)/2;
        cts::FlatEarthProjection flatEarth(flatEarth_origin_y,flatEarth_origin_x);

        // Project the four points to flat earth units
        double flatEarth_left = flatEarth.convertGeoToLocalX(left);
        double flatEarth_right = flatEarth.convertGeoToLocalX(right);
        double flatEarth_top = flatEarth.convertGeoToLocalY(top);
        double flatEarth_bottom = flatEarth.convertGeoToLocalY(bottom);
        
        // Warning: this code will only work if app_ct is WGS84, which is always the case currently.
        postspacing_x = (flatEarth_right - flatEarth_left) / width;
        postspacing_y = (flatEarth_top - flatEarth_bottom) / height;

        GDALRasterBand *gdal_rasterband = gdal_dataset->GetRasterBand(1);
        if(gdal_rasterband)
        {
            nodata = gdal_rasterband->GetNoDataValue();
            gdal_rasterband->GetStatistics(0, 1, &value_min, &value_max, NULL, NULL);
        }

        for(int i = 0; i < depth; ++i)
        {
            GDALRasterBand *gdal_rasterband = gdal_dataset->GetRasterBand(i + 1);
            int size_x, size_y;
            gdal_rasterband->GetBlockSize(&size_x, &size_y);
            blocksizes.push_back(std::make_pair(size_x, size_y));
            GDALDataType datatype = gdal_rasterband->GetRasterDataType();
            datatypes.push_back(datatype);
        }

        GDALClose((GDALDatasetH)gdal_dataset);
        gdal_dataset = NULL;
        
        return true;
    }

    bool DataSource_Raster_GDAL::ValidatePost(double file_x, double file_y)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "ValidatePost(" << file_x << ", " << file_y << ")" << log.endl;
#endif
        double value;
        if(!LoadValue(file_x, file_y, value, 1))
            return false;
        if(value == nodata)
            return false;

        return true;
    }

    bool DataSource_Raster_GDAL::GetValue(double post_x, double post_y, double &value, int index)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "GetValue(" << post_x << ", " << post_y << ", &)" << log.endl;
#endif
        double file_x = post_x;
        double file_y = post_y;
        if(file_ct)
            file_ct->Transform(1, &file_x, &file_y);
        return LoadValue(file_x, file_y, value, index);
    }

    int DataSource_Raster_GDAL::GetValues(double post_x, double post_y, std::vector<double> &values)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "GetValues(" << post_x << ", " << post_y << ", &)" << log.endl;
#endif
        for(size_t i = 0; i < depth; i++)
        {
            double value;
            GetValue(post_x, post_y, value, int(i));
            values.push_back(value);
        }
        return int(depth);
    }

    bool DataSource_Raster_GDAL::LoadValue(double file_x, double file_y, double &value, int index)
    {
        
#ifdef DEBUG
        log << ccl::LDEBUG << "LoadValue(" << file_x << ", " << file_y << ", &)" << log.endl;
#endif
        std::pair<int, int> &blocksize_pair = blocksizes.at(index - 1);
        int size_x = blocksize_pair.first;
        int size_y = blocksize_pair.second;

        double dpixel_x, dpixel_y;
        file_cs_to_pixel(file_x, file_y, dpixel_x, dpixel_y);
        int pixel_x = std::round(dpixel_x);
        int pixel_y = std::round(dpixel_y);

        int block_offset_x = pixel_x / size_x;
        int block_offset_y = pixel_y / size_y;

        int num_blocks_x = ceil(double(width) / double(size_x));
        int num_blocks_y = ceil(double(height) / double(size_y));

        //log << ccl::LDEBUG << "LoadValue(" << file_x << ", " << file_y << ") ; pixel(" << pixel_x << "," << pixel_y << ") ; size(" << size_x << "," << size_y << ")" << log.endl;

        int datasize = GDALGetDataTypeSize(datatypes.at(index - 1)) / 8;
        CacheEntry *entry = cache->GetEntry(this, (block_offset_y * size_x) + block_offset_x, datasize * size_x * size_y);
        if(!entry->loaded)
        {
            GDALRasterBand *gdal_rasterband = gdal_dataset->GetRasterBand(index);
            if (gdal_rasterband->ReadBlock(block_offset_x, block_offset_y, entry->data) != CE_None)
                return false;
            entry->loaded = true;
        }
        value = SRCVAL(entry->data, datatypes.at(index - 1), ((pixel_y % size_y) * size_x) + (pixel_x % size_x));

        if(value == nodata)
            return false;
        return true;
    }

    void DataSource_Raster_GDAL::ref(void)
    {
        if (refcount == 0)
        {
            std::string tableName;
            std::string strippedFilename = filename;
            ccl::GetFilenameAndTable(filename, strippedFilename, tableName);
            if (!tableName.empty())
            {
                //This is a geopackage file with the tablename included
                char** papszOptions = NULL;
                std::string tableStr = "TABLE=" + tableName;
                papszOptions = CSLAddString(papszOptions, tableStr.c_str());
                gdal_dataset = (GDALDataset*)GDALOpenEx(strippedFilename.c_str(),
                    GDAL_OF_READONLY, NULL, papszOptions, NULL);
            }
            else
            {
                gdal_dataset = (GDALDataset *)GDALOpen(this->filename.c_str(), GA_ReadOnly);
            }
        }
        DataSource::ref();
    }

    void DataSource_Raster_GDAL::unref(void)
    {
        DataSource::unref();
        if(refcount == 0)
        {
            GDALClose((GDALDatasetH)gdal_dataset);
            gdal_dataset = NULL;
        }
    }

    bool DataSource_Raster_GDAL::getElevationBounds(double &elevation_min, double &elevation_max)
    {
        if(value_min > value_max)
            return false;
        elevation_min = value_min;
        elevation_max = value_max;
        return true;
    }


}

