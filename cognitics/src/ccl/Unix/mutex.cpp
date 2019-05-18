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
#include <pthread.h>

namespace ccl
{
	class mutex_pthread : public mutex_impl
	{
	private:
	  pthread_mutex_t _mutex;

	public:
		~mutex_pthread(void);
		mutex_pthread(void);

		virtual void lock(void);
		virtual bool try_lock(void);
#ifndef APPLE
		virtual bool try_lock_for(boost::int32_t milliSeconds);
#endif
		virtual void unlock(void);
	};

	mutex_pthread::~mutex_pthread(void)
	{
	  pthread_mutex_destroy(&_mutex);
	}

	mutex_pthread::mutex_pthread(void)
	{
	  pthread_mutex_init(&_mutex,NULL);
	}

	void mutex_pthread::lock(void)
	{
	  pthread_mutex_lock(&_mutex);
	}

	bool mutex_pthread::try_lock(void)
	{
	  int result = pthread_mutex_trylock(&_mutex);
	  return (result == 0);
	}
#ifndef APPLE
	bool mutex_pthread::try_lock_for(boost::int32_t milliseconds)
	{
      struct timespec ts;
	  ts.tv_sec = milliseconds / 1000;
	  ts.tv_nsec = milliseconds % (1000000L);
	  int result = pthread_mutex_timedlock(&_mutex,&ts);
	  return (result == 0);
	}
#endif

	void mutex_pthread::unlock(void)
	{
          pthread_mutex_unlock(&_mutex);
	}


        void mutex::lock(void) 
        { 
	  impl->lock(); 
	}
		
        bool mutex::try_lock(void) 
        { 
          return impl->try_lock(); 
        }
#ifndef APPLE
	bool mutex::try_lock_for(boost::int32_t milliseconds) 
	{ 
	  return impl->try_lock_for(milliseconds); 
	}
#endif
       	void mutex::unlock(void) 
	{ 
	  impl->unlock(); 
	}

	mutex::~mutex(void)
	{
		delete impl;
	}

	mutex::mutex(void) : impl(new mutex_pthread())
	{
	}

}
