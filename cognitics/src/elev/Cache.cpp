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
//! \file Cache.cpp
#include <stdexcept>
#include "elev/Cache.h"
#include "elev/DataSource.h"

namespace elev
{
    Cache::~Cache()
    {
        Clear();
    }

    void Cache::Clear()
    {
        while(!entries.empty())
        {
            CacheEntry *entry = entries.back();
            entries.pop_back();
            delete entry;
        }
        size = 0;
    }

    CacheEntry *Cache::GetEntry(DataSource *owner, unsigned int offset, unsigned int size)
    {
        if(size > this->maxsize)    // entry too large for the cache
        {
            throw std::runtime_error("Cache::GetEntry(): requested entry size > maximum cache size");
            return NULL;
        }

        std::list<CacheEntry *>::iterator entry_i = entries.begin();
        while(entry_i != entries.end())
        {
            CacheEntry *entry = *entry_i;
            if(entry->IsMatch(owner, offset))
            {
                if(entry_i == entries.begin())
                {
                    ++hits;
                    return(entry);
                }
                // move the element to the front of the list and return our match
                entry_i = entries.erase(entry_i);
                entries.push_front(entry);
                ++hits;
                return(entry);
            }
            else ++entry_i;
        }

        this->size += size;

        // make space at the end of the list (oldest)
        while(this->size > this->maxsize)
        {
            CacheEntry *entry = entries.back();
            entries.pop_back();
            this->size -= entry->size;
            delete entry;
        }

        CacheEntry *entry = new CacheEntry(owner, offset, size);
        entries.push_front(entry);

        ++misses;

        return entry;
    }

}



