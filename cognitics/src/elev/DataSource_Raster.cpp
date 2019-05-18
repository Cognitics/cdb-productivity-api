/****************************************************************************
Copyright 2018 Cognitics, Inc.
****************************************************************************/

//#pragma optimize("", off)

#include <float.h>
#include "elev/DataSource_Raster.h"

using namespace ccl;
namespace elev
{
    DataSource_Raster::DataSource_Raster(std::string filename, Cache *cache) : DataSource(filename, cache), nodata(DBL_MAX), priority(0)
    {
        log.init("DataSource_Raster", this);
#ifdef DEBUG
        log << ccl::LDEBUG << "DataSource_Raster(" << filename << ", *)" << log.endl;
#endif
        this->type = DATASOURCE_TYPE_RASTER;
        this->bound_x_low = 0;
        this->bound_x_high = 0;
        this->bound_y_low = 0;
        this->bound_y_high = 0;
        this->geo_bound_x_low = 0;
        this->geo_bound_x_high = 0;
        this->geo_bound_y_low = 0;
        this->geo_bound_y_high = 0;
        this->width = 0;
        this->height = 0;
        this->depth = 0;
        this->spacing_x = 1;
        this->spacing_y = 1;
        this->rotation_x = 1;
        this->rotation_y = 1;
    }

    void DataSource_Raster::set_priority(int p)
    {
        priority = p;
    }

    bool DataSource_Raster::CheckBounds(double x, double y)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "CheckBounds(" << x << ", " << y << ")" << log.endl;
#endif
        if((spacing_x > 0) && (x < bound_x_low))
            return false;
        if((spacing_x > 0) && (x > bound_x_high))
            return false;
        if((spacing_x < 0) && (x > bound_x_low))
            return false;
        if((spacing_x < 0) && (x < bound_x_high))
            return false;
        if((spacing_y > 0) && (y < bound_y_low))
            return false;
        if((spacing_y > 0) && (y > bound_y_high))
            return false;
        if((spacing_y < 0) && (y > bound_y_low))
            return false;
        if((spacing_y < 0) && (y < bound_y_high))
            return false;
        return true;
    }

    DataSource_Raster::~DataSource_Raster()
    {
       for(std::map<sfa::Point, sfa::Feature *>::iterator it = debug_features.begin(), end = debug_features.end(); it != end; ++it)
           delete it->second;
    }

    int DataSource_Raster::GetPostsForPoint(double file_x, double file_y, std::vector<DataPost*> &geoposts)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "GetPostsForPoint(" << file_x << ", " << file_y << ", &)" << log.endl;
#endif
        if(file_ct)
            file_ct->Transform(1, &file_x, &file_y);

        std::vector<sfa::Point> points;

        double dpx, dpy;
        file_cs_to_pixel(file_x, file_y, dpx, dpy);
        int ipx = std::floor(dpx);
        int ipy = std::floor(dpy);

        double px, py;

        pixel_to_file_cs(ipx, ipy, px, py);
        if(CheckBounds(px, py) && ValidatePost(px, py))
            points.push_back(sfa::Point(px, py));

        pixel_to_file_cs(ipx, ipy + 1, px, py);
        if(CheckBounds(px, py) && ValidatePost(px, py))
            points.push_back(sfa::Point(px, py));

        pixel_to_file_cs(ipx + 1, ipy + 1, px, py);
        if(CheckBounds(px, py) && ValidatePost(px, py))
            points.push_back(sfa::Point(px, py));

        pixel_to_file_cs(ipx + 1, ipy, px, py);
        if(CheckBounds(px, py) && ValidatePost(px, py))
            points.push_back(sfa::Point(px, py));

        for(size_t i = 0, c = points.size(); i < c; ++i)
        {
            double app_x = points[i].X();
            double app_y = points[i].Y();
            if(app_ct)
                app_ct->Transform(1, &app_x, &app_y);
            DataPost *dp = new DataPost(this, app_x, app_y);
            geoposts.push_back(dp);
        }
        return points.size();
    }

    bool DataSource_Raster::GetValue(DataPost *post, double &value, int index)
    {
        return GetValue(post->X(), post->Y(), value, index);
    }

    int DataSource_Raster::GetValues(DataPost *post, std::vector<double> &values)
    {
        return GetValues(post->X(), post->Y(), values);
    }

    bool DataSource_Raster::getElevationBounds(double &elevation_min, double &elevation_max)
    {
        // TODO: if we do non-gdal, we need to implement this
        return false;
    }

    bool DataSource_Raster::pixel_to_file_cs(int col, int row, double &file_x, double &file_y)
    {
        file_x = bound_x_low + (spacing_x * col) + (rotation_x * row);
        file_y = bound_y_low + (spacing_y * row) + (rotation_y * col);
        return true;
    }

    bool DataSource_Raster::file_cs_to_pixel(double file_x, double file_y, double &col, double &row)
    {
        double denom = (rotation_x * rotation_y) - (spacing_x * spacing_y);
        if(denom == 0)
            return false;
        col = -(rotation_x * (bound_y_low - file_y) + (spacing_y * file_x) - (bound_x_low * spacing_y)) / denom;
        row = (spacing_x * (bound_y_low - file_y) + (rotation_y * file_x) - (bound_x_low * rotation_y)) / denom;
        return true;
    }




}
