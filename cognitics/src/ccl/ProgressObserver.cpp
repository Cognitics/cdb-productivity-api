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

#include "ccl/ProgressObserver.h"

namespace ccl
{
    void ProgressObserver::onStart(unsigned long count)
    {
    }

    void ProgressObserver::onUpdate(unsigned long progress)
    {
    }

    void ProgressObserver::onFinish(void)
    {
    }

    void ProgressObserver::onMaximumChanged(unsigned long maximum)
    {
    }

    unsigned long ProgressObserver::getCurrentIndex(void)
    {
        return index;
    }

    float ProgressObserver::getTimeElapsed(void)
    {
        return timer.getElapsedTime();
    }

    float ProgressObserver::getTimeRemaining(void)
    {
        if(index < 1)
            return 0.0f;
        float elapsed = getTimeElapsed();
        return ((elapsed / index) * total) - elapsed;
    }

    ProgressObserver::~ProgressObserver(void)
    {
    }

    ProgressObserver::ProgressObserver(void) : index(0), total(1)
    {
    }

    void ProgressObserver::start(unsigned long count)
    {
        index = 0;
        total = count;
        timer.startTimer();
        onStart(count);
    }

    void ProgressObserver::update(unsigned long step)
    {
        index += step;
        onUpdate(index);
    }

    void ProgressObserver::finish(void)
    {
        onFinish();
    }

    void ProgressObserver::setValue(unsigned long value)
    {
        index = value;
    }

    unsigned long ProgressObserver::getValue(void)
    {
        return index;
    }

    void ProgressObserver::setMaximum(unsigned long maximum)
    {
        total = maximum;
        onMaximumChanged(total);
    }

    unsigned long ProgressObserver::getMaximum(void)
    {
        return total;
    }

    void ProgressObserver::addToMaximum(unsigned long addAmount)
    {
        total += addAmount;
        onMaximumChanged(total);
    }

    void ProgressObserver::setTitle(const std::string &title)
    {
    }

    void ProgressObserver::setText(const std::string &text)
    {
    }

    LogProgressObserver::~LogProgressObserver(void)
    {
    }

    LogProgressObserver::LogProgressObserver(void)
    {
        log << ccl::LINFO;
    }

    void LogProgressObserver::setTitle(const std::string &title)
    {
        this->title = title;
    }

    void LogProgressObserver::onStart(unsigned long count)
    {
        ProgressObserver::onStart(count);
        timer.resetTimer();
        log << title << " starting..." << log.endl;
    }

    void LogProgressObserver::onUpdate(unsigned long progress)
    {
        ccl::ProgressObserver::onUpdate(progress);
        if(timer.getElapsedTime() > 5.0f)
        {
            timer.resetTimer();
            float pct = float(getValue()) / float(getMaximum());
            log << title << " " << (pct * 100.0) << "% complete..." << log.endl;                
        }
    }

    void LogProgressObserver::onFinish(void)
    {
        log << title << " finished." << log.endl;
    }

}