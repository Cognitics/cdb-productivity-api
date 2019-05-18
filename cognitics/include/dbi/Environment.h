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
/*! \file dbi/include/Environment.h
\headerfile dbi/include/Environment.h
\brief Provides dbi::Environment.
\author Aaron Brinton <abrinton@cognitics.net>
\date 25 June 2009
*/
#pragma once

#include "Handle.h"

namespace dbi
{
    typedef std::map<std::string, std::string> DataSourceList;
    typedef std::map<std::string, std::map<std::string, std::string> > DriverList;

    class Environment;
    typedef std::shared_ptr<Environment> EnvironmentSP;

    class Connection;
    //typedef std::shared_ptr<Connection> ConnectionSP;
    typedef std::shared_ptr<Connection> ConnectionSP;

/*! \class dbi::Environment Environment.h Environment.h
\brief ODBC Environment class.

This class is a wrapper for an ODBC environment handle.

\sa dbi::Handle, dbi::Connection
*/
    class Environment : public Handle
    {
    private:
        Environment(SQLHANDLE handle);

    public:
        virtual ~Environment(void);

/*! \brief Create a new Environment instance.

\code
dbi::EnvironmentSP env(dbi::Environment::create());
\endcode

\return The new Environment instance; empty if failed.
*/
        static EnvironmentSP create(void);

/*! \brief Get datasources information.

Retrieves a list of datasources available for connect().

\code
dbi::DataSourceList dsList;
if(!env->getDataSources(dsList))
    throw std::runtime_error("getDataSources() failed");
for(dbi::DataSourceList::iterator it = dsList.begin(), end = dsList.end(); it != end; ++it)
    std::cout << it->first << ": " << it->second << std::endl;
\endcode

\param dsList A dbi::DataSourceList to populate with results.
\return The list of datasources available for connect().
*/
        bool getDataSources(DataSourceList &dsList);

/*! \brief Get datasources information.

This is a shortcut version ignoring the result code.

\code
dbi::DataSourceList dsList = env->getDataSources();
\endcode

\return The list of datasources available for connect().
*/
        DataSourceList getDataSources(void);

/*! \brief Get driver information.

Retrieves a list of drivers (with parameters) available for driverConnect().

\code
dbi::DriverList driverList;
if(!env->getDrivers(driverList))
    throw std::runtime_error("getDrivers() failed");
for(dbi::DriverList::iterator it = driverList.begin(), end = driverList.end(); it != end; ++it)
{
    std::cout << it->first << ": " << std::endl;
    for(std::map<std::string,std::string>::iterator itx = it->second.begin(), endx = it->second.end(); itx != endx; ++itx)
        std::cout << "\t" << itx->first << " = " << itx->second << std::endl;
}
\endcode

\param driverList A dbi::DriverList to populate with results.
\return The list of drivers available for driverConnect().
*/
        bool getDrivers(DriverList &driverList);

/*! \brief Get driver information.

This is a shortcut version ignoring the result code.

\code
dbi::DriverList driverList = env->getDrivers();
\endcode

\return The list of drivers available for driverConnect().
*/
        DriverList getDrivers(void);

/*! \brief Create a Connection object.
\return The new Connection object, or empty if failed.
*/
        ConnectionSP createConnection(void);

/*! \brief Connect using a DSN.

\param dsn A datasource name matching an entry from getDataSources().
\param username The username for authentication.
\param password The password for authentication.
\return A new dbi::Connection instance.
*/
        ConnectionSP connect(const std::string &dsn, const std::string &username, const std::string &password);

/*! \brief Connect using a driver connection string.

This bypasses the user/system data source abstraction.

\code
dbi::EnvironmentSP env(dbi::Environment::create());
dbi::ConnectionSP dbc = env->driverConnect("DRIVER={SQL Server};SERVER=(local);Trusted_Connection=yes");
\endcode

\param connectString The database driver connection string.
\return A new dbi::Connection instance.
*/
        ConnectionSP driverConnect(const std::string &connectString);

/*! \brief Connect via browsing [NOT CURRENTLY IMPLEMENTED].
*/
        //void browseConnect();

/*! \brief Get status of connection pooling.
\return Current connection pooling setting: SQL_CP_OFF, SQL_CP_ONE_PER_DRIVER, or SQL_CP_ONE_PER_HENV.
*/
        static SQLUINTEGER getAttrConnectionPooling(void);

/*! \brief Set connection pooling.

Values:
\li SQL_CP_OFF - Connection pooling is turned off. This is the default.
\li SQL_CP_ONE_PER_DRIVER - A single connection pool is supported for each driver. Every connection in a pool is associated with one driver.
\li SQL_CP_ONE_PER_HENV - A single connection pool is supported for each environment. Every connection in a pool is associated with one environment.

\param value The value to assign (SQL_CP_OFF, SQL_CP_ONE_PER_DRIVER, or SQL_CP_ONE_PER_HENV).
\return True if successful; false otherwise.
\warning This is a static method that must be called PRIOR to creating the environment.
\sa ODBC Connection Pooling
*/
        static bool setAttrConnectionPooling(SQLUINTEGER value);

/*! \brief Get status of connection pool matching.
\return Current connection pool matching setting: SQL_CP_STRICT_MATCH OR SQL_CP_RELAXED_MATCH.
*/
        SQLUINTEGER getAttrConnectionPoolMatching(void);

/*! \brief Set connection pool matching.

Values:
\li SQL_CP_STRICT_MATCH - Only connections that exactly match the connection options in the call and the connection attributes set by the application are reused. This is the default.
\li SQL_CP_RELAXED_MATCH - Connections with matching connection string keywords can be used. Keywords must match, but not all connection attributes must match.

\param value The value to assign (SQL_CP_STRICT_MATCH or SQL_CP_RELAXED_MATCH).
\return True if successful; false otherwise.
\sa ODBC Connection Pooling
*/
        bool setAttrConnectionPoolMatching(SQLUINTEGER value);

/*! \brief Get status of version behavior.
\return Current connection pool matching setting: SQL_OV_ODBC3_80, SQL_OV_ODBC3, OR SQL_OV_ODBC2.
*/
        SQLUINTEGER getAttrODBCVersion(void);

/*! \brief Set version behavior status.

Values:
\li SQL_OV_ODBC3_80 - The Driver Manager and driver exhibit ODBC 3.8 behavior.
\li SQL_OV_ODBC3 - The Driver Manager and driver exhibit ODBC 3.x behavior. 
\li SQL_OV_ODBC2 - The Driver Manager and driver exhibit ODBC 2.x behavior. This is especially useful for an ODBC 2.x application working with an ODBC 3.x driver. 

\param value The value to assign (SQL_OV_ODBC3_80, SQL_OV_ODBC3, OR SQL_OV_ODBC2).
\return True if successful; false otherwise.
\sa ODBC Behavioral Changes
*/
        bool setAttrODBCVersion(SQLUINTEGER value);

/*! \brief Get status of null-termination of strings.
\return Current string null-termination setting: SQL_TRUE or SQL_FALSE.
*/
        SQLUINTEGER getAttrOutputNTS(void);

/*! \brief Set null-termination of strings.

Values:
\li SQL_TRUE - Result strings are null-terminated. This is the default.
\li SQL_FALSE - Result strings are not terminated.

\param value The value to assign (SQL_TRUE or SQL_FALSE).
\return True if successful; false otherwise.
*/
        bool setAttrOutputNTS(SQLUINTEGER value);

        //! [ODBC] SQLDataSources
        bool DataSources(SQLUSMALLINT Direction, SQLCHAR *ServerName, SQLSMALLINT BufferLength1, SQLSMALLINT *NameLength1Ptr, SQLCHAR *Description, SQLSMALLINT BufferLength2, SQLSMALLINT *NameLength2Ptr);
        //! [ODBC] SQLDrivers
        bool Drivers(SQLUSMALLINT Direction, SQLCHAR *DriverDescription, SQLSMALLINT BufferLength1, SQLSMALLINT *DescriptionLengthPtr, SQLCHAR *DriverAttributes, SQLSMALLINT BufferLength2, SQLSMALLINT *AttributesLengthPtr);
        //! [ODBC] SQLGetEnvAttr
        bool GetEnvAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER BufferLength, SQLINTEGER *StringLengthPtr);
        //! [ODBC] SQLSetEnvAttr
        bool SetEnvAttr(SQLINTEGER Attribute, SQLPOINTER ValuePtr, SQLINTEGER StringLength);
    };

}
