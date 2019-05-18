/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/

#ifdef COGNITICS_INSPECTION

#include <map>
#include <new>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ccl/mutex.h>
#include <ccl/Timer.h>

namespace cognitics
{
    namespace inspection
    {
        class Memory
        {
        public:
            struct Entry
            {
                const char *file;
                int line;
                size_t size;
                Entry(const char *file = NULL, int line = 0, size_t size = 0) : file(file), line(line), size(size) { }
                Entry(const Entry &other) : file(other.file), line(other.line), size(other.size) { }
                Entry &operator=(const Entry &other)
                {
                    file = other.file;
                    line = other.line;
                    size = other.size;
                    return *this;
                }
            };

            // disable copy constructor/operator
            Memory(const Memory &);
            Memory &operator=(const Memory &);

            std::map<std::string, const char *> filenames;
            std::map<void *, Entry> entries;
            std::map<Entry, std::pair<size_t, size_t> > counts;
            ccl::mutex mutex;

            static bool ready;

            Memory(void)
            {
                ready = true;
            }

            ~Memory(void)
            {
                ready = false;
                report();
            }

            void alloc(void *p, const char *file, int line, size_t size)
            {
                mutex.lock();
                ready = false;
                // __FILE__ strings may match but be different pointers based on translation unit
                // this maintains a table of filename strings to __FILE__ pointers using the first found
                // the table value is then used for all cases of that filename
                {
                    std::string filename((file == NULL) ? "" : file);
                    if(filenames.find(filename) == filenames.end())
                        filenames[filename] = file;
                    const char *entry_file = filenames[filename];
                    entries[p] = Entry(entry_file, line, size);
                    ++counts[entries[p]].first;
                }
                ready = true;
                mutex.unlock();
            }

            void dealloc(void *p)
            {
                if(!p)
                    return;
                mutex.lock();
                ready = false;
                {
                    std::map<void *, Entry>::iterator entry_it = entries.find(p);
                    if(entry_it != entries.end())
                    {
                        ++counts[entry_it->second].second;
                        entries.erase(entry_it);
                    }
                }
                ready = true;
                mutex.unlock();
            }

            void report(void)
            {
                ccl::sleep(5000);
                mutex.lock();
                std::cout << "================================================================================" << std::endl;
                std::cout << "Memory Inspection Results" << std::endl;
                std::cout << "================================================================================" << std::endl;
                {
                    // total allocated bytes => entry
                    std::multimap<size_t, Entry> ordered_entries;
                    for(std::map<Entry, std::pair<size_t, size_t> >::iterator it = counts.begin(), end = counts.end(); it != end; ++it)
                    {
                        std::pair<size_t, Entry> ordered_pair = std::make_pair(it->second.first * it->first.size, it->first);
                        ordered_entries.insert(ordered_pair);
                    }
                    std::stringstream title;
                    title << "Total Allocations & Deallocations (" << ordered_entries.size() << ")";
                    std::cout << title.str() << std::endl;
                    std::cout << std::string(title.str().length(), '-') << std::endl;
                    for(std::multimap<size_t, Entry>::reverse_iterator it = ordered_entries.rbegin(), end = ordered_entries.rend(); it != end; ++it)
                    {
                        size_t total = it->first;
                        Entry &entry = it->second;
                        const std::pair<size_t, size_t> &entry_counts = counts[entry];
                        std::stringstream ss;
                        ss << std::fixed << std::setw(12) << total;
                        std::cout << ss.str() << " bytes : [" << entry.size << " +" << entry_counts.first << " -" << entry_counts.second << "] " << entry.file << ":" << entry.line << std::endl;
                    }
                }
                std::cout << "================================================================================" << std::endl;
                {
                    // leaked bytes => entry
                    std::multimap<size_t, Entry> ordered_entries;
                    for(std::map<Entry, std::pair<size_t, size_t> >::iterator it = counts.begin(), end = counts.end(); it != end; ++it)
                    {
                        size_t new_count = it->second.first;
                        size_t del_count = it->second.second;
                        if(new_count != del_count)
                        {
                            std::pair<size_t, Entry> ordered_pair = std::make_pair((new_count - del_count) * it->first.size, it->first);
                            ordered_entries.insert(ordered_pair);
                        }
                    }
                    std::stringstream title;
                    title << "Mismatched Allocations (" << ordered_entries.size() << ")";
                    std::cout << title.str() << std::endl;
                    std::cout << std::string(title.str().length(), '-') << std::endl;
                    for(std::multimap<size_t, Entry>::reverse_iterator it = ordered_entries.rbegin(), end = ordered_entries.rend(); it != end; ++it)
                    {
                        size_t total = it->first;
                        Entry &entry = it->second;
                        const std::pair<size_t, size_t> &entry_counts = counts[entry];
                        std::stringstream ss;
                        ss << std::fixed << std::setw(12) << total;
                        std::cout << ss.str() << " bytes : [" << entry.size << " +" << entry_counts.first << " -" << entry_counts.second << "] " << entry.file << ":" << entry.line << std::endl;
                    }
                }
                std::cout << "================================================================================" << std::endl;
                std::cout << std::flush;
                ccl::sleep(1000);
                mutex.unlock();
            }
        };

        bool operator<(const Memory::Entry &lhs, const Memory::Entry &rhs)
        {
            if(lhs.file < rhs.file)
                return true;
            if(lhs.file > rhs.file)
                return false;
            if(lhs.line < rhs.line)
                return true;
            if(lhs.line > rhs.line)
                return false;
            return (lhs.size < rhs.size);
        }

        bool operator==(const Memory::Entry &lhs, const Memory::Entry &rhs)
        {
            return (lhs.file == rhs.file) || (lhs.line == rhs.line) || (lhs.size == rhs.size);
        }

        bool operator>(const Memory::Entry &lhs, const Memory::Entry &rhs)
        {
            return operator<(rhs, lhs);
        }

        bool operator<=(const Memory::Entry &lhs, const Memory::Entry &rhs)
        {
            return !operator>(lhs, rhs);
        }

        bool operator>=(const Memory::Entry &lhs, const Memory::Entry &rhs)
        {
            return !operator<(lhs, rhs);
        }

        bool operator!=(const Memory::Entry &lhs, const Memory::Entry &rhs)
        {
            return !operator==(lhs, rhs);
        }

        bool Memory::ready = false;
        Memory memory;

    }
}

bool cognitics_inspection_make_ready(void)
{
    return cognitics::inspection::Memory::ready;

}

void *operator new(size_t size, const char *file, int line)
{
    void *p = malloc(size);
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.alloc(p, file, line, size);
    return p;
}

void *operator new(size_t size)
{
    void *p = malloc(size);
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.alloc(p, "<unknown>", 0, size);
    return p;
}

void *operator new[](size_t size, const char *file, int line)
{
    void *p = malloc(size);
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.alloc(p, file, line, size);
    return p;
}

void *operator new[](size_t size)
{
    void *p = malloc(size);
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.alloc(p, "<unknown>", 0, size);
    return p;
}

void operator delete(void *p, const char *file, int line)
{
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.dealloc(p);
    free(p);
}

void operator delete(void *p)
{
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.dealloc(p);
    free(p);
}

void operator delete[](void *p, const char *file, int line)
{
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.dealloc(p);
    free(p);
}

void operator delete[](void *p)
{
    if(cognitics::inspection::Memory::ready)
        cognitics::inspection::memory.dealloc(p);
    free(p);
}

#endif
