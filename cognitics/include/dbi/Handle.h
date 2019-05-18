/*************************************************************************
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
/*! \file dbi/include/Handle.h
\headerfile dbi/include/Handle.h
\brief Provides dbi::Handle.
\author Aaron Brinton <abrinton@cognitics.net>
\date 25 June 2009
*/
#pragma once

#include <ccl/ccl.h>
#include <boost/utility.hpp>
//#include <boost/tr1/memory.hpp>

namespace dbi
{
    typedef ccl::Variant Variant;
    typedef ccl::VariantList VariantList;
    typedef ccl::VariantMap VariantMap;
    typedef ccl::VariantMapList VariantMapList;
    typedef VariantList Column;
    typedef VariantMap Row;
    typedef VariantMapList Rows;

/*! \class dbi::Handle Handle.h Handle.h
\brief ODBC Handle base class.

This class is a wrapper for ODBC handles.
It provides handle operations and diagnostic record information.

\sa Environment, Connection, Statement
*/
    class Handle : boost::noncopyable
    {
    protected:
        static const int stringSize = 1024 * 2;
        static const int blockSize = 1024 * 64;

        SQLSMALLINT type;        //!< ODBC type
        SQLHANDLE handle;        //!< ODBC handle
        SQLRETURN sqlReturn;    //!< SQLRETURN code

        virtual ~Handle(void);
        Handle(SQLSMALLINT type, SQLHANDLE h);

        //! Wrapper for ODBC native calls to generate diagnostic events and return a boolean success indicator.
        bool ODBC(SQLRETURN res);

    public:
        //! Get the ODBC handle type.
        SQLSMALLINT getType(void);
        //! Get the ODBC handle.
        SQLHANDLE getHandle(void);

        //! [ODBC] Allocate a new handle.
        bool AllocHandle(SQLSMALLINT type, SQLHANDLE *handle);
        //! [ODBC] End transaction.
        bool EndTran(SQLSMALLINT CompletionType);
        //! [ODBC] Free a handle.
        bool FreeHandle(SQLSMALLINT type, SQLHANDLE handle);
        //! [ODBC] Get a diagnostic field.
        bool GetDiagField(SQLSMALLINT RecNumber, SQLSMALLINT DiagIdentifier, SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr);
        //! [ODBC] Get a diagnostic record.
        bool GetDiagRec(SQLSMALLINT RecNumber, SQLCHAR *SQLState, SQLINTEGER *NativeErrorPtr, SQLCHAR *MessageText, SQLSMALLINT BufferLength, SQLSMALLINT *TextLengthPtr);
    };

}


