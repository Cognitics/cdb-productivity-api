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

#pragma warning ( disable : 4244 )    // possible loss of data

#include "ccl/datetime.h"
#include <sstream>

namespace ccl
{
    datetime::~datetime(void)
    {
    }

    datetime::datetime(void)
    {
    }

    datetime::datetime(int16_t year, uint16_t month, uint16_t day, uint16_t hour, uint16_t minute, uint16_t second, uint32_t fraction) : d(year, month, day), t(hour, minute, second, fraction)
    {
    }

    datetime::datetime(const date &d, const time &t) : d(d), t(t)
    {
    }

    datetime::datetime(const time &t) : d(), t(t)
    {
    }

    date datetime::getDate(void) const
    {
        return d;
    }

    void datetime::setDate(const date &d)
    {
        this->d = d;
    }

    int16_t datetime::getYear(void) const
    {
        return d.getYear();
    }

    void datetime::setYear(int16_t year)
    {
        d.setYear(year);
    }

    uint16_t datetime::getMonth(void) const
    {
        return d.getMonth();
    }

    void datetime::setMonth(uint16_t month)
    {
        d.setMonth(month);
    }

    uint16_t datetime::getDay(void) const
    {
        return d.getDay();
    }

    void datetime::setDay(uint16_t day)
    {
        d.setDay(day);
    }

    time datetime::getTime(void) const
    {
        return t;
    }

    void datetime::setTime(const time &t)
    {
        this->t = t;
    }

    uint16_t datetime::getHour(void) const
    {
        return t.getHour();
    }

    void datetime::setHour(uint16_t hour)
    {
        t.setHour(hour);
    }

    uint16_t datetime::getMinute(void) const
    {
        return t.getMinute();
    }

    void datetime::setMinute(uint16_t minute)
    {
        t.setMinute(minute);
    }

    uint16_t datetime::getSecond(void) const
    {
        return t.getSecond();
    }

    void datetime::setSecond(uint16_t second)
    {
        t.setSecond(second);
    }

    uint32_t datetime::getFraction(void) const
    {
        return t.getFraction();
    }

    void datetime::setFraction(uint32_t fraction)
    {
        t.setFraction(fraction);
    }

    std::string datetime::getClassName(void) const
    {
        return "ccl::datetime";
    }

    std::string datetime::getClassDescription(void) const
    {
        return "CCL DateTime Object";
    }

    std::string datetime::getSuperClassName(void) const
    {
        return object::getClassName();
    }

    object *datetime::clone(void) const
    {
        return new datetime(getDate(), getTime());
    }

    int8_t datetime::as_int8_t(void) const
    {
        return as_int64_t();
    }

    uint8_t datetime::as_uint8_t(void) const
    {
        return as_int64_t();
    }

    int16_t datetime::as_int16_t(void) const
    {
        return as_int64_t();
    }

    uint16_t datetime::as_uint16_t(void) const
    {
        return as_int64_t();
    }

    int32_t datetime::as_int32_t(void) const
    {
        return as_int64_t();
    }

    uint32_t datetime::as_uint32_t(void) const
    {
        return as_int64_t();
    }

    int64_t datetime::as_int64_t(void) const
    {
        tm tmEpoch = { 0, 0, 0, 1, 0, 70, 0, 0, 0 };
        tm tmValue = { getSecond(), getMinute(), getHour(), getDay(), getMonth() - 1, getYear() - 1900, 0, 0, 0 };
        return difftime(mktime(&tmValue), mktime(&tmEpoch));
    }

    uint64_t datetime::as_uint64_t(void) const
    {
        return as_int64_t();
    }

    float datetime::as_float(void) const
    {
        return as_int64_t();
    }

    double datetime::as_double(void) const
    {
        return as_int64_t();
    }

    long double datetime::as_long_double(void) const
    {
        return as_int64_t();
    }

    datetime datetime::as_datetime(void) const
    {
        return datetime(getDate(), getTime());
    }

    date datetime::as_date(void) const
    {
        return getDate();
    }

    time datetime::as_time(void) const
    {
        return getTime();
    }

    std::string datetime::as_std_string(void) const
    {
        return d.as_std_string() + " " + t.as_std_string();
    }

    std::wstring datetime::as_std_wstring(void) const
    {
        return d.as_std_wstring() + L" " + t.as_std_wstring();
    }

}