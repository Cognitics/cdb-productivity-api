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
/*! \file Cache.h
\headerfile Cache.h cache/Cache.h

\brief Generic caching management class.

\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009
*/
#pragma once

#include "CacheEntry.h"

#include <list>

namespace elev
{
/*! \class elev::Cache Cache.h cache/Cache.h
\brief Generic cache management class.

\code
#include <cache/Cache.h>

elev::Cache *cache = new elev::Cache(32 * 1024 * 1024);

\endcode

\sa elev::CacheEntry
*/
    class Cache
    {
        unsigned int maxsize;                    //!< maximum size in bytes
        unsigned int size;                        //!< current size in bytes
        unsigned int hits;                        //!< cache hits
        unsigned int misses;                    //!< cache misses
        std::list<CacheEntry *> entries;        //!< list of elev::CacheEntry objects

    public:
        //! Create a new cache of maxsize bytes.
        Cache(unsigned int maxsize) : maxsize(maxsize), size(0), hits(0), misses(0) { }

        //! Destroy the cache and all entries.
        ~Cache();

        //! Clear the cache.
        void Clear();

        //! Get the maximum cache size in bytes.
        unsigned int GetMaxSize(void) { return this->maxsize; }
        //! Set the maximum cache size in bytes.
        void SetMaxSize(unsigned int maxsize) { this->maxsize = maxsize; }

        //! Get the current cache size in bytes.
        unsigned int GetSize(void) { return this->size; }

        //! Get cache hit count.
        unsigned int GetHits(void) { return this->hits; }
        //! Get cache miss count.
        unsigned int GetMisses(void) { return this->misses; }

        //! Get the cache entry for the owner and offset.
        /*! This will also move the requested entry to the front of the list, so that
            the list is ordered inversely by last access.
        
            If the entry does not exist, entries will be removed from the end of the list
            as needed to make space for the size specified and create a new entry.

            \warning Attempting to get an entry with size > maxsize will throw an exception.
        */
        CacheEntry *GetEntry(DataSource *owner, unsigned int offset, unsigned int size);

    };

}

