/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.
****************************************************************************/

#include <ccl/uuid.h>
#include <ccl/mutex.h>

namespace cognitics
{
    namespace uuid
    {
        boost::random::mt19937 boost_random_mt19937;
        boost::uuids::basic_random_generator<boost::random::mt19937> boost_uuids_random_generator;

        namespace
        {
            ccl::mutex boost_uuids_random_mutex;
        }

        boost::uuids::uuid traits<boost::uuids::uuid>::string_generator::operator()(const std::string &str) const
        {
            return boost::uuids::string_generator()(str);
        }

        boost::uuids::uuid traits<boost::uuids::uuid>::nil_generator::operator()(void) const
        {
            return boost::uuids::nil_generator()();
        }

        boost::uuids::uuid traits<boost::uuids::uuid>::random_generator::operator()() const
        {
            ccl::scoped_mutex m(&boost_uuids_random_mutex);
            return boost_uuids_random_generator();
        }

        void traits<boost::uuids::uuid>::seed(unsigned int s)
        {
            boost_random_mt19937.seed(s);
            boost_uuids_random_generator = boost::uuids::basic_random_generator<boost::random::mt19937>(boost_random_mt19937);
        }
    }
}

