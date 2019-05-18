/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/

#pragma once

#include <string>
#include "ccl/Timer.h"
#include "ccl/mutex.h"
#include <map>
#include <boost/current_function.hpp>

//#define COGNITICS_PROFILING_ENABLED


namespace ccl
{
    /*
    ================================================================================
    This module provides a simple performance profiling tool.
    
    It consists of a singleton class and a user class.
    
    The singleton (ccl::Profiling) manages operation, including collection and
    reporting of data.
    
    Profiling is disabled by default. To enable in a normal environment, call the
    ccl::Profiling::enable() static method. For a multithreaded environment, the
    ccl::Profiling::enable_with_mutex() must be called to protect the internal data
    structures.

    Results are sent to ccl::Log on program termination, but may also be output
    using ccl::Profiling::report().
    

    To profile an operation, the ccl::Profile class is used. It is based on RAII
    (Resource Acquisition is Initialization), with the creation of the instance
    starting a timer and the destructor updating the singleton with the elapsed
    time.

    To minimize the impact of ccl::Profile when profiling is not desired, the
    implementation is entirely a wrapper around a private class that performs the
    timer and update operations. This reduces the ccl::Profile instantiation to
    only the pointer initialization to NULL and a check of ccl::Profiling::enabled().
    */

        class _Profile;
        class _Profiling;
    class Profiling
    {
        friend class _Profile;
        friend class _Profiling;
    private:
        static Profiling *_instance;
        static _Profiling _destroyer;
        bool _enabled;
        bool _mutex_enabled;
        mutex _mutex;

        // accumulated data
        std::map<std::string, size_t> _counts;
        std::map<std::string, double> _times;

        // private constructor/destructor (for singleton)
        Profiling(void);
        ~Profiling(void);

        // get the singleton instance pointer, creating it if necessary
        static Profiling *instance(void);

        // report method for profile objects
        static void update(const std::string &name, double elapsed);

    public:

        // enable/disable profile updates (disabled by default)
        // when enabled, the Profile class destructor will call update()
        // for multithreaded environments, use the mutex variation
        static void enable(void);
        static void disable(void);
        static bool enabled(void);

        // when enabled with this, a mutex is locked and unlocked around data access
        static void enable_with_mutex(void);

        // report the results; if clear is true, accumulated data is purged
        static void report(bool clear = false);

        // clear accumulated data
        static void clear(void);

    };

    // destroyer
    class _Profiling
    {
        friend class Profiling;
    private:
        _Profiling(void);
        ~_Profiling(void);
        Profiling *instance;
    };

    // private implementation class of Profile
    // this is only instantiated if Profiling::is_enabled()
    // when not enabled, the profile
    class _Profile
    {
        friend class Profile;
    private:
        _Profile(const std::string &name);
        ~_Profile(void);
        std::string name;
        Timer timer;
    };

    class Profile
    {
    private:
        _Profile *_profile;
    public:
        Profile(const std::string &name);
        ~Profile(void);
    };


}

#ifdef COGNITICS_PROFILING_ENABLED

#define COGNITICS_PROFILE_CONCAT_(a, b)                a##b
#define COGNITICS_PROFILE_CONCAT(a, b)                COGNITICS_PROFILE_CONCAT_(a, b)
#define COGNITICS_PROFILE_STRINGIFY_(s)                #s
#define COGNITICS_PROFILE_STRINGIFY(s)                COGNITICS_PROFILE_STRINGIFY_(s)

#define COGNITICS_PROFILE_VAR                        COGNITICS_PROFILE_CONCAT(cognitics_profile_line_, __LINE__)

#ifdef WIN32
#define COGNITICS_PROFILE_NAME                        std::string(__FUNCSIG__) + " [" + COGNITICS_PROFILE_STRINGIFY(__LINE__) + "]"
#else
#define COGNITICS_PROFILE_NAME                        std::string(__PRETTY_FUNCTION__) + " [" + COGNITICS_PROFILE_STRINGIFY(__LINE__) + "]"
#endif

#define COGNITICS_PROFILING_ENABLE                    ccl::Profiling::enable();
#define COGNITICS_PROFILING_ENABLE_WITH_MUTEX        ccl::Profiling::enable_with_mutex();

#define COGNITICS_PROFILE                            ccl::Profile COGNITICS_PROFILE_VAR(COGNITICS_PROFILE_NAME);
#define COGNITICS_PROFILE_NAMED(name)                ccl::Profile COGNITICS_PROFILE_VAR(COGNITICS_PROFILE_NAME + " : " + name);

#define COGNITICS_PROFILE_BEGIN(var)                ccl::Profile *(var) = new ccl::Profile(COGNITICS_PROFILE_NAME);
#define COGNITICS_PROFILE_BEGIN_NAMED(var, name)    ccl::Profile *(var) = new ccl::Profile(COGNITICS_PROFILE_NAME + " : " + name);
#define COGNITICS_PROFILE_END(var)                    delete (var);

#else

#define COGNITICS_PROFILING_ENABLE
#define COGNITICS_PROFILING_ENABLE_WITH_MUTEX

#define COGNITICS_PROFILE
#define COGNITICS_PROFILE_NAMED(name)

#define COGNITICS_PROFILE_BEGIN(var)
#define COGNITICS_PROFILE_BEGIN_NAMED(var, name)
#define COGNITICS_PROFILE_END(var)

#endif
