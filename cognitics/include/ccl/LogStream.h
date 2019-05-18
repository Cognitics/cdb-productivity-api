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
/*! \file ccl/LogStream.h
\headerfile ccl/LogStream.h
\brief Provides ccl::LogStream
\author Aaron Brinton <abrinton@cognitics.net>
\date 16 June 2009
*/
#pragma once

#include "Log.h"
#include <iostream>

namespace ccl
{
    //! Stream observer class for ccl::Log.
    class LogStream : public LogObserver
    {
    private:
        struct LOGLEVEL level;        //!< maximum log level for stream output
        std::ostream *stream;        //!< output stream

    public:
        virtual ~LogStream(void);
        LogStream(struct LOGLEVEL level, std::ostream &stream = std::cout);

        //! Log event handler.
        /*! This sends the log string to the output stream if the event log level is less than (more critical) than the level property. */
        virtual void write(struct LOGLEVEL level, const std::string &str);

    };


}


