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
#pragma warning ( disable : 4996 )    // unsafe

#include "ccl/Variant.h"
#include "ccl/BigEndian.h"
#include "ccl/LittleEndian.h"

#include <stdexcept>
//#include <sstream>
#include <time.h>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/lexical_cast.hpp>
//#include <sstream>

#ifndef _WIN32
#include <stdio.h>
#endif

#include <string>
#include <sstream>

namespace ccl
{
    std::wstring wstring(const std::string &value)
    {
        std::wstring result;
        std::locale loc;
        for(std::string::size_type i = 0; i < value.size(); ++i)
            result += std::use_facet<std::ctype<wchar_t> >(loc).widen(value[i]);
        return result;
    }

    std::string string(const std::wstring &value)
    {
        std::string result;
        std::locale loc;
        for(std::wstring::size_type i = 0; i < value.size(); ++i)
        {
#ifdef WIN32
            result += std::use_facet<std::ctype<wchar_t> >(loc).narrow(value[i]);
#else
            result += std::use_facet<std::ctype<wchar_t> >(loc).narrow(value[i], '*');
#endif
        }
        return result;
    }

    std::string padnum(int num)
    {
        std::stringstream ss;
        if(num < 10)
            ss << "0";
        ss << num;
        return ss.str();
    }

    class VariantType
    {
    public:
        virtual ~VariantType(void) { }
        virtual uint8_t type(void) const = 0;
        virtual std::string typeString(void) const = 0;
        virtual const std::type_info &typeinfo(void) const = 0;
        virtual VariantType *clone(void) const = 0;
        virtual void *ptr(void) = 0;
        virtual boost::uuids::uuid as_uuid(void) const = 0;
        virtual std::string as_string(void) const = 0;
        virtual std::wstring as_wstring(void) const = 0;
        virtual int32_t as_int(void) const = 0;
        virtual int64_t as_bigint(void) const = 0;
        virtual double as_double(void) const = 0;
        virtual binary as_binary(uint8_t byteOrder) const = 0;
    };

    class VariantUUID : public VariantType
    {
    private:
        boost::uuids::uuid value;

    public:
        virtual ~VariantUUID(void) { }
        VariantUUID(const boost::uuids::uuid &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_UUID;
        }

        virtual std::string typeString(void) const
        {
            return "uuid";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(boost::uuids::uuid);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantUUID(value);
        }

        virtual void *ptr(void)
        {
            return &value;
        }

        virtual boost::uuids::uuid as_uuid(void) const
        {
            return value;
        }

        virtual std::string as_string(void) const
        {
            return boost::uuids::to_string(value);
        }

        virtual std::wstring as_wstring(void) const
        {
            return boost::uuids::to_wstring(value);
        }

        virtual int32_t as_int(void) const
        {
            throw VariantException("uuid", "int");
        }

        virtual int64_t as_bigint(void) const
        {
            throw VariantException("uuid", "bigint");
        }

        virtual double as_double(void) const
        {
            throw VariantException("uuid", "double");
        }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            binary result;
            result.resize(sizeof(boost::uuids::uuid));
            memcpy(&(result[0]), &value, sizeof(boost::uuids::uuid));
            return result;
        }
    };

    class VariantString : public VariantType
    {
    private:
        std::string value;

    public:
        virtual ~VariantString(void) { }
        VariantString(const std::string &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_STRING;
        }

        virtual std::string typeString(void) const
        {
            return "string";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(std::string);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantString(value);
        }

        virtual void *ptr(void)
        {
            return (void *)value.c_str();
        } 

        virtual boost::uuids::uuid as_uuid(void) const
        {
            return boost::uuids::string_generator()(value);
        } 

        virtual std::string as_string(void) const
        {
            return value;
        }

        virtual std::wstring as_wstring(void) const
        {
            return ccl::wstring(value);
        }

        virtual int32_t as_int(void) const
        {
            std::istringstream is(value);
            int32_t result;
            return (is >> result) ? result : 0;
        }

        virtual int64_t as_bigint(void) const
        {
            std::istringstream is(value);
            int64_t result;
            return (is >> result) ? result : 0;
        }

        virtual double as_double(void) const
        {
            std::istringstream is(value);
            double result;
            return (is >> result) ? result : 0;
        }

        virtual binary as_binary(uint8_t byteOrder) const
        {
            if(value.size() == 0)
                return binary();
            binary result;
            result.resize(value.size());
            memcpy(&(result[0]), &(value[0]), value.size());
            return result;
        }
    };

    class VariantWideString : public VariantType
    {
    private:
        std::wstring value;

    public:
        virtual ~VariantWideString(void) { }
        VariantWideString(const std::wstring &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_WSTRING;
        }

        virtual std::string typeString(void) const
        {
            return "wstring";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(std::wstring);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantWideString(value);
        }

        virtual void *ptr(void)
        {
            return (void *)value.c_str();
        }

        virtual boost::uuids::uuid as_uuid(void) const
        {
            return boost::uuids::string_generator()(value);
        }

        virtual std::string as_string(void) const
        {
            return ccl::string(value);
        }

        virtual std::wstring as_wstring(void) const
        {
            return value;
        }

        virtual int32_t as_int(void) const
        {
            std::wistringstream is(value);
            int32_t result;
            return (is >> result) ? result : 0;
        }

        virtual int64_t as_bigint(void) const
        {
            std::wistringstream is(value);
            int64_t result;
            return (is >> result) ? result : 0;
        }

        virtual double as_double(void) const
        {
            std::wistringstream is(value);
            double result;
            return (is >> result) ? result : 0;
        }

        virtual binary as_binary(uint8_t byteOrder) const
        {
            binary result;
            result.resize(value.size() * sizeof(wchar_t));
            memcpy(&(result[0]), &(value[0]), value.size() * sizeof(wchar_t));
            return result;
        }
    };

    class VariantInteger : public VariantType
    {
    private:
        int32_t value;

    public:
        virtual ~VariantInteger(void) { }
        VariantInteger(const int32_t &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_INT32;
        } 

        virtual std::string typeString(void) const
        {
            return "int";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(int32_t);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantInteger(value);
        }

        virtual void *ptr(void)
        {
            return &value;
        }

        virtual boost::uuids::uuid as_uuid(void) const
        {
            throw VariantException("int", "uuid");
        }

        virtual std::string as_string(void) const
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        virtual std::wstring as_wstring(void) const
        {
            std::wstringstream ss;
            ss << value;
            return ss.str();
        }

        virtual int32_t as_int(void) const
        {
            return value;
        }

        virtual int64_t as_bigint(void) const
        {
            return value;
        }

        virtual double as_double(void) const
        {
            return value;
        }

        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<int32_t> l(value);
            BigEndian<int32_t> b(value);
            return (byteOrder == Variant::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class VariantBigInteger : public VariantType
    {
    private:
        int64_t value;

    public:
        virtual ~VariantBigInteger(void) { }
        VariantBigInteger(const int64_t &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_INT64;
        }

        virtual std::string typeString(void) const
        {
            return "bigint";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(int64_t);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantBigInteger(value);
        }

        virtual void *ptr(void)
        {
            return &value;
        }

        virtual boost::uuids::uuid as_uuid(void) const
        {
            throw VariantException("bigint", "uuid");
        }

        virtual std::string as_string(void) const
        {
            std::stringstream ss;
            ss << value;
            return ss.str();
        }

        virtual std::wstring as_wstring(void) const
        {
            std::wstringstream ss;
            ss << value;
            return ss.str();
        }

        virtual int32_t as_int(void) const
        {
            return value;
        }

        virtual int64_t as_bigint(void) const
        {
            return value;
        }

        virtual double as_double(void) const
        {
            return value;
        }

        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<int64_t> l(value);
            BigEndian<int64_t> b(value);
            return (byteOrder == Variant::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class VariantDouble : public VariantType
    {
    private:
        double value;

    public:
        virtual ~VariantDouble(void) { } 
        VariantDouble(const double &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_DOUBLE;
        }

        virtual std::string typeString(void) const
        {
            return "double";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(double);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantDouble(value);
        }

        virtual void *ptr(void)
        {
            return &value;
        }

        virtual boost::uuids::uuid as_uuid(void) const
        {
            throw VariantException("double", "uuid");
        }

        virtual std::string as_string(void) const
        {
            std::stringstream ss;
            ss.precision(Variant::precision);
            //ss << std::fixed << value;
            ss << value;
            return ss.str();
        }

        virtual std::wstring as_wstring(void) const
        {
            std::wstringstream ss;
            ss.precision(Variant::precision);
            //ss << std::fixed << value;
            ss << value;
            return ss.str();
        }

        virtual int32_t as_int(void) const
        {
            return value;
        }

        virtual int64_t as_bigint(void) const
        {
            return value;
        }

        virtual double as_double(void) const
        {
            return value;
        }

        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<double> v(value);
            return v.as_binary();
        }
    };

    class VariantBinary : public VariantType
    {
    private:
        binary value;

    public:
        virtual ~VariantBinary(void) { }
        VariantBinary(const binary &value) : value(value) { }

        virtual uint8_t type(void) const
        {
            return Variant::TYPE_BINARY;
        }

        virtual std::string typeString(void) const
        {
            return "binary";
        }

        virtual const std::type_info &typeinfo(void) const
        {
            return typeid(binary);
        }

        virtual VariantType *clone(void) const
        {
            return new VariantBinary(value);
        }

        virtual void *ptr(void)
        {
            return &value;
        }

        virtual boost::uuids::uuid as_uuid(void) const
        {
            if(value.size() < sizeof(boost::uuids::uuid))
                return boost::uuids::nil_generator()();
            boost::uuids::uuid result;
            memcpy(&result, &(value[0]), sizeof(boost::uuids::uuid));
            return result;
        }

        virtual std::string as_string(void) const
        {
            if(value.size() == 0)
                return std::string();
            std::string result;
            result.resize(value.size());
            memcpy(&(result[0]), &(value[0]), value.size());
            return result;
        }

        virtual std::wstring as_wstring(void) const
        {
            std::wstring result;
            result.resize(value.size() / sizeof(wchar_t));
            memcpy(&(result[0]), &(value[0]), value.size());
            return result;
        }

        virtual int32_t as_int(void) const
        {
            if(value.size() < sizeof(int32_t))
                return 0;
            LittleEndian<int32_t> v(0);
            return (v.from_binary(value)) ? int32_t(v) : 0;
        }

        virtual int64_t as_bigint(void) const
        {
            if(value.size() < sizeof(int64_t))
                return 0;
            LittleEndian<int64_t> v(0);
            return (v.from_binary(value)) ? int64_t(v) : 0;
        }

        virtual double as_double(void) const
        {
            if(value.size() < sizeof(double))
                return 0;
            LittleEndian<double> v(0);
            return (v.from_binary(value)) ? double(v) : 0;
        }

        virtual binary as_binary(uint8_t byteOrder) const
        {
            return value;
        }
    };

//----------------------------------------------------------------------

    int Variant::precision = 16;

    Variant::~Variant(void)
    {
        delete content;
    }

    Variant::Variant(void) : content(NULL) { }
    Variant::Variant(const Variant &value) : content(value.clone()) { }
    Variant::Variant(const boost::uuids::uuid &value) : content(new VariantUUID(value)) { }
    Variant::Variant(const char *value) : content(new VariantString(std::string(value))) { }
    Variant::Variant(const std::string &value) : content(new VariantString(value)) { }
    Variant::Variant(const wchar_t *value) : content(new VariantWideString(std::wstring(value))) { }
    Variant::Variant(const std::wstring &value) : content(new VariantWideString(value)) { }
    Variant::Variant(const bool &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const int8_t &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const uint8_t &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const int16_t &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const uint16_t &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const int32_t &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const uint32_t &value) : content(new VariantInteger(value)) { }
    Variant::Variant(const int64_t &value) : content(new VariantBigInteger(value)) { }
    Variant::Variant(const uint64_t &value) : content(new VariantBigInteger(value)) { }
    Variant::Variant(const float &value) : content(new VariantDouble(value)) { }
    Variant::Variant(const double &value) : content(new VariantDouble(value)) { }
    Variant::Variant(const long double &value) : content(new VariantDouble(value)) { }
    Variant::Variant(const binary &value) : content(new VariantBinary(value)) { }

    Variant &Variant::operator=(const Variant &value)
    {
        delete content;
        content = value.clone();
        return *this;
    }
    
    Variant &Variant::operator=(const boost::uuids::uuid &value)
    {
        delete content;
        content = new VariantUUID(value);
        return *this;
    }

    Variant &Variant::operator=(const char *value)
    {
        delete content;
        content = new VariantString(std::string(value));
        return *this;
    }

    Variant &Variant::operator=(const std::string &value)
    {
        delete content;
        content = new VariantString(value);
        return *this;
    }

    Variant &Variant::operator=(const wchar_t *value)
    {
        delete content;
        content = new VariantWideString(std::wstring(value));
        return *this;
    }

    Variant &Variant::operator=(const std::wstring &value)
    {
        delete content;
        content = new VariantWideString(value);
        return *this;
    }

    Variant &Variant::operator=(const bool &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const int8_t &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const uint8_t &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const int16_t &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const uint16_t &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const int32_t &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const uint32_t &value)
    {
        delete content;
        content = new VariantInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const int64_t &value)
    {
        delete content;
        content = new VariantBigInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const uint64_t &value)
    {
        delete content;
        content = new VariantBigInteger(value);
        return *this;
    }

    Variant &Variant::operator=(const float &value)
    {
        delete content;
        content = new VariantDouble(value);
        return *this;
    }

    Variant &Variant::operator=(const double &value)
    {
        delete content;
        content = new VariantDouble(value);
        return *this;
    }

    Variant &Variant::operator=(const long double &value)
    {
        delete content;
        content = new VariantDouble(value);
        return *this;
    }
    Variant &Variant::operator=(const binary &value)
    {
        delete content;
        content = new VariantBinary(value);
        return *this;
    }

    bool Variant::operator==(const Variant &rhs) const
    {
        if(type() != rhs.type())
            return false;
        switch(type())
        {
            case TYPE_EMPTY:
                return true;
            case TYPE_INT8:
            case TYPE_UINT8:
            case TYPE_INT16:
            case TYPE_UINT16:
            case TYPE_INT32:
            case TYPE_UINT32:
            case TYPE_DATETIME:
            case TYPE_DATE:
            case TYPE_TIME:
                return (as_int() == rhs.as_int());
            case TYPE_INT64:
            case TYPE_UINT64:
                return (as_bigint() == rhs.as_bigint());
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
            case TYPE_LONGDOUBLE:
                return (as_double() == rhs.as_double());
            case TYPE_UUID:
                return (as_uuid() == rhs.as_uuid());
            case TYPE_STRING:
                return (as_string() == rhs.as_string());
            case TYPE_WSTRING:
                return (as_wstring() == rhs.as_wstring());
            //case TYPE_BINARY:

        }
        return false;
    }

    bool Variant::operator!=(const Variant &rhs) const
    {
        return !(*this == rhs);
    }

    bool Variant::operator<(const Variant &rhs) const
    {
        if(type() != rhs.type())
            return (*this < rhs.convert(type()));    // convert to lhs type

        switch(type())
        {
            case TYPE_EMPTY:
                return false;
            case TYPE_INT8:
            case TYPE_UINT8:
            case TYPE_INT16:
            case TYPE_UINT16:
            case TYPE_INT32:
            case TYPE_UINT32:
            case TYPE_DATETIME:
            case TYPE_DATE:
            case TYPE_TIME:
                return (as_int() < rhs.as_int());
            case TYPE_INT64:
            case TYPE_UINT64:
                return (as_bigint() < rhs.as_bigint());
            case TYPE_FLOAT:
            case TYPE_DOUBLE:
            case TYPE_LONGDOUBLE:
                return (as_double() < rhs.as_double());
            case TYPE_UUID:
                return (as_uuid() < rhs.as_uuid());
            case TYPE_STRING:
                return (as_string() < rhs.as_string());
            case TYPE_WSTRING:
                return (as_wstring() < rhs.as_wstring());
            //case TYPE_BINARY:

        }
        return false;
    }

    bool Variant::operator<=(const Variant &rhs) const
    {
        return (*this == rhs) || (*this < rhs);
    }

    bool Variant::operator>(const Variant &rhs) const
    {
        return !(*this <= rhs);
    }

    bool Variant::operator>=(const Variant &rhs) const
    {
        return !(*this < rhs);
    }

    uint8_t Variant::type(void) const
    {
        return content ? content->type() : Variant::TYPE_EMPTY;
    }

    std::string Variant::typeString(void) const
    {
        return content ? content->typeString() : std::string();
    }

    Variant Variant::convert(uint8_t type) const
    {
        switch(type)
        {
            case TYPE_UUID:            return Variant(as_uuid());
            case TYPE_STRING:        return Variant(as_string());
            case TYPE_WSTRING:        return Variant(as_wstring());
            case TYPE_INT:            return Variant(as_int());
            case TYPE_BIGINT:        return Variant(as_bigint());
            case TYPE_DOUBLE:        return Variant(as_double());
            case TYPE_BINARY:        return Variant(as_binary());
        }
        return Variant();
    }

    Variant Variant::convert(const std::string &type) const
    {
        if(type == "uuid")
            return convert(TYPE_UUID);
        if(type == "string")
            return convert(TYPE_STRING);
        if(type == "wstring")
            return convert(TYPE_WSTRING);
        if(type == "int")
            return convert(TYPE_INT);
        if(type == "bigint")
            return convert(TYPE_BIGINT);
        if(type == "double")
            return convert(TYPE_DOUBLE);
        if(type == "binary")
            return convert(TYPE_BINARY);
        return Variant();
    }

    const std::type_info &Variant::typeinfo(void) const
    {
        return content ? content->typeinfo() : typeid(void);
    }

    VariantType *Variant::clone(void) const
    {
        return content ? content->clone() : NULL;
    }

    void *Variant::ptr(void) const
    {
        return content ? content->ptr() : NULL;
    }

    bool Variant::empty(void) const
    {
        return (content == NULL);
    }

    boost::uuids::uuid Variant::as_uuid(void) const
    {
        return content ? content->as_uuid() : boost::uuids::uuid();
    }

    std::string Variant::as_string(void) const
    {
        return content ? content->as_string() : std::string();
    }

    std::wstring Variant::as_wstring(void) const
    {
        return content ? content->as_wstring() : std::wstring();
    }
    
    int32_t Variant::as_int(void) const
    {        
        return content ? content->as_int() : 0;
    }

    int64_t Variant::as_bigint(void) const
    {
        return content ? content->as_bigint() : 0;
    }

    double Variant::as_double(void) const
    {
        return content ? content->as_double() : 0;
    }
    binary Variant::as_binary(uint8_t byteOrder) const
    {
        if(!content)
            return binary();
        if(byteOrder == Variant::ENDIAN_HOST)
            byteOrder = machLittleEndian() ? Variant::ENDIAN_LITTLE : Variant::ENDIAN_BIG;
        return content->as_binary(byteOrder);
    }

    binary Variant::encode(uint8_t byteOrder) const
    {
        if(!content)
            return binary();
        if(byteOrder == Variant::ENDIAN_HOST)
            byteOrder = machLittleEndian() ? Variant::ENDIAN_LITTLE : Variant::ENDIAN_BIG;
        binary variantBinary(as_binary(byteOrder));
        uint32_t size = uint32_t(variantBinary.size());
        binary sizeBinary(Variant(size).as_binary());
        binary result;
        result.insert(result.end(), type());
        result.insert(result.end(), sizeBinary.begin(), sizeBinary.end());
        result.insert(result.end(), variantBinary.begin(), variantBinary.end());
        return result;
    }

    Variant Variant::decode(binary::iterator &it, uint8_t byteOrder)
    {
        if(byteOrder == Variant::ENDIAN_HOST)
            byteOrder = machLittleEndian() ? Variant::ENDIAN_LITTLE : Variant::ENDIAN_BIG;
        // TODO: use byteOrder
        binary::iterator pos = it;
        try
        {
            uint8_t type = *it;
            ++it;
            uint32_t size =  Variant(binary(it, it + sizeof(int32_t))).as_int();
            it += sizeof(int32_t);
            Variant binValue(binary(it, it + size));
            it += size;
            switch(type)
            {
                case Variant::TYPE_UUID:
                    return Variant(binValue.as_uuid());
                case Variant::TYPE_STRING:
                    return Variant(binValue.as_string());
                case Variant::TYPE_WSTRING:
                    return Variant(binValue.as_wstring());
                case Variant::TYPE_INT32:
                    return Variant(binValue.as_int());
                    /*
                    {
                        binarystringstream bs(binValue.as_binary());
                        if(byteOrder == Variant::ENDIAN_LITTLE)
                        {
                            LittleEndian<int32_t> value;
                            value << bs;
                            return Variant(int32_t(value));
                        }
                        BigEndian<int32_t> value;
                        value << bs;
                        return Variant(int32_t(value));
                    }
                    */
                case Variant::TYPE_INT64:
                    return Variant(binValue.as_bigint());
                    /*
                    {
                        binarystringstream bs(binValue.as_binary());
                        if(byteOrder == Variant::ENDIAN_LITTLE)
                        {
                            LittleEndian<int64_t> value;
                            value << bs;
                            return Variant(int64_t(value));
                        }
                        BigEndian<int64_t> value;
                        value << bs;
                        return Variant(int64_t(value));
                    }
                    */
                case Variant::TYPE_DOUBLE:
                    return Variant(binValue.as_double());
                    /*
                    {
                        binarystringstream bs(binValue.as_binary());
                        if(byteOrder == Variant::ENDIAN_LITTLE)
                        {
                            LittleEndian<double> value;
                            value << bs;
                            return Variant(double(value));
                        }
                        BigEndian<double> value;
                        value << bs;
                        return Variant(double(value));
                    }
                    */
                case Variant::TYPE_BINARY:
                    return binValue;
            }
            return Variant();
        }
        catch(std::exception e)
        {
            it = pos;
            return Variant();
        }
    }

    template <class t>
    bool Variant::parseable()
    {
        try
        {
            boost::lexical_cast<t>(content->as_string());
            return true;
        }
        catch(boost::bad_lexical_cast &)
        {
            return false;
        }
    }

    template bool Variant::parseable<int>();
    template bool Variant::parseable<double>();



}