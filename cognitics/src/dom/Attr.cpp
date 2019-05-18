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

#include "dom/Attr.h"
#include "dom/DOMException.h"
#include "dom/Element.h"

#include <typeinfo>

namespace dom
{
    struct Attr::_Attr
    {
        DOMString namespaceURI;
        DOMString name;
        Variant value;
        bool specified;
        bool Id;
    };

    Attr::~Attr(void)
    {
        delete _data;
    }

    Attr::Attr(void) : _data(new _Attr)
    {
    }

    unsigned short Attr::getNodeType(void)
    {
        return ATTRIBUTE_NODE;
    }

    DOMString Attr::getNodeName(void)
    {
        return getName();
    }

    DOMString Attr::getNodeValue(void)
    {
        return getValue().as_string();
    }

    DOMString Attr::getNamespaceURI(void)
    {
        return _data->namespaceURI;
    }

    void Attr::setNamespaceURI(const DOMString &namespaceURI)
    {
        _data->namespaceURI = namespaceURI;
    }

    DOMString Attr::getLocalName(void)
    {
        DOMString::size_type pos = _data->name.find(':');
        return (pos == _data->name.npos) ? _data->name : _data->name.substr(pos + 1);
    }

    DOMString Attr::getPrefix(void)
    {
        DOMString::size_type pos = _data->name.find(':');
        return (pos == _data->name.npos) ? DOMString() : _data->name.substr(0, pos - 1);
    }

    void Attr::setPrefix(const DOMString &prefix)
    {
        if(prefix.empty())
            _data->name = getLocalName();
        else
            _data->name = prefix + ":" + getLocalName();
    }

    NodeSP Attr::cloneNode(bool deep)
    {
        AttrSP clone = std::dynamic_pointer_cast<Attr>(Node::cloneNode(true));
        clone->_data->namespaceURI = _data->namespaceURI;
        clone->_data->name = _data->name;
        clone->_data->specified = _data->specified;
        clone->_data->value = _data->value;
        clone->_data->Id = _data->Id;
        return clone;
    }

    bool Attr::isId(void)
    {
        return _data->Id;
    }

    void Attr::setId(bool Id)
    {
        _data->Id = Id;
    }

    DOMString Attr::getName(void)
    {
        return _data->name;
    }

    void Attr::setName(const DOMString &name)
    {
        _data->name = name;
    }

    ElementSP Attr::getOwnerElement(void)
    {
        NodeSP parent(getParentNode());
        while(parent)
        {
            if(parent->getNodeType() == ELEMENT_NODE)
                return std::dynamic_pointer_cast<Element>(parent);
            parent = parent->getParentNode();
        }
        return ElementSP();
    }

/*
    TypeInfoSP Attr::getSchemaTypeInfo(void)
    {
        // TODO
        throw std::runtime_error("Attr::getSchemaTypeInfo(): not yet implemented");
    }
*/

    bool Attr::getSpecified(void)
    {
        return _data->specified;
    }

    void Attr::setSpecified(bool specified)
    {
        _data->specified = specified;
    }

    Variant Attr::getValue(void)
    {
        return _data->value;
    }

    void Attr::setValue(const Variant &value)
    {
        _data->value = value;
    }

}
