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

#include "dom/Element.h"
#include "dom/Document.h"
#include "dom/Attr.h"
#include "dom/DOMException.h"

namespace dom
{
    
    struct Element::_Element
    {
        DOMString namespaceURI;
        DOMString tagName;
        NamedNodeMap attributes;
    };

    Element::~Element(void)
    {
        delete _data;
    }

    Element::Element(void) : _data(new _Element)
    {
    }

    bool Element::isSupported(const DOMString &feature, const DOMString &version)
    {
        if(getFeature(feature, version))
            return true;
        return Node::isSupported(feature, version);
    }

    unsigned short Element::getNodeType(void)
    {
        return ELEMENT_NODE;
    }

    DOMString Element::getNodeName(void)
    {
        return getTagName();
    }

    NamedNodeMap Element::getAttributes(void)
    {
        return _data->attributes;
    }

    bool Element::hasAttributes(void)
    {
        return !(_data->attributes.empty());
    }

    DOMString Element::getNamespaceURI(void)
    {
        return _data->namespaceURI;
    }

    void Element::setNamespaceURI(const DOMString &namespaceURI)
    {
        _data->namespaceURI = namespaceURI;
    }

    DOMString Element::getLocalName(void)
    {
        DOMString::size_type pos = _data->tagName.find(':');
        return (pos == _data->tagName.npos) ? _data->tagName : _data->tagName.substr(pos + 1);
    }

    DOMString Element::getPrefix(void)
    {
        DOMString::size_type pos = _data->tagName.find(':');
        return (pos == _data->tagName.npos) ? _data->tagName : _data->tagName.substr(0, pos - 1);
    }

    void Element::setPrefix(const DOMString &prefix)
    {
        _data->tagName = prefix + ":" + getLocalName();
    }

    NodeSP Element::cloneNode(bool deep)
    {
        ElementSP clone = std::dynamic_pointer_cast<Element>(Node::cloneNode(deep));
        clone->_data->namespaceURI = _data->namespaceURI;
        clone->_data->tagName = _data->tagName;
        for(NamedNodeMap::iterator it = _data->attributes.begin(), end = _data->attributes.end(); it != end; ++it)
            clone->_data->attributes[it->first] = it->second->cloneNode(deep);
        return clone;
    }

/*
    bool Element::isDefaultNamespace(const DOMString &namespaceURI)
    {
        throw std::runtime_error("Element::isDefaultNamespace(): not yet implemented");

        if(getPrefix().empty())
            return getNamespaceURI() == namespaceURI;
 TODO:
if ( Element has attributes and there is a valid DOM Level 2
default namespace declaration, i.e. Attr�s localName == "xmlns" )
{
return (Attr�s value == namespaceURI);
}
        return Node::isDefaultNamespace(namespaceURI);
    }
*/

/*
    DOMString Element::lookupPrefix(const DOMString &namespaceURI)
    {
        // TODO
        throw std::runtime_error("Element::lookupPrefix(): not yet implemented");
    }
*/

/*
    DOMString Element::lookupNamespaceURI(const DOMString &prefix)
    {
        // TODO
        throw std::runtime_error("Element::lookupNamespaceURI(): not yet implemented");
    }
*/

/*
    TypeInfoSP Element::getSchemaTypeInfo(void)
    {
        // TODO
        throw std::runtime_error("Element::getSchemaTypeInfo(): not yet implemented");
    }
*/

    DOMString Element::getTagName(void)
    {
        return _data->tagName;
    }

    void Element::setTagName(const DOMString &tagName)
    {
        _data->tagName = tagName;
    }

    Variant Element::getAttribute(const DOMString &name)
    {
        AttrSP attr = getAttributeNode(name);
        return (attr) ? attr->getValue() : DOMString();
    }

    void Element::setAttribute(const DOMString &name, const Variant &value)
    {
        if(value.empty())
        {
            removeAttribute(name);
            return;
        }
        AttrSP attr(getAttributeNode(name));
        if(!attr)
        {
            attr = getOwnerDocument()->createAttribute(name);
            _data->attributes[name] = attr;
        }
        attr->setValue(value);
    }

    void Element::removeAttribute(const DOMString &name)
    {
        _data->attributes.erase(name);
    }

    AttrSP Element::getAttributeNode(const DOMString &name)
    {
        NamedNodeMap::iterator it = _data->attributes.find(name);
        return (it == _data->attributes.end()) ? AttrSP() : std::dynamic_pointer_cast<Attr>(it->second);
    }

    AttrSP Element::setAttributeNode(AttrSP newAttr)
    {
        AttrSP old = getAttributeNode(newAttr->getName());
        _data->attributes[newAttr->getName()] = newAttr;
        return old;
    }

    AttrSP Element::removeAttributeNode(AttrSP oldAttr)
    {
        _data->attributes.erase(oldAttr->getName());
        return oldAttr;
    }

    NodeList Element::getElementsByTagName(const DOMString &name)
    {
        NodeList result;
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(), end = childNodes.end(); it != end; ++it)
        {
            if((*it)->getNodeType() != ELEMENT_NODE)
                continue;
            ElementSP element(std::dynamic_pointer_cast<Element>(*it));
            if((name == "*") || (element->getTagName() == name))
                result.push_back(element);
        }
        return result;
    }

    Variant Element::getAttributeNS(const DOMString &namespaceURI, const DOMString &localName)
    {
        AttrSP attr = getAttributeNodeNS(namespaceURI, localName);
        return (attr) ? attr->getValue() : DOMString();
    }

    void Element::setAttributeNS(const DOMString &namespaceURI, const DOMString &qualifiedName, const Variant &value)
    {
        AttrSP attr(getAttributeNodeNS(namespaceURI, qualifiedName));
        if(!attr)
        {
            attr = getOwnerDocument()->createAttributeNS(namespaceURI, qualifiedName);
            _data->attributes[namespaceURI + ":" + qualifiedName] = attr;
        }
        attr->setValue(value);
    }

    void Element::removeAttributeNS(const DOMString &namespaceURI, const DOMString &localName)
    {
        removeAttribute(namespaceURI + ":" + localName);
    }

    AttrSP Element::getAttributeNodeNS(const DOMString &namespaceURI, const DOMString &localName)
    {
        NamedNodeMap::iterator it = _data->attributes.find(namespaceURI + ":" + localName);
        return (it == _data->attributes.end()) ? AttrSP() : std::dynamic_pointer_cast<Attr>(it->second);
    }

    AttrSP Element::setAttributeNodeNS(AttrSP newAttr)
    {
        AttrSP old = getAttributeNode(newAttr->getNamespaceURI() + ":" + newAttr->getLocalName());
        _data->attributes[newAttr->getNamespaceURI() + ":" + newAttr->getLocalName()] = newAttr;
        return old;
    }

    NodeList Element::getElementsByTagNameNS(const DOMString &namespaceURI, const DOMString &localName)
    {
        NodeList result;
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
        {
            if((namespaceURI == "*") || ((*it)->getNamespaceURI() == namespaceURI))
                if((localName == "*") || ((*it)->getLocalName() == localName))
                    result.push_back(*it);
        }
        return result;
    }

    bool Element::hasAttribute(const DOMString &name)
    {
        return (getAttributeNode(name).get() != NULL);
    }

    bool Element::hasAttributeNS(const DOMString &namespaceURI, const DOMString &localName)
    {
        return (getAttributeNodeNS(namespaceURI, localName).get() != NULL);
    }

    void Element::setIdAttribute(const DOMString &name, bool isId)
    {
        AttrSP attr(getAttributeNode(name));
        if(!attr)
            throw DOMException(NOT_FOUND_ERR);
        attr->setId(isId);
    }

    void Element::setIdAttributeNS(const DOMString &namespaceURI, const DOMString &localName, bool isId)
    {
        AttrSP attr(getAttributeNodeNS(namespaceURI, localName));
        if(!attr)
            throw DOMException(NOT_FOUND_ERR);
        attr->setId(isId);
    }

    void Element::setIdAttributeNode(AttrSP idAttr, bool isId)
    {
        idAttr->setId(isId);
    }

}
