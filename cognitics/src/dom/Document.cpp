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

#include "dom/Document.h"
#include "dom/DOMException.h"
#include "dom/Attr.h"
#include "dom/CDATASection.h"
#include "dom/Comment.h"
#include "dom/DocumentType.h"
#include "dom/DocumentFragment.h"
#include "dom/DOMConfiguration.h"
#include "dom/DOMImplementation.h"
#include "dom/Element.h"
#include "dom/EntityReference.h"
#include "dom/ProcessingInstruction.h"
#include "dom/NodeIterator.h"
#include "dom/TreeWalker.h"

namespace dom
{
    struct Document::_Document
    {
        DOMImplementationSP implementation;
        DOMString inputEncoding;
        DOMString xmlEncoding;
        bool xmlStandalone;
        DOMString xmlVersion;
        bool strictErrorChecking;
        DOMString documentURI;
        DOMConfigurationSP domConfig;
    };

    Document::~Document(void)
    {
        delete _data;
    }

    Document::Document(void) : _data(new _Document)
    {
        _data->strictErrorChecking = true;
        _data->xmlStandalone = false;
        _data->xmlVersion = "1.0";
        _data->domConfig = DOMConfigurationSP(new DOMConfiguration);
    }

    unsigned short Document::getNodeType(void)
    {
        return DOCUMENT_NODE;
    }

    DOMString Document::getNodeName(void)
    {
        return "#document";
    }

    DOMString Document::getTextContent(void)
    {
        return DOMString();
    }

    void Document::setTextContent(const DOMString &textContent)
    {
    }

    bool Document::isDefaultNamespace(const DOMString &namespaceURI)
    {
        ElementSP element = getDocumentElement();
        return (element) ? element->isDefaultNamespace(namespaceURI) : false;
    }

    DOMString Document::lookupNamespaceURI(const DOMString &prefix)
    {
        ElementSP element = getDocumentElement();
        return (element) ? element->lookupNamespaceURI(prefix) : DOMString();
    }

    DOMString Document::lookupPrefix(const DOMString &namespaceURI)
    {
        ElementSP element = getDocumentElement();
        return (element) ? element->lookupPrefix(namespaceURI) : DOMString();
    }

    NodeSP Document::cloneNode(bool deep)
    {
        DocumentSP clone = std::dynamic_pointer_cast<Document>(Node::cloneNode(deep));
        clone->_data->implementation = _data->implementation;
        clone->_data->inputEncoding = _data->inputEncoding;
        clone->_data->xmlEncoding = _data->xmlEncoding;
        clone->_data->xmlStandalone = _data->xmlStandalone;
        clone->_data->strictErrorChecking = _data->strictErrorChecking;
        clone->_data->documentURI = _data->documentURI;
        return clone;
    }

    NodeSP Document::appendChild(NodeSP newChild)
    {
        unsigned short type = newChild->getNodeType();
        if((type == ELEMENT_NODE) || (type == DOCUMENT_TYPE_NODE))
        {
            if((type == DOCUMENT_TYPE_NODE) && getDoctype())
                throw DOMException(HIERARCHY_REQUEST_ERR);
            if((type == ELEMENT_NODE) && getDocumentElement())
                throw DOMException(HIERARCHY_REQUEST_ERR);
            return Node::appendChild(newChild);
        }
        if((type == PROCESSING_INSTRUCTION_NODE) || (type == COMMENT_NODE))
            return Node::appendChild(newChild);
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP Document::insertBefore(NodeSP newChild, NodeSP refChild)
    {
        unsigned short type = newChild->getNodeType();
        if((type == ELEMENT_NODE) || (type == DOCUMENT_TYPE_NODE))
        {
            if((type == DOCUMENT_TYPE_NODE) && getDoctype())
                throw DOMException(HIERARCHY_REQUEST_ERR);
            if((type == ELEMENT_NODE) && getDocumentElement())
                throw DOMException(HIERARCHY_REQUEST_ERR);
            return Node::appendChild(newChild);
        }
        if((type == PROCESSING_INSTRUCTION_NODE) || (type == COMMENT_NODE))
            return Node::appendChild(newChild);
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP Document::replaceChild(NodeSP newChild, NodeSP oldChild)
    {
        unsigned short type = newChild->getNodeType();
        if((type == ELEMENT_NODE) || (type == DOCUMENT_TYPE_NODE))
        {
            if((type == DOCUMENT_TYPE_NODE) && (getDoctype() != oldChild))
                throw DOMException(HIERARCHY_REQUEST_ERR);
            if((type == ELEMENT_NODE) && (getDocumentElement() != oldChild))
                throw DOMException(HIERARCHY_REQUEST_ERR);
            return Node::appendChild(newChild);
        }
        if((type == PROCESSING_INSTRUCTION_NODE) || (type == COMMENT_NODE))
            return Node::appendChild(newChild);
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    DocumentTypeSP Document::getDoctype(void)
    {
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
        {
            if((*it)->getNodeType() == DOCUMENT_TYPE_NODE)
                return std::dynamic_pointer_cast<DocumentType>(*it);
        }
        return DocumentTypeSP();
    }

    ElementSP Document::getDocumentElement(void)
    {
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
        {
            if((*it)->getNodeType() == ELEMENT_NODE)
                return std::dynamic_pointer_cast<Element>(*it);
        }
        return ElementSP();
    }

    DOMString Document::getDocumentURI(void)
    {
        return _data->documentURI;
    }

    void Document::setDocumentURI(const DOMString &documentURI)
    {
        _data->documentURI = documentURI;
    }

    DOMConfigurationSP Document::getDomConfig(void)
    {
        return _data->domConfig;
    }

    DOMImplementationSP Document::getImplementation(void)
    {
        return _data->implementation;
    }

    void Document::setImplementation(DOMImplementationSP implementation)
    {
        _data->implementation = implementation;
    }

    DOMString Document::getInputEncoding(void)
    {
        return _data->inputEncoding;
    }

    void Document::setInputEncoding(const DOMString &inputEncoding)
    {
        _data->inputEncoding = inputEncoding;
    }

    bool Document::getStrictErrorChecking(void)
    {
        return _data->strictErrorChecking;
    }

    void Document::setStrictErrorChecking(bool strictErrorChecking)
    {
        _data->strictErrorChecking = strictErrorChecking;
    }

    DOMString Document::getXmlEncoding(void)
    {
        return _data->xmlEncoding;
    }
    
    void Document::setXmlEncoding(const DOMString &xmlEncoding)
    {
        _data->xmlEncoding = xmlEncoding;
    }

    bool Document::getXmlStandalone(void)
    {
        return _data->xmlStandalone;
    }

    void Document::setXmlStandalone(bool xmlStandalone)
    {
        _data->xmlStandalone = xmlStandalone;
    }

    DOMString Document::getXmlVersion(void)
    {
        return _data->xmlVersion;
    }

    void Document::setXmlVersion(const DOMString &xmlVersion)
    {
        if(xmlVersion != "1.0")
            throw DOMException(NOT_SUPPORTED_ERR);
        _data->xmlVersion = xmlVersion;
    }

    ElementSP Document::createElement(const DOMString &tagName)
    {
        return createElementNS(DOMString(), tagName);
    }

    ElementSP Document::createElementNS(const DOMString &namespaceURI, const DOMString &qualifiedName)
    {
        DOMImplementationSP dom(std::dynamic_pointer_cast<DOMImplementation>(getImplementation()));
        ElementSP element = dom->createElementNS(namespaceURI, qualifiedName);
        element->setOwnerDocument(std::dynamic_pointer_cast<Document>(shared_from_this()));
        element->setNamespaceURI(namespaceURI);
        if(element->getTagName().empty())
            element->setTagName(qualifiedName);
        return element;
    }

    DocumentFragmentSP Document::createDocumentFragment(void)
    {
        DocumentFragmentSP fragment(new DocumentFragment());
        fragment->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        return fragment;
    }

    CommentSP Document::createComment(const DOMString &data)
    {
        CommentSP comment(new Comment());
        comment->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        comment->setData(data);
        return comment;
    }

    TextSP Document::createTextNode(const DOMString &data)
    {
        TextSP text(new Text());
        text->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        text->setData(data);
        return text;
    }

    CDATASectionSP Document::createCDATASection(const DOMString &data)
    {
        CDATASectionSP cdata(new CDATASection());
        cdata->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        cdata->setData(data);
        return cdata;
    }

    ProcessingInstructionSP Document::createProcessingInstruction(const DOMString &target, const DOMString &data)
    {
        ProcessingInstructionSP pi(new ProcessingInstruction());
        pi->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        pi->setTarget(target);
        pi->setData(data);
        return pi;
    }

    AttrSP Document::createAttribute(const DOMString &name)
    {
        return createAttributeNS(DOMString(), name);
    }

    AttrSP Document::createAttributeNS(const DOMString &namespaceURI, const DOMString &qualifiedName)
    {
        AttrSP attr(new Attr());
        attr->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        attr->setNamespaceURI(namespaceURI);
        attr->setName(qualifiedName);
        return attr;
    }

    EntityReferenceSP Document::createEntityReference(const DOMString &name)
    {
        EntityReferenceSP entityref(new EntityReference());
        entityref->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        entityref->setNodeName(name);
        return entityref;
    }

    NodeList Document::getElementsByTagName(const DOMString &tagname)
    {
        NodeList result;
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
        {
            if((*it)->getNodeType() == ELEMENT_NODE)
            {
                if((tagname == "*") || (tagname == (*it)->getNodeName()))
                    result.push_back(*it);
            }
        }
        return result;
    }

    NodeSP Document::importNode(NodeSP importedNode, bool deep)
    {
        // TODO: validate per specification
        NodeSP clone(importedNode->cloneNode(deep));
        return adoptNode(clone);
    }

    NodeList Document::getElementsByTagNameNS(const DOMString &namespaceURI, const DOMString &localName)
    {
        NodeList result;
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(); it != childNodes.end(); ++it)
        {
            if((*it)->getNodeType() == ELEMENT_NODE)
            {
                bool match = true;
                if((namespaceURI != "*") && (namespaceURI != (*it)->getNamespaceURI()))
                    match = false;
                if((localName != "*") && (localName != (*it)->getLocalName()))
                    match = false;
                if(match)
                    result.push_back(*it);
            }
        }
        return result;
    }

/*
    ElementSP Document::getElementById(const DOMString &elementId)
    {
        // TODO
        throw std::runtime_error("Document::getElementById(): not yet implemented");
    }
*/

    NodeSP Document::adoptNode(NodeSP source)
    {
        // TODO: validate per specification
        source->setOwnerDocument(std::dynamic_pointer_cast<Document, DOMObject>(shared_from_this()));
        NodeList childNodes = getChildNodes();
        for(NodeList::iterator it = childNodes.begin(); it == childNodes.end(); ++it)
            adoptNode(*it);
        if(source->getNodeType() == ELEMENT_NODE)
        {
            ElementSP element(std::dynamic_pointer_cast<Element>(source));
            NamedNodeMap attrNodes = element->getAttributes();
            for(NamedNodeMap::iterator it = attrNodes.begin(); it != attrNodes.end(); ++it)
                adoptNode(it->second);
        }
        return source;
    }

/*
    void Document::normalizeDocument(void)
    {
        throw std::runtime_error("Document::normalizeDocument(): not yet implemented");
    }
*/

/*
    NodeSP Document::renameNode(NodeSP n, const DOMString &namespaceURI, const DOMString &qualifiedName)
    {
        throw std::runtime_error("Document::renameNode(): not yet implemented");
    }
*/

    NodeIteratorSP Document::createNodeIterator(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion)
    {
        return NodeIteratorSP(new NodeIterator(root, whatToShow, filter, entityReferenceExpansion));
    }

    TreeWalkerSP Document::createTreeWalker(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion)
    {
        return TreeWalkerSP(new TreeWalker(root, whatToShow, filter, entityReferenceExpansion));
    }

/*
    RangeSP Document::createRange(void)
    {
        throw std::runtime_error("Document::createRange(): not yet implemented");
    }
*/

}

