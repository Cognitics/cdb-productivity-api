/****************************************************************************
Copyright 2016 Cognitics, Inc.
****************************************************************************/

//#pragma optimize("", off)

#pragma warning ( disable : 4996 )

#include "ccl/config.h"

#include "ccl/FileInfo.h"
#include "ccl/Log.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/istreamwrapper.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/error/en.h"

#include <cassert>
#include <map>
#include <iostream>
#include <sstream>
#include <cstdio>

#undef GetObject

namespace cognitics
{
    namespace config
    {
        DOM::keys::keys(void)
        {
        }

        DOM::keys::keys(const std::vector<std::string> &names) : names(names)
        {
        }

        DOM::keys::keys(const std::string &name)
        {
            names.push_back(name);
        }

        DOM::keys &DOM::keys::add(const std::string &name)
        {
            names.push_back(name);
            return *this;
        }

        DOM::~DOM(void)
        {
        }

        DOM::DOM(const char *appname) : appname(appname), document(rapidjson::kObjectType), allocator(document.GetAllocator())
        {
        }

        rapidjson::Value::Object DOM::get_app(void)
        {
            if(appname.empty())
                return document.GetObject();
            rapidjson::Value::MemberIterator it = document.FindMember(appname.c_str());
            if(it != document.MemberEnd())
            {
                if(!it->value.IsObject())
                {
                    it->value.SetObject();
                    log << ccl::LWARNING << "[CONFIG] WARNING: JSON '" << appname << "' type is not Object. It will be replaced with current parameters." << log.endl;
                }
                return it->value.GetObject();
            }
            rapidjson::Value jk(appname.c_str(), allocator);
            rapidjson::Value jv(rapidjson::kObjectType);
            rapidjson::Value &app = document.AddMember(jk, jv, allocator);
            return app.GetObject();
        }

        bool DOM::substitute_if_var(std::string &str)
        {
            size_t v1 = str.find_first_of("%");
            if(v1 == std::string::npos)
                return false;
            size_t v2 = str.find_first_of("%", v1 + 1);
            if(v2 == std::string::npos)
                return false;
            std::string var = str.substr(v1 + 1, v2 - v1 - 1);
            rapidjson::Value::MemberIterator it = document.FindMember(var.c_str());
            if(it == document.MemberEnd())
            {
                str.replace(v1, v2 - v1 + 1, "### MISSING VALUE FOR VARIABLE: " + var + " ###");
                return true;
            }
            ccl::Variant value;
            if(!get_variant(it->value, value))
            {
                str.replace(v1, v2 - v1 + 1, "### INVALID VALUE FOR VARIABLE: " + var + " ###");
                return true;
            }
            str.replace(v1, v2 - v1 + 1, value.as_string());
            return substitute_if_var(str);
        }

        bool DOM::get_variant(rapidjson::Value &jv, ccl::Variant &value)
        {
            /*
            kNullType = 0,      //!< null
            kFalseType = 1,     //!< false
            kTrueType = 2,      //!< true
            kObjectType = 3,    //!< object
            kArrayType = 4,     //!< array 
            kStringType = 5,    //!< string
            kNumberType = 6     //!< number
            */
            if(jv.IsNull())
            {
                value = ccl::Variant();
                return true;
            }
            if(jv.IsBool())
            {
                value = jv.GetBool();
                return true;
            }
            if(jv.IsString())
            {
                std::string str = jv.GetString();
                while(substitute_if_var(str))
                    continue;
                value = str;
                return true;
            }
            if(jv.IsNumber())
            {
                if(jv.IsDouble())
                    value = jv.GetDouble();
                else
                    value = jv.GetInt();
                return true;
            }
            return false;
        }

        bool DOM::has(const char *key)
        {
            return get_app().HasMember(key);
        }

        bool DOM::get(const char *key, ccl::Variant &value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it == app.MemberEnd())
                return false;
            if(get_variant(it->value, value))
                return true;
            ccl::VariantList values;
            if(!get(key, values) || values.empty())
                return false;
            value = *values.begin();
            return true;
        }

        bool DOM::get(const char *key, ccl::VariantList &values, bool clear)
        {
            if(clear)
                values.clear();
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it == app.MemberEnd())
                return false;
            if(!it->value.IsArray())
            {
                ccl::Variant value;
                if(!get(key, value))
                    return false;
                values.push_back(value);
                return true;
            }
            for(rapidjson::Value::ValueIterator array_it = it->value.GetArray().Begin(), array_end = it->value.GetArray().End(); array_it != array_end; ++array_it)
            {
                ccl::Variant value;
                if(!get_variant(*array_it, value))
                    return false;
                values.push_back(value);
            }
            return true;
        }

        void DOM::set(const char *key, bool value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it != app.MemberEnd())
            {
                it->value.SetBool(value);
                return;
            }
            rapidjson::Value jk(key, allocator);
            rapidjson::Value jv(value);
            app.AddMember(jk, jv, allocator);
        }

        void DOM::set(const char *key, int value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it != app.MemberEnd())
            {
                it->value.SetInt(value);
                return;
            }
            rapidjson::Value jk(key, allocator);
            rapidjson::Value jv(value);
            app.AddMember(jk, jv, allocator);
        }

        void DOM::set(const char *key, double value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it != app.MemberEnd())
            {
                it->value.SetDouble(value);
                return;
            }
            rapidjson::Value jk(key, allocator);
            rapidjson::Value jv(value);
            app.AddMember(jk, jv, allocator);
        }

        void DOM::set(const char *key, const char *value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it != app.MemberEnd())
            {
                it->value.SetString(value, allocator);
                return;
            }
            rapidjson::Value jk(key, allocator);
            rapidjson::Value jv(value, allocator);
            app.AddMember(jk, jv, allocator);
        }

        void DOM::add(const char *key, int value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it == app.MemberEnd())
            {
                rapidjson::Value jk(key, allocator);
                rapidjson::Value jv(rapidjson::kArrayType);
                app.AddMember(jk, jv, allocator);
                add(key, value);
                return;
            }
            if(!it->value.IsArray())
                it->value.SetArray();
            rapidjson::Value jv(value);
            it->value.PushBack(jv, allocator);
        }

        void DOM::add(const char *key, double value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it == app.MemberEnd())
            {
                rapidjson::Value jk(key, allocator);
                rapidjson::Value jv(rapidjson::kArrayType);
                app.AddMember(jk, jv, allocator);
                add(key, value);
                return;
            }
            if(!it->value.IsArray())
                it->value.SetArray();
            rapidjson::Value jv(value);
            it->value.PushBack(jv, allocator);
        }

        void DOM::add(const char *key, const char *value)
        {
            rapidjson::Value::Object app = get_app();
            rapidjson::Value::MemberIterator it = app.FindMember(key);
            if(it == app.MemberEnd())
            {
                rapidjson::Value jk(key, allocator);
                rapidjson::Value jv(rapidjson::kArrayType);
                app.AddMember(jk, jv, allocator);
                add(key, value);
                return;
            }
            if(!it->value.IsArray())
            {
                // convert whatever might be there to an array
                // note that this (intentionally) gets rid of any existing content
                it->value.SetArray();
            }
            rapidjson::Value jv(value, allocator);
            it->value.PushBack(jv, allocator);
        }

        bool DOM::remove(const char *key)
        {
            rapidjson::Value::Object app = get_app();
            return app.RemoveMember(key);
        }

        bool DOM::parse(int argc, char **argv, const keys &tail_keys)
        {
            bool dash_dump_config = false;
            bool dash_config = false;
            int argc_max = 3;
            for(int argi = 1; argi < argc; ++argi)
            {
                if(std::string(argv[argi]) == "-dump-config")
                {
                    ++argc_max;
                    dash_dump_config = true;
                    continue;
                }
                if(std::string(argv[argi]) != "-config")
                    continue;
                dash_config = true;
                if(argc > argc_max)
                {
                    log << ccl::LERR << "[CONFIG] ERROR: -config parameter cannot be used with other parameters." << log.endl;
                    return false;
                }
                if(argi + 1 > argc)
                {
                    log << ccl::LERR << "[CONFIG] ERROR: -config parameter missing filename." << log.endl;
                    return false;
                }
                std::string filename(argv[argi + 1]);
                if(!parse_file(filename))
                    return false;
            }
            if(!dash_config)
                parse_args(argc, argv, tail_keys);
            if(dash_dump_config)
                dump();
            return true;
        }

        void DOM::parse_args(int argc, char **argv, const keys &tail_keys)
        {
            int tail_count = int(tail_keys.names.size());

            rapidjson::Value::Object app = get_app();

            char *option = NULL;
            for(int argi = 0; argi < argc; ++argi)
            {
                char *param = argv[argi];

                // only check for options after the executable and before tail parameters
                if((argi > 0) && (argi < argc - tail_count))
                {
                    // if parameter has the '-' prefix and is not a number, it's an option key
                    if((param[0] == '-') && ((param[1] < '0') || (param[1] > '9')))
                    {
                        option = param;
                        // the option alone is treated as a boolean
                        // if it's not a standalone option, the boolean will get overwritten later
                        if(!app.HasMember(option))
                            set(option, true);
                        continue;
                    }

                    // if option is set, add the value to its list
                    if(option)
                    {
                        add(option, param);
                        continue;
                    }

                    // if in the option range but no option is set, fall through to the default handler
                }

                std::stringstream argi_ss;
                argi_ss << argi;
                std::string key = argi_ss.str();
                int tail_index = argi - argc + tail_count;
                if(tail_index >= 0)
                    key = tail_keys.names[tail_index];
                set(key.c_str(), param);
            }
        }

        bool DOM::parse_text(const std::string &json)
        {
            rapidjson::StringStream json_ss(json.c_str());
            rapidjson::ParseResult parse_ok = document.ParseStream(json_ss);
            if(!parse_ok)
            {
                log << ccl::LERR << "[CONFIG] PARSE ERROR: [" << parse_ok.Offset() << "] " << rapidjson::GetParseError_En(parse_ok.Code()) << " (" << parse_ok.Code() << ")" << log.endl;
                return false;
            }
            return true;
        }

        bool DOM::parse_file(const std::string &filename)
        {
            FILE *file = fopen(filename.c_str(), "r");
            if(!file)
            {
                log << ccl::LERR << "[CONFIG] READ ERROR: failed opening " << filename << log.endl;
                return false;
            }
            char buffer[1024 * 100];
            rapidjson::FileReadStream json_is(file, buffer, sizeof(buffer));
            rapidjson::ParseResult parse_ok = document.ParseStream(json_is);
            if(!parse_ok)
            {
                log << ccl::LERR << "[CONFIG] PARSE ERROR: [" << parse_ok.Offset() << "] " << rapidjson::GetParseError_En(parse_ok.Code()) << " (" << parse_ok.Code() << ")" << log.endl;
                fclose(file);
                return false;
            }
            fclose(file);
            return true;
        }

        bool DOM::write_text(std::string &json)
        {
            rapidjson::StringBuffer buffer;
            rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
            if(!document.Accept(writer))
                return false;
            json = buffer.GetString();
            return true;
        }

        bool DOM::write_file(const std::string &filename)
        {
            FILE *file = fopen(filename.c_str(), "w");
            if(!file)
            {
                log << ccl::LERR << "[CONFIG] WRITE ERROR: failed opening " << filename << log.endl;
                return false;
            }
            char buffer[1024 * 100];
            rapidjson::FileWriteStream json_os(file, buffer, sizeof(buffer));
            //rapidjson::Writer<rapidjson::FileWriteStream> writer(json_os);
            rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(json_os);
            if(!document.Accept(writer))
            {
                log << ccl::LERR << "[CONFIG] WRITE ERROR: failed writing " << filename << log.endl;
                fclose(file);
                return false;
            }
            fclose(file);
            return true;
        }

        void DOM::dump(void)
        {
            rapidjson::OStreamWrapper osw(std::cout);
            rapidjson::PrettyWriter<rapidjson::OStreamWrapper> writer(osw);
            document.Accept(writer);
        }

    }
}

