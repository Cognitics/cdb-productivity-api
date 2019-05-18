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
/* \brief attribute traits and tools
\author Aaron Brinton <abrinton@cognitics.net>
\date 20 August 2012
*/
#pragma once

#include <map>
#include <set>
#include <string>

namespace ccl
{
    namespace traits
    {
        /*! \brief Template traits structure for class hierarchies

        Traits implementations for a class hierarchy should define a struct hierarchy { } in the base class definition.
        This will work with the attribute_traits template to provide "inherited" traits.

        Implementations should provide:
        <ul>
        <li><tt>typedef K key_type;</tt>
        <li><tt>typedef V value_type;</tt>
        </ul>

        */
        template <class H>
        struct attribute_hierarchy_traits
        {
        };

        /*! Template traits structure for classes in a class hierarchy

        Traits implementations for a specific class should specialize this.
        If traits should apply to the hierarchy, specialize attribute_hierarchy_traits instead for T::hierarchy

        Implementations should provide:
        <ul>
        <li><tt>typedef K key_type;</tt>
        <li><tt>typedef V value_type;</tt>
        </ul>

        */
        template <class T>
        struct attribute_traits : public attribute_hierarchy_traits<typename T::hierarchy>
        {
        };

        /*! \brief Template access structure for class hierarchies

        Traits implementations for a class hierarchy should define a struct hierarchy { } in the base class definition.
        This will work with the attribute_access template to provide "inherited" access methods.

        Implementations should provide:
        <ul>
        <li><tt>static std::set<K> getAttributeKeys(T &object) { }</tt>
        <li><tt>static bool hasAttribute(T &object, const K &key) { }</tt>
        <li><tt>static V getAttribute(T &object, const K &key, const V &defaultValue = V()) { }</tt>
        <li><tt>static void setAttribute(T &object, const K &key, const V &value) { }</tt>
        <li><tt>static void removeAttribute(T &object, const K &key) { }</tt>
        <li><tt>static void clearAttributes(T &object) { }</tt>
        </ul>

        */
        template <class H>
        struct attribute_hierarchy_access
        {
        };

        /*! Template access structure for classes in a class hierarchy

        Traits implementations for a specific class should specialize this.
        If traits should apply to the hierarchy, specialize attribute_hierarchy_access instead for T::hierarchy

        Implementations should provide:
        <ul>
        <li><tt>static std::set<K> getAttributeKeys(T &object) { }</tt>
        <li><tt>static bool hasAttribute(T &object, const K &key) { }</tt>
        <li><tt>static V getAttribute(T &object, const K &key, const V &defaultValue = V()) { }</tt>
        <li><tt>static void setAttribute(T &object, const K &key, const V &value) { }</tt>
        <li><tt>static void removeAttribute(T &object, const K &key) { }</tt>
        <li><tt>static void clearAttributes(T &object) { }</tt>
        </ul>

        */
        template <class T>
        struct attribute_access : public attribute_hierarchy_access<typename T::hierarchy>
        {
        };
    }

    //! Get a set of attribute keys
    template <typename T>
    std::set<typename traits::attribute_traits<T>::key_type> getAttributeKeys(T &object)
    {
        return traits::attribute_access<T>::getAttributeKeys(object);
    }

    //! Returns true if the attribute exists
    template <typename T>
    bool hasAttribute(T &object, const typename traits::attribute_traits<T>::key_type &key)
    {
        return traits::attribute_access<T>::hasAttribute(object, key);
    } 

    //! Get an attribute (or defaultValue if it does not exist)
    template <typename T>
    typename traits::attribute_traits<T>::value_type getAttribute(T &object, const typename traits::attribute_traits<T>::key_type &key, const typename traits::attribute_traits<T>::value_type &defaultValue = traits::attribute_traits<T>::value_type())
    {
        return traits::attribute_access<T>::getAttribute(object, key, defaultValue);
    } 

    //! Set an attribute
    template <typename T>
    void setAttribute(T &object, const typename traits::attribute_traits<T>::key_type &key, const typename traits::attribute_traits<T>::value_type &value)
    {
        traits::attribute_access<T>::setAttribute(object, key, value);
    } 

    //! Remove an attribute
    template <typename T>
    void removeAttribute(T &object, const typename traits::attribute_traits<T>::key_type &key)
    {
        traits::attribute_access<T>::removeAttribute(object, key);
    }

    //! Remove all attributes
    template <typename T>
    void clearAttributes(T &object)
    {
        traits::attribute_access<T>::clearAttributes(object);
    }

    /*! Find a string attribute name by case-insensitive comparison.
    
    This returns the case-sensitive match if it exists.
    If it doesn't, it attempts to find a case-insensitive match.
    If it still fails to find a match, it will return defaultKey.
    */
    std::string findAttribute(const std::set<std::string> &keys, const std::string &key, const std::string &defaultKey = std::string());

    //--------------------------------------------------------------------------------
    // implementation

    namespace base
    {
        /*! \brief Template base class for key-value attribute implementations

        Note that this implementation provides a <tt>hierarchy</tt> struct for class hierarchy implementations
        */
        template <typename K, typename V>
        class attributes_base
        {
        public:
            struct hierarchy { };

            typedef K key_type;
            typedef V value_type;
            typedef typename std::map<key_type, value_type> attribute_map;
            typedef typename attribute_map::value_type attribute;

        private:
            attribute_map attributes;

        public:
            // using default copy constructor/operator

            virtual ~attributes_base(void)
            {
            }

            //! Copy attributes from a matching attributes_base object
            virtual void copy(const attributes_base<K, V> &rhs)
            {
                attributes = rhs.attributes;
            }

            //! Get a set of attribute keys
            virtual std::set<key_type> getAttributeKeys(void) const
            {
                std::set<key_type> result;
                for(attribute_map::const_iterator it = attributes.begin(), end = attributes.end(); it != end; ++it)
                    result.insert(it->first);
                return result;
            }

            //! Returns true if the attribute exists
            virtual bool hasAttribute(const key_type &key) const
            {
                return (attributes.find(key) != attributes.end());
            }

            //! Get an attribute (or defaultValue if it does not exist)
            virtual V getAttribute(const key_type &key, const value_type &defaultValue = value_type()) const
            {
                attribute_map::const_iterator it = attributes.find(key);
                return (it != attributes.end()) ? it->second : defaultValue;
            }

            //! Set an attribute
            virtual void setAttribute(const key_type &key, const value_type &value)
            {
                attributes[key] = value;
            }

            //! Remove an attribute
            virtual void removeAttribute(const key_type &key)
            {
                attributes.erase(key);
            }

            //! Remove all attributes
            virtual void clearAttributes(void)
            {
                attributes.clear();
            }

        };
    }

    namespace traits
    {
        template <typename K, typename V>
        struct attribute_traits<base::attributes_base<K, V> >
        {
            typedef K key_type;
            typedef V value_type;
        };

        template <typename K, typename V>
        struct attribute_access<base::attributes_base<K, V> >
        {
            static std::set<K> getAttributeKeys(base::attributes_base<K, V> &object) { return object.getAttributeKeys(); }
            static bool hasAttribute(base::attributes_base<K, V> &object, const K &key) { return object.hasAttribute(key); } 
            static V getAttribute(base::attributes_base<K, V> &object, const K &key, const V &defaultValue = V()) { return object.getAttribute(key, defaultValue); } 
            static void setAttribute(base::attributes_base<K, V> &object, const K &key, const V &value) { object.setAttribute(key, value); } 
            static void removeAttribute(base::attributes_base<K, V> &object, const K &key) { object.removeAttribute(key); }
            static void clearAttributes(base::attributes_base<K, V> &object) { object.clearAttributes(); }
        };
    }

}
