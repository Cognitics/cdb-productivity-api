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

#include "dbi_sqlite/File.h"
#include "dbi/Statement.h"

namespace dbi
{
    namespace sqlite
    {

File::File()
{
    log.init("dbi_sqlite::File", this);
    log << ccl::LINFO;
    db = NULL;
}

File::~File()
{
    if (db)
    {
        sqlite3_close(db);
    }

}

bool File::connect(const std::string &filename)
{
    // Create the sql lite file if it doesn't exist
    // Try opening an existing file, or create it
    if (sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE, NULL))
    {
        // log << ccl::LERR << "Error: Unable to open or create " << filename << ". Error is " << std::string(sqlite3_errmsg(db)) << log.endl;
        return false;
    }
    sqlite3_enable_load_extension(db,1);
    return true;
}


bool File::create(const std::string &filename)
{
    // Create the sql lite file if it doesn't exist
    // Try opening an existing file, or create it
    if (sqlite3_open_v2(filename.c_str(), &db, SQLITE_OPEN_URI | SQLITE_OPEN_FULLMUTEX | SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE, NULL))
    {
        // log << ccl::LERR << "Error: Unable to open or create " << filename << ". Error is " << std::string(sqlite3_errmsg(db)) << log.endl;
        return false;
    }
    sqlite3_enable_load_extension(db, 1);

    return true;
}

Statement *File::createStatement()
{
    return new Statement(this);

}

void File::destroyStatement(Statement *stmt)
{
    delete stmt;
}


    }
}