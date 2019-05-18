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
/*! \file dom/EntityReference.h
\headerfile dom/EntityReference.h
\brief Provides dom::EntityReference.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::EntityReference EntityReference.h EntityReference.h
\brief Entity Reference

EntityReference nodes may be used to represent an entity reference in the tree.
Note that character references and references to predefined entities are considered to be expanded by the HTML or XML processor so that characters are represented by their Unicode equivalent rather than by an entity reference.
Moreover, the XML processor may completely expand references to entities while building the Document, instead of providing EntityReference nodes.
If it does provide such nodes, then for an EntityReference node that represents a reference to a known entity an Entity exists, and the subtree of the EntityReference node is a copy of the Entity node subtree.
However, the latter may not be true when an entity contains an unbound namespace prefix.
In such a case, because the namespace prefix resolution depends on where the entity reference is, the descendants of the EntityReference node may be bound to different namespace URIs.
When an EntityReference node represents a reference to an unknown entity, the node has no children and its replacement value, when used by Attr::value for example, is empty.

As for Entity nodes, EntityReference nodes and all their descendants are readonly.

\note EntityReference nodes may cause element content and attribute value normalization problems when, such as in XML 1.0 and XML Schema, the normalization is performed after entity reference are expanded.
\note This class is instantiated via Document::createEntityReference().
\sa Document, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5 [p118]
*/
    class EntityReference : public Node
    {
    private:
        struct _EntityReference;
        _EntityReference *_data;

    public:
        virtual ~EntityReference(void);
        EntityReference(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual void setNodeName(const DOMString &nodeName);
        virtual NodeSP cloneNode(bool deep);
        virtual bool isReadOnly(void);

    };

}
