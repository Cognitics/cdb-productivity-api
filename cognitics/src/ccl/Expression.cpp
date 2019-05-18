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

#include "ccl/Expression.h"
#include "ccl/Op.h"
#include "ccl/Key.h"
#include "ccl/Value.h"

namespace ccl
{
    Expression::~Expression(void)
    {
    }

    Expression::Expression(void)
    {
    }

    ExpressionSP Expression::add(ExpressionSP expression)
    {
        children.push_back(expression);
        return expression;
    }

    OpSP Expression::addOp(OpType opType, const std::string &key, const ccl::Variant &value)
    {
        OpSP result(new Op(opType));
        if(!key.empty())
            result->addKey(key);
        if(!value.empty())
            result->addValue(value);
        add(result);
        return result;
    }

    KeySP Expression::addKey(const std::string key)
    {
        KeySP result = KeySP(new Key(key));
        add(result);
        return result;
    }

    ValueSP Expression::addValue(const ccl::Variant value)
    {
        ValueSP result = ValueSP(new Value(value));
        add(result);
        return result;
    }

    ExpressionList Expression::getChildren(void)
    {
        return children;
    }

}