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

#pragma warning ( disable : 4244 )    // possible loss of data

#include "dbi/Handle.h"
#include "dbi/Diag.h"

#include <sstream>
#include <string.h>

namespace dbi
{
    Handle::~Handle()
    {
        SQLFreeHandle(type, handle);
    }

    Handle::Handle(SQLSMALLINT type, SQLHANDLE handle) : type(type), handle(handle)
    {
    }

    bool Handle::ODBC(SQLRETURN res)
    {
        SQLSMALLINT recNumber;
        SQLCHAR sqlState[7];
        SQLINTEGER nativeError;
        SQLCHAR messageText[stringSize + 1];
        SQLSMALLINT bufferLength = stringSize;
        SQLSMALLINT textLength;
        sqlReturn = res;
        for(int i = 1; true; ++i)
        {
            recNumber = i;
            if(!GetDiagRec(recNumber, sqlState, &nativeError, messageText, stringSize, &textLength))
                break;
            messageText[textLength] = 0;
            sqlState[6] = 0;
            DiagRecSP diagRec(new DiagRec(type, handle, sqlReturn, sqlState, nativeError, messageText));
            Diag::instance()->notify(diagRec);
        }
        return SQL_SUCCEEDED(res);
    }

    SQLSMALLINT Handle::getType(void)
    {
        return type;
    }

    SQLHANDLE Handle::getHandle(void)
    {
        return handle;
    }

    bool Handle::AllocHandle(SQLSMALLINT type, SQLHANDLE *handle)
    {
        return ODBC(SQLAllocHandle(type, this->handle, handle));
    }

    bool Handle::FreeHandle(SQLSMALLINT type, SQLHANDLE handle)
    {
        return ODBC(SQLFreeHandle(type, handle));
    }

    bool Handle::GetDiagRec(SQLSMALLINT RecNumber, SQLCHAR *SQLState, SQLINTEGER *NativeErrorPtr, SQLCHAR *MessageText, SQLSMALLINT BufferLength, SQLSMALLINT *TextLengthPtr)
    {
        return SQL_SUCCEEDED(SQLGetDiagRec(type, handle, RecNumber, SQLState, NativeErrorPtr, MessageText, BufferLength, TextLengthPtr));
    }

    bool Handle::GetDiagField(SQLSMALLINT RecNumber, SQLSMALLINT DiagIdentifier, SQLPOINTER DiagInfoPtr, SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr)
    {
        return ODBC(SQLGetDiagField(type, handle, RecNumber, DiagIdentifier, DiagInfoPtr, BufferLength, StringLengthPtr));
    }

    bool Handle::EndTran(SQLSMALLINT CompletionType)
    {
        return ODBC(SQLEndTran(type, handle, CompletionType));
    }

}
