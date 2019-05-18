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

#include "dom/Entity.h"

namespace dom
{
    struct Entity::_Entity
    {
        DOMString publicId;
        DOMString systemId;
        DOMString notationName;
        DOMString inputEncoding;
        DOMString xmlEncoding;
        DOMString xmlVersion;
    };

    Entity::~Entity(void)
    {
        delete _data;
    }

    Entity::Entity(void) : _data(new _Entity)
    {
    }

    unsigned short Entity::getNodeType(void)
    {
        return ENTITY_NODE;
    }

    DOMString Entity::getNodeName(void)
    {
        return getNotationName();
    }

    bool Entity::isDefaultNamespace(const DOMString &namespaceURI)
    {
        return false;
    }

    DOMString Entity::lookupNamespaceURI(const DOMString &prefix)
    {
        return DOMString();
    }

    DOMString Entity::lookupPrefix(const DOMString &namespaceURI)
    {
        return DOMString();
    }

    NodeSP Entity::cloneNode(bool deep)
    {
        EntitySP clone = std::dynamic_pointer_cast<Entity>(Node::cloneNode(deep));
        clone->_data->publicId = _data->publicId;
        clone->_data->systemId = _data->systemId;
        clone->_data->notationName = _data->notationName;
        clone->_data->inputEncoding = _data->inputEncoding;
        clone->_data->xmlEncoding = _data->xmlEncoding;
        clone->_data->xmlVersion = _data->xmlVersion;
        return clone;
    }

    bool Entity::isReadOnly(void)
    {
        return true;
    }

    DOMString Entity::getPublicId(void)
    {
        return _data->publicId;
    }

    void Entity::setPublicId(const DOMString &publicId)
    {
        _data->publicId = publicId;
    }

    DOMString Entity::getSystemId(void)
    {
        return _data->systemId;
    }

    void Entity::setSystemId(const DOMString &systemId)
    {
        _data->systemId = systemId;
    }

    DOMString Entity::getInputEncoding(void)
    {
        return _data->inputEncoding;
    }

    void Entity::setInputEncoding(const DOMString &inputEncoding)
    {
        _data->inputEncoding = inputEncoding;
    }

    DOMString Entity::getXmlEncoding(void)
    {
        return _data->xmlEncoding;
    }

    void Entity::setXmlEncoding(const DOMString &xmlEncoding)
    {
        _data->xmlEncoding = xmlEncoding;
    }

    DOMString Entity::getXmlVersion(void)
    {
        return _data->xmlVersion;
    }

    void Entity::setXmlVersion(const DOMString &xmlVersion)
    {
        _data->xmlVersion = xmlVersion;
    }

    DOMString Entity::getNotationName(void)
    {
        return _data->notationName;
    }

    void Entity::setNotationName(const DOMString &notationName)
    {
        _data->notationName = notationName;
    }

}