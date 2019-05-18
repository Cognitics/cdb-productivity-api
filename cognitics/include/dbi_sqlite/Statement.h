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
#pragma once


#include <ccl/Variant.h>
#include <ccl/ObjLog.h>
#include <sqlite3.h>
#include <dbi_sqlite/File.h>
namespace dbi
{
    namespace sqlite
    {


        typedef ccl::Variant Variant;
        typedef ccl::VariantList VariantList;
        typedef ccl::VariantMap VariantMap;
        typedef ccl::VariantMapList VariantMapList;
        typedef VariantList Column;
        typedef VariantMap Row;
        typedef VariantMapList Rows;
        typedef std::map<std::string, std::string> StringStringMap_t;

        struct ColumnInfo
        {
            std::string name;
            int datatype;
            int size;
            int digits;
            int nullable;
        };


        class Statement
        {
            friend class File;
            ccl::ObjLog log;
            dbi::sqlite::File *file;//weak
            sqlite3_stmt *statement;//strong
            //std::vector<BindData> bindList;
            int col;//The current col binding to
            int numColumns;
            std::map<int, ColumnInfo> columns;
            void FinalizeStatement()
            {
                if (statement)
                {
                    //No return check here because it would just be the error code
                    // of the last call to return an error.
                    sqlite3_finalize(statement);
                    statement = NULL;
                }
                col = 1;
            }
            bool statement_done;
        public:
            Statement(dbi::sqlite::File *_file);
            ~Statement();


            /*! \brief Bind a parameter to the statement handle.
            \param value Variant representing the value to bind.
            \return True if successful; false otherwise.
            */
            bool bindParameter(const ccl::Variant &value);

            /*! \brief Fetch the next row from the current query execution.
            \param data Row to populate.
            \return True if successful; false otherwise.
            */
            bool fetchRow(Row &data);

            /*! \brief Finish a query
            \return True if successful; false otherwise.
            */
            bool finish(void);

            int getNumColumns();
            bool getColumnInfo(int num, ColumnInfo &colInfo);

            /*! \brief Prepare a query with the given parameters.
            \param query The query to prepare.
            \param parameters List of bind parameters.
            \return True if successful; false otherwise.
            */
            bool prepare(const std::string &query, const VariantList &parameters = VariantList());

            /*! \brief Execute a query with a temporary statement handle.
            \param query The query to execute.
            \param parameters List of bind parameters.
            This expression tree is parsed and appended to the query.
            \return True if query was executed successfully, false otherwise.
            */
            bool doQuery(const std::string &query, const VariantList &parameters = VariantList());

            /*! \brief Execute a query and return a single row.
            \param data The Row to populate.
            \param query The query to execute.
            \param parameters List of bind parameters.
            \param expression An expression tree representing filter parameters.
            \return True if query was executed successfully, false otherwise.
            */
            bool selectRow(Row &data, const std::string &query, const VariantList &parameters = VariantList());

            /*! \brief Execute a query and return all rows.
            \param data The Rows to populate.
            \param query The query to execute. E.g. select a,b from table1 where a=? and b=?
            \param parameters List of bind parameters.
            \param expression An expression tree representing filter parameters.
            \return True if query was executed successfully, false otherwise.
            */
            bool selectRows(Rows &data, const std::string &query,  const VariantList &parameters = VariantList());

            /*! \brief Insert a row into the specified table.
            \param table The table to insert into.
            \param data Map of field data to insert.
            \param specialColumns is a map of columns to replace via key/pairs. For example
                   "geometry" -> AsBinary(GeomFromText(?,4326)) The mapping must include the ? for the parameter it is bound to.
            \return True if insert was executed successfully, false otherwise.
            */
            bool insert(const std::string &table, const VariantMap &data, const StringStringMap_t &specialColumns);

            /*! \brief Insert a set of rows into the specified table.
            \param table The table to insert into.
            \param fields The fields to assign values.
            \param data List of field data to insert. This must match the field order and be a multiple of the field count.
            \param specialColumns is a map of columns to replace via key/pairs. For example
            "geometry" -> AsBinary(GeomFromText(?,4326)) The mapping must include the ? for the parameter it is bound to.
            \return True if insert was executed successfully, false otherwise.
            */
            bool insert(const std::string &table, const std::vector<std::string> &fields, const VariantList &values, const StringStringMap_t &specialColumns);

            /*! \brief Update row(s) in the specified table.
            \param table The table to update.
            \param data Map of field data to update.
            \param parameters list of field values to bind to the where clause.
            \param specialColumns is a map of columns to replace via key/pairs. For example
            "geometry" -> AsBinary(GeomFromText(?,4326)) The mapping must include the ? for the parameter it is bound to.
            \param the where clause (including the word where and ? for each parameter).
            \return True if update was executed successfully, false otherwise.
            */
            bool update(const std::string &table, const VariantMap &data, const VariantList &parameters, const std::string &where , const StringStringMap_t &specialColumns);

        };
    };

}