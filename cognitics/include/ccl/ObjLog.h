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
/*! \file ccl/ObjLog.h
\headerfile ccl/ObjLog.h
\brief Object logging class using ccl::Log.
\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009
*/
#pragma once

#include "Log.h"
#include "LittleEndian.h"
#include "BigEndian.h"
#include <sstream>
#include "mutex.h"

namespace ccl
{
    //! Object logging class using ccl::Log.
    class ObjLog
    {
    protected:
        std::string prefix;            //!< log prefix string
        std::stringstream str;        //!< internal stream
        struct LOGLEVEL level;        //!< current stream log level
        std::string test;            //!< current stream PASS/FAIL status
        ccl::mutex _mutex;

    public:
        //! Terminates a log stream.
        struct nl {} endl;

        //! Create an object log stream.
        ObjLog(void);
        ObjLog(const char *name, void *obj = NULL);

        //! << template for stream output.
        template<typename T>
        ObjLog &operator<<(const T &t)
        {
            _mutex.lock();
            str << t;
            _mutex.unlock();
            return *this;
        }

        //! << operator for setting log level.
        ObjLog &operator<<(const struct LOGLEVEL &level);

        //! << operator for endl stream termination.
        ObjLog &operator<<(const nl &);

        template<typename T>
        ObjLog &operator<<(ccl::LittleEndian<T> &value)
        {
            str << T(value);
            return *this;
        }

        template<typename T>
        ObjLog &operator<<(ccl::BigEndian<T> &value)
        {
            str << T(value);
            return *this;
        }

        //! Initialize the object log stream.
        /*! The \c name and \c obj parameters are combined to form the prefix as \c "name[obj]: ", with \c obj as the hex pointer value. */
        void init(const char *name, void *obj = NULL);

    };

}

