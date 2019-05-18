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
/*! \file DataSource_Raster_GDAL.h
\headerfile DataSource_Raster_GDAL.h DataSource/DataSource_Raster_GDAL.h

\brief Provides elev::DataSource_Raster_GDAL, an extension of elev::DataSource_Raster for GDAL data sources.

\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009

\class elev::DataSource_Raster_GDAL DataSource_Raster_GDAL.h DataSource/DataSource_Raster_GDAL.h
\brief Extension of elev::DataSource_Raster for GDAL data sources.

\sa elev::DataSource_Raster, elev::DataSourceManager, elev::DataPost.
*/

#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_spatialref.h>
#pragma warning ( pop )

#include "DataSource_Raster.h"

#include <string>

#include <boost/any.hpp>
#include <ccl/mutex.h>
namespace elev
{
    class DataSource_Raster_GDAL : public DataSource_Raster
    {
    protected:
        GDALDataset *gdal_dataset;                            //!< GDALDataset instance for the file
        ccl::mutex cacheMutex;
        std::vector<std::pair<int, int> > blocksizes;
        std::vector<GDALDataType> datatypes;
        bool pixel_is_point;
        double value_min;
        double value_max;

        //! Open the datafile for reading.
        virtual bool Open();

        bool LoadValue(double file_x, double file_y, double &value, int index = 1);

    public:
        //! Instantiate a DataSource_Raster_GDAL object.
        DataSource_Raster_GDAL(std::string filename, Cache *cache);

        //! Destroy a DataSource_Raster_GDAL object.
        /*! This closes the dataset. */
        virtual ~DataSource_Raster_GDAL();

        //! Implementation of DataSource_Raster::ValidatePost().
        virtual bool ValidatePost(double post_x, double post_y);

        //! Implementation of DataSource_Raster::GetValue().
        virtual bool GetValue(double post_x, double post_y, double &value, int index = 1);

        //! Implementation of DataSource_Raster::GetValues().
        virtual int GetValues(double post_x, double post_y, std::vector<double> &values);

        virtual bool getElevationBounds(double &elevation_min, double &elevation_max);

        virtual void ref(void);
        virtual void unref(void);

    };

}