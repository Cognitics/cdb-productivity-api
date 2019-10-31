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
/*! \file ccl/Expression.h
\headerfile ccl/Expression.h
\brief Provides ccl::Expression.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009

\page exp_page Expressions
\ref ccl_page

*/
#pragma once

#include "Variant.h"
#include <memory>

namespace ccl
{
    enum ExpressionType { OP, KEY, VALUE, ACTION };
    enum OpType { AND, NOT, OR, XOR, EQ, NE, LT, LTEQ, GT, GTEQ };

    class Expression;
    typedef std::shared_ptr<Expression> ExpressionSP;
    typedef std::vector<ExpressionSP> ExpressionList;

    class Op;
    typedef std::shared_ptr<Op> OpSP;

    class Key;
    typedef std::shared_ptr<Key> KeySP;

    class Value;
    typedef std::shared_ptr<Value> ValueSP;

    class Action;
    typedef std::shared_ptr<Action> ActionSP;

    class Expression
    {
    private:
        ExpressionList children;

    public:
        virtual ~Expression(void);
        Expression(void);

        virtual ExpressionType getType(void) = 0;

        virtual ExpressionSP add(ExpressionSP expression);
        virtual OpSP addOp(OpType opType, const std::string &key = "", const Variant &value = Variant());
        virtual KeySP addKey(const std::string key);
        virtual ValueSP addValue(const Variant value);

        virtual ExpressionList getChildren(void);

    };

}

