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

#include "dbi/InsertQueue.h"
#include "dbi/Connection.h"

namespace dbi
{
    struct InsertQueue::_InsertQueue
    {
        ConnectionSP dbc;
        std::string table;
        std::vector<std::string> fields;
        ccl::VariantList values;
    };

    InsertQueue::~InsertQueue(void)
    {
        delete _data;
    }

    InsertQueue::InsertQueue(ConnectionSP dbc, const std::string &table, const std::vector<std::string> &fields) : _data(new _InsertQueue)
    {
        _data->dbc = dbc;
        _data->table = table;
        _data->fields = fields;
    }

    void InsertQueue::add(const ccl::VariantList &values)
    {
        _data->values.insert(_data->values.end(), values.begin(), values.end());
    }

    void InsertQueue::add(const ccl::Variant &value)
    {
        _data->values.push_back(value);
    }

    bool InsertQueue::execute(void)
    {
        if(!_data->dbc)
            return false;
        if(!_data->dbc->insert(_data->table, _data->fields, _data->values))
            return false;
        clear();
        return true;
    }

    void InsertQueue::clear(void)
    {
        _data->values.clear();
    }

}