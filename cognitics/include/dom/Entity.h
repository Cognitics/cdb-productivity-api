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
/*! \file dom/Entity.h
\headerfile dom/Entity.h
\brief Provides dom::Entity.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::Entity Entity.h Entity.h
\brief Entity

This interface represents a known entity, either parsed or unparsed, in an XML document.
Note that this models the entity itself not the entity declaration.

The nodeName attribute that is inherited from Node contains the name of the entity.

An XML processor may choose to completely expand entities before the structure model is passed to the DOM; in this case there will be no EntityReference nodes in the document tree.

XML does not mandate that a non-validating XML processor read and process entity declarations made in the external subset or declared in parameter entities.
This means that parsed entities declared in the external subset need not be expanded by some classes of applications, and that the replacement text of the entity may not be available.
When the replacement text is available, the corresponding Entity node's child list represents the structure of that replacement value.
Otherwise, the child list is empty.

DOM Level 3 does not support editing Entity nodes; if a user wants to make changes to the contents of an Entity, every related EntityReference node has to be replaced in the structure model by a clone of the Entity's contents, and then the desired changes must be made to each of those clones instead.
Entity nodes and all their descendants are readonly.

An Entity node does not have any parent.

\note If the entity contains an unbound namespace prefix, the namespaceURI of the corresponding node in the Entity node subtree is null.
The same is true for EntityReference nodes that refer to this entity, when they are created using the createEntityReference() method of the Document interface.

\sa DOMImplementation, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5 [p116]
*/
    class Entity : public Node
    {
    private:
        struct _Entity;
        _Entity *_data;

    public:
        virtual ~Entity(void);
        Entity(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual bool isDefaultNamespace(const DOMString &namespaceURI);
        virtual DOMString lookupNamespaceURI(const DOMString &prefix);
        virtual DOMString lookupPrefix(const DOMString &namespaceURI);
        virtual NodeSP cloneNode(bool deep);
        virtual bool isReadOnly(void);

/*! \brief Get the public identifier associated with the entity.
\return The public identifier associated with the entity if specified, empty otherwise.
*/
        virtual DOMString getPublicId(void);
        virtual void setPublicId(const DOMString &publicId);

/*! \brief Get the system identifier associated with the entity.
\return The system identifier associated with the entity if specified, null otherwise. This may be an absolute URI or not.
*/
        virtual DOMString getSystemId(void);
        virtual void setSystemId(const DOMString &systemId);

/*! \brief Get an attribute specifying the encoding used for this entity at the time of parsing.
\return An attribute specifying the encoding used for this entity at the time of parsing, when it is an external parsed entity. This is empty if it is an entity from the internal subset or if it is not known.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getInputEncoding(void);
        virtual void setInputEncoding(const DOMString &inputEncoding);

/*! \brief Get an attribute specifying, as part of the text declaration, the encoding of this entity.
\return An attribute specifying, as part of the declaration, the encoding of this entity. This is empty when it is not an external parsed entity.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getXmlEncoding(void);
        virtual void setXmlEncoding(const DOMString &xmlEncoding);

/*! \brief Get an attribute specifying, as part of the text declaration, the version number of this entity.
\return An attribute specifying, as part of the declaration, the version number of this entity. This is empty when it is not an external parsed entity.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getXmlVersion(void);
        virtual void setXmlVersion(const DOMString &xmlVersion);

/*! \brief Get the name of the notation for the entity.
\return The name of the notation for the entity, or empty if this is a parsed entity.
*/
        virtual DOMString getNotationName(void);
        virtual void setNotationName(const DOMString &notationName);

    };

}
