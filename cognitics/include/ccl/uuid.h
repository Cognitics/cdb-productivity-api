/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/
#pragma once

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <string>
#include <iostream>

namespace ccl
{
    typedef boost::uuids::uuid uuid;
}

namespace cognitics
{
    namespace uuid
    {
        template <typename T> struct traits { };

        template <typename T> inline bool is_nil(const T &id) { return traits<T>::is_nil(id); }
        template <typename T> inline bool are_equal(const T &id1, const T &id2) { return traits<T>::are_equal(id1, id2); }
        template <typename T> inline bool is_less_than(const T &id, const T &other) { return traits<T>::is_less_than(id, other); }
        template <typename T> inline std::string to_string(const T &id) { return traits<T>::to_string(id); }
        template <typename T> struct string_generator { T operator()(const std::string &str) const { return traits<T>::string_generator()(str); } };
        template <typename T> struct nil_generator { T operator()() const { return traits<T>::nil_generator()(); } };
        template <typename T> struct random_generator { T operator()() const { return traits<T>::random_generator()(); } };
        template <typename T> inline void seed(unsigned int s) { return traits<T>::seed(s); }

        //--------------------------------------------------------------------------------
        // utility function for testing implementations

        template <typename T>
        bool test(void)
        {
            std::string str = "12345678-1234-1234-1234-12345678abcd";
            std::string str_less = "12345678-1234-1234-1234-000000000000";

            if(!is_nil<T>(nil_generator<T>()()))
                return false;
            if(is_nil<T>(string_generator<T>()(str)))
                return false;
            if(is_nil<T>(random_generator<T>()()))
                return false;

            if(!are_equal(string_generator<T>()(str), string_generator<T>()(str)))
                return false;
            if(!is_less_than(string_generator<T>()(str_less), string_generator<T>()(str)))
                return false;

            if(are_equal(random_generator<T>()(), random_generator<T>()()))
                return false;

            seed<T>(0);
            T rand_uuid_a = random_generator<T>()();
            seed<T>(0);
            T rand_uuid_b = random_generator<T>()();
            if(!are_equal(rand_uuid_a, rand_uuid_b))
                return false;

            if(to_string(string_generator<T>()(str)) != str)
               return false;

            return true;
        }

        //--------------------------------------------------------------------------------
        // traits implementation for boost::uuids::uuid

        extern boost::random::mt19937 boost_random_mt19937;
        extern boost::uuids::basic_random_generator<boost::random::mt19937> boost_uuids_random_generator;

        template <>
        struct traits<boost::uuids::uuid>
        {
            static inline bool is_nil(const boost::uuids::uuid &id) { return id.is_nil(); }
            static inline bool are_equal(const boost::uuids::uuid &id1, const boost::uuids::uuid &id2) { return (id1 == id2); }
            static inline bool is_less_than(const boost::uuids::uuid &id, const boost::uuids::uuid &other) { return (id < other); }
            static inline std::string to_string(const boost::uuids::uuid &id) { return boost::uuids::to_string(id); }
            struct string_generator { boost::uuids::uuid operator()(const std::string &str) const; };
            struct nil_generator { boost::uuids::uuid operator()() const; };
            struct random_generator { boost::uuids::uuid operator()() const; };
            static void seed(unsigned int s);
        };

        //--------------------------------------------------------------------------------

    }
}

namespace ccl
{
    inline uuid GenerateRandomUUID() { return cognitics::uuid::random_generator<uuid>()(); }
}
