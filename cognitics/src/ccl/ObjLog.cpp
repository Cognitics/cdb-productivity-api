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

#include "ccl/ObjLog.h"

namespace ccl
{
    ObjLog::ObjLog(void) : prefix(""), str(""), level(LNOTICE)
    {
    }

    ObjLog::ObjLog(const char *name, void *obj) : prefix(""), str(""), level(LNOTICE)
    {
        init(name, obj);
    }

    ObjLog &ObjLog::operator<<(const struct LOGLEVEL &level)
    {
        this->level = level;
        return *this;
    }

    ObjLog &ObjLog::operator<<(const nl &)
    {
        _mutex.lock();
        std::string prestr = prefix;
        if(test.size())
            prestr += "\t" + test + "\t";
        if(this->level.value == LDEBUG.value)
            Log::instance()->write(this->level, prestr + str.str());
        else
            Log::instance()->write(this->level, str.str());
        test = "";
        str.str("");
        _mutex.unlock();
        return *this;
    }

    void ObjLog::init(const char *name, void *obj)
    {
        std::stringstream ss;
        ss << name;
        if(obj)
            ss << "[" << obj << "]";
        ss << ": ";
        _mutex.lock();
        prefix = ss.str();
        _mutex.unlock();
    }

}

