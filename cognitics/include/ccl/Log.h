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
/*! \file ccl/Log.h
\headerfile ccl/Log.h
\brief Provides ccl::Log and ccl::LogObserver
\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009

\page log_page Observer/Subscriber Logging
\ref ccl_page

\section Description

The Logging library provides a singleton observer-based logging class hierarchy using POSIX log levels.

It also provides a stream observer and object logging implementations.

It includes the following classes:
\li ccl::Log \copybrief ccl::Log
\li ccl::LogObserver \copybrief ccl::LogObserver
\li ccl::LogStream \copybrief ccl::LogStream
\li ccl::ObjLog \copybrief ccl::ObjLog

The POSIX log levels are implemented as follows:
\li <tt>[0] LEMERG</tt>: system is unusable
\li <tt>[1] LALERT</tt>: action must be taken immediately
\li <tt>[2] LCRIT</tt>: critical conditions
\li <tt>[3] LERR</tt>: error conditions
\li <tt>[4] LWARNING</tt>: warning conditions
\li <tt>[5] LNOTICE</tt>: normal but significant condition
\li <tt>[6] LINFO</tt>: informational
\li <tt>[7] LDEBUG</tt>: debug-level messages

\section Usage

The singleton Log class provides observer subscription and message logging by log level.

To write a message to all subscribed observers:
\code
ccl::Log::instance()->write(ccl::LNOTICE, "test log message");
\endcode

The LogStream class is a stream observer for attaching any output stream to the logging mechanism:
\code
#include <LogStream.h>

ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LWARNING)));    // std::cout is default
ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LERROR, std::cerr)));
\endcode

To create a custom observer, see the full implementation of LogStream in LogStream.h.

The ObjLog class provides prefixing functionality and a stream interface for sending log events.
It is designed to be used as a class member and initialized as in this example:
\code
class TestClass
{
private:
    ccl::ObjLog log;
public:
    TestClass()
    {
        log.init("TestClass", this);    // initialize with the name of the class and reference
        log << ccl::LDEBUG << "test log event" << log.endl;
    }
};
\endcode

\section Notes

It is the responsibility of observer implementations to handle filtering by log level.

*/
#pragma once

#include <set>
#include <string>
#include <memory>

#if defined(WIN32) && defined(COG_DEBUG)
#include <windows.h>
#endif

#include "inspection.h"

namespace ccl
{
    class Log;
    typedef std::shared_ptr<Log> LogSP;
    
    class LogObserver;
    typedef std::shared_ptr<LogObserver> LogObserverSP;

    //! POSIX log levels
    struct LOGLEVEL { char value; };
    const struct LOGLEVEL LEMERG    = { 0 };    //!< system is unusable
    const struct LOGLEVEL LALERT    = { 1 };    //!< action must be taken immediately
    const struct LOGLEVEL LCRIT        = { 2 };    //!< critical conditions
    const struct LOGLEVEL LERR        = { 3 };    //!< error conditions
    const struct LOGLEVEL LWARNING    = { 4 };    //!< warning conditions
    const struct LOGLEVEL LNOTICE    = { 5 };    //!< normal but significant condition
    const struct LOGLEVEL LINFO        = { 6 };    //!< informational
    const struct LOGLEVEL LDEBUG    = { 7 };    //!< debug-level messages

    //! Observer base class for ccl::Log.
    class LogObserver
    {
    public:
        virtual ~LogObserver(void) { }
        LogObserver(void) { }

        //! Log event handler.
        virtual void write(struct LOGLEVEL level, const std::string &str) = 0;
    };

    //! Singleton class for distributing log messages to registered observers.
    class Log
    {
    private:
        static LogSP _instance;
        std::set<LogObserverSP> observers;

    protected:
        Log(void) { }

        Log(const Log &);
        Log &operator=(const Log &);

    public:
        ~Log(void) { }

        //! Return the singleton log instance.
        static LogSP instance(void)
        {
            if(!_instance)
                _instance = LogSP(new Log());
            return _instance;
        }

        //! Attach an observer to the log.
        void attach(LogObserverSP observer)
        {
            observers.insert(observer);
        }

        //! Detach an observer from the log.
        void detach(LogObserverSP observer)
        {
            observers.erase(observers.find(observer));
        }

        //! Send a log string to all attached observers.
        void write(struct LOGLEVEL level, const std::string &str)
        {
#if defined(WIN32) && defined(COG_DEBUG)
            OutputDebugStringA(str.c_str());
            OutputDebugStringA("\n");
#endif
            for(std::set<LogObserverSP>::iterator it = observers.begin(), end = observers.end(); it != end; it++)
                (*it)->write(level, str);
        }
    };

}


