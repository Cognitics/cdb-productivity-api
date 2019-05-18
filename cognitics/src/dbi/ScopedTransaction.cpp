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

#pragma warning ( disable : 4996 )    // unsafes

#include "dbi/ScopedTransaction.h"
#include <stdio.h>

namespace dbi
{
    ScopedTransaction::~ScopedTransaction()
    {
        Rollback();
    }

    ScopedTransaction::ScopedTransaction(ConnectionSP dbc, std::string transactionName) : dbc(dbc), transactionName(transactionName), closed(false)
    {
        char query[1000] = "";
        sprintf(query, "BEGIN TRAN %s", transactionName.c_str());
        if(!dbc->Do(query))
            throw std::runtime_error("Unable to begin transaction.");
    }

    bool ScopedTransaction::Rollback(void)
    {
        if(closed)
            return false;
        char query[1000] = "";
        sprintf(query, "ROLLBACK TRAN %s", transactionName.c_str());
        if(!dbc->Do(query))
            throw std::runtime_error("Unable to rollback transaction.");
        closed = true;
        return true;
    }

    bool ScopedTransaction::Commit(void)
    {
        if(closed)
            return false;
        char query[1000] = "";
        sprintf(query, "COMMIT TRAN %s", transactionName.c_str());
        if(!dbc->Do(query))
            throw std::runtime_error("Unable to commit transaction.");
        closed = true;
        return true;
    }
    

}