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

#include "ccl/sem.h"
#include <windows.h>

namespace ccl
{
    class semaphore_win32 : public semaphore_impl
    {
    private:
        HANDLE _semaphore;

    public:
        virtual ~semaphore_win32(void);
        semaphore_win32(boost::int32_t milliseconds);

        virtual bool wait(boost::int32_t milliseconds);
        virtual void signal(long count);
    };

    semaphore_win32::~semaphore_win32(void)
    {
        CloseHandle(_semaphore);
    }

    semaphore_win32::semaphore_win32(boost::int32_t maxCount)
    {
        _semaphore = CreateSemaphore(NULL, 0, (maxCount > 0) ? maxCount : 65535, NULL);
    }

    bool semaphore_win32::wait(boost::int32_t milliseconds)
    {
        DWORD result = WaitForSingleObject(_semaphore, milliseconds);
        return (result == WAIT_OBJECT_0);
    }

    void semaphore_win32::signal(long count)
    {
        ReleaseSemaphore(_semaphore, count, NULL);
    }

    semaphore::~semaphore(void)
    {
        delete impl;
    }

    semaphore::semaphore(boost::int32_t maxCount) : impl(new semaphore_win32(maxCount))
    {
    }



}