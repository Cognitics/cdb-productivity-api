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
/*! \file ccl/Timer.h
\headerfile ccl/Timer.h
\brief Provides ccl::Timer.
\author Aaron Brinton <abrinton@cognitics.net>
\date 20 June 2009

\page timer_page Cognitics Timer Library
\ref ccl_page

\section Description

The Timer library provides a timer for measuring elapsed time.

There are two methodologies for use of this class:
\li A single instance as a stack-based timer list, using push() and pop() to mark times and retrieve differences.
\li An instance as a single timer, using startTimer(), resetTimer() and getElapsedTime().

\section Usage

Stack-based example:
\code
Timer myTimer;
myTimer.push();        // push the current time marker (A)
for(int i = 0; i < count; ++i)
{
    myTimer.push();        // push the current time marker again (B)
    // do some measured time process (B) here
    int64_t elapsedB = myTimer.pop();    // elapsedB contains the microseconds elapsed for the B process
}
int64_t elapsedA = myTimer.pop();    // elapsedA contains the microseconds elapsed for entire loop (A)
\endcode

Single timer example:
\code
Timer myTimer;
myTimer.startTimer();
// do some measured process (A) here
float elapsedA = myTimer.getElapsedTime();    // elapsedA contains the number of seconds (including fraction) elapsed for process A
// do more processing (B) here
float elapsedAB = myTimer.getElapsedTime();    // elapsedAB contains the number of seconds (including fraction) elapsed for processes A and B
myTimer.resetTimer();
// do more processing (C) here
float elapsedC = myTimer.getElapsedTime();    // elapsedC contains the number of seconds (including fraction) elapsed for process C
\endcode

*/
#pragma once

#include <vector>
#include "cstdint.h"

namespace ccl
{
    //! Sleep for a number of milliseconds using the platform's sleep() implementation.
    void sleep(ccl::uint32_t milliSeconds);

    //! Timer implementation base class.
    class Timer_impl
    {
    public:
        virtual ccl::uint64_t get(void) = 0;    //! < microsecond (usec) timer value
    };

    //! Timer class using the platform's time functionality.
    class Timer
    {
    private:
        Timer_impl *impl;

    protected:
        std::vector<ccl::uint64_t> timeStack;
        ccl::uint64_t timerStarted;

    public:
        virtual ~Timer(void);
        Timer(void);

/*! \brief Get a time marker.
\return A 64-bit integer value; varies by platform.
*/
        virtual ccl::uint64_t get(void)
        {
            return impl->get();
        }

//! \brief Push the current time marker onto the timer stack.
        virtual void push(void)
        {
            timeStack.push_back(get());
        }

/*! \brief Pop a time marker from the stack and return the difference.
\return A floating point value with the difference in seconds between the popped time marker and current time marker.
*/
        virtual float pop(void)
        { 
            float delta = 0;
            if(!timeStack.empty())
            {
                delta = float(get() - timeStack.back()) / 1000000.0f;
                timeStack.pop_back();
            }
            return delta;
        }

/*! \brief Start a timer 'now'.

Future calls to getElapsedTime will return the number of seconds (as a float) since calling startTimer() or resetTimer().
*/
        virtual void startTimer();

/*! \brief Get the elapsed time since startTimer() or resetTimer().
\return The number of seconds (as a float) since the last call to startTimer() or resetTimer().
*/
        virtual float getElapsedTime();

/*! \brief Restarts a timer 'now'.

Future calls to getElapsedTime will return the number of seconds (as a float) since calling startTimer() or resetTimer().
*/
        virtual void resetTimer();

    };

}