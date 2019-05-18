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

#include "dom/DocumentFragment.h"
#include "dom/DOMException.h"

namespace dom
{    
    struct DocumentFragment::_DocumentFragment
    {
    };

    DocumentFragment::~DocumentFragment(void)
    {
        delete _data;
    }

    DocumentFragment::DocumentFragment(void) : _data(new _DocumentFragment)
    {
    }

    unsigned short DocumentFragment::getNodeType(void)
    {
        return DOCUMENT_FRAGMENT_NODE;
    }

    DOMString DocumentFragment::getNodeName(void)
    {
        return "#document-fragment";
    }

    bool DocumentFragment::isDefaultNamespace(const DOMString &namespaceURI)
    {
        return false;
    }

    DOMString DocumentFragment::lookupNamespaceURI(const DOMString &prefix)
    {
        return DOMString();
    }

    DOMString DocumentFragment::lookupPrefix(const DOMString &namespaceURI)
    {
        return DOMString();
    }

    NodeSP DocumentFragment::appendChild(NodeSP newChild)
    {
        unsigned short type = newChild->getNodeType();
        if((type == ELEMENT_NODE) || (type == PROCESSING_INSTRUCTION_NODE) || (type == COMMENT_NODE)
            || (type == TEXT_NODE) || (type == CDATA_SECTION_NODE) || (type == ENTITY_REFERENCE_NODE))
            return Node::appendChild(newChild);
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP DocumentFragment::insertBefore(NodeSP newChild, NodeSP refChild)
    {
        unsigned short type = newChild->getNodeType();
        if((type == ELEMENT_NODE) || (type == PROCESSING_INSTRUCTION_NODE) || (type == COMMENT_NODE)
            || (type == TEXT_NODE) || (type == CDATA_SECTION_NODE) || (type == ENTITY_REFERENCE_NODE))
            return Node::insertBefore(newChild, refChild);
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP DocumentFragment::replaceChild(NodeSP newChild, NodeSP oldChild)
    {
        unsigned short type = newChild->getNodeType();
        if((type == ELEMENT_NODE) || (type == PROCESSING_INSTRUCTION_NODE) || (type == COMMENT_NODE)
            || (type == TEXT_NODE) || (type == CDATA_SECTION_NODE) || (type == ENTITY_REFERENCE_NODE))
            return Node::replaceChild(newChild, oldChild);
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

}
