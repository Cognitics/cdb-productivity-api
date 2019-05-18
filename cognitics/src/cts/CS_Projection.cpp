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

#include "cts/CS_Projection.h"
#include <sstream>

namespace cts
{
    CS_Projection::~CS_Projection(void)
    {
    }

    CS_Projection::CS_Projection(void)
    {
    }

    void CS_Projection::setClassName(const std::string &className)
    {
        this->className = className;
    }

    std::string CS_Projection::getClassName(void)
    {
        return className;
    }

    CS_ProjectionParameter CS_Projection::getParameter(unsigned int index)
    {
        if(parameters.size() >= index)
            return CS_ProjectionParameter();
        std::map<std::string, double>::iterator it = parameters.begin();
        for(unsigned int i = 0; i < index; ++i, ++it);
        CS_ProjectionParameter param;
        param.name = it->first;
        param.value = it->second;
        return param;
    }

    int CS_Projection::getNumParameters(void)
    {
        return int(parameters.size());
    }

    void CS_Projection::setParameter(const std::string &name, double value)
    {
        parameters[name] = value;
    }

    double CS_Projection::getParameter(const std::string &name)
    {
        if(parameters.find(name) != parameters.end())
            return parameters[name];
        return 0;
    }

    void CS_Projection::removeParameter(const std::string &name)
    {
        std::map<std::string, double>::iterator it = parameters.find(name);
        if(it != parameters.end())
            parameters.erase(it);
    }

    // <projection> = PROJECTION["<name>" {,<authority>}]
    std::string CS_Projection::getWKT(void)
    {
        std::string authwkt = getAuthorityWKT();
        std::stringstream ss;
        ss << "PROJECTION[\"" << getName();
        if(authwkt.size())
            ss << "," << authwkt;
        ss << "]";
        ss.precision(8);
        ss << std::fixed;
        for(std::map<std::string, double>::iterator it = parameters.begin(), end = parameters.end(); it != end; ++it)
            ss << ",PARAMETER[\"" << it->first << "\", " << it->second << "]";
        return ss.str();
    }

}