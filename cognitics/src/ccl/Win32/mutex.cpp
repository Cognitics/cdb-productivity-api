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

#include "ccl/mutex.h"

#include <windows.h>

namespace ccl
{
    class mutex_win32 : public mutex_impl
    {
    private:
        //HANDLE _mutex;

        CRITICAL_SECTION _cs;
        
    public:
        virtual ~mutex_win32(void);
        mutex_win32(void);

        virtual void lock(void);
        virtual bool try_lock(void);
        virtual bool try_lock_for(boost::int32_t milliSeconds);
        virtual void unlock(void);
    };

    mutex_win32::~mutex_win32(void)
    {
        //CloseHandle(_cs);
        DeleteCriticalSection(&_cs);
    }

    mutex_win32::mutex_win32(void)
    {
        InitializeCriticalSection(&_cs);
        //_mutex = CreateMutex(NULL, FALSE, NULL);
        
    }

    void mutex_win32::lock(void)
    {
        try_lock_for(INFINITE);
    }

    bool mutex_win32::try_lock(void)
    {
        return try_lock_for(0);
    }

    bool mutex_win32::try_lock_for(boost::int32_t milliseconds)
    {
        //DWORD result = WaitForSingleObject(_mutex, milliseconds);
        //return (result == WAIT_OBJECT_0);
        EnterCriticalSection(&_cs);
        return true;
    }

    void mutex_win32::unlock(void)
    {
        //ReleaseMutex(_mutex);
        LeaveCriticalSection(&_cs);
    }

        void mutex::lock(void) 
        { 
      impl->lock(); 
    }
        
        bool mutex::try_lock(void) 
        { 
          return impl->try_lock(); 
        }

    bool mutex::try_lock_for(boost::int32_t milliseconds) 
    { 
      return impl->try_lock_for(milliseconds); 
    }

           void mutex::unlock(void) 
    { 
      impl->unlock(); 
    }

    mutex::~mutex(void)
    {
        delete impl;
    }

    mutex::mutex(void) : impl(new mutex_win32())
    {
    }

}
