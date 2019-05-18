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

#include "dom/Notation.h"

namespace dom
{
    struct Notation::_Notation
    {
        DOMString nodeName;
        DOMString publicId;
        DOMString systemId;
    };

    Notation::~Notation(void)
    {
        delete _data;
    }

    Notation::Notation(void) : _data(new _Notation)
    {
    }

    unsigned short Notation::getNodeType(void)
    {
        return NOTATION_NODE;
    }

    DOMString Notation::getNodeName(void)
    {
        return _data->nodeName;
    }

    DOMString Notation::getTextContent(void)
    {
        return DOMString();
    }

    void Notation::setTextContent(const DOMString &textContent)
    {
    }

    NodeSP Notation::cloneNode(bool deep)
    {
        NotationSP clone = std::dynamic_pointer_cast<Notation>(Node::cloneNode(deep));
        clone->_data->nodeName = _data->nodeName;
        clone->_data->publicId = _data->publicId;
        clone->_data->systemId = _data->systemId;
        return clone;
    }

    bool Notation::isDefaultNamespace(const DOMString &namespaceURI)
    {
        return false;
    }

    DOMString Notation::lookupNamespaceURI(const DOMString &prefix)
    {
        return DOMString();
    }

    DOMString Notation::lookupPrefix(const DOMString &namespaceURI)
    {
        return DOMString();
    }

    bool Notation::isReadOnly(void)
    {
        return true;
    }

    DOMString Notation::getPublicId(void)
    {
        return _data->publicId;
    }

    void Notation::setPublicId(const DOMString &publicId)
    {
        _data->publicId = publicId;
    }

    DOMString Notation::getSystemId(void)
    {
        return _data->systemId;
    }

    void Notation::setSystemId(const DOMString &systemId)
    {
        _data->systemId = systemId;
    }

}