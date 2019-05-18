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

#pragma warning ( disable : 4244 )        // possible loss of data

#include "ccl/Number.h"
#include "ccl/LittleEndian.h"
#include "ccl/BigEndian.h"
#include <sstream>
#include <typeinfo>

namespace ccl
{
    class NumberType
    {
    public:
        virtual ~NumberType(void) { }
        virtual void operator++(void) = 0;
        virtual void operator--(void) = 0;
        virtual uint8_t type(void) const = 0;
        virtual const std::type_info &typeinfo(void) const = 0;
        virtual void *ptr(void) = 0;
        virtual NumberType *clone(void) const = 0;
        virtual int8_t as_int8(void) const = 0;
        virtual uint8_t as_uint8(void) const = 0;
        virtual int16_t as_int16(void) const = 0;
        virtual uint16_t as_uint16(void) const = 0;
        virtual int32_t as_int32(void) const = 0;
        virtual uint32_t as_uint32(void) const = 0;
        virtual int64_t as_int64(void) const = 0;
        virtual uint64_t as_uint64(void) const = 0;
        virtual float as_float(void) const = 0;
        virtual double as_double(void) const = 0;
        virtual long double as_long_double(void) const = 0;
        virtual std::string as_string(void) const = 0;
        virtual binary as_binary(uint8_t byteOrder) const = 0;
    };

    class NumberINT8 : public NumberType
    {
    public:
        int8_t value;

        virtual ~NumberINT8(void) { }
        NumberINT8(const int8_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_INT8; }
        virtual const std::type_info &typeinfo(void) const { return typeid(int8_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberINT8(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<int8_t> l(value);
            BigEndian<int8_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberUINT8 : public NumberType
    {
    public:
        uint8_t value;

        virtual ~NumberUINT8(void) { }
        NumberUINT8(const uint8_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_UINT8; }
        virtual const std::type_info &typeinfo(void) const { return typeid(uint8_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberUINT8(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<uint8_t> l(value);
            BigEndian<uint8_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberINT16 : public NumberType
    {
    public:
        int16_t value;

        virtual ~NumberINT16(void) { }
        NumberINT16(const int16_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_INT16; }
        virtual const std::type_info &typeinfo(void) const { return typeid(int16_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberINT16(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<int16_t> l(value);
            BigEndian<int16_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberUINT16 : public NumberType
    {
    public:
        uint16_t value;

        virtual ~NumberUINT16(void) { }
        NumberUINT16(const uint16_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_UINT16; }
        virtual const std::type_info &typeinfo(void) const { return typeid(uint16_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberUINT16(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<uint16_t> l(value);
            BigEndian<uint16_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberINT32 : public NumberType
    {
    public:
        int32_t value;

        virtual ~NumberINT32(void) { }
        NumberINT32(const int32_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_INT32; }
        virtual const std::type_info &typeinfo(void) const { return typeid(int32_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberINT32(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<int32_t> l(value);
            BigEndian<int32_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberUINT32 : public NumberType
    {
    public:
        uint32_t value;

        virtual ~NumberUINT32(void) { }
        NumberUINT32(const uint32_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_UINT32; }
        virtual const std::type_info &typeinfo(void) const { return typeid(uint32_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberUINT32(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<uint32_t> l(value);
            BigEndian<uint32_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberINT64 : public NumberType
    {
    public:
        int64_t value;

        virtual ~NumberINT64(void) { }
        NumberINT64(const int64_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_INT64; }
        virtual const std::type_info &typeinfo(void) const { return typeid(int64_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberINT64(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<int64_t> l(value);
            BigEndian<int64_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberUINT64 : public NumberType
    {
    public:
        uint64_t value;

        virtual ~NumberUINT64(void) { }
        NumberUINT64(const uint64_t &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_UINT64; }
        virtual const std::type_info &typeinfo(void) const { return typeid(uint64_t); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberUINT64(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<uint64_t> l(value);
            BigEndian<uint64_t> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberFLOAT : public NumberType
    {
    public:
        float value;

        virtual ~NumberFLOAT(void) { }
        NumberFLOAT(const float &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_FLOAT; }
        virtual const std::type_info &typeinfo(void) const { return typeid(float); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberFLOAT(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<float> l(value);
            BigEndian<float> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberDOUBLE : public NumberType
    {
    public:
        double value;

        virtual ~NumberDOUBLE(void) { }
        NumberDOUBLE(const double &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_DOUBLE; }
        virtual const std::type_info &typeinfo(void) const { return typeid(double); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberDOUBLE(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<double> l(value);
            BigEndian<double> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

    class NumberLONGDOUBLE : public NumberType
    {
    public:
        long double value;

        virtual ~NumberLONGDOUBLE(void) { }
        NumberLONGDOUBLE(const long double &value) : value(value) { }
        virtual void operator++(void) { ++value; }
        virtual void operator--(void) { --value; }
        virtual uint8_t type(void) const { return Number::TYPE_LONGDOUBLE; }
        virtual const std::type_info &typeinfo(void) const { return typeid(long double); }
        virtual void *ptr(void) { return &value; }
        virtual NumberType *clone(void) const { return new NumberLONGDOUBLE(value); }
        virtual int8_t as_int8(void) const { return value; }
        virtual uint8_t as_uint8(void) const { return value; }
        virtual int16_t as_int16(void) const { return value; }
        virtual uint16_t as_uint16(void) const { return value; }
        virtual int32_t as_int32(void) const { return value; }
        virtual uint32_t as_uint32(void) const { return value; }
        virtual int64_t as_int64(void) const { return value; }
        virtual uint64_t as_uint64(void) const { return value; }
        virtual float as_float(void) const { return value; }
        virtual double as_double(void) const { return value; }
        virtual long double as_long_double(void) const { return value; }
        virtual std::string as_string(void) const { std::stringstream ss; ss << value; return ss.str(); }
        virtual binary as_binary(uint8_t byteOrder) const
        {
            LittleEndian<long double> l(value);
            BigEndian<long double> b(value);
            return (byteOrder == Number::ENDIAN_LITTLE) ? l.as_binary() : b.as_binary();
        }
    };

//----------------------------------------------------------------------

    NumberType *Number::clone(void) const
    {
        return content ? content->clone() : NULL;
    }

    Number::~Number(void)
    {
        delete content;
    }

    Number::Number(void) : content(NULL) { }
    Number::Number(const Number &value) : content(value.clone()) { }
    Number::Number(const int8_t &value) : content(new NumberINT8(value)) { }
    Number::Number(const uint8_t &value) : content(new NumberUINT8(value)) { }
    Number::Number(const int16_t &value) : content(new NumberINT16(value)) { }
    Number::Number(const uint16_t &value) : content(new NumberUINT16(value)) { }
    Number::Number(const int32_t &value) : content(new NumberINT32(value)) { }
    Number::Number(const uint32_t &value) : content(new NumberUINT32(value)) { }
    Number::Number(const int64_t &value) : content(new NumberINT64(value)) { }
    Number::Number(const uint64_t &value) : content(new NumberUINT64(value)) { }
    Number::Number(const float &value) : content(new NumberFLOAT(value)) { }
    Number::Number(const double &value) : content(new NumberDOUBLE(value)) { }
    Number::Number(const long double &value) : content(new NumberLONGDOUBLE(value)) { }

    Number &Number::operator=(const Number &value)
    {
        delete content;
        content = value.clone();
        return *this;
    }
    
    Number &Number::operator=(const int8_t &value)
    {
        delete content;
        content = new NumberINT8(value);
        return *this;
    }

    Number &Number::operator=(const uint8_t &value)
    {
        delete content;
        content = new NumberUINT8(value);
        return *this;
    }

    Number &Number::operator=(const int16_t &value)
    {
        delete content;
        content = new NumberINT16(value);
        return *this;
    }

    Number &Number::operator=(const uint16_t &value)
    {
        delete content;
        content = new NumberUINT16(value);
        return *this;
    }

    Number &Number::operator=(const int32_t &value)
    {
        delete content;
        content = new NumberINT32(value);
        return *this;
    }

    Number &Number::operator=(const uint32_t &value)
    {
        delete content;
        content = new NumberUINT32(value);
        return *this;
    }

    Number &Number::operator=(const int64_t &value)
    {
        delete content;
        content = new NumberINT64(value);
        return *this;
    }

    Number &Number::operator=(const uint64_t &value)
    {
        delete content;
        content = new NumberUINT64(value);
        return *this;
    }

    Number &Number::operator=(const float &value)
    {
        delete content;
        content = new NumberFLOAT(value);
        return *this;
    }

    Number &Number::operator=(const double &value)
    {
        delete content;
        content = new NumberDOUBLE(value);
        return *this;
    }

    Number &Number::operator=(const long double &value)
    {
        delete content;
        content = new NumberLONGDOUBLE(value);
        return *this;
    }

    void Number::operator++(int)
    {
        if(content)
            content->operator++();
    }

    void Number::operator++(void)
    {
        if(content)
            content->operator++();
    }

    void Number::operator--(int)
    {
        if(content)
            content->operator--();
    }

    void Number::operator--(void)
    {
        if(content)
            content->operator--();
    }

    uint8_t Number::type(void) const
    {
        return content ? content->type() : Number::TYPE_EMPTY;
    }

    const std::type_info &Number::typeinfo(void) const
    {
        return content ? content->typeinfo() : typeid(void);
    }

    void *Number::ptr(void) const
    {
        return content ? content->ptr() : NULL;
    }

    bool Number::empty(void) const
    {
        return (content == NULL);
    }

    int8_t Number::as_int8(void) const
    {
        return (content) ? content->as_int8() : 0;
    }

    uint8_t Number::as_uint8(void) const
    {
        return (content) ? content->as_uint8() : 0;
    }

    int16_t Number::as_int16(void) const
    {
        return (content) ? content->as_int16() : 0;
    }

    uint16_t Number::as_uint16(void) const
    {
        return (content) ? content->as_uint16() : 0;
    }

    int32_t Number::as_int32(void) const
    {
        return (content) ? content->as_int32() : 0;
    }

    uint32_t Number::as_uint32(void) const
    {
        return (content) ? content->as_uint32() : 0;
    }

    int64_t Number::as_int64(void) const
    {
        return (content) ? content->as_int64() : 0;
    }

    uint64_t Number::as_uint64(void) const
    {
        return (content) ? content->as_uint64() : 0;
    }

    float Number::as_float(void) const
    {
        return (content) ? content->as_float() : 0;
    }

    double Number::as_double(void) const
    {
        return (content) ? content->as_double() : 0;
    }

    long double Number::as_long_double(void) const
    {
        return (content) ? content->as_long_double() : 0;
    }

    std::string Number::as_string(void) const
    {
        return (content) ? content->as_string() : "";
    }

    binary Number::as_binary(uint8_t byteOrder) const
    {
        if(!content)
            return binary();
        if(byteOrder == ENDIAN_HOST)
            byteOrder = machLittleEndian() ? ENDIAN_LITTLE : ENDIAN_BIG;
        return content->as_binary(byteOrder);
    }

    binary Number::encode(uint8_t byteOrder) const
    {
        if(!content)
            return binary();
        if(byteOrder == ENDIAN_HOST)
            byteOrder = machLittleEndian() ? ENDIAN_LITTLE : ENDIAN_BIG;
        binary numberBinary(as_binary(byteOrder));
        binary result;
        result.insert(result.end(), type());
        result.insert(result.end(), numberBinary.begin(), numberBinary.end());
        return result;
    }

    Number Number::decode(binary::iterator &it, uint8_t byteOrder)
    {
        // TODO: use Variant class for this?
        binary::iterator pos = it;
        try
        {
            uint8_t type = *it;
            ++it;
            switch(type)
            {
                case TYPE_INT8:
                    {

                    }

            }


            /*
            int32_t size =  Variant(binary(it, it + sizeof(int32_t))).as_int();
            it += sizeof(int32_t);
            Variant binValue(binary(it, it + size));
            it += size;
            switch(type)
            {
                case VARIANT_UUID:
                    return Variant(binValue.as_uuid());
                case VARIANT_STRING:
                    return Variant(binValue.as_string());
                case VARIANT_WSTRING:
                    return Variant(binValue.as_wstring());
                case VARIANT_U16STRING:
                    return Variant(binValue.as_u16string());
                case VARIANT_U32STRING:
                    return Variant(binValue.as_u32string());
                case VARIANT_INT:
                    return Variant(binValue.as_int());
                case VARIANT_BIGINT:
                    return Variant(binValue.as_bigint());
                case VARIANT_DOUBLE:
                    return Variant(binValue.as_double());
                case VARIANT_DATETIME:
                    return Variant(binValue.as_datetime());
                case VARIANT_DATE:
                    return Variant(binValue.as_date());
                case VARIANT_TIME:
                    return Variant(binValue.as_time());
                case VARIANT_INTERVAL:
                    return Variant(binValue.as_interval());
                case VARIANT_BINARY:
                    return binValue;
            }
            */
            return Number();
        }
        catch(std::exception e)
        {
            it = pos;
            return Number();
        }
    }

}



