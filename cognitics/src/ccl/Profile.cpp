/****************************************************************************
Copyright (c) 2016 Cognitics, Inc.
****************************************************************************/

#include "ccl/Profile.h"
#include <iomanip>
#include <iostream>
#include <sstream>

namespace ccl
{
    Profiling *Profiling::_instance = NULL;
    _Profiling Profiling::_destroyer;

    Profiling::Profiling(void) : _enabled(false), _mutex_enabled(false)
    {
    }

    Profiling::~Profiling(void)
    {
        report();
    }

    Profiling *Profiling::instance(void)
    {
        if(_instance == NULL)
        {
            _instance = new Profiling;
            _destroyer.instance = _instance;
        }
        return _instance;
    }

    void Profiling::update(const std::string &name, double elapsed)
    {
        // we don't need to check for an instance since this is unreachable unless enabled
        if(_instance->_mutex_enabled)
            _instance->_mutex.lock();
        std::map<std::string, size_t>::iterator counts_it = _instance->_counts.find(name);
        if(counts_it == _instance->_counts.end())
        {
            _instance->_counts[name] = 1;
            _instance->_times[name] = elapsed;
        }
        else
        {
            _instance->_counts[name] += 1;
            _instance->_times[name] += elapsed;
        }
        if(_instance->_mutex_enabled)
            _instance->_mutex.unlock();
    }

    void Profiling::enable(void)
    {
        Profiling::instance()->_enabled = true;
    }

    void Profiling::disable(void)
    {
        Profiling::instance()->_enabled = false;
        Profiling::instance()->_mutex_enabled = false;
    }

    bool Profiling::enabled(void)
    {
        return Profiling::instance()->_enabled;
    }

    void Profiling::enable_with_mutex(void)
    {
        enable();
        Profiling::instance()->_mutex_enabled = true;
    }

    void Profiling::report(bool clear)
    {
        Profiling *instance = Profiling::instance();
        if(instance->_counts.empty())
            return;

        // collect data into sorted multimap
        std::multimap<double, std::string> sorted_times;
        if(instance->_mutex_enabled)
            instance->_mutex.lock();
        for(std::map<std::string, double>::iterator it = instance->_times.begin(), end = instance->_times.end(); it != end; ++it)
            sorted_times.insert(std::make_pair(it->second, it->first));
        if(instance->_mutex_enabled)
            instance->_mutex.unlock();

        // output the results
        std::cout << "================================================================================" << std::endl;
        std::cout << "Profiling Results:" << std::endl;
        for(std::multimap<double, std::string>::reverse_iterator it = sorted_times.rbegin(), end = sorted_times.rend(); it != end; ++it)
        {
            std::stringstream ss;
            ss << std::fixed << std::setw(11) << std::setprecision(4) << it->first;
            std::cout << "  " << ss.str() << "s : " << it->second << "  [" << instance->_counts[it->second] << " calls]" << std::endl;
        }
        std::cout << "================================================================================" << std::endl;

        if(clear)
            Profiling::clear();
    }

    void Profiling::clear(void)
    {
        Profiling *instance = Profiling::instance();
        if(instance->_mutex_enabled)
            instance->_mutex.lock();
        instance->_counts.clear();
        instance->_times.clear();
        if(instance->_mutex_enabled)
            instance->_mutex.unlock();
    }

    _Profiling::_Profiling(void) : instance(NULL)
    {
    }

    _Profiling::~_Profiling(void)
    {
        delete instance;
    }


    //--------------------------------------------------------------------------------

    _Profile::_Profile(const std::string &name) : name(name)
    {
        timer.startTimer();
    }

    _Profile::~_Profile(void)
    {
        Profiling::update(name, timer.getElapsedTime());
    }

    Profile::Profile(const std::string &name) : _profile(NULL)
    {
        if(Profiling::enabled())
            _profile = new _Profile(name);
    }

    Profile::~Profile(void)
    {
        delete _profile;
    }

}