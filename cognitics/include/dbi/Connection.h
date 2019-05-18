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
/*! \file dbi/include/Connection.h
\headerfile dbi/include/Connection.h
\brief Provides dbi::Connection.
\author Aaron Brinton <abrinton@cognitics.net>
\date 25 June 2009
*/
#pragma once

#include "Handle.h"

namespace dbi
{
    class Statement;
    typedef std::shared_ptr<Statement> StatementSP;

/*! \class dbi::Connection Connection.h Connection.h
\brief ODBC Connection class.

This class is a wrapper for an ODBC connection handle.

\sa dbi::Handle, dbi::Environment, dbi::Statement
*/
    class Connection : public Handle
    {
        friend class Environment;

    private:
        struct _Connection;
        _Connection *_data;

        Connection(SQLHANDLE handle);

    public:
        virtual ~Connection(void);

/*! \brief Connect using a DSN.

\param dsn A datasource name matching an entry from Environment::getDataSources().
\param username The username for authentication.
\param password The password for authentication.
\return True if successful; false otherwise.
*/
        bool connect(const std::string &dsn, const std::string &username, const std::string &password);

/*! \brief Connect using a driver connection string.

This bypasses the user/system data source abstraction.

\code
dbi::EnvironmentSP env(dbi::Environment::create());
dbi::ConnectionSP dbc = env->createConnection();
if(!dbc->driverConnect("DRIVER={SQL Server};SERVER=(local);Trusted_Connection=yes"))
    throw std::runtime_error("connect failed");
\endcode

\param connectString The database driver connection string.
\return True if successful; false otherwise.
*/
        bool driverConnect(const std::string &connectString);

/*! \brief Create a Statement object.
\return The new Statement object, or empty if failed.
*/
        StatementSP createStatement(void);

/*! \brief Prepare a query with the given parameters.
\param query The query to prepare.
\param parameters List of bind parameters.
\return A Statement instance for the prepared query.
*/
        StatementSP prepare(const std::string &query, const VariantList &parameters = VariantList());

/*! \brief Execute a query with a temporary statement handle.
\param query The query to execute.
\param parameters List of bind parameters.
\param expression An expression tree representing filter parameters.
This expression tree is parsed and appended to the query.
\return True if query was executed successfully, false otherwise.
*/
        bool Do(const std::string &query, const VariantList &parameters = VariantList(), ccl::OpSP expression = ccl::OpSP());

/*! \brief Execute a query with a temporary statement handle.
\param query The query to execute.
\param parameter Single parameter to bind.
\param expression An expression tree representing filter parameters.
This expression tree is parsed and appended to the query.
\return True if query was executed successfully, false otherwise.
*/
        bool Do(const std::string &query, const Variant &parameter, ccl::OpSP expression = ccl::OpSP());

/*! \brief Execute a query and return a single row.
\param data The Row to populate.
\param query The query to execute.
\param parameters List of bind parameters.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRow(Row &data, const std::string &query, const VariantList &parameters = VariantList(), ccl::OpSP expression = ccl::OpSP());

/*! \brief Execute a query and return a single row.
\param data The Row to populate.
\param query The query to execute.
\param parameter Single parameter to bind.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRow(Row &data, const std::string &query, const Variant &parameter, ccl::OpSP expression = ccl::OpSP());

/*! \brief Select a single row from the specified table.
\param data The Row to populate.
\param table The table to query.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRow(Row &data, const std::string &table, ccl::OpSP expression);

/*! \brief Select a single row from the specified table.
\param data The Row to populate.
\param table The table to query.
\param whereParameters Map of WHERE clause parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRow(Row &data, const std::string &table, const VariantMap &whereParameters);

/*! \brief Execute a query and return all rows.
\param data The Rows to populate.
\param query The query to execute.
\param parameters List of bind parameters.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRows(Rows &data, const std::string &query, const VariantList &parameters = VariantList(), ccl::OpSP expression = ccl::OpSP());

/*! \brief Execute a query and return all rows.
\param data The Rows to populate.
\param query The query to execute.
\param parameter Single parameter to bind.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRows(Rows &data, const std::string &query, const Variant &parameter, ccl::OpSP expression = ccl::OpSP());

/*! \brief Select all rows from the specified table.
\param data The Rows to populate.
\param table The table to query.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRows(Rows &data, const std::string &table, ccl::OpSP expression);

/*! \brief Select all rows from the specified table.
\param data The Rows to populate.
\param table The table to query.
\param whereParameters Map of WHERE clause parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectRows(Rows &data, const std::string &table, const VariantMap &whereParameters);

/*! \brief Execute a query and return a column of data.
\param data The Column to populate.
\param query The query to execute.
\param parameters List of bind parameters.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectColumn(Column &data, const std::string &query, const VariantList &parameters = VariantList(), ccl::OpSP expression = ccl::OpSP());

/*! \brief Execute a query and return a column of data.
\param data The Column to populate.
\param query The query to execute.
\param parameter Single parameter to bind.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectColumn(Column &data, const std::string &query, const Variant &parameter, ccl::OpSP expression = ccl::OpSP());

/*! \brief Select a column of data from the specified table.
\param data The Column to populate.
\param table The table to query.
\param expression An expression tree representing filter parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectColumn(Column &data, const std::string &table, ccl::OpSP expression);

/*! \brief Select a column of data from the specified table.
\param data The Column to populate.
\param table The table to query.
\param whereParameters Map of WHERE clause parameters.
\return True if query was executed successfully, false otherwise.
*/
        bool selectColumn(Column &data, const std::string &table, const VariantMap &whereParameters);

/*! \brief Insert a row into the specified table.
\param table The table to insert into.
\param data Map of field data to insert.
\return True if insert was executed successfully, false otherwise.
*/
        bool insert(const std::string &table, const VariantMap &data);

/*! \brief Insert a set of rows into the specified table.
\param table The table to insert into.
\param fields The fields to assign values.
\param data List of field data to insert. This must match the field order and be a multiple of the field count.
\return True if insert was executed successfully, false otherwise.
*/
        bool insert(const std::string &table, const std::vector<std::string> &fields, const VariantList &values);

/*! \brief Update row(s) in the specified table.
\param table The table to update.
\param data Map of field data to update.
\param expression An expression tree representing filter parameters.
\return True if update was executed successfully, false otherwise.
*/
        bool update(const std::string &table, const VariantMap &data, ccl::OpSP expression = ccl::OpSP());

/*! \brief Update row(s) in the specified table.
\param table The table to update.
\param data Map of field data to update.
\param whereParameters Map of WHERE clause parameters.
\return True if update was executed successfully, false otherwise.
*/
        bool update(const std::string &table, const VariantMap &data, const VariantMap &whereParameters);

/*! \brief Update a row in the specified table by uuid.
\param table The table to update.
\param data Map of field data to update.
\param uuid The uuid of the row to update. This field must be named 'uuid'.
\return True if update was executed successfully, false otherwise.
*/
        bool update(const std::string &table, const VariantMap &data, const boost::uuids::uuid &uuid);

/*! \brief Delete row(s) in the specified table.
\param table The table to update.
\param expression An expression tree representing filter parameters.
\return True if delete was executed successfully, false otherwise.
*/
        bool Delete(const std::string &table, ccl::OpSP expression = ccl::OpSP());

/*! \brief Delete row(s) in the specified table.
\param table The table to update.
\param whereParameters Map of WHERE clause parameters.
\return True if delete was executed successfully, false otherwise.
*/
        bool Delete(const std::string &table, const VariantMap &whereParameters);

/*! \brief Delete a row in the specified table by uuid.
\param table The table to update.
\param uuid The uuid of the row to delete. This field must be named 'uuid'.
\return True if delete was executed successfully, false otherwise.
*/
        bool Delete(const std::string &table, const boost::uuids::uuid &uuid);


        /* TODO
        void getColumnPrivileges() { }
        void getColumns() { }
        void getForeignKeys() { }
        void getPrimaryKeys() { }
        void getProcedureColumns() { }
        void getProcedures() { }
        void getSpecialColumns() { }
        void getStatistics() { }
        void getTablePrivileges() { }
        void getTables() { }
        */


/*! \brief Get status of autocommit.
\return True if autocommit is ON; false if OFF.
*/
        bool getAutoCommit(void);

/*! \brief Set autocommit.
\param autoCommit True to set autocommit to ON; false to set to OFF.
\return True if successful; false otherwise.
*/
        bool setAutoCommit(bool autoCommit);

/*! \brief Get status of autocommit.
\return Current autocommit setting: SQL_AUTOCOMMIT_ON or SQL_AUTOCOMMIT_OFF.
*/
        SQLUINTEGER getAttrAutoCommit(void);

/*! \brief Set autocommit.

Values:
\li SQL_AUTOCOMMIT_ON - The driver uses autocommit mode. Each statement is committed immediately after it is executed. This is the default. Any open transactions on the connection are committed when SQL_ATTR_AUTOCOMMIT is set to SQL_AUTOCOMMIT_ON to change from manual-commit mode to autocommit mode.
\li SQL_AUTOCOMMIT_OFF - The driver uses manual-commit mode, and the application must explicitly commit or roll back transactions with SQLEndTran.

\param value The value to assign (SQL_AUTOCOMMIT_ON or SQL_AUTOCOMMIT_OFF).
\return True if successful; false otherwise.
\sa ODBC Commit Mode
*/
        bool setAttrAutoCommit(SQLUINTEGER value);

        //! [ODBC] SQLBrowseConnect
        bool BrowseConnect(SQLCHAR *InConnectionString, SQLSMALLINT StringLength1, SQLCHAR *OutConnectionString, SQLSMALLINT BufferLength, SQLSMALLINT *StringLength2Ptr);
        //! [ODBC] SQLConnect
        bool Connect(SQLCHAR *ServerName, SQLSMALLINT NameLength1, SQLCHAR *UserName, SQLSMALLINT NameLength2, SQLCHAR *Authentication, SQLSMALLINT NameLength3);
        //! [ODBC] SQLDisconnect
        bool Disconnect();
        //! [ODBC] SQLDriverConnect
        bool DriverConnect(SQLCHAR *InConnectionString, SQLSMALLINT StringLength1, SQLCHAR *OutConnectionString, SQLSMALLINT BufferLength, SQLSMALLINT *StringLength2Ptr, SQLUSMALLINT DriverCompletion);
        //! [ODBC] SQLGetConnectAttr
        bool GetConnectAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr);
        //! [ODBC] SQLGetFunctions
        bool GetFunctions(SQLUSMALLINT FunctionId, SQLUSMALLINT *SupportedPtr);
        //! [ODBC] SQLGetInfo
        bool GetInfo(SQLUSMALLINT InfoType, SQLPOINTER InfoValuePtr, SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr);
        //! [ODBC] SQLNativeSql
        bool NativeSql(SQLCHAR *InStatementText, SQLINTEGER TextLength1, SQLCHAR *OutStatementText, SQLINTEGER BufferLength, SQLINTEGER *TextLength2Ptr);
        //! [ODBC] SQLSetConnectAttr
        bool SetConnectAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength);
    };

    typedef std::shared_ptr<Connection> ConnectionSP;

}
