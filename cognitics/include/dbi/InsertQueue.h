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
/*! \file dbi/include/InsertQueue.h
\headerfile dbi/include/InsertQueue.h
\brief Provides dbi::InsertQueue.
\author Aaron Brinton <abrinton@cognitics.net>
\date 06 November 2010
*/
#pragma once

#include <ccl/ccl.h>
#include <string>
#include <vector>
//#include <boost/tr1/memory.hpp>

namespace dbi
{
    class Connection;
    typedef std::shared_ptr<Connection> ConnectionSP;

/*! \class dbi::InsertQueue InsertQueue.h InsertQueue.h
\brief Bulk Insert Queue

This class provides a queue for performing bulk inserts.

\code
// create a list of fields to populate
std::vector<std::string> fields;
fields.push_back("name");
fields.push_back("phone");

// create a queue for an existing database connection (dbc).
dbi::InsertQueue myQueue(dbc, "MyTable", fields);

// add using single values
myQueue.add("Aaron Brinton");
myQueue.add("208-123-4567");

// add using variant list
VariantList values;
values.push_back("Kevin Bentley");
values.push_back("208-321-7654");
myQueue.add(values);

// execute the insertion
if(!myQueue.execute())
    throw std::runtime_error("insert failed");
\endcode
*/
    class InsertQueue
    {
    private:
        struct _InsertQueue;
        _InsertQueue *_data;

    public:
        ~InsertQueue(void);

/*! \brief Create a new insert queue.
\param connection The dbi::Connection object to use for insertions.
\param table The database table into which execute() will insert rows.
\param fields A list of fields for value association. The value count must match a multiple of the field count.
*/
        InsertQueue(ConnectionSP connection, const std::string &table, const std::vector<std::string> &fields);

/*! \brief Add multiple values to the queue.
\param values The values to add to the queue.
*/
        void add(const ccl::VariantList &values);

/*! \brief Add a single value to the queue.
\param value The value to add to the queue.
*/
        void add(const ccl::Variant &value);

/*! \brief Execute the insertion query.
\note The queue is cleared if successful.
\return True if successful; false otherwise.
\sa Connection::insert()
*/
        bool execute(void);

/*! \brief Clear the queue.

This removes all values from the queue.
*/
        void clear(void);

    };

    typedef std::shared_ptr<InsertQueue> InsertQueueSP;

}
