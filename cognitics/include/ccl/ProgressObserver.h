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
/*! \file ccl/ProgressObserver.h
\headerfile ccl/ProgressObserver.h
\brief Provides ccl::ProgressObserver.
\author Aaron Brinton <abrinton@cognitics.net>
\date 08 June 2011
*/
#pragma once

#include <string>
#include "ccl/ObjLog.h"
#include "ccl/Timer.h"

namespace ccl
{
    class ProgressObserver
    {
    private:
        Timer timer;
        unsigned long index;
        unsigned long total;

    protected:
        virtual void onStart(unsigned long count);
        virtual void onUpdate(unsigned long progress);
        virtual void onFinish(void);
        virtual void onMaximumChanged(unsigned long maximum);

        unsigned long getCurrentIndex(void);
        float getTimeElapsed(void);
        float getTimeRemaining(void);

    public:
        virtual ~ProgressObserver(void);
        ProgressObserver(void);

        void start(unsigned long count);
        void update(unsigned long step = 1);
        void finish(void);

        void setValue(unsigned long value);
        unsigned long getValue(void);

        void setMaximum(unsigned long maximum);
        unsigned long getMaximum(void);
        void addToMaximum(unsigned long addAmount);

        virtual void setTitle(const std::string &title);
        virtual void setText(const std::string &text);

    };

    class LogProgressObserver : public ProgressObserver
    {
    private:
        std::string title;
        ccl::Timer timer;
        ccl::ObjLog log;

    public:
        virtual ~LogProgressObserver(void);
        LogProgressObserver(void);
        virtual void setTitle(const std::string &title);

        virtual void onStart(unsigned long count);
        virtual void onUpdate(unsigned long progress);
        virtual void onFinish(void);
    };


}