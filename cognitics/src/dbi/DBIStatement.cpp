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

#include "dbi/Statement.h"

#include <cstdarg>
#include <stdexcept>
#include <string.h>
#include <stdio.h>

namespace dbi
{
    struct BindData
    {
        ccl::Variant value;
        SQLSMALLINT ctype;
        SQLSMALLINT sqltype;
        SQLULEN size;
        SQLSMALLINT digits;
        SQLINTEGER len;
        SQLLEN ind;
        SQLPOINTER ptr;
        ccl::binary temp;

        BindData(const ccl::Variant &v) : value(v), ctype(0), sqltype(0), size(0), digits(0), len(0), ind(0), ptr(value.ptr())
        {
            switch(value.type())
            {
                case ccl::Variant::TYPE_STRING:
                    ctype = SQL_C_CHAR;
                    sqltype = SQL_VARCHAR;
                    ind = SQL_NTS;
                    size = value.as_string().size() + 1;
                    break;
                case ccl::Variant::TYPE_WSTRING:
                //case ccl::Variant::TYPE_U16STRING:
                //case ccl::Variant::TYPE_U32STRING:
                    ctype = SQL_C_WCHAR;
                    sqltype = SQL_WVARCHAR;
                    ind = SQL_NTS;
                    size = value.as_wstring().size() + 1;
                    break;
                case ccl::Variant::TYPE_INT:
                    ctype = SQL_C_SLONG;
                    sqltype = SQL_INTEGER;
                    break;
                case ccl::Variant::TYPE_BIGINT:
                    ctype = SQL_C_SBIGINT;
                    sqltype = SQL_BIGINT;
                    break;
                case ccl::Variant::TYPE_DOUBLE:
                    ctype = SQL_C_DOUBLE;
                    sqltype = SQL_DOUBLE;
                    break;
                case ccl::Variant::TYPE_BINARY:
                    ctype = SQL_C_BINARY;
                    sqltype = SQL_LONGVARBINARY;
                    ind = ((ccl::binary *)(value.ptr()))->size();
                    size = ind + 1;
                    ccl::binary *temp = (ccl::binary *)(value.ptr());
                    ptr = &(*temp)[0];
                    break;
            }
        }

    };
    typedef std::shared_ptr<BindData> BindDataSP;

    struct Statement::_Statement
    {
        std::vector<BindDataSP> bindList;
    };

    Statement::~Statement(void)
    {
        delete _data;
    }

    Statement::Statement(SQLHANDLE handle) : Handle(SQL_HANDLE_STMT, handle), _data(new _Statement)
    {
    }

    bool Statement::Do(const std::string &query)
    {
        return ExecDirect((SQLCHAR *)(query.c_str()), SQL_NTS);
    }

    bool Statement::prepare(const std::string &query)
    {
        _data->bindList.clear();
        return Prepare((SQLCHAR *)(query.c_str()), SQL_NTS);
    }

    bool Statement::bindParameter(const ccl::Variant &value)
    {
        int index = _data->bindList.size() + 1;
        BindDataSP bindData(new BindData(value));
        if(!BindParameter((SQLUSMALLINT)index, SQL_PARAM_INPUT, bindData->ctype, bindData->sqltype, bindData->size, bindData->digits, bindData->ptr, bindData->len, &bindData->ind))
            return false;
        _data->bindList.push_back(bindData);
        return true;
    }

    bool Statement::fetchRow(Row &data)
    {
        data.clear();
        if(!Fetch())
            return false;

        SQLSMALLINT num;
        if(!NumResultCols(&num))
            return false;

        for(SQLUSMALLINT i = 1; i <= num; ++i)
        {
            ColumnInfo colInfo;
            if(!getColumnInfo(i, colInfo))        // TODO: we may want to buffer this
                return false;
            switch(colInfo.datatype)
            {
                case SQL_BIT:
                case SQL_SMALLINT:
                case SQL_TINYINT:
                case SQL_INTEGER:
                case SQL_DECIMAL:
                case SQL_NUMERIC:
                    {
                        ccl::int32_t value;
                        SQLLEN datasize = 0;
                        if(GetData(i, (SQLSMALLINT)SQL_C_ULONG, (SQLPOINTER)&value, (SQLLEN)sizeof(ccl::int32_t), &datasize) && (datasize >= 0))
                            data[colInfo.name] = value;
                    }
                    break;
                case SQL_BIGINT:
                    {
                        ccl::int64_t value;
                        SQLLEN datasize = 0;
                        if(GetData(i, (SQLSMALLINT)SQL_C_SBIGINT, (SQLPOINTER)&value, (SQLLEN)sizeof(ccl::int64_t), &datasize) && (datasize >= 0))
                            data[colInfo.name] = value;
                    }
                    break;
                case SQL_REAL:
                case SQL_FLOAT:
                case SQL_DOUBLE:
                    {
                        double value;
                        SQLLEN datasize = 0;
                        if(GetData(i, (SQLSMALLINT)SQL_C_DOUBLE, (SQLPOINTER)&value, (SQLLEN)sizeof(double), &datasize) && (datasize >= 0))
                            data[colInfo.name] = value;
                    }
                    break;

                case SQL_WCHAR:
                case SQL_WVARCHAR:
                case SQL_WLONGVARCHAR:
                    {
                        std::wstring result;
                        int size = colInfo.size;
                        std::wstring buffer;
                        buffer.resize(blockSize + 1);
                        while(size > 0)
                        {
                            SQLLEN datasize = 0;
                            if(GetData(i, (SQLSMALLINT)SQL_C_WCHAR, (SQLPOINTER)(buffer.data()), (SQLLEN)(sizeof(wchar_t) * blockSize), &datasize) && (datasize > 0))
                            {
                                // TODO: this isn't kosher even though it works... ended up with some stack corruption for LMCO
                                result.append(buffer.data(), datasize / sizeof(wchar_t));
                                size -= blockSize;
                            }
                            else
                            {
                                size = 0;
                            }
                        }
                        data[colInfo.name] = result;
                    }
                    break;
                case SQL_BINARY:
                case SQL_VARBINARY:
                case SQL_LONGVARBINARY:
                    {
                        ccl::binary result;
                        int size = colInfo.size;
                        ccl::binary buffer;
                        buffer.resize(blockSize + 1);
                        while(size > 0)
                        {
                            SQLLEN datasize = 0;
                                // TODO: this isn't kosher even though it works... ended up with some stack corruption for LMCO
                            if(GetData(i, (SQLSMALLINT)SQL_C_BINARY, (SQLPOINTER)(&buffer[0]), (SQLLEN)(sizeof(char) * blockSize), &datasize) && (datasize > 0))
                            {
                                int copySize = (datasize > blockSize) ? blockSize : datasize;
                                result.insert(result.end(), buffer.begin(), buffer.begin() + copySize);
                                if(size > datasize)
                                    size = datasize;
                                size -= blockSize;
                            }
                            else
                            {
                                size = 0;
                            }
                        }
                        data[colInfo.name] = result;
                    }
                    break;
                case SQL_CHAR:
                case SQL_VARCHAR:
                case SQL_LONGVARCHAR:
                default:
                    {
                        std::string result;
                        int size = colInfo.size;
                        std::string buffer;
                        buffer.resize(blockSize + 1);
                        while(size > 0)
                        {
                            SQLLEN datasize = 0;
                                // TODO: this isn't kosher even though it works... ended up with some stack corruption for LMCO
                            if(GetData(i, (SQLSMALLINT)SQL_C_CHAR, (SQLPOINTER)(buffer.data()), (SQLLEN)(sizeof(char) * blockSize), &datasize) && (datasize > 0))
                            {
                                result.append(buffer.c_str(), datasize);
                                size -= blockSize;
                            }
                            else
                            {
                                size = 0;
                            }
                        }
                        data[colInfo.name] = result;
                    }
                    break;
            }
        }
        return true;
    }

    bool Statement::finish(void)
    {
        _data->bindList.clear();
        return FreeStmt(SQL_CLOSE);
    }

    bool Statement::reset(void)
    {
        _data->bindList.clear();
        return FreeStmt(SQL_UNBIND) && FreeStmt(SQL_RESET_PARAMS);
    }

    bool Statement::getColumnInfo(int num, ColumnInfo &colInfo)
    {
        SQLCHAR *name = new SQLCHAR[stringSize + 1];
        SQLSMALLINT namelen;
        SQLSMALLINT datatype;
        SQLULEN size;
        SQLSMALLINT digits;
        SQLSMALLINT nullable;
        if(!DescribeCol(num, (SQLCHAR *)name, stringSize, &namelen, &datatype, &size, &digits, &nullable))
        {
            delete [] name;
            return false;
        }
        colInfo.name = (char *)name;
        colInfo.datatype = datatype;
        colInfo.size = size;
        colInfo.digits = digits;
        colInfo.nullable = nullable;
        delete [] name;
        return true;
    }

    bool Statement::BindCol(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_Ind)
    {
        return ODBC(SQLBindCol(handle, ColumnNumber, TargetType, TargetValuePtr, BufferLength, StrLen_or_Ind));
    }

    bool Statement::BindParameter(SQLUSMALLINT ParameterNumber, SQLSMALLINT InputOutputType, SQLSMALLINT ValueType, SQLSMALLINT ParameterType, SQLULEN ColumnSize, SQLSMALLINT DecimalDigits, SQLPOINTER ParameterValuePtr, SQLINTEGER BufferLength, SQLLEN *StrLen_or_IndPtr)
    {
        return ODBC(SQLBindParameter(handle, ParameterNumber, InputOutputType, ValueType, ParameterType, ColumnSize, DecimalDigits, ParameterValuePtr, BufferLength, StrLen_or_IndPtr));
    }

    bool Statement::BulkOperations(SQLUSMALLINT Operation)
    {
        return ODBC(SQLBulkOperations(handle, Operation));
    }

    bool Statement::Cancel(void)
    {
        return ODBC(SQLCancel(handle));
    }

    bool Statement::CloseCursor(void)
    {
        return ODBC(SQLCloseCursor(handle));
    }

    bool Statement::ColAttribute(SQLUSMALLINT ColumnNumber, SQLUSMALLINT FieldIdentifier, SQLPOINTER CharacterAttributePtr, SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr, SQLLEN *NumericAttributePtr)
    {
        return ODBC(SQLColAttribute(handle, ColumnNumber, FieldIdentifier, CharacterAttributePtr, BufferLength, StringLengthPtr, NumericAttributePtr));
    }

    bool Statement::ColumnPrivileges(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4)
    {
        return ODBC(SQLColumnPrivileges(handle, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3, ColumnName, NameLength4));
    }

    bool Statement::Columns(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4)
    {
        return ODBC(SQLColumns(handle, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3, ColumnName, NameLength4));
    }

    bool Statement::DescribeCol(SQLSMALLINT ColumnNumber, SQLCHAR *ColumnName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLengthPtr, SQLSMALLINT *DataTypePtr, SQLULEN *ColumnSizePtr, SQLSMALLINT *DecimalDigitsPtr, SQLSMALLINT *NullablePtr)
    {
        return ODBC(SQLDescribeCol(handle, ColumnNumber, ColumnName, BufferLength, NameLengthPtr, DataTypePtr, ColumnSizePtr, DecimalDigitsPtr, NullablePtr));
    }

    bool Statement::DescribeParam(SQLSMALLINT ParameterNumber, SQLSMALLINT *DataTypePtr, SQLULEN *ParameterSizePtr, SQLSMALLINT *DecimalDigitsPtr, SQLSMALLINT *NullablePtr)
    {
        return ODBC(SQLDescribeParam(handle, ParameterNumber, DataTypePtr, ParameterSizePtr, DecimalDigitsPtr, NullablePtr));
    }

    bool Statement::ExecDirect(SQLCHAR *StatementText, SQLINTEGER TextLength)
    {
        return ODBC(SQLExecDirect(handle, StatementText, TextLength));
    }

    bool Statement::Execute(void)
    {
        return ODBC(SQLExecute(handle));
    }
    
    bool Statement::Fetch(void)
    {
        return ODBC(SQLFetch(handle));
    }

    bool Statement::FetchScroll(SQLSMALLINT FetchOrientation, SQLLEN FetchOffset)
    {
        return ODBC(SQLFetchScroll(handle, FetchOrientation, FetchOffset));
    }

    bool Statement::ForeignKeys(SQLCHAR *PKCatalogName, SQLSMALLINT NameLength1, SQLCHAR *PKSchemaName, SQLSMALLINT NameLength2, SQLCHAR *PKTableName, SQLSMALLINT NameLength3, SQLCHAR *FKCatalogName, SQLSMALLINT NameLength4, SQLCHAR *FKSchemaName, SQLSMALLINT NameLength5, SQLCHAR *FKTableName, SQLSMALLINT NameLength6)
    {
        return ODBC(SQLForeignKeys(handle, PKCatalogName, NameLength1, PKSchemaName, NameLength2, PKTableName, NameLength3, FKCatalogName, NameLength4, FKSchemaName, NameLength5, FKTableName, NameLength6));
    }

    bool Statement::FreeStmt(SQLUSMALLINT Option)
    {
        return ODBC(SQLFreeStmt(handle, Option));
    }
    
    bool Statement::GetCursorName(SQLCHAR *CursorName, SQLSMALLINT BufferLength, SQLSMALLINT *NameLengthPtr)
    {
        return ODBC(SQLGetCursorName(handle, CursorName, BufferLength, NameLengthPtr));
    }

    bool Statement::GetData(SQLUSMALLINT ColumnNumber, SQLSMALLINT TargetType, SQLPOINTER TargetValuePtr, SQLLEN BufferLength, SQLLEN *StrLen_or_IndPtr)
    {
        return ODBC(SQLGetData(handle, ColumnNumber, TargetType, TargetValuePtr, BufferLength, StrLen_or_IndPtr));
    }

    bool Statement::GetStmtAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr)
    {
        return ODBC(SQLGetStmtAttr(handle, Attribute, ValuePtr, BufferLength, StringLengthPtr));
    }

    bool Statement::GetTypeInfo(SQLSMALLINT DataType)
    {
        return ODBC(SQLGetTypeInfo(handle, DataType));
    }

    bool Statement::MoreResults(void)
    {
        return ODBC(SQLMoreResults(handle));
    }

    bool Statement::NumParams(SQLSMALLINT *ParameterCountPtr)
    {
        return ODBC(SQLNumParams(handle, ParameterCountPtr));
    }

    bool Statement::NumResultCols(SQLSMALLINT *ColumnCountPtr)
    {
        return ODBC(SQLNumResultCols(handle, ColumnCountPtr));
    }

    bool Statement::ParamData(SQLPOINTER *ValuePtrPtr)
    {
        return ODBC(SQLParamData(handle, ValuePtrPtr));
    }

    bool Statement::Prepare(SQLCHAR *StatementText, SQLINTEGER TextLength)
    {
        return ODBC(SQLPrepare(handle, StatementText, TextLength));
    }

    bool Statement::PrimaryKeys(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3)
    {
        return ODBC(SQLPrimaryKeys(handle, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3));
    }

    bool Statement::ProcedureColumns(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *ProcName, SQLSMALLINT NameLength3, SQLCHAR *ColumnName, SQLSMALLINT NameLength4)
    {
        return ODBC(SQLProcedureColumns(handle, CatalogName, NameLength1, SchemaName, NameLength2, ProcName, NameLength3, ColumnName, NameLength4));
    }

    bool Statement::Procedures(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *ProcName, SQLSMALLINT NameLength3)
    {
        return ODBC(SQLProcedures(handle, CatalogName, NameLength1, SchemaName, NameLength2, ProcName, NameLength3));
    }

    bool Statement::PutData(SQLPOINTER DataPtr, SQLLEN StrLen_or_Ind)
    {
        return ODBC(SQLPutData(handle, DataPtr, StrLen_or_Ind));
    }

    bool Statement::RowCount(SQLLEN *RowCountPtr)
    {
        return ODBC(SQLRowCount(handle, RowCountPtr));
    }

    bool Statement::SetCursorName(SQLCHAR *CursorName, SQLSMALLINT NameLength)
    {
        return ODBC(SQLSetCursorName(handle, CursorName, NameLength));
    }

    bool Statement::SetPos(SQLSETPOSIROW RowNumber, SQLUSMALLINT Operation, SQLUSMALLINT LockType)
    {
        return ODBC(SQLSetPos(handle, RowNumber, Operation, LockType));
    }

    bool Statement::SetStmtAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength)
    {
        return ODBC(SQLSetStmtAttr(handle, Attribute, ValuePtr, StringLength));
    }

    bool Statement::SpecialColumns(SQLSMALLINT IdentifierType, SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLSMALLINT Scope, SQLSMALLINT Nullable)
    {
        return ODBC(SQLSpecialColumns(handle, IdentifierType, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3, Scope, Nullable));
    }

    bool Statement::Statistics(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLUSMALLINT Unique, SQLUSMALLINT Reserved)
    {
        return ODBC(SQLStatistics(handle, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3, Unique, Reserved));
    }

    bool Statement::TablePrivileges(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3)
    {
        return ODBC(SQLTablePrivileges(handle, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3));
    }

    bool Statement::Tables(SQLCHAR *CatalogName, SQLSMALLINT NameLength1, SQLCHAR *SchemaName, SQLSMALLINT NameLength2, SQLCHAR *TableName, SQLSMALLINT NameLength3, SQLCHAR *TableType, SQLSMALLINT NameLength4)
    {
        return ODBC(SQLTables(handle, CatalogName, NameLength1, SchemaName, NameLength2, TableName, NameLength3, TableType, NameLength4));
    }

}