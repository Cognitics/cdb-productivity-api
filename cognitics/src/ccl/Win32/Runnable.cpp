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

#include "ccl/Runnable.h"
#include <windows.h>
#include <process.h>

namespace ccl
{
    unsigned int __stdcall Runnable_start(void *obj);

    class Runnable_win32 : public Runnable_impl
    {
    private:
        HANDLE threadId;

    public:
        ~Runnable_win32(void);
        Runnable_win32(Runnable *shell);

        virtual bool start(void);
        virtual bool pause(void);
        virtual bool stop(void);
        virtual bool kill(void);
    };

    Runnable_win32::~Runnable_win32(void)
    {
        if(threadId)
            kill();
        threadId = 0;
    }

    Runnable_win32::Runnable_win32(Runnable *shell) : Runnable_impl(shell), threadId(0)
    {
    }

    bool Runnable_win32::start(void)
    {
        if(threadId)
            return (ResumeThread(threadId) != (DWORD)-1);

        unsigned tid = 0;
        threadId = (HANDLE)_beginthreadex((void *)NULL, (unsigned)0, &Runnable_start, (void *)this, (unsigned)0, (unsigned *)&tid);
        return (threadId != (HANDLE)0);
    }

    bool Runnable_win32::pause(void)
    {
        return (SuspendThread(threadId) != (DWORD)-1);
    }

    bool Runnable_win32::stop(void)
    {
        return kill();
    }

    bool Runnable_win32::kill(void)
    {
        bool result = (TerminateThread(threadId, -1) == TRUE);
        threadId = 0;
        return result;
    }

    Runnable::~Runnable(void)
    {
        delete impl;
    }

    Runnable::Runnable(void) : state(INACTIVE), returnCode(0)
    {
        impl = new Runnable_win32(this);
    }

    unsigned int __stdcall Runnable_start(void *obj)
    {
        ((Runnable_win32 *)obj)->shell->run();
        return 0;
    }


}