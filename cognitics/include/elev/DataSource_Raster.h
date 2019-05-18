/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/

#pragma once

#include "DataSource.h"
#include "DataPost.h"
#include <sfa/Feature.h>

namespace elev
{
    class DataSource_Raster : public DataSource
    {
    public:
        int priority;
        double bound_x_low;                    //!< x-axis low boundary
        double bound_x_high;                //!< x-axis high boundary
        double bound_y_low;                    //!< y-axis low boundary
        double bound_y_high;                //!< y-axis high boundary
        double geo_bound_x_low;                //!< x-axis low boundary (geographic)
        double geo_bound_x_high;            //!< x-axis high boundary (geographic)
        double geo_bound_y_low;                //!< y-axis low boundary (geographic)
        double geo_bound_y_high;            //!< y-axis high boundary (geographic)
        int width;                            //!< width of data in posts
        int height;                            //!< height of data in posts
        int depth;                            //!< band/layer (value) depth
        double spacing_x;                    //!< coordinate distance between post x-values
        double spacing_y;                    //!< coordinate distance between post y-values
        double rotation_x;                    //!< rotation between post x-values
        double rotation_y;                    //!< rotation between post y-values
        double nodata;

        std::map<sfa::Point, sfa::Feature *> debug_features;

        //! Check if the passed coordinates are inside the file bounds.
        virtual bool CheckBounds(double x, double y);

        DataSource_Raster(std::string filename, Cache *cache);

        virtual ~DataSource_Raster();

        void set_priority(int p);

        //! Get posts surrounding provided application coordinates.
        virtual int GetPostsForPoint(double x, double y, std::vector<DataPost*> &posts);

        //! Checks if a post value is valid (by FILE PROJECTION post coordinates).
        virtual bool ValidatePost(double file_x, double file_y) { return true; }

        //! Retrieve a value from the dataset for a post (by post coordinates).
        /*!    The index parameter is used to specify a raster band index (default is 1).
            \return True if a value was retrieved successfully, false otherwise.
        */
        virtual bool GetValue(double post_x, double post_y, double &value, int index = 1) = 0;

        //! Retrieve a value from the dataset for a post (by DataPost instance).
        /*!    The index parameter is used to specify a raster band index (default is 1).
            \return True if a value was retrieved successfully, false otherwise.
        */
        virtual bool GetValue(DataPost *post, double &value, int index = 1);

        //! Retrieve all values from the dataset for a post (by post coordinates).
        /*!    \return The number of values retrieved successfully. */
        virtual int GetValues(double post_x, double post_y, std::vector<double> &values) = 0;

        //! Retrieve all values from the dataset for a post (by DataPost instance).
        /*!    \return The number of values retrieved successfully. */
        virtual int GetValues(DataPost *post, std::vector<double> &values);

        virtual bool getElevationBounds(double &elevation_min, double &elevation_max);

        virtual bool file_cs_to_pixel(double file_x, double file_y, double &col, double &row);
        virtual bool pixel_to_file_cs(int col, int row, double &file_x, double &file_y);





    };

}


