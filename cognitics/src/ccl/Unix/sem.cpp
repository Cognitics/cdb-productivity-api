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
#include <sem.h> 
#include <semaphore.h>

namespace ccl
{
	class semaphore_pthread : public semaphore_impl
	{
	private:
	  sem_t _semaphore;

	public:
		~semaphore_pthread(void);
		semaphore_pthread(boost::int32_t milliseconds);

		virtual bool wait(boost::int32_t milliseconds);
		virtual void signal(void);
		virtual void signal(long);
	};

	semaphore_pthread::~semaphore_pthread(void)
	{
	  sem_destroy(&_semaphore);
	}

	semaphore_pthread::semaphore_pthread(boost::int32_t maxCount)
	{
	  sem_init(&_semaphore,false,maxCount);
	}

	bool semaphore_pthread::wait(boost::int32_t milliseconds)
	{
	  if(milliseconds==INFINITE)
	    {
	      int result = sem_wait(&_semaphore);
	      return result==0;
	    }
	  else
	    {
	      struct timespec ts;
	      ts.tv_sec = milliseconds / 1000;
	      ts.tv_nsec = milliseconds % (1000000L);
	      int result = sem_timedwait(&_semaphore,&ts);
	      return result==0;
	    }
	}

	void semaphore_pthread::signal(void)
	{
	  sem_post(&_semaphore);
	}

	void semaphore_pthread::signal(long count)
	{
	  for(long i=0;i<count;i++)
	    sem_post(&_semaphore);
	}

	semaphore::~semaphore(void)
	{
		delete impl;
	}

	semaphore::semaphore(boost::int32_t maxCount) : impl(new semaphore_pthread(maxCount))
	{
	}



}
