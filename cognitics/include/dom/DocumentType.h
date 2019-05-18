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
/*! \file dom/DocumentType.h
\headerfile dom/DocumentType.h
\brief Provides dom::DocumentType.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::DocumentType DocumentType.h DocumentType.h
\brief Document Type

Each Document has a doctype attribute whose value is either empty or a DocumentType object.
The DocumentType interface in the DOM Core provides an interface to the list of entities that are defined for the document, and little else because the effect of namespaces and the various XML schema efforts on DTD representation are not clearly understood as of this writing.

\note DOM Level 3 doesn't support editing DocumentType nodes. DocumentType nodes are read-only.
\note This class is instantiated via DOMImplementation::createDocumentType().
\sa DOMImplementation, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5 [p115]
*/
    class DocumentType : public Node
    {
    private:
        struct _DocumentType;
        _DocumentType *_data;

    public:
        virtual ~DocumentType(void);
        DocumentType(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual DOMString getTextContent(void);
        virtual void setTextContent(const DOMString &textContent);
        virtual NodeSP cloneNode(bool deep);
        virtual bool isEqualNode(NodeSP arg);
        virtual bool isDefaultNamespace(const DOMString &namespaceURI);
        virtual DOMString lookupNamespaceURI(const DOMString &prefix);
        virtual DOMString lookupPrefix(const DOMString &namespaceURI);
        virtual bool isReadOnly(void);

/*! \brief Get the DTD name.
\return The name of DTD; i.e. the name immediately following the DOCTYPE keyword.
*/
        virtual DOMString getName(void);
        virtual void setName(const DOMString &name);

/*! \brief Get the DTD entities.

Parameter entities are not contained.
Duplicates are discarded.
Every node in this map also implements the Entity interface.

\return A NamedNodeMap containing the general entities, both external and internal, declared in the DTD.
\note The DOM Level 2 does not support editing entities, therefore entities cannot be altered in any way.
*/
        virtual NamedNodeMap getEntities(void);

/*! \brief Get the DTD notations.

Duplicates are discarded.
Every node in this map also implements the Notation interface.

\return A NamedNodeMap containing the notations declared in the DTD.
\note The DOM Level 2 does not support editing notations, therefore notations cannot be altered in any way.
*/
        virtual NamedNodeMap getNotations(void);

/*! \brief Get the public identifier of the internal subset.
\return The public identifier of the external subset.
\note Introduced in DOM Level 2
*/
        virtual DOMString getPublicId(void);
        virtual void setPublicId(const DOMString &publicId);

/*! \brief Get the system identifier of the external subset.

This may be an absolute URI or not.

\return The system identifier of the external subset.
\note Introduced in DOM Level 2
*/
        virtual DOMString getSystemId(void);
        virtual void setSystemId(const DOMString &systemId);

/*! \brief Get the internal subset.

This does not contain the delimiting square brackets.

\return The internal subset as a string, or empty if there is none.
\note Introduced in DOM Level 2
*/
        virtual DOMString getInternalSubset(void);
        virtual void setInternalSubset(const DOMString &internalSubset);

    };

}
