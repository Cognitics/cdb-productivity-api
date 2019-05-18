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

#include "ccl/Timer.h"
#include <windows.h>

#pragma comment(lib,"winmm.lib")

namespace ccl
{
    class Timer_win32 : public Timer_impl
    {
    public:
        virtual ccl::uint64_t get(void);
        virtual ~Timer_win32(void) { }
        Timer_win32(void) { }
    };

    ccl::uint64_t Timer_win32::get(void)
    {
        ccl::uint64_t value = timeGetTime();    // milliseconds; note that this wraps approximately every 49.71 days
        return value * 1000;
    }

    void sleep(ccl::uint32_t milliSeconds)
    {
        ::Sleep(milliSeconds);
    }

    Timer::~Timer(void)
    {
        if(impl)
            delete impl;
    }

    Timer::Timer(void) : timerStarted(0),impl(new Timer_win32())
    {
    }

    float Timer::getElapsedTime()
    {
        float delta = 0;
        delta = float(get() - timerStarted) / 1000000.0f;
        return delta;
    }

    void Timer::startTimer()
    {
        timerStarted = get();
    }

    void Timer::resetTimer()
    {
        timerStarted = get();
    }

}