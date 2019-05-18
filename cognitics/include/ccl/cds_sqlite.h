/****************************************************************************
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
#pragma once

#include "ccl/cds.h"
#include "sqlite/sqlite3.h"

namespace cognitics
{
    namespace cds
    {
        template <>
        struct traits<sqlite3>
        {
            typedef sqlite3_stmt *handle;

            static bool construct(sqlite3 *obj)
            {
                return (sqlite3_exec(obj, "CREATE TABLE IF NOT EXISTS cds (uuid TEXT, type TEXT, description TEXT); CREATE UNIQUE INDEX IF NOT EXISTS cds_uuid ON cds (uuid)", NULL, NULL, NULL) == SQLITE_OK);
            }

            static bool empty(sqlite3 *obj)
            {
                return (size(obj) == 0);
            }

            static size_t size(sqlite3 *obj)
            {
                sqlite3_stmt *stmt = NULL;
                std::string query = "SELECT COUNT(*) FROM cds";
                if(sqlite3_prepare_v2(obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return 0;
                if(sqlite3_step(stmt) != SQLITE_ROW)
                    return 0;
                size_t result = (size_t)sqlite3_column_int(stmt, 0);
                sqlite3_finalize(stmt);
                return result;
            }

            static size_t max_size(sqlite3 *obj)
            {
                return size_t(-1);
            }

            static std::pair<registry<sqlite3>::iterator, bool> insert(sqlite3 *obj, const boost::uuids::uuid &uuid, const std::string &type, const std::string &description)
            {
                registry<sqlite3>::iterator it = find(obj, uuid);
                if(it != end(obj))
                    return std::make_pair(it, false);
                sqlite3_stmt *stmt = NULL;
                std::string uuidstr = boost::uuids::to_string(uuid);
                std::string query = "INSERT INTO cds VALUES (?, ?, ?)";
                if(sqlite3_prepare_v2(obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return std::make_pair(end(obj), false);
                if(sqlite3_bind_text(stmt, 1, uuidstr.c_str(), int(uuidstr.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return std::make_pair(end(obj), false);
                if(sqlite3_bind_text(stmt, 2, type.c_str(), int(type.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return std::make_pair(end(obj), false);
                if(sqlite3_bind_text(stmt, 3, description.c_str(), int(description.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return std::make_pair(end(obj), false);
                if(sqlite3_step(stmt) != SQLITE_DONE)
                    return std::make_pair(end(obj), false);
                if(sqlite3_finalize(stmt) != SQLITE_OK)
                    return std::make_pair(end(obj), false);
                it = find(obj, uuid);
                return std::make_pair(it, true);
            }

            static bool erase(sqlite3 *obj, const boost::uuids::uuid &uuid)
            {
                sqlite3_stmt *stmt = NULL;
                std::string uuidstr = boost::uuids::to_string(uuid);
                std::string query = "DELETE FROM cds WHERE uuid=?";
                if(sqlite3_prepare_v2(obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 1, uuidstr.c_str(), int(uuidstr.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_step(stmt) != SQLITE_DONE)
                    return false;
                if(sqlite3_finalize(stmt) != SQLITE_OK)
                    return false;
                return true;
            }

            static bool clear(sqlite3 *obj)
            {
                return (sqlite3_exec(obj, "DELETE FROM cds", NULL, NULL, NULL) == SQLITE_OK);
            }

            static registry<sqlite3>::iterator find(sqlite3 *obj, const boost::uuids::uuid &uuid)
            {
                sqlite3_stmt *stmt = NULL;
                std::string uuidstr = boost::uuids::to_string(uuid);
                std::string query = "SELECT * FROM cds WHERE uuid=?";
                if(sqlite3_prepare_v2(obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return end(obj);
                if(sqlite3_bind_text(stmt, 1, uuidstr.c_str(), int(uuidstr.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return end(obj);
                if(sqlite3_step(stmt) != SQLITE_ROW)
                    return end(obj);
                return registry<sqlite3>::iterator(obj, stmt);
            }

            static registry<sqlite3>::iterator begin(sqlite3 *obj)
            {
                sqlite3_stmt *stmt = NULL;
                std::string query = "SELECT * FROM cds";
                if(sqlite3_prepare_v2(obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return end(obj);
                if(sqlite3_step(stmt) != SQLITE_ROW)
                    return end(obj);
                return registry<sqlite3>::iterator(obj, stmt);
            }

            static registry<sqlite3>::iterator end(sqlite3 *obj)
            {
                return registry<sqlite3>::iterator(obj, NULL);
            }

            static void advance(registry<sqlite3>::entry &e)
            {
                if(sqlite3_step(e.handle) != SQLITE_ROW)
                    e.handle = NULL;
            }

            static registry<sqlite3>::iterator advance(registry<sqlite3>::entry &e, int)
            {
                // TODO: this needs to create a new statement at the appropriate location
                sqlite3_step(e.handle);
                return registry<sqlite3>::iterator(e.obj, e.handle);
            }

            static boost::uuids::uuid get_uuid(const registry<sqlite3>::entry &e)
            {
                return (e.handle == NULL) ? boost::uuids::nil_generator()() : boost::uuids::string_generator()((char *)sqlite3_column_text(e.handle, 0));
            }

            static std::string get_type(const registry<sqlite3>::entry &e)
            {
                return (e.handle == NULL) ? std::string() : std::string((char *)sqlite3_column_text(e.handle, 1));
            }

            static std::string get_description(const registry<sqlite3>::entry &e)
            {
                return (e.handle == NULL) ? std::string() : std::string((char *)sqlite3_column_text(e.handle, 2));
            }

            static bool set(registry<sqlite3>::entry &e, const std::string &type, const std::string &description)
            {
                sqlite3_stmt *stmt = NULL;
                std::string uuidstr = boost::uuids::to_string(get_uuid(e));
                std::string query = "UPDATE cds SET type=?,description=? WHERE uuid=?";
                if(sqlite3_prepare_v2(e.obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 1, uuidstr.c_str(), int(uuidstr.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 2, type.c_str(), int(type.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 3, description.c_str(), int(description.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_step(stmt) != SQLITE_DONE)
                    return false;
                if(sqlite3_finalize(stmt) != SQLITE_OK)
                    return false;
                return true;
            }

            static bool set_type(registry<sqlite3>::entry &e, const std::string &type)
            {
                sqlite3_stmt *stmt = NULL;
                std::string uuidstr = boost::uuids::to_string(get_uuid(e));
                std::string query = "UPDATE cds SET type=? WHERE uuid=?";
                if(sqlite3_prepare_v2(e.obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 1, uuidstr.c_str(), int(uuidstr.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 2, type.c_str(), int(type.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_step(stmt) != SQLITE_DONE)
                    return false;
                if(sqlite3_finalize(stmt) != SQLITE_OK)
                    return false;
                return true;
            }

            static bool set_description(registry<sqlite3>::entry &e, const std::string &description)
            {
                sqlite3_stmt *stmt = NULL;
                std::string uuidstr = boost::uuids::to_string(get_uuid(e));
                std::string query = "UPDATE cds SET description=? WHERE uuid=?";
                if(sqlite3_prepare_v2(e.obj, query.c_str(), int(query.size()), &stmt, NULL) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 1, uuidstr.c_str(), int(uuidstr.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_bind_text(stmt, 2, description.c_str(), int(description.size()), SQLITE_TRANSIENT) != SQLITE_OK)
                    return false;
                if(sqlite3_step(stmt) != SQLITE_DONE)
                    return false;
                if(sqlite3_finalize(stmt) != SQLITE_OK)
                    return false;
                return true;
            }

        };

    }
}