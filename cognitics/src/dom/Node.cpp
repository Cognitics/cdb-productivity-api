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

#include "dom/Node.h"
#include "dom/Document.h"
#include "dom/Text.h"
#include "dom/Element.h"
#include "dom/Attr.h"
#include "dom/UserDataHandler.h"
#include "dom/CDATASection.h"
#include "dom/EntityReference.h"
#include "dom/Entity.h"
#include "dom/ProcessingInstruction.h"
#include "dom/Comment.h"
#include "dom/DocumentFragment.h"
#include "dom/DocumentType.h"
#include "dom/Notation.h"
#include "dom/DOMImplementation.h"
#include "dom/DOMException.h"

namespace dom
{
    struct Node::_Node
    {
        NodeWP parentNode;
        NodeList childNodes;
        DocumentWP ownerDocument;
        std::map<DOMString, DOMUserDataSP> userData;
        std::map<DOMString, UserDataHandlerSP> userDataHandlers;
    };

    Node::~Node(void)
    {
        delete _data;
    }

    Node::Node(void) : _data(new _Node)
    {
    }

    NamedNodeMap Node::getAttributes(void)
    {
        return NamedNodeMap();
    }

    DOMString Node::getBaseURI(void)
    {
        return DOMString();
    }

    NodeList Node::getChildNodes(void)
    {
        return _data->childNodes;
    }

    NodeSP Node::getFirstChild(void)
    {
        return _data->childNodes.empty() ? NodeSP() : *(_data->childNodes.begin());
    }

    NodeSP Node::getLastChild(void)
    {
        return _data->childNodes.empty() ? NodeSP() : *(_data->childNodes.end() - 1);
    }

    DOMString Node::getLocalName(void)
    {
        return DOMString();
    }

    DOMString Node::getNamespaceURI(void)
    {
        return DOMString();
    }

    NodeSP Node::getNextSibling(void)
    {
        NodeSP parent(_data->parentNode.lock());
        if(!parent)
            return NodeSP();
        if(parent->_data->childNodes.empty())
            return NodeSP();
        NodeSP self = std::dynamic_pointer_cast<Node, DOMObject>(shared_from_this());
        NodeList::iterator it = std::find(parent->_data->childNodes.begin(), parent->_data->childNodes.end(), self);
        if(it == parent->_data->childNodes.end())
            return NodeSP();
        ++it;
        if(it == parent->_data->childNodes.end())
            return NodeSP();
        return *it;
    }

    DOMString Node::getNodeName(void)
    {
        return DOMString();
    }

    unsigned short Node::getNodeType(void)
    {
        return 0;
    }

    DOMString Node::getNodeValue(void)
    {
        return 0;
    }

    void Node::setNodeValue(const Variant &nodeValue)
    {
    }

    DocumentSP Node::getOwnerDocument(void)
    {
        return _data->ownerDocument.lock();
    }

    NodeSP Node::getParentNode(void)
    {
        return _data->parentNode.lock();
    }

    DOMString Node::getPrefix(void)
    {
        return DOMString();
    }

    void Node::setPrefix(const DOMString &prefix)
    {
    }

    NodeSP Node::getPreviousSibling(void)
    {
        NodeSP parent(_data->parentNode.lock());
        if(!parent)
            return NodeSP();
        if(parent->_data->childNodes.empty())
            return NodeSP();
        NodeSP self = std::dynamic_pointer_cast<Node, DOMObject>(shared_from_this());
        NodeList::iterator it = std::find(parent->_data->childNodes.begin(), parent->_data->childNodes.end(), self);
        if(it == parent->_data->childNodes.end())
            return NodeSP();
        if(it == parent->_data->childNodes.begin())
            return NodeSP();
        return *(it - 1);
    }

    DOMString Node::getTextContent(void)
    {
        DOMString result;
        for(NodeList::iterator it = _data->childNodes.begin(), end = _data->childNodes.end(); it != end; ++it)
        {
            if((*it)->getNodeType() == COMMENT_NODE)
                continue;
            if((*it)->getNodeType() == PROCESSING_INSTRUCTION_NODE)
                continue;
            result.append((*it)->getTextContent());
        }
        return result;
    }

    void Node::setTextContent(const DOMString &textContent)
    {
        _data->childNodes.clear();
        TextSP node(getOwnerDocument()->createTextNode(textContent));
        appendChild(node);
    }

    NodeSP Node::appendChild(NodeSP newChild)
    {
        NodeSP parent(_data->parentNode.lock());
        if(newChild->getNodeType() == DOCUMENT_FRAGMENT_NODE)
        {
            for(NodeList::iterator it = newChild->_data->childNodes.begin(), end = newChild->_data->childNodes.end(); it != end; ++it)
                appendChild(*it);
        }
        else
        {
            _data->childNodes.insert(_data->childNodes.end(), newChild);
            newChild->setParentNode(std::dynamic_pointer_cast<Node, DOMObject>(shared_from_this()));
        }
        return newChild;
    }

    NodeSP Node::cloneNode(bool deep)
    {
        NodeSP clone;
        switch(getNodeType())
        {
            case ELEMENT_NODE:
                {
                    ElementSP spthis(std::dynamic_pointer_cast<Element, DOMObject>(shared_from_this()));
                    clone = getOwnerDocument()->createElementNS(spthis->getNamespaceURI(), spthis->getTagName());
                }
                break;
            case ATTRIBUTE_NODE:
                clone = NodeSP(new Attr());
                break;
            case TEXT_NODE:
                clone = NodeSP(new Text());
                break;
            case CDATA_SECTION_NODE:
                clone = NodeSP(new Text());
                break;
            case ENTITY_REFERENCE_NODE:
                clone = NodeSP(new EntityReference());
                break;
            case ENTITY_NODE:
                clone = NodeSP(new Entity());
                break;
            case PROCESSING_INSTRUCTION_NODE:
                clone = NodeSP(new ProcessingInstruction());
                break;
            case COMMENT_NODE:
                clone = NodeSP(new Comment());
                break;
            case DOCUMENT_NODE:
                clone = NodeSP(new Document());
                break;
            case DOCUMENT_TYPE_NODE:
                clone = NodeSP(new DocumentType());
                break;
            case DOCUMENT_FRAGMENT_NODE:
                clone = NodeSP(new DocumentFragment());
                break;
            case NOTATION_NODE:
                clone = NodeSP(new Notation());
                break;
            default:
                throw std::runtime_error("Node::cloneNode(): invalid node type");
        }
        if(!clone)
            return NodeSP();
        clone->_data->ownerDocument = _data->ownerDocument;
        clone->_data->parentNode = NodeWP();
        if(deep)
        {
            for(NodeList::iterator it = _data->childNodes.begin(); it != _data->childNodes.end(); ++it)
                clone->_data->childNodes.push_back((*it)->cloneNode(deep));
        }
        return clone;
    }

/*
    unsigned short Node::compareDocumentPosition(NodeSP other)
    {

        // TODO p59

        // this needs to run from the document node

        // this is the reference node
        // other is the other node
        // container precedes contained
        // DOCUMENT_POSITION_CONTAINED_BY = other contained by this
        // DOCUMENT_POSITION_CONTAINS = this contained by other
        // DOCUMENT_POSITION_FOLLOWING = other is after this
        // DOCUMENT_POSITION_PRECEDING = other is before this
        // DOCUMENT_POSITION_DISCONNECT
        // DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC

        throw std::runtime_error("Node::compareDocumentPosition(): not yet implemented");
    }
*/

    DOMObjectSP Node::getFeature(const DOMString &feature, const DOMString &version)
    {
        return getOwnerDocument()->getImplementation()->getFeature(feature, version);
    }

    DOMUserDataSP Node::getUserData(const DOMString &key)
    {
        std::map<DOMString, DOMUserDataSP>::iterator it = _data->userData.find(key);
        return (it == _data->userData.end()) ? DOMUserDataSP() : it->second;
    }

    bool Node::hasAttributes(void)
    {
        return false;
    }

    bool Node::hasChildNodes(void)
    {
        return _data->childNodes.size() > 0;
    }

    NodeSP Node::insertBefore(NodeSP newChild, NodeSP refChild)
    {
        if(!refChild)
            return appendChild(newChild);
        NodeSP parent(_data->parentNode.lock());
        NodeList::iterator loc = std::find(_data->childNodes.begin(), _data->childNodes.end(), refChild);
        if(loc == _data->childNodes.end())
            throw DOMException(NOT_FOUND_ERR);
        if(newChild->getNodeType() == DOCUMENT_FRAGMENT_NODE)
        {
            for(NodeList::iterator it = newChild->_data->childNodes.begin(), end = newChild->_data->childNodes.end(); it != end; ++it)
                insertBefore(*it, refChild);
        }
        else
        {
            _data->childNodes.insert(loc, newChild);
            newChild->setParentNode(std::dynamic_pointer_cast<Node, DOMObject>(shared_from_this()));
        }
        return newChild;
    }

    bool Node::isDefaultNamespace(const DOMString &namespaceURI)
    {
        NodeSP parent(_data->parentNode.lock());
        return (parent) ? parent->isDefaultNamespace(namespaceURI) : false;
    }

    bool Node::isEqualNode(NodeSP arg)
    {
        if(arg->getNodeType() != getNodeType())
            return false;
        if(arg->getNodeName() != getNodeName())
            return false;
        if(arg->getLocalName() != getLocalName())
            return false;
        if(arg->getNamespaceURI() != getNamespaceURI())
            return false;
        if(arg->getPrefix() != getPrefix())
            return false;
        if(arg->getNodeValue() != getNodeValue())
            return false;
        if(arg->getAttributes().size() != getAttributes().size())
            return false;
        NamedNodeMap argattr = arg->getAttributes();
        NamedNodeMap attr = getAttributes();
        for(NamedNodeMap::iterator it = attr.begin(); it != attr.end(); ++it)
        {
            if(argattr.find(it->first) == argattr.end())
                return false;
            if(!it->second->isEqualNode(argattr[it->first]))
                return false;
        }
        if(arg->_data->childNodes.size() != _data->childNodes.size())
            return false;
        for(NodeList::iterator it = _data->childNodes.begin(), argit = arg->_data->childNodes.begin(); it != _data->childNodes.end(); ++it, ++argit)
        {
            if(!(*it)->isEqualNode(*argit))
                return false;
        }
        return true;
    }

    bool Node::isSameNode(NodeSP other)
    {
        return other == shared_from_this();
    }

    bool Node::isSupported(const DOMString &feature, const DOMString &version)
    {
        return getOwnerDocument()->getImplementation()->hasFeature(feature, version);
    }

    DOMString Node::lookupNamespaceURI(const DOMString &prefix)
    {
        NodeSP parent(_data->parentNode.lock());
        return (parent) ? parent->lookupNamespaceURI(prefix) : DOMString();
    }

    DOMString Node::lookupPrefix(const DOMString &namespaceURI)
    {
        NodeSP parent(_data->parentNode.lock());
        return (parent) ? parent->lookupPrefix(namespaceURI) : DOMString();
    }

/*
    void Node::normalize(void)
    {

        // TODO

        // combine Text nodes (no adjacent or empty should remain)
        // do the same for all children recursively

        throw std::runtime_error("Node::normalize(): not yet implemented");
    }
*/

    NodeSP Node::removeChild(NodeSP oldChild)
    {
        NodeList::iterator it = std::find(_data->childNodes.begin(), _data->childNodes.end(), oldChild);
        if(it == _data->childNodes.end())
            throw DOMException(NOT_FOUND_ERR);
        it = _data->childNodes.erase(it);
        return oldChild;
    }

    NodeSP Node::replaceChild(NodeSP newChild, NodeSP oldChild)
    {
        NodeList::iterator loc = std::find(_data->childNodes.begin(), _data->childNodes.end(), oldChild);
        if(loc == _data->childNodes.end())
            throw DOMException(NOT_FOUND_ERR);
        loc = _data->childNodes.erase(loc);
        oldChild->setParentNode(NodeSP());
        if(newChild->getNodeType() == DOCUMENT_FRAGMENT_NODE)
        {
            for(NodeList::iterator it = newChild->_data->childNodes.begin(); it != newChild->_data->childNodes.end(); ++it)
            {
                if(loc == _data->childNodes.end())
                    appendChild(*it);
                else
                    insertBefore(*it, *loc);
            }
        }
        else
        {
            _data->childNodes.insert(loc, newChild);
            newChild->setParentNode(std::dynamic_pointer_cast<Node, DOMObject>(shared_from_this()));
        }
        return oldChild;
    }

    DOMUserDataSP Node::setUserData(const DOMString &key, DOMUserDataSP data, UserDataHandlerSP handler)
    {
        std::map<DOMString, DOMUserDataSP>::iterator it = _data->userData.find(key);
        _data->userDataHandlers.erase(_data->userDataHandlers.find(key));
        DOMUserDataSP result = (it == _data->userData.end()) ? DOMUserDataSP() : it->second;
        _data->userData[key] = data;
        if(handler)
            _data->userDataHandlers[key] = handler;
        return result;
    }

    bool Node::isReadOnly(void)
    {
        NodeSP parent(_data->parentNode.lock());
        return (parent) ? parent->isReadOnly() : false;
    }

    void Node::setParentNode(NodeSP parent)
    {
        _data->parentNode = parent;
    }

    void Node::setOwnerDocument(DocumentSP document)
    {
        _data->ownerDocument = document;
    }

    ElementList Node::getChildElements(void)
    {
        ElementList result;
        for(NodeList::iterator it = _data->childNodes.begin(), end = _data->childNodes.end(); it != end; ++it)
        {
            if((*it)->getNodeType() == ELEMENT_NODE)
                result.push_back(std::dynamic_pointer_cast<Element>(*it));
        }
        return result;
    }

    NodeSP Node::findNode(Node *ptr)
    {
        for(NodeList::iterator it = _data->childNodes.begin(), end = _data->childNodes.end(); it != end; ++it)
        {
            if(it->get() == ptr)
                return *it;
        }
        return NodeSP();
    }

    ccl::uint32_t Node::getNodeIndex(NodeSP node)
    {
        for(NodeList::iterator it = _data->childNodes.begin(), end = _data->childNodes.end(); it != end; ++it)
        {
            if(*it == node)
                return it - _data->childNodes.begin();
        }
        return 0;
    }

    ccl::uint32_t Node::getElementIndex(NodeSP node)
    {
        ElementList elements = getChildElements();
        for(ElementList::iterator it = elements.begin(), end = elements.end(); it != end; ++it)
        {
            if(*it == node)
                return it - elements.begin();
        }
        return 0;
    }

    NodeSP Node::getNodeAt(ccl::uint32_t index)
    {
        return (index >= _data->childNodes.size()) ? NodeSP() : _data->childNodes[index];
    }

    ElementSP Node::getElementAt(ccl::uint32_t index)
    {
        ElementList elements = getChildElements();
        return (index >= elements.size()) ? ElementSP() : elements[index];
    }


}