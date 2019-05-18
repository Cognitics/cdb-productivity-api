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

#include "dom/DOMError.h"

namespace dom
{
    struct DOMError::_DOMError
    {
        unsigned short severity;
        DOMString message;
        DOMString type;
        DOMObjectSP relatedException;
        DOMObjectSP relatedData;
        DOMLocatorSP location;
    };

    DOMError::~DOMError(void)
    {
        delete _data;
    }

    DOMError::DOMError(void) : _data(new _DOMError)
    {
    }

    unsigned short DOMError::getSeverity(void)
    {
        return _data->severity;
    }

    void DOMError::setSeverity(unsigned short severity)
    {
        _data->severity = severity;
    }

    DOMString DOMError::getMessage(void)
    {
        return _data->message;
    }

    void DOMError::setMessage(const DOMString &message)
    {
        _data->message = message;
    }

    DOMString DOMError::getType(void)
    {
        return _data->type;
    }

    void DOMError::setType(const DOMString &type)
    {
        _data->type = type;
    }

    DOMObjectSP DOMError::getRelatedException(void)
    {
        return _data->relatedException;
    }

    void DOMError::setRelatedException(DOMObjectSP relatedException)
    {
        _data->relatedException = relatedException;
    }

    DOMObjectSP DOMError::getRelatedData(void)
    {
        return _data->relatedData;
    }

    void DOMError::setRelatedData(DOMObjectSP relatedData)
    {
        _data->relatedData = relatedData;
    }

    DOMLocatorSP DOMError::getLocation(void)
    {
        return _data->location;
    }
    
    void DOMError::setLocation(DOMLocatorSP location)
    {
        _data->location = location;
    }

}