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

#include "dom/DOMConfiguration.h"

namespace dom
{
    struct DOMConfiguration::_DOMConfiguration
    {
        std::map<DOMString, DOMUserDataSP> parameters;
    };

    DOMConfiguration::~DOMConfiguration(void)
    {
        delete _data;
    }

    DOMConfiguration::DOMConfiguration(void) : _data(new _DOMConfiguration)
    {
    }

    DOMStringList DOMConfiguration::getParameterNames(void)
    {
        DOMStringList result;
        for(std::map<DOMString, DOMUserDataSP>::iterator it = _data->parameters.begin(), end = _data->parameters.end(); it != end; ++it)
            result.push_back(it->first);
        return result;
    }

    void DOMConfiguration::setParameter(const DOMString &name, DOMUserDataSP value)
    {
        _data->parameters[name] = value;
    }

    DOMUserDataSP DOMConfiguration::getParameter(const DOMString &name)
    {
        std::map<DOMString, DOMUserDataSP>::iterator it = _data->parameters.find(name);
        return (it == _data->parameters.end()) ? DOMUserDataSP() : it->second;
    }

/*
    bool DOMConfiguration::canSetParameter(const DOMString &name, DOMUserDataSP value)
    {
        // TODO
        throw std::runtime_error("DOMConfiguration::canSetParameter(): not yet implemented");
    }
*/

}
