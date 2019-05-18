/****************************************************************************
Copyright (c) 2015 Cognitics, Inc.

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

//#pragma optimize("", off)

//#include <cstdint.h>
#include "dbi_sqlite/Statement.h"
#include "dbi_sqlite/File.h"


#include <sfa/Geometry.h>

namespace dbi
{
    namespace sqlite
    {

        struct BindData
        {
            ccl::Variant var;
            sfa::Geometry *geom;//strong
            BindData(const ccl::Variant &v) : var(v), geom(NULL)
            {

            }

            BindData(sfa::Geometry *_geom) : var(0)
            {
                geom = _geom->copy();
            }

            ~BindData()
            {
                if (geom)
                    delete geom;
            }

        };

        Statement::Statement(dbi::sqlite::File *_file) : file(_file), statement(NULL)
        {
            log.init("dbi_sqlite::File", this);
            log << ccl::LINFO;

        }

        Statement::~Statement()
        {
            FinalizeStatement();
        }

        bool Statement::doQuery(const std::string &query, const VariantList &parameters)
        {
            prepare(query, parameters);
            int res = sqlite3_step(statement);

            if (res == SQLITE_ROW || res == SQLITE_DONE)
            {
                return true;
            }
            else
            {
                log << ccl::LERR << "Unable to perfom query: (" << query << ") : " << file->getErrorMessage() << log.endl;
                return false;
            }

        }


        bool Statement::prepare(const std::string &query, const VariantList &parameters)
        {
            FinalizeStatement();// We need a new statement
            int res = sqlite3_prepare_v2(file->db, query.c_str(), int(query.size()), &statement, NULL);
            if (res != SQLITE_OK)
            {
                log << ccl::LERR << "Unable to prepare statement. " << file->getErrorMessage() << log.endl;
                return false;
            }
            sqlite3_reset(statement);
            statement_done = false;
            sqlite3_clear_bindings(statement);


            VariantList::const_iterator iter = parameters.begin();
            while (iter != parameters.end())
            {
                bindParameter(*iter++);
            }
            numColumns = 0;
            columns.clear();
            return true;
        }


        bool Statement::bindParameter(const ccl::Variant &value)
        {
            switch (value.type())
            {
            case ccl::Variant::TYPE_BIGINT:
            {
                if (sqlite3_bind_int64(statement, col, value.as_bigint()) != SQLITE_OK)
                {
                    log << ccl::LERR << "Error: Unable to bind parameter " << col << "(" << file->getErrorMessage() << ")" << log.endl;
                }
                break;
            }
            case ccl::Variant::TYPE_INT16://Variant automatically handles these type conversions
            case ccl::Variant::TYPE_INT8:
            case ccl::Variant::TYPE_INT:
            {
                if (sqlite3_bind_int(statement, col, value.as_int()) != SQLITE_OK)
                {
                    log << ccl::LERR << "Error: Unable to bind parameter " << col << "(" << file->getErrorMessage() << ")" << log.endl;
                }
                break;
            }

            case ccl::Variant::TYPE_FLOAT://Variant automatically handles these type conversions
            case ccl::Variant::TYPE_DOUBLE:
            {
                if (sqlite3_bind_double(statement, col, value.as_double()) != SQLITE_OK)
                {
                    log << ccl::LERR << "Error: Unable to bind double parameter " << col << "(" << file->getErrorMessage() << ")" << log.endl;
                }
                break;
            }

            case ccl::Variant::TYPE_BINARY:
            {
                ccl::binary bin = value.as_binary();
                int res = sqlite3_bind_blob(statement, col, bin.c_str(), int(bin.size()), SQLITE_TRANSIENT);
                                              
                if (res != SQLITE_OK)
                {
                    log << ccl::LERR << "Error: Unable to bind blob parameter " << col << "(" << file->getErrorMessage() << ")" << log.endl;
                    return false;
                }
                break;
            }
            case ccl::Variant::TYPE_STRING:
            default:
            {
                std::string val = value.as_string();
                if (sqlite3_bind_text(statement, col, val.c_str(), int(val.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                {
                    log << ccl::LERR << "Error: Unable to bind string parameter " << col << "(" << file->getErrorMessage() << ")" << log.endl;
                    return false;
                }
            }
            }
            col++;
            return true;
        }

        bool Statement::selectRow(Row &data, const std::string &query, const VariantList &whereParameters)
        {
            FinalizeStatement();// We need a new statement
            prepare(query, whereParameters);
            return fetchRow(data);
        }

        bool Statement::fetchRow(Row &data)
        {
            int res = sqlite3_step(statement);

            if (res == SQLITE_ROW )
            {
                //Get the column info after the first step, but not later.
                if (numColumns == 0)
                {
                    int num_cols = getNumColumns();
                    for (int i = 0; i < num_cols; i++)
                    {
                        ColumnInfo ci;
                        getColumnInfo(i, ci);
                    }
                }
                //for (size_t i = 0, ic = columns.size(); i < ic; i++)//0 based column indices for results, which is different than the binding
                std::map<int, ColumnInfo>::iterator iter = columns.begin();
                while(iter!=columns.end())
                {
                    ColumnInfo &ci = iter->second;
                    int i = iter->first;
                    iter++;
                    switch (ci.datatype)
                    {
                    case ccl::Variant::TYPE_BIGINT:
                    {
                        ccl::int64_t value = sqlite3_column_int64(statement, (int)i);
                        ccl::Variant var(value);
                        data[ci.name] = var;
                        break;
                    }
                    case ccl::Variant::TYPE_DOUBLE:
                    {
                        double value = sqlite3_column_double(statement, (int)i);
                        ccl::Variant var(value);
                        data[ci.name] = var;
                        break;
                    }
                    case ccl::Variant::TYPE_STRING:
                    {
                        int len = sqlite3_column_bytes(statement, (int)i);
                        const unsigned char *p = sqlite3_column_text(statement, (int)i);
                        std::string value((const char *)p, len);
                        ccl::Variant var(value);
                        data[ci.name] = var;
                        break;
                    }
                    case ccl::Variant::TYPE_BINARY:
                    {
                        int len = sqlite3_column_bytes(statement, (int)i);
                        const unsigned char *p = sqlite3_column_text(statement, (int)i);
                        ccl::binary value(p, len);
                        ccl::Variant var(value);
                        data[ci.name] = var;
                        break;
                    }
                    case ccl::Variant::TYPE_EMPTY:
                    {
                        ccl::Variant var;
                        data[ci.name] = var;
                        break;
                    }
                    default:
                        ;
                    }
                }
                return true;
            }
            if (res != SQLITE_DONE)
            {
                log << "Error: " << __FILE__ << __LINE__ << std::string(sqlite3_errmsg(file->db)) << log.endl;
            }
            return false;
        }

        int Statement::getNumColumns()
        {
            numColumns = sqlite3_column_count(statement);
            return numColumns;
        }

        bool Statement::getColumnInfo(int num, ColumnInfo &colInfo)
        {
            const char *name = sqlite3_column_name(statement, num);
            if (!name)
                return false;
            colInfo.name = name;
            int type = sqlite3_column_type(statement, num);
            switch (type)
            {
            case SQLITE_INTEGER:
                colInfo.datatype = ccl::Variant::TYPE_BIGINT;
                break;
            case SQLITE_FLOAT:
                colInfo.datatype = ccl::Variant::TYPE_DOUBLE;
                break;
            case SQLITE_BLOB:
                colInfo.datatype = ccl::Variant::TYPE_BINARY;
                break;
            case SQLITE_TEXT:
                colInfo.datatype = ccl::Variant::TYPE_STRING;
                break;
            case SQLITE_NULL:
                colInfo.datatype = ccl::Variant::TYPE_EMPTY;
                break;
            default:
                colInfo.datatype = ccl::Variant::TYPE_STRING;
                break;
            }
            columns[num] = colInfo;
            return true;
        }

        bool Statement::selectRows(Rows &data, const std::string &query, const VariantList &parameters)
        {
            prepare(query, parameters);
            Row row;
            while (fetchRow(row))
            {
                data.push_back(row);
                row.clear();
            }
            return data.size() > 0;
        }

        bool Statement::insert(const std::string &table, const VariantMap &data, const StringStringMap_t &specialColumns)
        {
            std::vector<std::string> fields;
            VariantList values;
            VariantMap::const_iterator iter = data.begin();
            while (iter != data.end())
            {
                fields.push_back(iter->first);
                values.push_back(iter->second);
            }
            return insert(table, fields, values, specialColumns);
        }

        bool Statement::insert(const std::string &table, const std::vector<std::string> &fields, const VariantList &values, const StringStringMap_t &specialColumns)
        {
            if (fields.size() != values.size())
            {
                log << "Statement::insert(): Values and field sizes are not equal.";
                return false;
            }
            std::stringstream queryss;
            queryss << "insert into " << table;
            if (fields.size() > 0)
            {
                queryss << "(";
                for (size_t i = 0, ic = fields.size(); i < ic; i++)
                {
                    if (i > 0)
                        queryss << ",";
                    queryss << fields.at(i);
                }
                queryss << ")";
            }
            if (values.size() > 0)
            {
                queryss << "values(";
                for (size_t i = 0, ic = values.size(); i < ic; i++)
                {
                    if (i > 0)
                        queryss << ",";

                    StringStringMap_t::const_iterator citer = specialColumns.find(fields.at(i));
                    if (citer != specialColumns.end())
                    {
                        queryss << citer->second;
                    }
                    else
                    {
                        queryss << "?";
                    }
                }
                queryss << ")";
            }
            //log << "Query: " << queryss.str() << log.endl;
            //log << "Values:" << log.endl;
            //for (size_t i = 0, ic = values.size(); i < ic; i++)
            //{
            //    log << "\t" << values.at(i).as_string() << log.endl;
            //}

            return doQuery(queryss.str(), values);
        }

        /*! \brief Update row(s) in the specified table.
        \param table The table to update.
        \param data Map of field data to update.
        \param the where clause (including the word where and ? for each parameter).
        \return True if update was executed successfully, false otherwise.
        */
        bool Statement::update(const std::string &table, const VariantMap &data, const VariantList &parameters, const std::string &where, const StringStringMap_t &specialColumns)
        {
            std::stringstream queryss;
            queryss << "update " << table;
            if (data.size() > 0)
            {
                queryss << "set ";
                bool needComma = false;
                VariantMap::const_iterator fielditer = data.begin();
                while (fielditer != data.end());
                
                {
                    if (needComma)
                        queryss << ",";
                    StringStringMap_t::const_iterator citer = specialColumns.find(fielditer->first);
                    if (citer != specialColumns.end())
                    {
                        queryss << fielditer->first;
                        queryss << "=";
                        queryss << citer->second;
                    }
                    else
                    {
                        queryss << fielditer->first;
                        queryss << "=?";
                    }
                    needComma = true;
                    bindParameter(fielditer->second);
                }                
            }

            return false;
        }

    }
}