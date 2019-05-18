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
/*! \file ccl/LogBuffer.h
\headerfile ccl/LogBuffer.h
\brief Provides ccl::LogBuffer
\author Aaron Brinton <abrinton@cognitics.net>
\date 27 May 2011
*/
#pragma once

#include "Log.h"
#include <queue>

namespace ccl
{
    struct LogEntry
    {
        bool valid;
        struct LOGLEVEL level;
        std::string str;
        LogEntry(void) : valid(false) { }
        LogEntry(struct LOGLEVEL level, const std::string &str) : valid(true), level(level), str(str) { }
    };

    class LogBuffer : public LogObserver
    {
    private:
        struct LOGLEVEL level;        //!< maximum log level for stream output
        std::queue<LogEntry> entries;

    public:
        virtual ~LogBuffer(void);
        LogBuffer(struct LOGLEVEL level);
        virtual void write(struct LOGLEVEL level, const std::string &str);
        LogEntry getNextEntry(void);

    };

}
