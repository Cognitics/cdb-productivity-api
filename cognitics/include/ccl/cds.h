/****************************************************************************
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

#include <boost/uuid/uuid.hpp>
#include <string>
#include <map>
#include <iostream>
#include <sstream>

namespace cognitics
{
    namespace cds
    {
        //--------------------------------------------------------------------------------
        // cds data source traits
        template <typename T>
        struct traits
        {
            // type definition for an object referencing a data source entry
            //typedef T::handle handle;

            // construct a registry; this can be used to verify/create database tables
            //static bool construct(T *obj);

            // get if the registry is empty
            //static bool empty(T *obj);

            // get the number of entries in the registry
            //static size_t size(T *obj);

            // get the maximum number of entries the registry can support
            //static size_t max_size(T *obj);

            // insert an entry into the registry
            //static std::pair<registry<T>::iterator, bool> insert(T *obj, const boost::uuids::uuid &uuid, const std::string &type, const std::string &description);

            // erase an entry from the registry
            //static bool erase(T *obj, const boost::uuids::uuid &uuid);

            // clear the registry
            //static bool clear(T *obj);

            // find an entry in the registry
            //static registry<T>::iterator find(T *obj, const boost::uuids::uuid &uuid);

            // get the beginning iterator for the registry (the first entry)
            //static registry<T>::iterator begin(T *obj);

            // get the ending iterator for the registry (nil entry)
            //static registry<T>::iterator end(T *obj);

            // advance an iterator (prefix)
            //static void advance(registry<T>::entry &e);

            // advance an iterator (postfix)
            //static registry<T>::iterator advance(registry<T>::entry &e, int);

            // get the uuid from an entry
            //static boost::uuids::uuid get_uuid(const registry<T>::entry &e);

            // get the type from an entry
            //static std::string get_type(const registry<T>::entry &e);

            // get the description from an entry
            //static std::string get_description(const registry<T>::entry &e);

            // set an entry type and description
            //static bool set(registry<T>::entry &e, const std::string &type, const std::string &description);

            // set an entry type
            //static bool set_type(registry<T>::entry &e, const std::string &type);

            // set an entry description
            //static bool set_description(registry<T>::entry &e, const std::string &description);
        };

        //--------------------------------------------------------------------------------
        // cds registry; associative mapped container; unique keys
        template <typename T>
        class registry
        {
        private:
            T *obj;

        public:
            // entry subclass represents a map value
            class entry
            {
                friend class iterator;

            private:
                entry(T *obj = NULL, typename traits<T>::handle handle = typename traits<T>::handle()) : obj(obj), handle(handle)
                {
                }

                entry &operator=(const entry &source)
                {
                    obj = source.obj;
                    handle = source.handle;
                    return *this;
                }

            public:
                T *obj;
                typename traits<T>::handle handle;

                entry(const entry &source) : obj(source.obj), handle(source.handle)
                {
                }

                boost::uuids::uuid get_uuid(void) const
                {
                    return traits<T>::get_uuid(*this);
                }

                std::string get_type(void) const
                {
                    return traits<T>::get_type(*this);
                }

                std::string get_description(void) const
                {
                    return traits<T>::get_description(*this);
                }

                // table name prefix (last segment of uuid)
                std::string get_prefix(void) const
                {
                    return boost::uuids::to_string(get_uuid()).substr(24);
                }

                bool set(const std::string &type, const std::string &description)
                {
                    return traits<T>::set(*this, type, description);
                }

                bool set_type(const std::string &type)
                {
                    return traits<T>::set_type(*this, type);
                }

                bool set_description(const std::string &description)
                {
                    return traits<T>::set_description(*this, description);
                }

                bool operator==(const entry &rhs)
                {
                    return (obj == rhs.obj) && (handle == rhs.handle) && (get_uuid() == rhs.get_uuid())
                        && (get_type() == rhs.get_type()) && (get_description() == rhs.get_description());
                }

                bool operator!=(const entry &rhs)
                {
                    return !(*this == rhs);
                }

            };

            // forward input iterator; references an entry object
            class iterator
            {
            private:
                entry e;

            public:
                iterator(T *obj = NULL, typename traits<T>::handle handle = typename traits<T>::handle()) : e(obj, handle)
                {
                }

                iterator(const iterator &source) : e(source.e.obj, source.e.handle)
                {
                }

                iterator &operator=(const iterator &source)
                {
                    e.obj = source.e.obj;
                    e.handle = source.e.handle;
                    return *this;
                }

                entry &operator*(void)
                {
                    return e;
                }

                entry *operator->(void)
                {
                    return &e;
                }

                iterator &operator++(void)
                {
                    traits<T>::advance(e);
                    return *this;
                }

                iterator operator++(int)
                {
                    return traits<T>::advance(e, int(0));
                }

                bool operator==(const iterator &rhs)
                {
                    return (e == rhs.e);
                }

                bool operator!=(const iterator &rhs)
                {
                    return !(e == rhs.e);
                }

            };

            typedef const iterator const_iterator;

            // destructible
            ~registry(void)
            {
            }

            // empty constructor
            registry(T *obj) : obj(obj)
            {
                if(!traits<T>::construct(obj))
                    throw std::runtime_error("unable to construct cds");
            }

            // copy constructor; clones the registry object
            registry(const registry &source) : obj(copy.obj)
            {
            }

            // copy operator; copies the source entries from one registry to the other
            registry &operator=(const registry &source)
            {
                insert(source.begin(), source.end());
                return *this;
            }

            bool empty(void) const
            {
                return traits<T>::empty(obj);
            }

            size_t size(void) const
            {
                return traits<T>::size(obj);
            }

            size_t max_size(void) const
            {
                return traits<T>::max_size(obj);
            }

            // element access (entry is an accessor object, so we pass a copy rather than a reference to prevent memory management problems)
            entry operator[](const boost::uuids::uuid &uuid)
            {
                iterator it = find(uuid);
                if(it == end())
                    it = insert(uuid).first;
                return *it;
            }

            // element access (entry is an accessor object, so we pass a copy rather than a reference to prevent memory management problems)
            entry at(const boost::uuids::uuid &uuid)
            {
                iterator it = find(uuid);
                if(it == end())
                    throw std::out_of_range("uuid not found");
                return *it;
            }

            // element access (entry is an accessor object, so we pass a copy rather than a reference to prevent memory management problems)
            const entry at(const boost::uuids::uuid &uuid) const
            {
                const_iterator it = find(uuid);
                if(it == end())
                    throw std::out_of_range("uuid not found");
                return *it;
            }

            std::pair<iterator, bool> insert(const boost::uuids::uuid &uuid, const std::string &type = std::string(), const std::string &description = std::string())
            {
                return traits<T>::insert(obj, uuid, type, description);
            }

            void insert(iterator first, iterator last)
            {
                for(iterator it = first; it != last; ++it)
                    insert(it->get_uuid(), it->get_type(), it->get_description());
            }

            bool erase(iterator position)
            {
                return erase(position->get_uuid());
            }

            bool erase(const boost::uuids::uuid &uuid)
            {
                return traits<T>::erase(obj, uuid);
            }

            bool erase(iterator first, iterator last)
            {
                bool result = true;
                for(iterator it = first; it != last; ++it)
                    result &= erase(it->get_uuid());
                return result
            }

            bool clear(void)
            {
                return traits<T>::clear(obj);
            }

            iterator find(const boost::uuids::uuid &uuid)
            {
                return traits<T>::find(obj, uuid);
            }

            const_iterator find(const boost::uuids::uuid &uuid) const
            {
                return traits<T>::find(obj, uuid);
            }

            iterator begin(void)
            {
                return traits<T>::begin(obj);
            }

            const_iterator begin(void) const
            {
                return traits<T>::begin(obj);
            }

            iterator end(void)
            {
                return traits<T>::end(obj);
            }

            const_iterator end(void) const
            {
                return traits<T>::end(obj);
            }

        };

        //--------------------------------------------------------------------------------
        // utility function for testing a cds implementation
        template <typename T>
        bool test(T *obj, std::ostream &stream)
        {
            std::vector<boost::uuids::uuid> uuids;
            registry<T> reg(obj);

            for(size_t i = 0; i < 10; ++i)
            {
                uuids.push_back(cognitics::uuid::random_generator<ccl::uuid>()());
                std::stringstream ss;
                ss << i;
                stream << "reg.insert('" << boost::uuids::to_string(uuids[i]) << "', 'test:type" << ss.str() << "', 'test description " << ss.str() << "')" << std::endl;
                if(reg.insert(uuids[i], "test:type" + ss.str(), "test description " + ss.str()).second == false)
                {
                    stream << "\tFAILED" << std::endl;
                    return false;
                }
            }

            stream << "reg.empty()" << std::endl;
            if(reg.empty())
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            stream << "reg.size() == 10" << std::endl;
            if(reg.size() != 10)
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            stream << "reg.max_size() >= reg.size()" << std::endl;
            if(reg.max_size() < reg.size())
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            stream << "reg.begin() -> reg.end()" << std::endl;
            for(registry<T>::iterator it = reg.begin(), end = reg.end(); it != end; ++it)
                stream << "\t" << boost::uuids::to_string(it->get_uuid()) << " : " << it->get_type() << " : " << it->get_description() << std::endl;

            stream << "reg.find('" << boost::uuids::to_string(uuids[6]) << "') [6]" << std::endl;
            registry<T>::iterator it = reg.find(uuids[6]);
            stream << "\t" << boost::uuids::to_string(it->get_uuid()) << " : " << it->get_type() << " : " << it->get_description() << std::endl;
            stream << ">>>>>>>>>>>" << it->get_prefix() << std::endl;

            stream << "reg.at('" << boost::uuids::to_string(uuids[7]) << "') [7]" << std::endl;
            stream << "\t" << boost::uuids::to_string(reg.at(uuids[7]).get_uuid()) << " : " << reg.at(uuids[7]).get_type() << " : " << reg.at(uuids[7]).get_description() << std::endl;

            stream << "reg['" << boost::uuids::to_string(uuids[8]) << "'] [8]" << std::endl;
            stream << "\t" << boost::uuids::to_string(reg[uuids[8]].get_uuid()) << " : " << reg[uuids[8]].get_type() << " : " << reg[uuids[8]].get_description() << std::endl;

            stream << "reg['" << boost::uuids::to_string(uuids[2]) << "'].set('new type 2', 'new description 2')" << std::endl;
            reg[uuids[2]].set("new type 2", "new description 2");
            stream << "\t" << boost::uuids::to_string(reg[uuids[2]].get_uuid()) << " : " << reg[uuids[2]].get_type() << " : " << reg[uuids[2]].get_description() << std::endl;

            stream << "reg['" << boost::uuids::to_string(uuids[3]) << "'].set_type('new type 3')" << std::endl;
            reg[uuids[3]].set_type("new type 3");
            stream << "\t" << boost::uuids::to_string(reg[uuids[3]].get_uuid()) << " : " << reg[uuids[3]].get_type() << " : " << reg[uuids[3]].get_description() << std::endl;

            stream << "reg['" << boost::uuids::to_string(uuids[3]) << "'].set_description('new description 3')" << std::endl;
            reg[uuids[3]].set_description("new description 3");
            stream << "\t" << boost::uuids::to_string(reg[uuids[3]].get_uuid()) << " : " << reg[uuids[3]].get_type() << " : " << reg[uuids[3]].get_description() << std::endl;

            stream << "reg.erase('" << boost::uuids::to_string(uuids[5]) << "')" << std::endl;
            if(!reg.erase(uuids[5]))
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            stream << "reg.size() == 9" << std::endl;
            if(reg.size() != 9)
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            stream << "reg.clear()" << std::endl;
            if(!reg.clear())
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            stream << "!reg.empty()" << std::endl;
            if(!reg.empty())
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }
            
            stream << "reg.size() != 0" << std::endl;
            if(reg.size() != 0)
            {
                stream << "\tFAILED" << std::endl;
                return false;
            }

            return true;
        }

        //--------------------------------------------------------------------------------
        // example traits for std::map registry representation
        typedef std::map<boost::uuids::uuid, std::pair<std::string, std::string> > cds_map;
        template <>
        struct traits<cds_map>
        {
            // the std::map iterator can be used directly as the registry handle
            typedef cds_map::iterator handle;

            // no construction is necessary, so we just return true
            static bool construct(cds_map *obj)
            {
                return true;
            }

            // empty() maps directly
            static bool empty(cds_map *obj)
            {
                return obj->empty();
            }

            // size() maps directly
            static size_t size(cds_map *obj)
            {
                return obj->size();
            }

            // max_size() maps directly
            static size_t max_size(cds_map *obj)
            {
                return obj->max_size();
            }

            // insert() requires wrapping of the iterator, but otherwise maps directly
            static std::pair<registry<cds_map>::iterator, bool> insert(cds_map *obj, const boost::uuids::uuid &uuid, const std::string &type, const std::string &description)
            {
                std::pair<cds_map::iterator, bool> result = obj->insert(std::make_pair(uuid, std::make_pair(type, description)));
                return std::make_pair(registry<cds_map>::iterator(obj, result.first), result.second);
            }

            // erase() maps directly
            static bool erase(cds_map *obj, const boost::uuids::uuid &uuid)
            {
                obj->erase(uuid);
                return true;
            }

            // clear() maps directly
            static bool clear(cds_map *obj)
            {
                obj->clear();
                return true;
            }

            // find() requires wrapping of the iterator, but otherwise maps directly
            static registry<cds_map>::iterator find(cds_map *obj, const boost::uuids::uuid &uuid)
            {
                return registry<cds_map>::iterator(obj, obj->find(uuid));
            }

            // begin() requires wrapping of the iterator, but otherwise maps directly
            static registry<cds_map>::iterator begin(cds_map *obj)
            {
                return registry<cds_map>::iterator(obj, obj->begin());
            }

            // end() requires wrapping of the iterator, but otherwise maps directly
            static registry<cds_map>::iterator end(cds_map *obj)
            {
                return registry<cds_map>::iterator(obj, obj->end());
            }

            // advance() maps to operator++
            static void advance(registry<cds_map>::entry &e)
            {
                ++e.handle;
            }

            // advance(int) maps to operator++(int), but requires iterator wrapping
            static registry<cds_map>::iterator advance(registry<cds_map>::entry &e, int)
            {
                registry<cds_map>::iterator result(e.obj, e.handle);
                ++e.handle;
                return result;
            }

            // get_uuid() retrieves the map key; returns nil uuid if the entry is invalid
            static boost::uuids::uuid get_uuid(const registry<cds_map>::entry &e)
            {
                return (e.handle == e.obj->end()) ? boost::uuids::nil_generator()() : e.handle->first;
            }

            // get_type() retrieves the map pair first value; returns empty string if the entry is invalid
            static std::string get_type(const registry<cds_map>::entry &e)
            {
                return (e.handle == e.obj->end()) ? std::string() : e.handle->second.first;
            }

            // get_value() retrieves the map pair second value; returns empty string if the entry is invalid
            static std::string get_description(const registry<cds_map>::entry &e)
            {
                return (e.handle == e.obj->end()) ? std::string() : e.handle->second.second;
            }

            // set() assigns the type and description values to the entry
            static bool set(registry<cds_map>::entry &e, const std::string &type, const std::string &description)
            {
                e.handle->second.first = type;
                e.handle->second.second = description;
                return true;
            }

            // set_type() assigns the type value to the entry
            static bool set_type(registry<cds_map>::entry &e, const std::string &type)
            {
                e.handle->second.first = type;
                return true;
            }

            // set_description() assigns the description value to the entry
            static bool set_description(registry<cds_map>::entry &e, const std::string &description)
            {
                e.handle->second.second = description;
                return true;
            }

        };

    }

}