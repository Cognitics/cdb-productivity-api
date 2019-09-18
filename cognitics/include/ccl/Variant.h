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
/*! \brief Provides ccl::Variant.
\author Aaron Brinton <abrinton@cognitics.net>
\date 20 January 2010

\page variant_page Variant
\ref ccl_page

\section Description

The Variant class represents data types based on the Microsoft Open Database Connectivity (ODBC) C library.
The purpose of this class is to provide a limited set of types that are enumerable and storable.

Conversions between numeric values and date/time values are done with POSIX time ("unixtime"). 

\section Usage


*/
#pragma once

#include "VariantException.h"
#include "cstdint.h"
#include "binary.h"

#ifdef _WIN32
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
#else
#include <string.h>
#undef HAVE_UNISTD_H
#endif



#include <boost/uuid/uuid.hpp>
#include <typeinfo>
#include <string>
#include <vector>
#include <map>

namespace ccl
{
    class VariantType;

    std::wstring wstring(const std::string &value);
    std::string string(const std::wstring &value);

    //! Variant class for representing ODBC data types.
    class Variant
    {
    private:
        VariantType *content;
        VariantType *clone(void) const;

    public:
        static int precision;

        ~Variant(void);
        Variant(void);

        static const int TYPE_EMPTY            = 0;
        static const int TYPE_INT8            = TYPE_EMPTY + 1;
        static const int TYPE_UINT8            = TYPE_EMPTY + 2;
        static const int TYPE_INT16            = TYPE_EMPTY + 3;
        static const int TYPE_UINT16        = TYPE_EMPTY + 4;
        static const int TYPE_INT32            = TYPE_EMPTY + 5;
        static const int TYPE_UINT32        = TYPE_EMPTY + 6;
        static const int TYPE_INT64            = TYPE_EMPTY + 7;
        static const int TYPE_UINT64        = TYPE_EMPTY + 8;
        static const int TYPE_FLOAT            = TYPE_EMPTY + 9;
        static const int TYPE_DOUBLE        = TYPE_EMPTY + 10;
        static const int TYPE_LONGDOUBLE    = TYPE_EMPTY + 11;
        static const int TYPE_UUID            = TYPE_EMPTY + 12;
        static const int TYPE_STRING        = TYPE_EMPTY + 13;
        static const int TYPE_WSTRING        = TYPE_EMPTY + 14;
        static const int TYPE_DATETIME        = TYPE_EMPTY + 15;
        static const int TYPE_DATE            = TYPE_EMPTY + 16;
        static const int TYPE_TIME            = TYPE_EMPTY + 17;
        static const int TYPE_INTERVAL        = TYPE_EMPTY + 18;
        static const int TYPE_BINARY        = TYPE_EMPTY + 19;
        static const int TYPE_MAX            = TYPE_EMPTY + 100;
        static const int TYPE_INT            = TYPE_INT32;
        static const int TYPE_BIGINT        = TYPE_INT64;

        static const int ENDIAN_LITTLE        = 0;
        static const int ENDIAN_BIG            = 1;
        static const int ENDIAN_HOST        = 2;

        Variant(const Variant &value);
        Variant(const boost::uuids::uuid &value);
        //Variant(const SQLGUID &value);
        Variant(const char *value);
        Variant(const std::string &value);
        Variant(const wchar_t *value);
        Variant(const std::wstring &value);
        Variant(const bool &value);
        Variant(const int8_t &value);
        Variant(const uint8_t &value);
        Variant(const int16_t &value);
        Variant(const uint16_t &value);
        Variant(const int32_t &value);
        Variant(const uint32_t &value);
        Variant(const int64_t &value);
        Variant(const uint64_t &value);
        Variant(const float &value);
        Variant(const double &value);
        Variant(const long double &value);
        /*
        Variant(const TIMESTAMP_STRUCT &value);
        Variant(const DATE_STRUCT &value);
        Variant(const TIME_STRUCT &value);
        Variant(const SQL_INTERVAL_STRUCT &value);
        
        */
        Variant(const binary &value);

        Variant &operator=(const Variant &value);
        Variant &operator=(const boost::uuids::uuid &value);
        //Variant &operator=(const SQLGUID &value);
        Variant &operator=(const char *value);
        Variant &operator=(const std::string &value);
        Variant &operator=(const wchar_t *value);
        Variant &operator=(const std::wstring &value);
        Variant &operator=(const bool &value);
        Variant &operator=(const int8_t &value);
        Variant &operator=(const uint8_t &value);
        Variant &operator=(const int16_t &value);
        Variant &operator=(const uint16_t &value);
        Variant &operator=(const int32_t &value);
        Variant &operator=(const uint32_t &value);
        Variant &operator=(const int64_t &value);
        Variant &operator=(const uint64_t &value);
        Variant &operator=(const float &value);
        Variant &operator=(const double &value);
        Variant &operator=(const long double &value);
        //Variant &operator=(const TIMESTAMP_STRUCT &value);
        //Variant &operator=(const DATE_STRUCT &value);
        //Variant &operator=(const TIME_STRUCT &value);
        //Variant &operator=(const SQL_INTERVAL_STRUCT &value);
        Variant &operator=(const binary &value);

        bool operator==(const Variant &rhs) const;
        bool operator!=(const Variant &rhs) const;
        bool operator<(const Variant &rhs) const;
        bool operator<=(const Variant &rhs) const;
        bool operator>(const Variant &rhs) const;
        bool operator>=(const Variant &rhs) const;

        //! Get the enumerated data type for the stored value.
        uint8_t type(void) const;

        //! Get a string representing the enumerated data type for the stored value.
        std::string typeString(void) const;

        //! Convert variant to specified type.
        Variant convert(uint8_t type) const;

        //! Convert variant to specified type string.
        Variant convert(const std::string &type) const;

        //! Get the RTTI type information for the stored value.
        const std::type_info &typeinfo(void) const;

        //! Get a pointer to the stored value.
        void *ptr(void) const;

        //! Returns true if the Variant has no content (is NULL).
        bool empty(void) const;

        //! Get the stored value as a Universally Unique Identifier (UUID).
        boost::uuids::uuid as_uuid(void) const;

        //! Get the stored value as a string.
        std::string as_string(void) const;

        //! Get the stored value as a wide string.
        std::wstring as_wstring(void) const;

        //! Get the stored value as a 32-bit integer.
        int32_t as_int(void) const;

        //! Get the stored value as a 64-bit integer.
        int64_t as_bigint(void) const;

        //! Get the stored value as a double.
        double as_double(void) const;
/*
        //! Get the stored value as an SQL TIMESTAMP_STRUCT.
        TIMESTAMP_STRUCT as_datetime(void) const;

        //! Get the stored value as an SQL DATE_STRUCT.
        DATE_STRUCT as_date(void) const;

        //! Get the stored value as an SQL TIME_STRUCT.
        TIME_STRUCT as_time(void) const;

        //! Get the stored value as an SQL INTERVAL_STRUCT.
        SQL_INTERVAL_STRUCT as_interval(void) const;
*/
        //! Get the stored value as binary using the specified byte encoding (if applicable).
        binary as_binary(uint8_t byteOrder = ENDIAN_LITTLE) const;

/*! \brief Encode the variant for storage.

This encodes the type (uint8_t), size (uint32_t), and content.
It is intended for use in storing multiple Variants into a single binary value.

\sa Variant::decode()
*/
        binary encode(uint8_t byteOrder = ENDIAN_LITTLE) const;

/*! \brief Decode the next variant in a binary value.

This generates a Variant based on a type, size, and content.
It is intended for decoding binary values created with encode().

The iterator parameter is updated to reflect the next binary position.
If parsing failed, an empty Variant is returned and the iterator is reset.

\sa Variant::encode()
\todo This does not currently support big endian decoding.
*/
        static Variant decode(binary::iterator &binary, uint8_t byteOrder = ENDIAN_LITTLE);

        //! Returns true if the variant can be successfully parsed as an int
        template <class t>
        inline bool parseable();
    };

    typedef std::vector<Variant> VariantList;
    typedef std::map<std::string, Variant> VariantMap;
    typedef std::vector<VariantMap> VariantMapList;

}
/*
inline int operator==(const TIMESTAMP_STRUCT &lhs, const TIMESTAMP_STRUCT &rhs)
{
    return (lhs.year == rhs.year) && (lhs.month == rhs.month) && (lhs.day == rhs.day)
        && (lhs.hour == rhs.hour) && (lhs.minute == rhs.minute) && (lhs.second == rhs.second) && (lhs.fraction == rhs.fraction);
}

inline int operator!=(const TIMESTAMP_STRUCT &lhs, const TIMESTAMP_STRUCT &rhs)
{
    return !(lhs == rhs);
}

inline int operator==(const DATE_STRUCT &lhs, const DATE_STRUCT &rhs)
{
    return (lhs.year == rhs.year) && (lhs.month == rhs.month) && (lhs.day == rhs.day);
}

inline int operator!=(const DATE_STRUCT &lhs, const DATE_STRUCT &rhs)
{
    return !(lhs == rhs);
}

inline int operator==(const TIME_STRUCT &lhs, const TIME_STRUCT &rhs)
{
    return (lhs.hour == rhs.hour) && (lhs.minute == rhs.minute) && (lhs.second == rhs.second);
}

inline int operator!=(const TIME_STRUCT &lhs, const TIME_STRUCT &rhs)
{
    return !(lhs == rhs);
}

inline int operator==(const SQL_INTERVAL_STRUCT &lhs, const SQL_INTERVAL_STRUCT &rhs)
{
    return (memcmp(&lhs, &rhs, sizeof(SQL_INTERVAL_STRUCT)) == 0);
}

inline int operator!=(const SQL_INTERVAL_STRUCT &lhs, const SQL_INTERVAL_STRUCT &rhs)
{
    return !(lhs == rhs);
}
*/







