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
/*! \file DataPost.h
\headerfile DataPost.h DataSource/DataPost.h

\brief Provides elev::DataPost, a single point from a raster data file.

\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009

\class elev::DataPost DataPost.h DataSource/DataPost.h
\brief Represents a single point from a raster data file.

DataPost objects are instantiated by DataFile_Raster::GetPostsForPoint().
An instance represents a single point in the file and provides methods for fetching data for that point.

The coordinates stored in a DataPost instance are application-projection, not file-projection.
Value retrieval functions handle this transformation automatically.

\sa elev::DataSourceManager, elev::DataSource_Raster.
*/

#pragma once

#include <ccl/ccl.h>
#include <sfa/Point.h>

#include <vector>
#include <boost/any.hpp>

namespace elev
{
    class DataSource_Raster;

    class DataPost : public sfa::Point
    {
    public:
        DataSource_Raster *source;        //!< DataSource instance for the point

        //! Instantiate a DataPost object.
        /*!    A DataPost is generally instantiated as part of DataFile::GetPostsForPoint(). */
        DataPost(DataSource_Raster *source, double x, double y) : sfa::Point(x, y), source(source) { }

        //! Retrieve a value from the dataset for the DataPost.
        /*!    The index parameter is used to specify a value index, such as a raster band.
            \return True if a value was retrieved successfully, false otherwise.
        */
        bool GetValue(double &value, int index = 1);

        //! Retrieve all values from the dataset for the DataPost.
        /*!    \return The number of values retrieved successfully. */
        int GetValues(std::vector<double> &values);

    };

    typedef std::shared_ptr<DataPost> DataPostSP;

}

