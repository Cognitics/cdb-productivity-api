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
#include <unistd.h>
#include <sys/time.h>

namespace ccl
{
	class Timer_unix : public Timer_impl
	{
	public:
		virtual ccl::uint64_t get(void);
	  ~Timer_unix(void) {}
	  Timer_unix(void) {}
	};

	ccl::uint64_t Timer_unix::get(void)
	{
		struct timeval tv;
		if(gettimeofday(&tv, NULL) == 0)
		{
			return (tv.tv_sec * 1000 * 1000) + tv.tv_usec;
		}
		return 0;
	}

	void sleep(ccl::uint32_t milliSeconds)
	{
		usleep(milliSeconds * 1000);
	}

	Timer::~Timer(void)
	{
	}

	Timer::Timer(void) : impl(new Timer_unix())
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
