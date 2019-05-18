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
/*! \file dbi/include/dbi.h
\headerfile dbi/include/dbi.h
\brief Database Interface Library (DBI)
\author Aaron Brinton <abrinton@cognitics.net>
\date 25 June 2009

\page dbi_page Cognitics Database Interface Library (DBI)

\section Description

The Cognitics Database Interface Library (DBI) provides a C++ class interface to the ODBC C library.

It provides the following classes:
- dbi::Handle - base class encapsulating generic ODBC handle functionality.
- dbi::Environment - extends dbi::Handle to represent an ODBC environment handle.
- dbi::Connection - extends dbi::Handle to represent an ODBC connection handle.
- dbi::Statement - extends dbi::Handle to represent an ODBC statement handle.
- dbi::DiagRec - encapsulates an ODBC diagnostic record.
- dbi::Diag - singleton class for distributing diagnostic records (dbi::DiagRec) to registered observers.
- dbi::Observer - observer base class for dbi::Diag.
- dbi::ScopedTransaction - represents a single transaction that is terminated on destruction.

\section Usage

The basic sequence of operations is as follows:
-# Instantiate an ODBC environment handle.
-# Connect to a datasource using the environment handle, which returns a connection handle.
-# Query the datasource using the connection handle, which may include statement handle operations.

Typical usage demonstration:
\code
#include <dbi.h>

// create the environment
dbi::EnvironmentSP env(dbi::EnvironmentSP::create());
if(!env)
    throw std::runtime_error("failed to create ODBC environment");

// connect to a local database called MyDatabase using Microsoft SQL Server
dbi::ConnectionSP dbc = env->driverConnect("DRIVER={SQL Server};SERVER=(local);Trusted_Connection=yes;DATABASE=MyDatabase");
if(!dbc)
    throw std::runtime_error("connection failed");



    // set autocommit
    dbc->SetConnectAttr(SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON, 0);

    // query for a single row
    dbi::Row row;
    dbc->selectRow(row, "SELECT * FROM MyTable WHERE id=?", "int", 5);
    int id = row["id"].as_int();
    std::string name = row["name"].as_string();

    // query for a set of rows
    dbi::Rows rows;
    dbc->selectAll(rows, "SELECT * FROM MyTable");
    for(dbi::Rows::iterator it = rows.begin(), end = rows.end(); it != end; ++it)
    {
        int id = it->second["id"].as_int();
        std::string name = it->second["name"].as_string();
    }

    // execute a prepared query
    dbi::StatementSP stmt = dbc->prepare("SELECT * FROM MyTable")
    while(stmt->fetchRow(row))
    {
        int id = row["id"].as_int();
        std::string name = row["name"].as_string();
    }
    stmt->finish();

    // insert a row
    int myID = 123;
    std::string myName("Aaron");
    dbc->Do("INSERT INTO MyTable (id,name) VALUES (?,?)", "int,cstr", myID, myName.c_str());
}
\endcode

\section Notes

Descriptor functionality is not currently implemented:
\li SQLCopyDesc (handle)
\li SQLGetDescField (descriptor)
\li SQLSetDescField (descriptor)
\li SQLGetDescRec (descriptor)
\li SQLSetDescRec (descriptor)

*/
#pragma once

#define COGNITICS_DBI_VERSION 1.0

#include "Diag.h"
#include "Handle.h"
#include "Environment.h"
#include "Connection.h"
#include "Statement.h"
#include "ScopedTransaction.h"
#include "InsertQueue.h"

//! \namespace dbi Cognitics Database Interface Library
namespace dbi { }

