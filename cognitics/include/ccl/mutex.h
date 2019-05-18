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
/*! \file ipc/mutex.h
\headerfile ipc/mutex.h
\brief Provides ccl::mutex.
\author Aaron Brinton <abrinton@cognitics.net>
\date 20 June 2009
*/
#pragma once

#include <boost/cstdint.hpp>
#include <map>

namespace ccl
{
    //! Mutex implementation base class.
    class mutex_impl
    {
    public:
        virtual void lock(void) = 0;
        virtual bool try_lock(void) = 0;
#ifndef APPLE
        virtual bool try_lock_for(boost::int32_t milliseconds) = 0;
#endif
        virtual void unlock(void) = 0;
    };

    //! CCL mutex class
    class mutex
    {
    private:
        mutex_impl *impl;

        mutex(const mutex &);
        mutex &operator=(const mutex &);

    public:
        ~mutex(void);
        mutex(void);

        void lock(void);
        bool try_lock(void);
#ifndef APPLE
        bool try_lock_for(boost::int32_t milliseconds);
#endif
        void unlock(void);

    };

    //! Scoped mutex class
    class scoped_mutex
    {
    private:
        mutex *m;

    public:
        ~scoped_mutex(void)
        {
            m->unlock();
        }

        scoped_mutex(mutex *m) : m(m)
        {
            m->lock();
        }

    };

}
