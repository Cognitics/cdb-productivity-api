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

#pragma once

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <gdal.h>
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogr_spatialref.h>
#pragma warning ( pop )
#include "ccl/ObjLog.h"

namespace elev
{
    class SimpleDEMReader
    {
        GDALDataset *gdal_dataset;
        std::string filename;                    //!< GIS data filename
        ccl::ObjLog log;
        int height;
        int width;
        int depth;
        int windowTop;
        int windowBottom;
        int windowLeft;
        int windowRight;
        bool windowedMode;
        int windowHeight;
        int windowWidth;
        float scaleFactor;
        double spacing_x;                    //!< coordinate distance between post x-values
        double spacing_y;                    //!< coordinate distance between post y-values
        double rotation_x;                    //!< rotation between post x-values
        double rotation_y;                    //!< rotation between post y-values
        double bound_x_low;                    //!< x-axis low boundary
        double bound_x_high;                //!< x-axis high boundary
        double bound_y_low;                    //!< y-axis low boundary
        double bound_y_high;                //!< y-axis high boundary
        double geo_bound_x_low;                //!< x-axis low boundary (geographic)
        double geo_bound_x_high;            //!< x-axis high boundary (geographic)
        double geo_bound_y_low;                //!< y-axis low boundary (geographic)
        double geo_bound_y_high;            //!< y-axis high boundary (geographic)

        std::string reftype;                    //!< application reference (projection) type (default is "WGS84")
        OGRSpatialReference *file_srs;            //!< datafile spatial reference
        OGRSpatialReference *app_srs;            //!< application spatial reference
        OGRCoordinateTransformation *file_ct;    //!< app to file transformation
        OGRCoordinateTransformation *app_ct;    //!< file to app transformation
        void BuildCoordinateTransformations();

    public:
        ~SimpleDEMReader();
        SimpleDEMReader(const std::string &filename, 
            OGRSpatialReference destinationSRS, float scale = 1);
        SimpleDEMReader(const std::string &filename,
            OGRSpatialReference destinationSRS,
            int windowTop,
            int windowBottom,
            int windowRight,
            int windowLeft, float scale = 1);

        int getWidth() const;
        int getHeight() const;
        int getScaledWidth() const;
        int getScaledHeight() const;
        double getPostSpacingX() { return spacing_x / scaleFactor; }
        double getPostSpacingY() { return spacing_y / scaleFactor; }

        void getMBR(double &north, double &south, double &east, double &west);
        // Convert each post to a double value and return it as an array
        bool getGrid(std::vector<double> &grid);

        bool Open();
    };



}