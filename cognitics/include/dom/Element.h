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
/*! \file dom/Element.h
\headerfile dom/Element.h
\brief Provides dom::Element.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::Element Element.h Element.h
\brief Element

The Element interface represents an element in an HTML or XML document.
Elements may have attributes associated with them; since the Element interface inherits from Node, the generic Node interface attribute attributes may be used to retrieve the set of all attributes for an element.
There are methods on the Element interface to retrieve either an Attr object by name or an attribute value by name.
In XML, where an attribute value may contain entity references, an Attr object should be retrieved to examine the possibly fairly complex sub-tree representing the attribute value.
On the other hand, in HTML, where all attributes have simple string values, methods to directly access an attribute value can safely be used as a convenience.

\note In DOM Level 2, the method normalize is inherited from the Node interface where it was moved.
\note This class is instantiated via Document::createElement().
\sa Document, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p85]
*/
    class Element : public Node
    {
    private:
        struct _Element;
        _Element *_data;

    public:
        virtual ~Element(void);
        Element(void);

        virtual bool isSupported(const DOMString &feature, const DOMString &version = DOMString());
        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual NamedNodeMap getAttributes(void);
        virtual bool hasAttributes(void);
        virtual DOMString getNamespaceURI(void);
        virtual void setNamespaceURI(const DOMString &namespaceURI);
        virtual DOMString getLocalName(void);
        virtual DOMString getPrefix(void);
        virtual void setPrefix(const DOMString &prefix);
        virtual NodeSP cloneNode(bool deep);
        // TODO: virtual bool isDefaultNamespace(const DOMString &namespaceURI);
        // TODO: virtual DOMString lookupPrefix(const DOMString &namespaceURI);
        // TODO: virtual DOMString lookupNamespaceURI(const DOMString &prefix);

/*! \brief Get the type information associated with this element.
\return The type information associated with this element.
\note Introduced in DOM Level 3.
*/
        // TODO: virtual TypeInfoSP getSchemaTypeInfo(void);

/*! \brief Get the name of the element.

If Node.localName is different from empty, this attribute is a qualified name.
For example, in:
\code
<elementExample id="demo">
...
</elementExample>
\endcode
tagName has the value "elementExample".
Note that this is case-preserving in XML, as are all of the operations of the DOM.
The HTML DOM returns the tagName of an HTML element in the canonical uppercase form, regardless of the case in the source HTML document.

\return The name of the element.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getTagName(void);
        virtual void setTagName(const DOMString &tagName);

/*! \brief Retrieve an attribute value by name.
\param name The name of the attribute to retrieve.
\return The Attr value as a string, or the empty string if that attribute does not have a specified or default value.
*/
        virtual Variant getAttribute(const DOMString &name);

/*! \brief Retrieve an attribute value by local name and namespace URI

Per XML Namespaces, applications must use the value null as the namespaceURI parameter for methods if they wish to have no namespace.

\param namespaceURI The namespace URI of the attribute to retrieve.
\param localName The local name URI of the attribute to retrieve.
\return The Attr value
\note Introduced in DOM Level 2.
*/
        virtual Variant getAttributeNS(const DOMString &namespaceURI, const DOMString &localName);

/*! \brief Retrieve an attribute node by name.
\param name The name (nodeName) of the attribute to retrieve.
\return The Attr node with the specified name (nodeName) or empty if there is no such attribute.
\note Introduced in DOM Level 2.
*/
        virtual AttrSP getAttributeNode(const DOMString &name);

/*! \brief Retrieve an attribute node by local name and namespace URI.
\param namespaceURI The namespace URI of the attribute to retrieve.
\param localName The local name URI of the attribute to retrieve.
\return The Attr node with the specified attribute local name and namespace URI or empty if there is no such attribute.
\note Introduced in DOM Level 2.
*/
        virtual AttrSP getAttributeNodeNS(const DOMString &namespaceURI, const DOMString &localName);

/*! \brief Get a NodeList of all descendant Elements with a given tag name, in document order.
\param name The name of the tag to match on. The special value "*" matches all tags.
\return A list of matching Element nodes.
*/
        virtual NodeList getElementsByTagName(const DOMString &name);

/*! \brief Get a NodeList of all descendant Elements with a given local name and namespace URI in document order.
\param namespaceURI The namespace URI of the elements to match on. The special value "*" matches all namespaces.
\param localName The local name of the elements to match on. The special value "*" matches all local names.
\return A new NodeList object containing all the matched Elements.
\note Introduced in DOM Level 2.
*/
        virtual NodeList getElementsByTagNameNS(const DOMString &namespaceURI, const DOMString &localName);

/*! \brief Check if an attribute with a given name is specified on this element or has a default value.
\param name The name of the attribute to look for.
\return True if an attribute with the given name is specified on this element or has a default value, false otherwise.
\note Introduced in DOM Level 2.
*/
        virtual bool hasAttribute(const DOMString &name);

/*! \brief Check if an attribute with a given local name and namespace URI is specified on this element or has a default value.
\param namespaceURI The namespace URI of the attribute to look for.
\param localName The local name of the attribute to look for.
\return True if an attribute with the given local name and namespace URI is specified on this element or has a default value, false otherwise.
\note Introduced in DOM Level 2.
*/
        virtual bool hasAttributeNS(const DOMString &namespaceURI, const DOMString &localName);

/*! \brief Remove an attribute by name.

If a default value for the removed attribute is defined in the DTD, a new attribute immediately appears with the default value as well as the corresponding namespace URI, local name, and prefix when applicable.
The implementation may handle default values from other schemas similarly but applications should use Document::normalizeDocument() to guarantee this information is up-to-date.

If no attribute with this name is found, this method has no effect.

\param name The name of the attribute to remove.
\note Introduced in DOM Level 2.
*/
        virtual void removeAttribute(const DOMString &name);

/*! \brief Remove an attribute by local name and namespace URI.

If a default value for the removed attribute is defined in the DTD, a new attribute immediately appears with the default value as well as the corresponding namespace URI, local name, and prefix when applicable.
The implementation may handle default values from other schemas similarly but applications should use Document::normalizeDocument() to guarantee this information is up-to-date.

If no attribute with this local name and namespace URI is found, this method has no effect.

\param namespaceURI The namespace URI of the attribute to remove.
\param localName The local name of the attribute to remove.
\note Introduced in DOM Level 2.
*/
        virtual void removeAttributeNS(const DOMString &namespaceURI, const DOMString &localName);

/*! \brief Remove the specified attribute node.

If a default value for the removed attribute is defined in the DTD, a new attribute immediately appears with the default value as well as the corresponding namespace URI, local name, and prefix when applicable.

\param oldAttr The Attr node to remove from the attribute list.
\return The Attr node that was removed.
*/
        virtual AttrSP removeAttributeNode(AttrSP oldAttr);

/*! \brief Add a new attribute.

If an attribute with that name is already present in the element, its value is changed to be that of the value parameter.
This value is a simple string; it is not parsed as it is being set.
So any markup (such as syntax to be recognized as an entity reference) is treated as literal text, and needs to be appropriately escaped by the implementation when it is written out.
In order to assign an attribute value that contains entity references, the user must create an Attr node plus any Text and EntityReference nodes, build the appropriate subtree, and use setAttributeNode() to assign it as the value of an attribute.

\param name The name of the attribute to create or alter.
\param value Value to set in string form.
*/
        virtual void setAttribute(const DOMString &name, const Variant &value = Variant());

/*! \brief Add a new attribute.

If an attribute with the same local name and namespace URI is already present in the element, its value is changed to be that of the value parameter.
This value is a simple string; it is not parsed as it is being set.
So any markup (such as syntax to be recognized as an entity reference) is treated as literal text, and needs to be appropriately escaped by the implementation when it is written out.
In order to assign an attribute value that contains entity references, the user must create an Attr node plus any Text and EntityReference nodes, build the appropriate subtree, and use setAttributeNodeNS() or setAttributeNode() to assign it as the value of an attribute.

\param namespaceURI The namespaceURI of the attribute to create or alter.
\param qualifiedName The qualified name of the attribute to create or alter.
\param value The value to set in string form.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\exception NAMESPACE_ERR Raised if the qualifiedName is a malformed qualified name, if the qualifiedName has a prefix and the namespaceURI is empty, if the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace", if the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from "http://www.w3.org/2000/xmlns/", or if the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the qualifiedName nor its prefix is "xmlns".
\note Introduced in DOM Level 2.
*/
        virtual void setAttributeNS(const DOMString &namespaceURI, const DOMString &qualifiedName, const Variant &value);

/*! \brief Add a new attribute node.

If an attribute with that name (nodeName) is already present in the element, it is replaced by the new one.
Replacing an attribute node by itself has no effect.

\param newAttr The Attr node to add to the attribute list.
\return If the newAttr attribute replaces an existing attribute, the replaced Attr node is returned, otherwise empty is returned.
\exception INUSE_ATTRIBUTE_ERR newAttr is already an attribute of another Element object. The DOM user must explicitly clone Attr nodes to re-use them in other elements.
*/
        virtual AttrSP setAttributeNode(AttrSP newAttr);

/*! \brief Add a new attribute node.

If an attribute with that local name and that namespace URI is already present in the element, it is replaced by the new one.
Replacing an attribute node by itself has no effect.

\param newAttr The Attr node to add to the attribute list.
\return If the newAttr attribute replaces an existing attribute with the same local name and namespace URI, the replaced Attr node is returned, otherwise empty is returned.
\exception INUSE_ATTRIBUTE_ERR newAttr is already an attribute of another Element object. The DOM user must explicitly clone Attr nodes to re-use them in other elements.
\note Introduced in DOM Level 2.
\note Modified in CGL: removed WRONG_DOCUMENT_ERR exception.
*/
        virtual AttrSP setAttributeNodeNS(AttrSP newAttr);

/*! \brief Declare (or undeclare) the specified attribute to be an ID attribute.

If the parameter isId is true, this method declares the specified attribute to be a user-determined ID attribute.
This affects the value of Attr::isId and the behavior of Document::getElementById, but does not change any schema that may be in use, in particular this does not affect the Attr::schemaTypeInfo of the specified Attr node.
Use the value false for the parameter isId to undeclare an attribute for being a user-determined ID attribute.

\param name The name of the attribute.
\param isId Whether the attribute is of type ID.
\note Introduced in DOM Level 3.
*/
        virtual void setIdAttribute(const DOMString &name, bool isId);

/*! \brief Declare (or undeclare) the specified attribute to be an ID attribute.

If the parameter isId is true, this method declares the specified attribute to be a user-determined ID attribute.
This affects the value of Attr::isId and the behavior of Document::getElementById, but does not change any schema that may be in use, in particular this does not affect the Attr::schemaTypeInfo of the specified Attr node.
Use the value false for the parameter isId to undeclare an attribute for being a user-determined ID attribute.

\param namespaceURI The namespace URI of the attribute.
\param localName The local name of the attribute.
\param isId Whether the attribute is of type ID.
\note Introduced in DOM Level 3.
*/
        virtual void setIdAttributeNS(const DOMString &namespaceURI, const DOMString &localName, bool isId);

/*! \brief Declare (or undeclare) the specified attribute to be an ID attribute.

If the parameter isId is true, this method declares the specified attribute to be a user-determined ID attribute.
This affects the value of Attr::isId and the behavior of Document::getElementById, but does not change any schema that may be in use, in particular this does not affect the Attr::schemaTypeInfo of the specified Attr node.
Use the value false for the parameter isId to undeclare an attribute for being a user-determined ID attribute.

\param idAttr The attribute node.
\param isId Whether the attribute is of type ID.
\note Introduced in DOM Level 3.
\exception NOT_FOUND_ERR The specified node is not an attribute of this element.
*/
        virtual void setIdAttributeNode(AttrSP idAttr, bool isId);

    };

}
