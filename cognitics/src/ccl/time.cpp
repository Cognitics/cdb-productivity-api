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
    time::~time(void)
    {
    }

    time::time(void) : hour(0), minute(0), second(0), fraction(0)
    {
    }

    time::time(uint16_t hour, uint16_t minute, uint16_t second, uint32_t fraction) : hour(hour), minute(minute), second(second), fraction(fraction)
    {
    }

    uint16_t time::getHour(void) const
    {
        return hour;
    }

    void time::setHour(uint16_t hour)
    {
        this->hour = hour;
    }

    uint16_t time::getMinute(void) const
    {
        return minute;
    }

    void time::setMinute(uint16_t minute)
    {
        this->minute = minute;
    }

    uint16_t time::getSecond(void) const
    {
        return second;
    }

    void time::setSecond(uint16_t second)
    {
        this->second = second;
    }

    uint32_t time::getFraction(void) const
    {
        return fraction;
    }

    void time::setFraction(uint32_t fraction)
    {
        this->fraction = fraction;
    }

    std::string time::getClassName(void) const
    {
        return "ccl::time";
    }

    std::string time::getClassDescription(void) const
    {
        return "CCL Time Object";
    }

    std::string time::getSuperClassName(void) const
    {
        return object::getClassName();
    }

    object *time::clone(void) const
    {
        return new time(hour, minute, second, fraction);
    }

    int8_t time::as_int8_t(void) const
    {
        return as_int64_t();
    }

    uint8_t time::as_uint8_t(void) const
    {
        return as_int64_t();
    }

    int16_t time::as_int16_t(void) const
    {
        return as_int64_t();
    }

    uint16_t time::as_uint16_t(void) const
    {
        return as_int64_t();
    }

    int32_t time::as_int32_t(void) const
    {
        return as_int64_t();
    }

    uint32_t time::as_uint32_t(void) const
    {
        return as_int64_t();
    }

    int64_t time::as_int64_t(void) const
    {
        tm tmEpoch = { 0, 0, 0, 1, 0, 70, 0, 0, 0 };
        tm tmValue = { second, minute, hour, 1, 0, 70, 0, 0, 0 };
        return difftime(mktime(&tmValue), mktime(&tmEpoch));
    }

    uint64_t time::as_uint64_t(void) const
    {
        return as_int64_t();
    }

    float time::as_float(void) const
    {
        return as_int64_t();
    }

    double time::as_double(void) const
    {
        return as_int64_t();
    }

    long double time::as_long_double(void) const
    {
        return as_int64_t();
    }

    datetime time::as_datetime(void) const
    {
        return datetime(as_time());
    }

    time time::as_time(void) const
    {
        return time(hour, minute, second, fraction);
    }

    std::string time::as_std_string(void) const
    {
        std::stringstream ss;
        ss << hour << ":";
        if(minute < 10)
            ss << "0";
        ss << minute << ":";
        if(second < 10)
            ss << "0";
        ss << second;
        if(fraction)
            ss << "." << fraction;
        return ss.str();
    }

    std::wstring time::as_std_wstring(void) const
    {
        std::wstringstream ss;
        ss << hour << L":";
        if(minute < 10)
            ss << L"0";
        ss << minute << L":";
        if(second < 10)
            ss << L"0";
        ss << second;
        if(fraction)
            ss << L"." << fraction;
        return ss.str();
    }

}