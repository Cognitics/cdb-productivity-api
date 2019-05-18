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

#include "dom/DocumentType.h"

namespace dom
{    
    struct DocumentType::_DocumentType
    {
        DOMString name;
        NamedNodeMap entities;
        NamedNodeMap notations;
        DOMString publicId;
        DOMString systemId;
        DOMString internalSubset;
    };

    DocumentType::~DocumentType(void)
    {
        delete _data;
    }

    DocumentType::DocumentType(void) : _data(new _DocumentType)
    {
    }

    unsigned short DocumentType::getNodeType(void)
    {
        return DOCUMENT_TYPE_NODE;
    }

    DOMString DocumentType::getNodeName(void)
    {
        return getName();
    }

    DOMString DocumentType::getTextContent(void)
    {
        return DOMString();
    }

    void DocumentType::setTextContent(const DOMString &textContent)
    {
    }

    NodeSP DocumentType::cloneNode(bool deep)
    {
        DocumentTypeSP clone = std::dynamic_pointer_cast<DocumentType>(Node::cloneNode(deep));
        clone->_data->internalSubset = _data->internalSubset;
        clone->_data->name = _data->name;
        clone->_data->publicId = _data->publicId;
        clone->_data->systemId = _data->systemId;
        for(NamedNodeMap::iterator it = _data->entities.begin(); it != _data->entities.end(); ++it)
            clone->_data->entities[it->first] = it->second->cloneNode(deep);
        for(NamedNodeMap::iterator it = _data->notations.begin(); it != _data->notations.end(); ++it)
            clone->_data->notations[it->first] = it->second->cloneNode(deep);
        return clone;
    }

    bool DocumentType::isEqualNode(NodeSP arg)
    {
        if(!Node::isEqualNode(arg))
            return false;
        DocumentTypeSP argp = std::dynamic_pointer_cast<DocumentType>(arg);

        if(argp->getPublicId() != getPublicId())
            return false;
        if(argp->getSystemId() != getSystemId())
            return false;
        if(argp->getInternalSubset() != getInternalSubset())
            return false;
        if(argp->_data->entities.size() != _data->entities.size())
            return false;
        if(argp->_data->notations.size() != _data->notations.size())
            return false;
        for(NamedNodeMap::iterator it = _data->entities.begin(); it != _data->entities.end(); ++it)
        {
            if(argp->_data->entities.find(it->first) == argp->_data->entities.end())
                return false;
            if(!it->second->isEqualNode(argp->_data->entities[it->first]))
                return false;
        }
        for(NamedNodeMap::iterator it = _data->notations.begin(); it != _data->notations.end(); ++it)
        {
            if(argp->_data->notations.find(it->first) == argp->_data->notations.end())
                return false;
            if(!it->second->isEqualNode(argp->_data->notations[it->first]))
                return false;
        }
        return true;
    }

    bool DocumentType::isDefaultNamespace(const DOMString &namespaceURI)
    {
        return false;
    }

    DOMString DocumentType::lookupNamespaceURI(const DOMString &prefix)
    {
        return DOMString();
    }

    DOMString DocumentType::lookupPrefix(const DOMString &namespaceURI)
    {
        return DOMString();
    }

    bool DocumentType::isReadOnly(void)
    {
        return true;
    }

    DOMString DocumentType::getName(void)
    {
        return _data->name;
    }

    void DocumentType::setName(const DOMString &name)
    {
        _data->name = name;
    }

    NamedNodeMap DocumentType::getEntities(void)
    {
        return _data->entities;
    }

    NamedNodeMap DocumentType::getNotations(void)
    {
        return _data->notations;
    }

    DOMString DocumentType::getPublicId(void)
    {
        return _data->publicId;
    }

    void DocumentType::setPublicId(const DOMString &publicId)
    {
        _data->publicId = publicId;
    }

    DOMString DocumentType::getSystemId(void)
    {
        return _data->systemId;
    }

    void DocumentType::setSystemId(const DOMString &systemId)
    {
        _data->systemId = systemId;
    }

    DOMString DocumentType::getInternalSubset(void)
    {
        return _data->internalSubset;
    }

    void DocumentType::setInternalSubset(const DOMString &internalSubset)
    {
        _data->internalSubset = internalSubset;
    }

}
