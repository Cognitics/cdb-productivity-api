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

#include "dbi/Environment.h"
#include "dbi/Connection.h"

namespace dbi
{
    Environment::~Environment(void)
    {
    }

    Environment::Environment(SQLHANDLE handle) : Handle(SQL_HANDLE_ENV, handle)
    {
        SetEnvAttr(SQL_ATTR_ODBC_VERSION, (SQLPOINTER *)SQL_OV_ODBC3, 0);
    }

    EnvironmentSP Environment::create(void)
    {
        SQLHANDLE henv;
        SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &henv);
        return EnvironmentSP(new Environment(henv));
    }

    bool Environment::getDataSources(DataSourceList &dsList)
    {
        bool result = true;
        SQLCHAR *dsn = new SQLCHAR[stringSize + 1];
        SQLCHAR *description = new SQLCHAR[stringSize + 1];
        SQLSMALLINT dsnlen;
        SQLSMALLINT desclen;
        SQLUSMALLINT dir = SQL_FETCH_FIRST;
        while(DataSources(dir, dsn, stringSize, &dsnlen, description, stringSize, &desclen))
        {
            if(sqlReturn == SQL_ERROR)
                result = false;
            dir = SQL_FETCH_NEXT;
            if(SQL_SUCCEEDED(sqlReturn))
            {
                dsn[dsnlen] = 0;
                description[desclen] = 0;
                std::string k = (char *)dsn;
                std::string v = (char *)description;
                dsList[k] = v;
            }
        }
        delete [] dsn;
        delete [] description;
        return result;
    }

    DataSourceList Environment::getDataSources(void)
    {
        DataSourceList dsList;
        getDataSources(dsList);
        return dsList;
    }

    bool Environment::getDrivers(DriverList &driverList)
    {
        bool result = true;
        SQLCHAR *description = new SQLCHAR[stringSize + 1];
        SQLCHAR *attributes = new SQLCHAR[stringSize + 1];
        SQLSMALLINT desclen;
        SQLSMALLINT attrlen;
        SQLUSMALLINT dir = SQL_FETCH_FIRST;
        while(Drivers(dir, description, stringSize, &desclen, attributes, stringSize, &attrlen))
        {
            if(sqlReturn == SQL_ERROR)
                result = false;
            dir = SQL_FETCH_NEXT;
            if(SQL_SUCCEEDED(sqlReturn))
            {
                description[desclen] = 0;
                attributes[attrlen] = 0;
                std::string d = (char *)description;
                driverList[d].clear();        // create driver entry (so we get an entry even if there are no attributes)
                SQLCHAR *pk = attributes;
                while(*pk)
                {
                    SQLCHAR *pv = pk;
                    while(*pv && (*pv != '=')) { ++pv; }
                    if(*pv == '=')
                    {
                        *pv = 0;
                        ++pv;
                    }
                    else
                        pv = pk;
                    std::string k = (char *)pk;
                    std::string v = (char *)pv;
                    driverList[d][k] = v;
                    while(*pv)
                        ++pv;
                    ++pv;
                    pk = pv;
                }
            }
        }
        delete [] description;
        delete [] attributes;
        return result;
    }

    DriverList Environment::getDrivers(void)
    {
        DriverList driverList;
        getDrivers(driverList);
        return driverList;
    }

    ConnectionSP Environment::createConnection(void)
    {
        SQLHANDLE hdbc;
        return (AllocHandle(SQL_HANDLE_DBC, &hdbc)) ? ConnectionSP(new Connection(hdbc)) : ConnectionSP();
    }

    ConnectionSP Environment::connect(const std::string &dsn, const std::string &username, const std::string &password)
    {
        ConnectionSP dbc = createConnection();
        if(!dbc)
            return ConnectionSP();
        return dbc->connect(dsn, username, password) ? dbc : ConnectionSP();
    }

    ConnectionSP Environment::driverConnect(const std::string &connectString)
    {
        ConnectionSP dbc = createConnection();
        if(!dbc)
            return ConnectionSP();
        return dbc->driverConnect(connectString) ? dbc : ConnectionSP();
    }

/*
    void Environment::browseConnect()
    {
    }
*/

    SQLUINTEGER Environment::getAttrConnectionPooling(void)
    {
        SQLUINTEGER result = 0;
        return SQL_SUCCEEDED(SQLGetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, SQLPOINTER(&result), 0, NULL)) ? result : 0;
    }

    bool Environment::setAttrConnectionPooling(SQLUINTEGER value)
    {
        return SQL_SUCCEEDED(SQLSetEnvAttr(NULL, SQL_ATTR_CONNECTION_POOLING, (SQLPOINTER)value, 0));
    }

    SQLUINTEGER Environment::getAttrConnectionPoolMatching(void)
    {
        SQLUINTEGER result = 0;
        return GetEnvAttr(SQL_ATTR_CP_MATCH, SQLPOINTER(&result), 0, NULL) ? result : 0;
    }

    bool Environment::setAttrConnectionPoolMatching(SQLUINTEGER value)
    {
        return SetEnvAttr(SQL_ATTR_CP_MATCH, (SQLPOINTER)value, 0);
    }

    SQLUINTEGER Environment::getAttrODBCVersion(void)
    {
        SQLUINTEGER result = 0;
        return GetEnvAttr(SQL_ATTR_ODBC_VERSION, SQLPOINTER(&result), 0, NULL) ? result : 0;
    }

    bool Environment::setAttrODBCVersion(SQLUINTEGER value)
    {
        return SetEnvAttr(SQL_ATTR_ODBC_VERSION, (SQLPOINTER)value, 0);
    }

    SQLUINTEGER Environment::getAttrOutputNTS(void)
    {
        SQLUINTEGER result = 0;
        return GetEnvAttr(SQL_ATTR_OUTPUT_NTS, SQLPOINTER(&result), 0, NULL) ? result : 0;
    }

    bool Environment::setAttrOutputNTS(SQLUINTEGER value)
    {
        return SetEnvAttr(SQL_ATTR_OUTPUT_NTS, (SQLPOINTER)value, 0);
    }

    bool Environment::GetEnvAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr)
    {
        return ODBC(SQLGetEnvAttr(handle, Attribute, ValuePtr, BufferLength, StringLengthPtr));
    }

    bool Environment::SetEnvAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength)
    {
        return ODBC(SQLSetEnvAttr(handle, Attribute, ValuePtr, StringLength));
    }

    bool Environment::DataSources(SQLUSMALLINT Direction, SQLCHAR *ServerName, SQLSMALLINT BufferLength1, SQLSMALLINT *NameLength1Ptr, SQLCHAR *Description, SQLSMALLINT BufferLength2, SQLSMALLINT *NameLength2Ptr)
    {
        return ODBC(SQLDataSources(handle, Direction, ServerName, BufferLength1, NameLength1Ptr, Description, BufferLength2, NameLength2Ptr));
    }

    bool Environment::Drivers(SQLUSMALLINT Direction, SQLCHAR *DriverDescription, SQLSMALLINT BufferLength1, SQLSMALLINT *DescriptionLengthPtr, SQLCHAR *DriverAttributes, SQLSMALLINT BufferLength2, SQLSMALLINT *AttributesLengthPtr)
    {
        return ODBC(SQLDrivers(handle, Direction, DriverDescription, BufferLength1, DescriptionLengthPtr, DriverAttributes, BufferLength2, AttributesLengthPtr));
    }


}
