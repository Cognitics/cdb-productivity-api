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
/*! \file ccl/datetime.h
\headerfile ccl/datetime.h
\brief Provides ccl::datetime, ccl::date, and ccl::time
\author Aaron Brinton <abrinton@cognitics.net>
\date 07 February 2011
*/
#pragma once

#include "primitive.h"
#include "date.h"
#include "ccltime.h"

namespace ccl
{
    class datetime : public primitive
    {
    private:
        date d;
        time t;

    public:
        virtual ~datetime(void);
        datetime(void);
        datetime(int16_t year, uint16_t month = 0, uint16_t day = 0, uint16_t hour = 0, uint16_t minute = 0, uint16_t second = 0, uint32_t fraction = 0);
        datetime(const date &d, const time &t = time());
        datetime(const time &t);

        date getDate(void) const;
        void setDate(const date &d);
        int16_t getYear(void) const;
        void setYear(int16_t year);
        uint16_t getMonth(void) const;
        void setMonth(uint16_t month);
        uint16_t getDay(void) const;
        void setDay(uint16_t day);

        time getTime(void) const;
        void setTime(const time &t);
        uint16_t getHour(void) const;
        void setHour(uint16_t hour);
        uint16_t getMinute(void) const;
        void setMinute(uint16_t minute);
        uint16_t getSecond(void) const;
        void setSecond(uint16_t second);
        uint32_t getFraction(void) const;
        void setFraction(uint32_t fraction);

        virtual std::string getClassName(void) const;
        virtual std::string getClassDescription(void) const;
        virtual std::string getSuperClassName(void) const;
        virtual object *clone(void) const;

        virtual int8_t as_int8_t(void) const;
        virtual uint8_t as_uint8_t(void) const;
        virtual int16_t as_int16_t(void) const;
        virtual uint16_t as_uint16_t(void) const;
        virtual int32_t as_int32_t(void) const;
        virtual uint32_t as_uint32_t(void) const;
        virtual int64_t as_int64_t(void) const;
        virtual uint64_t as_uint64_t(void) const;
        virtual float as_float(void) const;
        virtual double as_double(void) const;
        virtual long double as_long_double(void) const;
        virtual datetime as_datetime(void) const;
        virtual date as_date(void) const;
        virtual time as_time(void) const;
        virtual std::string as_std_string(void) const;
        virtual std::wstring as_std_wstring(void) const;

    };

}