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
/*! \file dom/Attr.h
\headerfile dom/Attr.h
\brief Provides dom::Attr.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::Attr Attr.h Attr.h
\brief Attribute

The Attr interface represents an attribute in an Element object.
Typically the allowable values for the attribute are defined in a schema associated with the document.

Attr objects inherit the Node interface, but since they are not actually child nodes of the element they describe, the DOM does not consider them part of the document tree.
Thus, the Node attributes parentNode, previousSibling, and nextSibling have a null value for Attr objects.
The DOM takes the view that attributes are properties of elements rather than having a separate identity from the elements they are associated with; this should make it more efficient to implement such features as default attributes associated with all elements of a given type.

Furthermore, Attr nodes may not be immediate children of a DocumentFragment.
However, they can be associated with Element nodes contained within a DocumentFragment.
In short, users and implementors of the DOM need to be aware that Attr nodes have some things in common with other objects inheriting the Node interface, but they also are quite distinct.

The attribute's effective value is determined as follows: if this attribute has been explicitly assigned any value, that value is the attribute's effective value; otherwise, if there is a declaration for this attribute, and that declaration includes a default value, then that default value is the attribute's effective value; otherwise, the attribute does not exist on this element in the structure model until it has been explicitly added.
Note that the Node::nodeValue attribute on the Attr instance can also be used to retrieve the string version of the attribute's value(s).

If the attribute was not explicitly given a value in the instance document but has a default value provided by the schema associated with the document, an attribute node will be created with specified set to false.
Removing attribute nodes for which a default value is defined in the schema generates a new attribute node with the default value and specified set to false.
If validation occurred while invoking Document::normalizeDocument(), attribute nodes with specified equals to false are recomputed according to the default attribute values provided by the schema.
If no default value is associate with this attribute in the schema, the attribute node is discarded.

In XML, where the value of an attribute can contain entity references, the child nodes of the Attr node may be either Text or EntityReference nodes (when these are in use; see the description of EntityReference for discussion).

The DOM Core represents all attribute values as simple strings, even if the DTD or schema associated with the document declares them of some specific type such as tokenized.

The way attribute value normalization is performed by the DOM implementation depends on how much the implementation knows about the schema in use.
Typically, the value and nodeValue attributes of an Attr node initially returns the normalized value given by the parser.
It is also the case after Document::normalizeDocument() is called (assuming the right options have been set).
But this may not be the case after mutation, independently of whether the mutation is performed by setting the string value directly or by changing the Attr child nodes.
In particular, this is true when character references are involved, given that they are not represented in the DOM and they impact attribute value normalization.
On the other hand, if the implementation knows about the schema in use when the attribute value is changed, and it is of a different type than CDATA, it may normalize it again at that time.
This is especially true of specialized DOM implementations, such as SVG DOM implementations, which store attribute values in an internal form different from a string.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p81]
*/
    class Attr : public Node
    {
    private:
        struct _Attr;
        _Attr *_data;

    public:
        virtual ~Attr(void);
        Attr(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual DOMString getNodeValue(void);
        virtual DOMString getNamespaceURI(void);
        virtual void setNamespaceURI(const DOMString &namespaceURI);
        virtual DOMString getLocalName(void);
        virtual DOMString getPrefix(void);
        virtual void setPrefix(const DOMString &prefix);
        virtual NodeSP cloneNode(bool deep);

/*! \brief Get whether this attribute is known to be of type ID or not.

Returns whether this attribute is known to be of type ID (i.e. to contain an identifier for its owner element) or not.
When it is and its value is unique, the ownerElement of this attribute can be retrieved using the method Document::getElementById.
The implementation could use several ways to determine if an attribute node is known to
contain an identifier:
\li If validation occurred using an XML Schema while loading the document or while invoking Document::normalizeDocument(), the post-schema-validation infoset contributions (PSVI contributions) values are used to determine if this attribute is a schema-determined ID attribute using the schema-determined ID definition in XPointer.
\li If validation occurred using a DTD while loading the document or while invoking Document::normalizeDocument(), the infoset [type definition] value is used to determine if this attribute is a DTD-determined ID attribute using the DTD-determined ID definition in XPointer.
\li From the use of the methods Element::setIdAttribute(), Element::setIdAttributeNS(), or Element::setIdAttributeNode(), i.e. it is an user-determined ID attribute;
\note XPointer framework consider the DOM user-determined ID attribute as being part of the XPointer externally-determined ID definition.
\li Using mechanisms that are outside the scope of this specification, it is then an externally-determined ID attribute.
This includes using schema languages different from XML schema and DTD.

If validation occurred while invoking Document::normalizeDocument(), all user-determined ID attributes are reset and all attribute nodes ID information are then reevaluated in accordance to the schema used.
As a consequence, if the Attr::schemaTypeInfo attribute contains an ID type, isId will always return true.

\return Whether this attribute is known to be of type ID or not.
\note Introduced in DOM Level 3.
*/
        virtual bool isId(void);
        virtual void setId(bool Id);

/*! \brief Get the name of this attribute.

If Node::localName is different from empty, this attribute is a qualified name.

\return The name of this attribute.
*/
        virtual DOMString getName(void);
        virtual void setName(const DOMString &name);

/*! \brief Get the element node this attribute is attached to.
\return The element node this attribute is attached to, or empty if this attribute is not in use.
\note Introduced in DOM Level 2.
*/
        virtual ElementSP getOwnerElement(void);

/*! \brief Get the type information associated with this attribute.

While the type information contained in this attribute is guarantee to be correct after loading the document or invoking Document::normalizeDocument(), schemaTypeInfo may not be reliable if the node was moved.

\return The type information associated with this attribute.
\note Introduced in DOM Level 3.
*/
        // TODO: virtual TypeInfoSP getSchemaTypeInfo(void);

/*! \brief Get whether this attribute was explicitly given a value in the instance document.

If the application changed the value of this attribute node (even if it ends up having the same value as the default value) then it is set to true.
The implementation may handle attributes with default values from other schemas similarly but applications should use Document::normalizeDocument() to guarantee this information is up-to-date.

\return True if this attribute was explicitly given a value in the instance document, false otherwise.
*/
        virtual bool getSpecified(void);
        virtual void setSpecified(bool specified);

/*! \brief Get the value of the attribute.

Character and general entity references are replaced with their values.

Some specialized implementations, such as some SVG 1.1 implementations, may do normalization automatically, even after mutation; in such case, the value on retrieval may differ from the value on setting.

\return The value of the attribute.
\sa Element::getAttribute()
*/
        Variant getValue(void);

/*! \brief Set the value of the attribute.
\param value The value to set.
\sa Element::setAttribute()
*/
        void setValue(const Variant &value);

    };

}
