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
/*! \file ipc/Runnable.h
\headerfile ipc/Runnable.h
\brief Provides ccl::Runnable.
\author Aaron Brinton <abrinton@cognitics.net>
\date 20 June 2009
*/
#pragma once

#include <boost/cstdint.hpp>
#include <string>

namespace ccl
{
    class Runnable;

    //! Runnable implementation base class.
    class Runnable_impl
    {
    public:
        Runnable *shell;

        Runnable_impl(Runnable *shell) : shell(shell) { }

        virtual bool start(void) = 0;
        virtual bool pause(void) = 0;
        virtual bool stop(void) = 0;
        virtual bool kill(void) = 0;

    };

/*! \class ccl::Runnable Runnable.h ccl/Runnable.h
\brief CCL Runnable class

\code
class MyThread : public Runnable
{
protected:
    virtual ~MyThread(void);    // enforce heap allocation

public:
    virtual int run(void)
    {
        // do thread processing here

        if(something failed)
        {
            state = FAILURE;
            return -1;
        }

        return Runnable::run();        // default implementation sets state to FINISHED and returns 0
    }
};

int main(int argc, char **argv)
{
    MyThread *myThread = new MyThread();

    if(!myThread->start())
        throw std::runtime_error("thread failed to start");

    // do main processing here

    // highly inefficient example of waiting on the thread
    while(myThread->isRunning());

    // thread is done running here

    myThread->destroy();
}
\endcode

\warning Runnable instances MUST be allocated on the heap.
*/
    class Runnable
    {
        friend class Runnable_impl;

    private:
        Runnable_impl *impl;        
        std::string error;
        int returnCode;

        Runnable(const Runnable &);
        Runnable &operator=(const Runnable &);

    protected:
        void setError(const std::string &error) { this->error = error; }
        int state;
        virtual ~Runnable(void);    // enforce heap allocation

    public:
        static const int FAILURE            = -1;
        static const int INACTIVE            =  0;
        static const int PENDING_RUN        =  1;
        static const int RUNNING            =  2;
        static const int PENDING_PAUSE        =  3;
        static const int PAUSED                =  4;
        static const int PENDING_STOP        =  5;
        static const int STOPPED            =  6;
        static const int FINISHED            =  7;

        void destroy(void) { delete this; }

        Runnable(void);

        virtual int run(void)
        {
            state = FINISHED;
            return 0;
        }

        virtual bool start(void)
        {
            if(impl->start())
            {
                state = RUNNING;
                return true;
            }
            return false;
        }

        virtual bool pause(void)
        {
            if(impl->pause())
            {
                state = PAUSED;
                return true;
            }
            return false;
        }

        virtual bool stop(void)
        {
            state = STOPPED;
            return impl->stop();
        }

        virtual bool kill(void)
        {
            state = STOPPED;
            return impl->kill();
        }

        int getState(void) { return state; }
        std::string getError(void) { return error; }
        int getReturnCode(void) { return returnCode; }

        bool isRunning(void) { return state == RUNNING; }
        bool isPaused(void) { return state == PAUSED; }
        bool isStopped(void) { return state == STOPPED; }
        bool isFinished(void) { return state == FINISHED; }

    };

}