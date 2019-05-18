/****************************************************************************
Copyright 2016 Cognitics, Inc.
****************************************************************************/
#pragma once

#include <rapidjson/document.h>
#include <ccl/Variant.h>
#include <ccl/ObjLog.h>
#include <vector>
#include <string>

/*
The cognitics::config namespace contains functionality for managing configuration
parameters. It uses the RapidJSON DOM as the data structure.
*/


namespace cognitics
{
    namespace config
    {
        class DOM
        {
        private:
            std::string appname;
            ccl::ObjLog log;
            rapidjson::Document document;
            rapidjson::Document::AllocatorType &allocator;

            rapidjson::Value::Object get_app(void); 
            bool substitute_if_var(std::string &str);
            bool get_variant(rapidjson::Value &jv, ccl::Variant &value);

        public:

            struct keys
            {
                std::vector<std::string> names;
                keys(void);
                keys(const std::vector<std::string> &names);
                keys(const std::string &name);
                keys &add(const std::string &name);
            };

            ~DOM(void);
            DOM(const char *appname = NULL);

            // does the app object have the specified key?
            bool has(const char *key);

            // get a variant value for the specified key
            // returns false if not found, is an object, or is an empty array
            // if value is an array, the first value is returned
            bool get(const char *key, ccl::Variant &value);

            // get a variant list for the specified key
            // result is array elements or a single element if not an array
            // returns false if not found or is an object
            // note that this will return true if empty
            // if clear is true (default), variant list is cleared prior to populating
            bool get(const char *key, ccl::VariantList &values, bool clear = true);

            // set value for the specified key, overwriting any existing value
            void set(const char *key, bool value);
            void set(const char *key, int value);
            void set(const char *key, double value);
            void set(const char *key, const char *value);

            // add value for the specified key
            // any non-array will be converted, overwriting any existing value
            void add(const char *key, int value);
            void add(const char *key, double value);
            void add(const char *key, const char *value);

            // remove value for the specified key
            bool remove(const char *key);

            // combines parse_args() and parse_config_file() based on -config <filename>
            bool parse(int argc, char **argv, const keys &tail_keys = keys());

            // parse command-line arguments into the document
            // parameters are required to be in the form: [-option [value]*]* <tail>*
            void parse_args(int argc, char **argv, const keys &tail_keys = keys());

            // parse configuration parameters from a JSON string
            // returns false if parsing failed
            bool parse_text(const std::string &json);

            // parse configuration parameters from a JSON file
            // returns false if the file is missing or has parse errors
            bool parse_file(const std::string &filename);

            bool write_text(std::string &json);

            bool write_file(const std::string &filename);

            void dump(void);

        };


    }
}