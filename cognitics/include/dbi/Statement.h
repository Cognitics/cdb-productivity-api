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
/*! \file dbi/include/Statement.h
\headerfile dbi/include/Statement.h
\brief Provides dbi::Statement.
\author Aaron Brinton <abrinton@cognitics.net>
\date 25 June 2009
*/
#pragma once

#include "Handle.h"

#include <boost/uuid/uuid.hpp>

namespace dbi
{
    struct ColumnInfo
    {
        std::string name;
        int datatype;
        int size;
        int digits;
        int nullable;
    };

/*! \class dbi::Statement Statement.h Statement.h
\brief ODBC Statement class.

This class is a wrapper for an ODBC statement handle.

\sa dbi::Handle, dbi::Environment, dbi::Connection
*/
    class Statement : public Handle
    {
        friend class Connection;

    private:
        struct _Statement;
        _Statement *_data;

        Statement(SQLHANDLE handle);

    public:
        virtual ~Statement(void);

/*! \brief Execute a query directly with no result set.
\param query The query to execute.
\return True if query was executed successfully, false otherwise.
*/
        bool Do(const std::string &query);

/*! \brief Prepare a query for execution.
\note This call will clear the parameter binding list.
\param query The query to prepare.
\return True if successful; false otherwise.
*/
        bool prepare(const std::string &query);

/*! \brief Bind a parameter to the statement handle.
\param value Variant representing the value to bind.
\return True if successful; false otherwise.
*/
        bool bindParameter(const Variant &value);

/*! \brief Fetch the next row from the current query execution.
\param data Row to populate.
\return True if successful; false otherwise.
*/
        bool fetchRow(Row &data);

/*! \brief Finish a query (FreeStmt with SQL_CLOSE).
\return True if successful; false otherwise.
*/
        bool finish(void);

/*! \brief Reset the statement (FreeStmt with SQL_UNBIND and SQL_RESET_PARAMS).
\return True if successful; false otherwise.
*/
        bool reset(void);

/*! \brief Get information about the specified result column.
\param num The index of the column requested.
\param colInfo ColumnInfo structure to populate.
\return True if successful; false otherwise.
*/
        bool getColumnInfo(int num, ColumnInfo &colInfo);

        // TODO: attributes

        //! [ODBC] SQLBindCol
        bool BindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_Ind);
        //! [ODBC] SQLBindParameter
        bool BindParameter(SQLUSMALLINT ParameterNumber, SQLSMALLINT InputOutputType, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize, SQLSMALLINT DecimalDigits, SQLPOINTER ParameterValuePtr, SQLINTEGER BufferLength, SQLLEN *StrLen_or_IndPtr);
        //! [ODBC] SQLBulkOperations
        bool BulkOperations(SQLUSMALLINT Operation);
        //! [ODBC] SQLCancel
        bool Cancel(void);
        //! [ODBC] SQLCloseCursor
        bool CloseCursor(void);
        //! [ODBC] SQLColAttribute
        bool ColAttribute(SQLUSMALLINT ColumnNumber, SQLUSMALLINT FieldIdentifier, SQLPOINTER CharacterAttributePtr, SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr, SQLLEN *NumericAttributePtr);
        //! [ODBC] SQLColumnPrivileges
        bool ColumnPrivileges(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4);
        //! [ODBC] SQLColumns
        bool Columns(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4);
        //! [ODBC] SQLDescribeCol
        bool DescribeCol(SQLSMALLINT ColumnNumber, SQLCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLengthPtr, SQLSMALLINT *DataTypePtr, SQLULEN *ColumnSizePtr, SQLSMALLINT *DecimalDigitsPtr, SQLSMALLINT *NullablePtr);
        //! [ODBC] SQLDescribeParam
        bool DescribeParam(SQLSMALLINT ParameterNumber, SQLSMALLINT *DataTypePtr, SQLULEN *ParameterSizePtr, SQLSMALLINT *DecimalDigitsPtr, SQLSMALLINT *NullablePtr);
        //! [ODBC] SQLExecDirect
        bool ExecDirect(SQLCHAR *StatementText, SQLINTEGER TextLength);
        //! [ODBC] SQLExecute
        bool Execute(void);
        //! [ODBC] SQLFetch
        bool Fetch(void);
        //! [ODBC] SQLFetchScroll
        bool FetchScroll(SQLSMALLINT FetchOrientation, SQLLEN FetchOffset);
        //! [ODBC] SQLForeignKeys
        bool ForeignKeys(SQLCHAR *PKCatalogName, SQLSMALLINT NameLength1, SQLCHAR *PKSchemaName, SQLSMALLINT NameLength2, SQLCHAR *PKTableName, SQLSMALLINT NameLength3, SQLCHAR *FKCatalogName, SQLSMALLINT NameLength4, SQLCHAR *FKSchemaName, SQLSMALLINT NameLength5, SQLCHAR *FKTableName, SQLSMALLINT NameLength6);
        //! [ODBC] SQLFreeStmt
        bool FreeStmt(SQLUSMALLINT Option);
        //! [ODBC] SQLGetCursorName
        bool GetCursorName(SQLCHAR *CursorName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLengthPtr);
        //! [ODBC] SQLGetData
        bool GetData(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr);
        //! [ODBC] SQLGetStmtAttr
        bool GetStmtAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr);
        //! [ODBC] SQLGetTypeInfo
        bool GetTypeInfo(SQLSMALLINT DataType);
        //! [ODBC] SQLMoreResults
        bool MoreResults(void);
        //! [ODBC] SQLNumParams
        bool NumParams(SQLSMALLINT *ParameterCountPtr);
        //! [ODBC] SQLNumResultCols
        bool NumResultCols(SQLSMALLINT *ColumnCountPtr);
        //! [ODBC] SQLParamData
        bool ParamData(SQLPOINTER *ValuePtrPtr);
        //! [ODBC] SQLPrepare
        bool Prepare(SQLCHAR *StatementText, SQLINTEGER TextLength);
        //! [ODBC] SQLPrimaryKeys
        bool PrimaryKeys(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3);
        //! [ODBC] SQLProcedureColumns
        bool ProcedureColumns(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *ProcName, SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4);
        //! [ODBC] SQLProcedures
        bool Procedures(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *ProcName, SQLSMALLINT NameLength3);
        //! [ODBC] SQLPutData
        bool PutData(SQLPOINTER DataPtr, SQLLEN StrLen_or_Ind);
        //! [ODBC] SQLRowCount
        bool RowCount(SQLLEN *RowCountPtr);
        //! [ODBC] SQLSetCursorName
        bool SetCursorName(SQLCHAR *CursorName, SQLSMALLINT NameLength);
        //! [ODBC] SQLSetPos
        bool SetPos(SQLSETPOSIROW RowNumber, SQLUSMALLINT Operation, SQLUSMALLINT LockType);
        //! [ODBC] SQLSetStmtAttr
        bool SetStmtAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength);
        //! [ODBC] SQLSpecialColumns
        bool SpecialColumns(SQLSMALLINT IdentifierType, SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLSMALLINT Scope, SQLSMALLINT Nullable);
        //! [ODBC] SQLStatistics
        bool Statistics(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLUSMALLINT Unique, SQLUSMALLINT Reserved);
        //! [ODBC] SQLTablePrivileges
        bool TablePrivileges(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3);
        //! [ODBC] SQLTables
        bool Tables(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLCHAR *TableType, SQLSMALLINT NameLength4);
    };

    typedef std::shared_ptr<Statement> StatementSP;

}

