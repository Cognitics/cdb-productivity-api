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
/*! \file DataSource.h
\headerfile DataSource.h DataSource/DataSource.h

\brief Provides elev::DataSource, a base class for data sources.

\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009

\class elev::DataSource DataSource.h DataSource/DataSource.h
\brief Base class representing a single GIS data file.

A elev::DataSource instance provides an abstracted interface for data access.
These objects are created and maintained by elev::DataSourceManager.

\sa elev::DataSourceManager.
*/
#pragma once

#include "DataPost.h"
#include <ccl/ObjLog.h>
#include <elev/Cache.h>

#include <string>
#include <vector>
#ifndef WIN32
#undef HAVE_UNISTD_H
#endif
#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <ogr_spatialref.h>
#pragma warning ( pop )

namespace elev
{
    #define DATASOURCE_TYPE_UNDEFINED    0    //!< undefined datafile type
    #define DATASOURCE_TYPE_RASTER        1    //!< raster datafile type
    #define DATASOURCE_TYPE_VECTOR        2    //!< vector datafile type

    class DataSource
    {
    public:
        ccl::ObjLog log;                                //!< ObjLog instance
        Cache *cache;                            //!< Cache instance, weak reference
        std::string filename;                    //!< GIS data filename
        std::string reftype;                    //!< application reference (projection) type (default is "WGS84")
        int type;                                //!< datafile type (raster or vector, see defines)
        bool valid;                                //!< valid file indicator, set to true if load was successful
        OGRSpatialReference *file_srs;            //!< datafile spatial reference
        OGRSpatialReference *app_srs;            //!< application spatial reference
        OGRCoordinateTransformation *file_ct;    //!< app to file transformation
        OGRCoordinateTransformation *app_ct;    //!< file to app transformation
        double postspacing_x;                    //!< calculated nominal post spacing in meters along the x axis
        double postspacing_y;                    //!< calculated nominal post spacing in meters along the y axis
        int refcount;

        //! Build coordinate transformation objects
        virtual void BuildCoordinateTransformations();

        //! Load file projection from a matching .prj if it exists.
        bool LoadProjectionFromPRJ();

    public:
        //! Instantiate a DataSource object.
        DataSource(std::string filename, Cache *cache);

        //! Destroy a DataSource object
        virtual ~DataSource() 
        {
            if(file_ct)
                OGRCoordinateTransformation::DestroyCT(file_ct);
            if(app_ct)
                OGRCoordinateTransformation::DestroyCT(app_ct);
            if(app_srs) 
                OSRDestroySpatialReference(OGRSpatialReferenceH(app_srs));
            if(file_srs && file_srs!=app_srs) 
                OSRDestroySpatialReference(OGRSpatialReferenceH(file_srs));
        }

        //! Set the application geospacial reference type.
        virtual void SetReferenceType(std::string reftype);
        //! Get the application geospacial reference type.
        virtual std::string GetReferenceType() { return this->reftype; }

        //! Get datasource type.
        int GetType() { return this->type; }

        //! \return True if the datasource is valid, false otherwise.
        bool IsValid() { return this->valid; }

        //! \returns the average of the x and y postspacing.
        double GetAveragePostSpacing() { return (postspacing_x + postspacing_y)/2; }

        virtual void ref(void);
        virtual void unref(void);
    };

}
