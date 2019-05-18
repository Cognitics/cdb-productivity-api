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
    date::~date(void)
    {
    }

    date::date(void) : year(0), month(0), day(0)
    {
    }

    date::date(int16_t year, uint16_t month, uint16_t day) : year(year), month(month), day(day)
    {
    }

    int16_t date::getYear(void) const
    {
        return year;
    }

    void date::setYear(int16_t year)
    {
        this->year = year;
    }

    uint16_t date::getMonth(void) const
    {
        return month;
    }

    void date::setMonth(uint16_t month)
    {
        this->month = month;
    }

    uint16_t date::getDay(void) const
    {
        return day;
    }

    void date::setDay(uint16_t day)
    {
        this->day = day;
    }

    std::string date::getClassName(void) const
    {
        return "ccl::date";
    }

    std::string date::getClassDescription(void) const
    {
        return "CCL Date Object";
    }

    std::string date::getSuperClassName(void) const
    {
        return object::getClassName();
    }

    object *date::clone(void) const
    {
        return new date(year, month, day);
    }

    int8_t date::as_int8_t(void) const
    {
        return as_int64_t();
    }

    uint8_t date::as_uint8_t(void) const
    {
        return as_int64_t();
    }

    int16_t date::as_int16_t(void) const
    {
        return as_int64_t();
    }

    uint16_t date::as_uint16_t(void) const
    {
        return as_int64_t();
    }

    int32_t date::as_int32_t(void) const
    {
        return as_int64_t();
    }

    uint32_t date::as_uint32_t(void) const
    {
        return as_int64_t();
    }

    int64_t date::as_int64_t(void) const
    {
        tm tmEpoch = { 0, 0, 0, 1, 0, 70, 0, 0, 0 };
        tm tmValue = { 0, 0, 0, day, month - 1, year - 1900, 0, 0, 0 };
        return difftime(mktime(&tmValue), mktime(&tmEpoch));
    }

    uint64_t date::as_uint64_t(void) const
    {
        return as_int64_t();
    }

    float date::as_float(void) const
    {
        return as_int64_t();
    }

    double date::as_double(void) const
    {
        return as_int64_t();
    }

    long double date::as_long_double(void) const
    {
        return as_int64_t();
    }

    datetime date::as_datetime(void) const
    {
        return datetime(as_date());
    }

    date date::as_date(void) const
    {
        return date(year, month, day);
    }

    std::string date::as_std_string(void) const
    {
        std::stringstream ss;
        ss << year << "-";
        if(month < 10)
            ss << "0";
        ss << month << "-";
        if(day < 10)
            ss << "0";
        ss << day;
        return ss.str();
    }

    std::wstring date::as_std_wstring(void) const
    {
        std::wstringstream ss;
        ss << year << L"-";
        if(month < 10)
            ss << L"0";
        ss << month << L"-";
        if(day < 10)
            ss << L"0";
        ss << day;
        return ss.str();
    }

}