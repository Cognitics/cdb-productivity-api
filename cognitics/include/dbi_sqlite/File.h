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
#include <ccl/cstdint.h>
#include <string>
#include <sqlite3.h>
#include <ccl/ObjLog.h>

#include <dbi_sqlite/Statement.h>
namespace dbi
{
    namespace sqlite
    {
        class Statement;
        class File
        {
            
            ccl::ObjLog log;
        public:
            sqlite3 *db;

            friend class Statement;
            File();

            ~File();

            /*! \brief Connect using a filename.

            \param filename An SQLite Database file
            */
            bool connect(const std::string &filename);

            /*! \brief Create a new file using a filename.

            \param filename An SQLite Database file
            */
            bool create(const std::string &filename);

            /*! \brief Create a Statement object.
            \return The new Statement object, or NULL if failed.
            */
            dbi::sqlite::Statement *createStatement();

            void destroyStatement(dbi::sqlite::Statement *stmt);

            std::string getErrorMessage()
            {
                return std::string(sqlite3_errmsg(db));
            }

            ccl::int64_t    getLastAutoIncrementKey()
            {
                return sqlite3_last_insert_rowid(db);
            }
        };

    }
}