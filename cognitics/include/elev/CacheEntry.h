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
/*! \file CacheEntry.h
\headerfile CacheEntry.h cache/CacheEntry.h

\brief Entry class for elev::Cache

\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009
*/
#pragma once

namespace elev
{
    class DataSource;

/*! \class elev::CacheEntry CacheEntry.h elev/CacheEntry.h
\brief Entry class for elev::Cache.

\sa elev::Cache
*/
    class CacheEntry
    {
        DataSource *owner;                //!< the owner class of the cache entry
        unsigned int offset;        //!< an offset value for use by the owner

    public:
        unsigned int size;            //!< size of the dataset in bytes
        void *data;                    //!< dataset
        bool loaded;                //!< flag to identify if the data property has been populated

        //! Create a new entry.
        CacheEntry(DataSource *owner, unsigned int offset, unsigned int size);

        //! Destroy the entry.
        ~CacheEntry();

        //! Check if this entry matches the requested owner and offset.
        bool IsMatch(DataSource *owner, unsigned int offset);

    };

}

