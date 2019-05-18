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

#include "dbi/Connection.h"
#include "dbi/Statement.h"

#include <cstdarg>

namespace dbi
{
    struct Connection::_Connection
    {
        std::map<ccl::OpType, std::string> ops;

        _Connection(void)
        {
            ops[ccl::AND] = "AND";
            ops[ccl::NOT] = "NOT";
            ops[ccl::OR] = "OR";
            ops[ccl::XOR] = "XOR";
            ops[ccl::EQ] = "=";
            ops[ccl::NE] = "<>";
            ops[ccl::LT] = "<";
            ops[ccl::LTEQ] = "<=";
            ops[ccl::GT] = ">";
            ops[ccl::GTEQ] = ">=";
        }

        std::string buildWhereClause(dbi::VariantList &parameters, ccl::OpSP expression)
        {
            std::string query;
            ccl::ExpressionList children = expression->getChildren();
            for(ccl::ExpressionList::iterator it = children.begin(), end = children.end(); it != end; ++it)
            {
                ccl::ExpressionSP child(*it);
                if(!query.empty())
                    query += " " + ops[expression->getOpType()] + " ";
                switch(child->getType())
                {
                    case ccl::KEY:
                        query += std::dynamic_pointer_cast<ccl::Key>(child)->getKey();
                        break;
                    case ccl::VALUE:
                        query += "?";
                        parameters.push_back(std::dynamic_pointer_cast<ccl::Value>(child)->getValue());
                        break;
                    case ccl::OP:
                        query += "(" + buildWhereClause(parameters, std::dynamic_pointer_cast<ccl::Op>(child)) + ")";
                        break;
                }
            }
            return query;
        }

    };

    Connection::~Connection(void)
    {
        SQLDisconnect(handle);
        delete _data;
    }

    Connection::Connection(SQLHANDLE handle) : Handle(SQL_HANDLE_DBC, handle), _data(new _Connection)
    {
    }

    bool Connection::connect(const std::string &dsn, const std::string &username, const std::string &password)
    {
        return Connect((SQLCHAR *)dsn.c_str(), SQL_NTS, (SQLCHAR *)username.c_str(), SQL_NTS, (SQLCHAR *)password.c_str(), SQL_NTS);
    }

    bool Connection::driverConnect(const std::string &connectString)
    {
        SQLSMALLINT connstrlen;
        SQLCHAR *dbc_connstr = new SQLCHAR[stringSize + 1];
        bool result = DriverConnect((SQLCHAR *)connectString.c_str(), SQL_NTS, dbc_connstr, stringSize, &connstrlen, SQL_DRIVER_NOPROMPT);
        delete [] dbc_connstr;
        return result;
    }

    StatementSP Connection::createStatement(void)
    {
        SQLHANDLE hstmt;
        return (AllocHandle(SQL_HANDLE_STMT, &hstmt)) ? StatementSP(new Statement(hstmt)) : StatementSP();
    }

    StatementSP Connection::prepare(const std::string &query, const VariantList &parameters)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return StatementSP();
        if(!stmt->prepare(query))
            return StatementSP();
        for(VariantList::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return StatementSP();
        }
        return stmt;
    }

    bool Connection::Do(const std::string &query, const VariantList &parameters, ccl::OpSP expression)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        for(VariantList::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        if(!expression)
            return stmt->Do(query.c_str());

        VariantList whereParameters;
        std::string xQuery = query + " (" + _data->buildWhereClause(whereParameters, expression) + ")";
        for(VariantList::iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        return stmt->Do(xQuery.c_str());
    }

    bool Connection::Do(const std::string &query, const Variant &parameter, ccl::OpSP expression)
    {
        dbi::VariantList doParameters;
        doParameters.push_back(parameter);
        return Do(query, doParameters, expression);
    }

    bool Connection::selectRow(Row &data, const std::string &query, const VariantList &parameters, ccl::OpSP expression)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        VariantList whereParameters;
        std::string xQuery = (expression) ? query + " (" + _data->buildWhereClause(whereParameters, expression) + ")" : query;
        if(!stmt->prepare(xQuery))
            return false;
        data.clear();
        for(VariantList::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        for(VariantList::iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        return (stmt->Execute() && stmt->fetchRow(data));
    }

    bool Connection::selectRow(Row &data, const std::string &query, const Variant &parameter, ccl::OpSP expression)
    {
        dbi::VariantList parameters;
        parameters.push_back(parameter);
        return selectRow(data, query, parameters, expression);
    }

    bool Connection::selectRow(Row &data, const std::string &table, ccl::OpSP expression)
    {
        return selectRow(data, "SELECT * FROM " + table + " WHERE ", VariantList(), expression);
    }

    bool Connection::selectRow(Row &data, const std::string &table, const VariantMap &whereParameters)
    {
        ccl::OpSP expression(new ccl::Op);
        for(VariantMap::const_iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
            expression->addOp(ccl::EQ, it->first, it->second);
        return selectRow(data, table, expression);
    }

    bool Connection::selectRows(Rows &data, const std::string &query, const VariantList &parameters, ccl::OpSP expression)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        VariantList whereParameters;
        std::string xQuery = (expression) ? query + " (" + _data->buildWhereClause(whereParameters, expression) + ")" : query;
        if(!stmt->prepare(xQuery))
            return false;
        data.clear();
        for(VariantList::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        for(VariantList::iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        if(!stmt->Execute())
            return false;
        Row row;
        while(stmt->fetchRow(row))
            data.push_back(row);
        return true;
    }

    bool Connection::selectRows(Rows &data, const std::string &query, const Variant &parameter, ccl::OpSP expression)
    {
        dbi::VariantList parameters;
        parameters.push_back(parameter);
        return selectRows(data, query, parameters, expression);
    }

    bool Connection::selectRows(Rows &data, const std::string &table, ccl::OpSP expression)
    {
        return selectRows(data, "SELECT * FROM " + table + " WHERE ", VariantList(), expression);
    }

    bool Connection::selectRows(Rows &data, const std::string &table, const VariantMap &whereParameters)
    {
        ccl::OpSP expression(new ccl::Op);
        for(VariantMap::const_iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
            expression->addOp(ccl::EQ, it->first, it->second);
        return selectRows(data, table, expression);
    }

    bool Connection::selectColumn(Column &data, const std::string &query, const VariantList &parameters, ccl::OpSP expression)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        VariantList whereParameters;
        std::string xQuery = (expression) ? query + " (" + _data->buildWhereClause(whereParameters, expression) + ")" : query;
        if(!stmt->prepare(xQuery))
            return false;
        data.clear();
        for(VariantList::const_iterator it = parameters.begin(), end = parameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        for(VariantList::iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        if(!stmt->Execute())
            return false;
        Row row;
        while(stmt->fetchRow(row))
            data.push_back(row.begin()->second);
        return true;
    }

    bool Connection::selectColumn(Column &data, const std::string &query, const Variant &parameter, ccl::OpSP expression)
    {
        dbi::VariantList parameters;
        parameters.push_back(parameter);
        return selectColumn(data, query, parameters, expression);
    }

    bool Connection::selectColumn(Column &data, const std::string &table, ccl::OpSP expression)
    {
        return selectColumn(data, "SELECT * FROM " + table + " WHERE ", VariantList(), expression);
    }

    bool Connection::selectColumn(Column &data, const std::string &table, const VariantMap &whereParameters)
    {
        ccl::OpSP expression(new ccl::Op);
        for(VariantMap::const_iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
            expression->addOp(ccl::EQ, it->first, it->second);
        return selectColumn(data, table, expression);
    }

    bool Connection::insert(const std::string &table, const VariantMap &data)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        std::string qnames = "";
        std::string qparams = "";
        for(VariantMap::const_iterator it = data.begin(), end = data.end(); it != end; ++it)
        {
            if(it != data.begin())
            {
                qnames += ",";
                qparams += ",";
            }
            qnames += it->first;
            qparams += "?";
            if(!stmt->bindParameter(it->second))
                return false;
        }
        std::string query = "INSERT INTO " + table;
        if(data.size())
            query += " (" + qnames + ") VALUES (" + qparams + ")";
        return stmt->Do(query);
    }

    bool Connection::insert(const std::string &table, const std::vector<std::string> &fields, const VariantList &values)
    {
        static int max = 100;
        if(fields.empty())
            return false;
        if(values.empty())
            return true;
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        std::string qnames;
        std::string qparams;
        for(std::vector<std::string>::const_iterator it = fields.begin(), end = fields.end(); it != end; ++it)
        {
            if(it != fields.begin())
            {
                qnames += ",";
                qparams += ",";
            }
            qnames += *it;
            qparams += "?";
        }
        //std::string baseQuery = "INSERT INTO " + table + " (" + qnames + ") VALUES ";
        std::string baseQuery = "INSERT INTO " + table + " (" + qnames + ") ";
        VariantList::const_iterator it = values.begin();
        while(it < values.end())
        {
            //std::string query = baseQuery + "(" + qparams + ")";
            std::string query = baseQuery + " SELECT " + qparams;
            for(int c = 0; c < max, it != values.end(); ++c)
            {
                if(c > 0)
                    query += " UNION ALL SELECT " + qparams;
                for(int i = 0; i != fields.size(); ++i)
                {
                    if(it == values.end())
                        return false;
                    if(!stmt->bindParameter(*it))
                        return false;
                    ++it;
                }
            }
            if(!stmt->Do(query))
                return false;
        }
        return true;
    }

    bool Connection::update(const std::string &table, const VariantMap &data, ccl::OpSP expression)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        std::string query = "UPDATE " + table + " SET ";
        for(VariantMap::const_iterator it = data.begin(), end = data.end(); it != end; ++it)
        {
            if(it != data.begin())
                query += ",";
            query += it->first + "=?";
        }
        VariantList whereParameters;
        std::string xQuery = (expression) ? query + " WHERE (" + _data->buildWhereClause(whereParameters, expression) + ")" : query;
        for(VariantMap::const_iterator it = data.begin(), end = data.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(it->second))
                return false;
        }
        for(VariantList::iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        return stmt->Do(xQuery);
    }

    bool Connection::update(const std::string &table, const VariantMap &data, const VariantMap &whereParameters)
    {
        ccl::OpSP expression(new ccl::Op);
        for(VariantMap::const_iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
            expression->addOp(ccl::EQ, it->first, it->second);
        return update(table, data, expression);
    }

    bool Connection::update(const std::string &table, const VariantMap &data, const boost::uuids::uuid &uuid)
    {
        dbi::VariantMap parameters;
        parameters["uuid"] = uuid;
        return update(table, data, parameters);
    }

    bool Connection::Delete(const std::string &table, ccl::OpSP expression)
    {
        StatementSP stmt = createStatement();
        if(!stmt)
            return false;
        std::string query = "DELETE FROM " + table;
        VariantList whereParameters;
        std::string xQuery = (expression) ? query + " WHERE (" + _data->buildWhereClause(whereParameters, expression) + ")" : query;
        for(VariantList::iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
        {
            if(!stmt->bindParameter(*it))
                return false;
        }
        return stmt->Do(xQuery);
    }

    bool Connection::Delete(const std::string &table, const VariantMap &whereParameters)
    {
        ccl::OpSP expression(new ccl::Op);
        for(VariantMap::const_iterator it = whereParameters.begin(), end = whereParameters.end(); it != end; ++it)
            expression->addOp(ccl::EQ, it->first, it->second);
        return Delete(table, expression);
    }

    bool Connection::Delete(const std::string &table, const boost::uuids::uuid &uuid)
    {
        dbi::VariantMap parameters;
        parameters["uuid"] = uuid;
        return Delete(table, parameters);
    }

    bool Connection::getAutoCommit(void)
    {
        return (getAttrAutoCommit() == SQL_AUTOCOMMIT_ON);
    }

    bool Connection::setAutoCommit(bool autoCommit)
    {
        return setAttrAutoCommit(autoCommit ? SQL_AUTOCOMMIT_ON : SQL_AUTOCOMMIT_OFF);
    }

    SQLUINTEGER Connection::getAttrAutoCommit(void)
    {
        SQLUINTEGER result = 0;
        return GetConnectAttr(SQL_ATTR_AUTOCOMMIT, SQLPOINTER(&result), 0, NULL) ? result : 0;
    }

    bool Connection::setAttrAutoCommit(SQLUINTEGER value)
    {
        return SetConnectAttr(SQL_ATTR_AUTOCOMMIT, (SQLPOINTER)value, 0);
    }

    bool Connection::BrowseConnect(SQLCHAR *InConnectionString, SQLSMALLINT StringLength1, SQLCHAR *OutConnectionString, SQLSMALLINT BufferLength, SQLSMALLINT *StringLength2Ptr)
    {
        return ODBC(SQLBrowseConnect(handle, InConnectionString, StringLength1, OutConnectionString, BufferLength, StringLength2Ptr));
    }

    bool Connection::Connect(SQLCHAR *ServerName, SQLSMALLINT NameLength1, SQLCHAR *UserName, SQLSMALLINT NameLength2, SQLCHAR *Authentication, SQLSMALLINT NameLength3)
    {
        return ODBC(SQLConnect(handle, ServerName, NameLength1, UserName, NameLength2, Authentication, NameLength3));
    }

    bool Connection::DriverConnect(SQLCHAR *InConnectionString, SQLSMALLINT StringLength1, SQLCHAR *OutConnectionString, SQLSMALLINT BufferLength, SQLSMALLINT *StringLength2Ptr, SQLUSMALLINT DriverCompletion)
    {
        return ODBC(SQLDriverConnect(handle, NULL, InConnectionString, StringLength1, OutConnectionString, BufferLength, StringLength2Ptr, DriverCompletion));
    }

    bool Connection::Disconnect()
    {
        return ODBC(SQLDisconnect(handle));
    }

    bool Connection::GetConnectAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr)
    {
        return ODBC(SQLGetConnectAttr(handle, Attribute, ValuePtr, BufferLength, StringLengthPtr));
    }

    bool Connection::SetConnectAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength)
    {
        return ODBC(SQLSetConnectAttr(handle, Attribute, ValuePtr, StringLength));
    }

    bool Connection::GetFunctions(SQLUSMALLINT FunctionId, SQLUSMALLINT *SupportedPtr)
    {
        return ODBC(SQLGetFunctions(handle, FunctionId, SupportedPtr));
    }

    bool Connection::GetInfo(SQLUSMALLINT InfoType, SQLPOINTER InfoValuePtr, SQLSMALLINT BufferLength, SQLSMALLINT *StringLengthPtr)
    {
        return ODBC(SQLGetInfo(handle, InfoType, InfoValuePtr, BufferLength, StringLengthPtr));
    }

    bool Connection::NativeSql(SQLCHAR *InStatementText, SQLINTEGER TextLength1, SQLCHAR *OutStatementText, SQLINTEGER BufferLength, SQLINTEGER *TextLength2Ptr)
    {
        return ODBC(SQLNativeSql(handle, InStatementText, TextLength1, OutStatementText, BufferLength, TextLength2Ptr));
    }

}