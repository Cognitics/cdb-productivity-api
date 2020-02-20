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

#include <ccl/ArgumentParser.h>

#include <iostream>
#include <sstream>

namespace cognitics
{
    void ArgumentParser::AddOption(const std::string& name, size_t parameter_count, const std::string& parameters, const std::string& description)
    {
        OptionsConfig[name] = OptionInfo { parameter_count, parameters, description };
    }

    void ArgumentParser::AddArgument(const std::string& name)
    {
        ArgumentsConfig.push_back(name);
    }

    int ArgumentParser::Parse(int argc, char** argv)
    {
        auto args = std::vector<std::string>(argc);
        for(int i = 0; i < argc; ++i)
            args[i] = argv[i];
        return Parse(args);
    }

    int ArgumentParser::Parse(const std::vector<std::string>& args)
    {
        Command = args[0];
        for(size_t i = 1, c = args.size(); i < c; ++i)
        {
            auto arg = args[i];
            if(arg[0] != '-')
            {
                ParsedArguments.push_back(arg);
                continue;
            }
            auto name = arg.substr(1);
            if(OptionsConfig.find(name) == OptionsConfig.end())
                return Usage("Unrecognized option: -" + name);
            auto& option = OptionsConfig[name];
            if(i + option.ParameterCount >= c)
                return Usage("Invalid option parameters: -" + name);
            if(ParsedOptions.find(name) == ParsedOptions.end())
                ParsedOptions[name] = std::vector<std::string>();
            for(int a = 0; a < option.ParameterCount; ++a)
                ParsedOptions[name].push_back(args[i + a + 1]);
            i += option.ParameterCount;
        }
        if(ParsedArguments.size() != ArgumentsConfig.size())
            return Usage("Missing arguments");
        return EXIT_SUCCESS;
    }

    int ArgumentParser::Usage(const std::string& message)
    {
        if(!message.empty())
            std::cerr << "ERROR: " << message << std::endl;
        std::cout << "Usage: " << Command;
        if(!OptionsConfig.empty())
            std::cout << " [options]";
        for(auto& arg : ArgumentsConfig)
            std::cout << " <" << arg << ">";
        std::cout << std::endl;
        std::cout << "Options:" << std::endl;
        for(auto& option : OptionsConfig)
        {
            // TODO: clean this up
            std::cout << "    -" << option.first << " " << option.second.Parameters << "    " << option.second.Description << std::endl;
        }
        return message.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    std::string ArgumentParser::Report()
    {
        std::stringstream ss;
        ss << Command;
        for(auto& arg : ParsedArguments)
            ss << " " << arg;
        ss << std::endl;
        for(auto& option : ParsedOptions)
        {
            ss << "    -" << option.first;
            for(auto& param : option.second)
                ss << " " << param;
            ss << std::endl;
        }
        return ss.str();
    }

    bool ArgumentParser::Option(const std::string& name)
    {
        return ParsedOptions.find(name) != ParsedOptions.end();
    }

    std::vector<std::string> ArgumentParser::Parameters(const std::string& name)
    {
        return Option(name) ? ParsedOptions[name] : std::vector<std::string>();
    }

    std::vector<std::string> ArgumentParser::Arguments()
    {
        return ParsedArguments;
    }


}