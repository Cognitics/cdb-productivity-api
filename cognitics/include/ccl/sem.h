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
/*! \file ipc/sem.h
\headerfile ipc/sem.h
\brief Provides ccl::semaphore.
\author Aaron Brinton <abrinton@cognitics.net>
\date 20 June 2009
*/
#pragma once

#include <boost/cstdint.hpp>

namespace ccl
{
#ifndef WIN32
#define INFINITE 0xFFFFFFFF
#endif
    //! Semaphore implementation base class.
    class semaphore_impl
    {
    public:
        virtual bool wait(boost::int32_t milliseconds) = 0;
        virtual void signal(long count) = 0;
    };

    //! Provides a semaphore implementation based on the platform.
    class semaphore
    {
    private:
        semaphore_impl *impl;

        semaphore(const semaphore &);
        semaphore &operator=(const semaphore &);

    public:
        ~semaphore(void);
        semaphore(boost::int32_t maxCount = 0);

        bool wait(boost::int32_t milliseconds)
        {
            return impl->wait(milliseconds);
        }

        void signal(long count = 1)
        {
            impl->signal(count);
        }

    };

}
