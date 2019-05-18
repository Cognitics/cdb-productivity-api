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
/*! \file dom/DocumentFragment.h
\headerfile dom/DocumentFragment.h
\brief Provides dom::DocumentFragment.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::DocumentFragment DocumentFragment.h dom/DocumentFragment.h
\brief Document Fragment

DocumentFragment is a "lightweight" or "minimal" Document object.
It is very common to want to be able to extract a portion of a document's tree or to create a new fragment of a document.
Imagine implementing a user command like cut or rearranging a document by moving fragments around.
It is desirable to have an object which can hold such fragments and it is quite natural to use a Node for this purpose.
While it is true that a Document object could fulfill this role, a Document object can potentially be a heavyweight object, depending on the underlying implementation.
What is really needed for this is a very lightweight object.
DocumentFragment is such an object.

Furthermore, various operations -- such as inserting nodes as children of another Node -- may take DocumentFragment objects as arguments; this results in all the child nodes of the DocumentFragment being moved to the child list of this node.

The children of a DocumentFragment node are zero or more nodes representing the tops of any sub-trees defining the structure of the document.
DocumentFragment nodes do not need to be well-formed XML documents (although they do need to follow the rules imposed upon well-formed XML parsed entities, which can have multiple top nodes).
For example, a DocumentFragment might have only one child and that child node could be a Text node.
Such a structure model represents neither an HTML document nor a well-formed XML document.

When a DocumentFragment is inserted into a Document (or indeed any other Node that may take children) the children of the DocumentFragment and not the DocumentFragment itself are inserted into the Node.
This makes the DocumentFragment very useful when the user wishes to create nodes that are siblings; the DocumentFragment acts as the parent of these nodes so that the user can use the standard methods from the Node interface, such as Node::insertBefore and Node::appendChild.

\note This class is instantiated via Document::createDocumentFragment().
\sa Document, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p37]
*/
    class DocumentFragment : public Node
    {
    private:
        struct _DocumentFragment;
        _DocumentFragment *_data;

    public:
        virtual ~DocumentFragment(void);
        DocumentFragment(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual bool isDefaultNamespace(const DOMString &namespaceURI);
        virtual DOMString lookupNamespaceURI(const DOMString &prefix);
        virtual DOMString lookupPrefix(const DOMString &namespaceURI);
        virtual NodeSP appendChild(NodeSP newChild);
        virtual NodeSP insertBefore(NodeSP newChild, NodeSP refChild = NodeSP());
        virtual NodeSP replaceChild(NodeSP newChild, NodeSP oldChild);

    };

}
