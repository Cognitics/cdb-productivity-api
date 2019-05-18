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
#include <pthread.h>
#include <signal.h>

namespace ccl
{
	void * Runnable_start(void *obj);

	class Runnable_pthread : public Runnable_impl
	{
	private:
	  pthread_t thread;
	  int threadId; // in POSIX, this is just 0 for not started and 1 for started.
	public:
		~Runnable_pthread(void);
		Runnable_pthread(Runnable *shell);

		virtual bool start(void);
		virtual bool pause(void);
		virtual bool stop(void);
		virtual bool kill(void);
	};

	Runnable_pthread::~Runnable_pthread(void)
	{
		if(threadId)
			kill();
		threadId = 0;
	}

	Runnable_pthread::Runnable_pthread(Runnable *shell) : Runnable_impl(shell), threadId(0)
	{
	}

	bool Runnable_pthread::start(void)
	{
	  /*
		if(threadId)
			return (ResumeThread(threadId) >= 0);
	  */

	  int ret = pthread_create(&thread,NULL, Runnable_start, (void *)this);
		if(ret==0)
		  {
		    threadId = 1;
		    return true;
		  }
		return false;
	}

	bool Runnable_pthread::pause(void)
	{
	  return false;// currently unimplemented.
	}

	bool Runnable_pthread::stop(void)
	{
		return kill();
	}

	bool Runnable_pthread::kill(void)
	{
	  int result = pthread_kill(thread,SIGKILL);
	  threadId = 0;
	  return (result==0);
	}

	Runnable::~Runnable(void)
	{
		delete impl;
	}

	Runnable::Runnable(void) : state(INACTIVE), returnCode(0)
	{
		impl = new Runnable_pthread(this);
	}

	void * Runnable_start(void *obj)
	{
		((Runnable_pthread *)obj)->shell->run();
		return 0;
	}


}
