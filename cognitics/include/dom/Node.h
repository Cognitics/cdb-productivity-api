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
/* \brief Provides dom::Node.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \brief Node

The Node interface is the primary datatype for the entire Document Object Model.
It represents a single node in the document tree.
While all objects implementing the Node interface expose methods for dealing with children, not all objects implementing the Node interface may have children.
For example, Text nodes may not have children, and adding children to such nodes results in a DOMException being raised.

The attributes nodeName, nodeValue and attributes are included as a mechanism to get at node information without casting down to the specific derived interface.
In cases where there is no obvious mapping of these attributes for a specific nodeType (e.g. nodeValue for an Element or attributes for a Comment), this returns null.
Note that the specialized interfaces may contain additional and more convenient mechanisms to get and set the relevant information.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p56]
*/
    class Node : public DOMObject, public boost::noncopyable
    {
    private:
        struct _Node;
        _Node *_data;

    public:
        virtual ~Node(void);
        Node(void);

        static const unsigned short ELEMENT_NODE                    = 1;
        static const unsigned short ATTRIBUTE_NODE                    = 2;
        static const unsigned short TEXT_NODE                        = 3;
        static const unsigned short CDATA_SECTION_NODE                = 4;
        static const unsigned short ENTITY_REFERENCE_NODE            = 5;
        static const unsigned short ENTITY_NODE                        = 6;
        static const unsigned short PROCESSING_INSTRUCTION_NODE        = 7;
        static const unsigned short COMMENT_NODE                    = 8;
        static const unsigned short DOCUMENT_NODE                    = 9;
        static const unsigned short DOCUMENT_TYPE_NODE                = 10;
        static const unsigned short DOCUMENT_FRAGMENT_NODE            = 11;
        static const unsigned short NOTATION_NODE                    = 12;

        static const unsigned short DOCUMENT_POSITION_DISCONNECTED                = 0x01;
        static const unsigned short DOCUMENT_POSITION_PRECEDING                    = 0x02;
        static const unsigned short DOCUMENT_POSITION_FOLLOWING                    = 0x04;
        static const unsigned short DOCUMENT_POSITION_CONTAINS                    = 0x08;
        static const unsigned short DOCUMENT_POSITION_CONTAINED_BY                = 0x10;
        static const unsigned short DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC    = 0x20;

/*! \brief Get the attributes of this node.

The default implementation returns an empty NamedNodeMap.

\return A NamedNodeMap containing the attributes of this node (if it is an Element) or empty otherwise.
*/
        virtual NamedNodeMap getAttributes(void);

/*! \brief Get the absolute base URI of this node.
\return The absolute base URI of this node or empty if the implementation wasn�t able to obtain an absolute URI.
*/
        virtual DOMString getBaseURI(void);

/*! \brief Get the children of this node.
\return A NodeList that contains all children of this node. If there are no children, this is a NodeList containing no nodes.
*/
        virtual NodeList getChildNodes(void);

/*! \brief Get the first child of this node.
\return The first child of this node. If there is no such node, this returns empty.
*/
        virtual NodeSP getFirstChild(void);

/*! \brief Get the last child of this node.
\return The last child of this node. If there is no such node, this returns empty.
*/
        virtual NodeSP getLastChild(void);

/*! \brief Get the local part of the qualified name of this node.

The default implementation returns an empty string.

\return The local part of the qualified name of this node.
\note Introduced in DOM Level 2.
*/
        virtual DOMString getLocalName(void);

/*! \brief Get the namespace URI of this node.

This is not a computed value that is the result of a namespace lookup based on an examination of the namespace declarations in scope.
It is merely the namespace URI given at creation time.

\note Per the Namespaces in XML Specification XML Namespaces, an attribute does not inherit its namespace from the element it is attached to.
If an attribute is not explicitly given a namespace, it simply has no namespace.

The default implementation returns an empty string.

\return The namespace URI of this node, or empty if it is unspecified.
\note Introduced in DOM Level 2.
*/
        virtual DOMString getNamespaceURI(void);

/*! \brief Get the node immediately following this node.
\return The node immediately following this node. If there is no such node, this returns empty.
*/
        virtual NodeSP getNextSibling(void);

/*! \brief Get the name of this node.
\return The name of this node, depending on its type.
*/
        virtual DOMString getNodeName(void);

/*! \brief Get the type of this node.

The following types are defined:
\li ELEMENT_NODE
\li ATTRIBUTE_NODE
\li TEXT_NODE
\li CDATA_SECTION_NODE
\li ENTITY_REFERENCE_NODE
\li ENTITY_NODE
\li PROCESSING_INSTRUCTION_NODE
\li COMMENT_NODE
\li DOCUMENT_NODE
\li DOCUMENT_TYPE_NODE
\li DOCUMENT_FRAGMENT_NODE
\li NOTATION_NODE

\return A code representing the type of the underlying object.
*/
        virtual unsigned short getNodeType(void);

/*! \brief Get the value of this node.

\return The value of this node, depending on its type.
*/
        virtual DOMString getNodeValue(void);

/*! \brief Set the value of this node.

This has no effect in the default implementation.
*/
        virtual void setNodeValue(const Variant &nodeValue);

/*! \brief Get the Document object associated with this node.

This is also the Document object used to create new nodes.
When this node is a Document or a DocumentType which is not used with any Document yet, this is empty.

\return The Document object associated with this node.
\note Modified in DOM Level 2.
*/
        virtual DocumentSP getOwnerDocument(void);
        virtual void setOwnerDocument(DocumentSP document);

/*! \brief Get the parent of this node.

All nodes, except Attr, Document, DocumentFragment, Entity, and Notation may have a parent.
However, if a node has just been created and not yet added to the tree, or if it has been removed from the tree, this is empty.

\return The parent of this node.
*/
        virtual NodeSP getParentNode(void);
        virtual void setParentNode(NodeSP parent);

/*! \brief Get the namespace prefix of this node.

Note that setting this attribute, when permitted, changes the nodeName attribute, which holds the qualified name, as well as the tagName and name attributes of the Element and Attr interfaces, when applicable.

Setting the prefix to empty makes it unspecified.

Note also that changing the prefix of an attribute that is known to have a default value, does not make a new attribute with the default value and the original prefix appear, since the namespaceURI and localName do not change.

The default implementation returns an empty string.

\return The namespace prefix of this node, or empty if it is unspecified.
\note Introduced in DOM Level 2.
*/
        virtual DOMString getPrefix(void);

/*! \brief Set the namespace prefix of this node.

The default implementation does nothing.

\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the Document::xmlVersion attribute.
\exception NAMESPACE_ERR Raised if the qualifiedName is a malformed qualified name, if the qualifiedName has a prefix and the namespaceURI is empty, if the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace", if the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from "http://www.w3.org/2000/xmlns/", or if the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the qualifiedName nor its prefix is "xmlns".
\note Introduced in DOM Level 2.
\sa getPrefix()
*/
        virtual void setPrefix(const DOMString &prefix);

/*! \brief Get the node immediately preceding this node.
\return The node immediately preceding this node. If there is no such node, this returns empty.
*/
        virtual NodeSP getPreviousSibling(void);

/*! \brief Get the text content of this node and its descendants.

No serialization is performed, the returned string does not contain any markup.
No whitespace normalization is performed and the returned string does not contain the white spaces in element content (see Text::isElementContentWhitespace).

\return The text content of this node and its descendants.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getTextContent(void);

/*! \brief Set the text content of this node.

Any possible children this node may have are removed and, if it the new string is not empty, replaced by a single Text node containing the string.
No parsing is performed; the input string is taken as pure textual content.

\note Introduced in DOM Level 3.
\sa getTextContent()
*/
        virtual void setTextContent(const DOMString &textContent);
    
/*! \brief Adds the node newChild to the end of the list of children of this node.

If the newChild is already in the tree, it is first removed.

\param newChild The node to add. If it is a DocumentFragment object, the entire contents of the document fragment are moved into the child list of this node.
\return The node added.
\exception DOMException::HIERARCHY_REQUEST_ERR this node is of a type that does not allow children of the type of the newChild node, or if the node to append is one of this node's ancestors or this node itself, or if this node is of type Document and the DOM application attempts to append a second DocumentType or Element node.
\exception DOMException::WRONG_DOCUMENT_ERR newChild was created from a different document than the one that created this node.
\exception DOMException::NO_MODIFICATION_ALLOWED_ERR this node is readonly or the previous parent of the node being inserted is readonly.
*/
        virtual NodeSP appendChild(NodeSP newChild);

/*! \brief Create a duplicate of this node.

This returns a duplicate of this node, i.e. serves as a generic copy constructor for nodes.
The duplicate node has no parent (parentNode is null) and no user data.
User data associated to the imported node is not carried over.
However, if any UserDataHandlers have been specified along with the associated data these handlers will be called with the appropriate parameters before this method returns.

Cloning an Element copies all attributes and their values, including those generated by the XML processor to represent defaulted attributes, but this method does not copy any children it contains unless it is a deep clone.
This includes text contained in an the Element since the text is contained in a child Text node.

Cloning an Attr directly, as opposed to be cloned as part of an Element cloning operation, returns a specified attribute (specified is true).
Cloning an Attr always clones its children, since they represent its value, no matter whether this is a deep clone or not.

Cloning an EntityReference automatically constructs its subtree if a corresponding Entity is available, no matter whether this is a deep clone or not.

Cloning any other type of node simply returns a copy of this node.

\param deep If true, recursively clone the subtree under the specified node; if false, clone only the node itself (and its attributes, if it is an Element).
\return The duplicate node.
*/
        virtual NodeSP cloneNode(bool deep);

/*! \brief Compare the given node with regard to document position and order.

Compares the reference node, i.e. the node on which this method is being called, with a node, i.e. the one passed as a parameter, with regard to their position in the document and according to the document order.

The following positions are defined:
\li DOCUMENT_POSITION_DISCONNECTED
\li DOCUMENT_POSITION_PRECEDING
\li DOCUMENT_POSITION_FOLLOWING
\li DOCUMENT_POSITION_CONTAINS
\li DOCUMENT_POSITION_CONTAINED_BY
\li DOCUMENT_POSITION_IMPLEMENTATION_SPECIFIC

Note that these are flags and multiple positions may be returned.

param other The node to compare against the reference node.
return How the node is positioned relative to the reference node.
\note Introduced in DOM Level 3.
*/
        // TODO: virtual unsigned short compareDocumentPosition(NodeSP other);

/*! \brief Get a specialized object which implements the specialized APIs of the specified feature and version.

The specialized object may also be obtained by using binding-specific casting methods but is not necessarily expected to, as discussed in Mixed DOM Implementations.
This method also allow the implementation to provide specialized objects which do not support the Node interface.

\param feature The feature requested.
\param version The version requested (default "").
\return An object which implements the specialized APIs of the specified feature and version, if any, or empty if there is no object which implements interfaces associated with that feature.
\sa DOMImplementation
\note Introduced in DOM Level 3.
*/
        virtual DOMObjectSP getFeature(const DOMString &feature, const DOMString &version = DOMString());

/*! \brief Get the object associated to a key on this node.

The object must first have been set to this node by calling setUserData() with the same key.

\param key The key the object is associated to.
\return The DOMUserData associated to the given key on this node, or empty if there was none.
\sa setUserData()
\note Introduced in DOM Level 3.
*/
        virtual DOMUserDataSP getUserData(const DOMString &key);

/*! \brief Returns whether this node (if it is an element) has any attributes.

The default implementation returns false.

\return True if this node has attributes, false otherwise.
\note Introduced in DOM Level 2.
*/
        virtual bool hasAttributes(void);

/*! \brief Returns whether this node has any children.
\return True if this node has any children, false otherwise.
*/
        virtual bool hasChildNodes(void);

/*! \brief Insert the node newChild before the existing child node refChild.

If refChild is null, this will insert newChild at the end of the list of children.

If newChild is a DocumentFragment object, all of its children are inserted, in the same order, before refChild.
If the newChild is already in the tree, it is first removed.

\param newChild The node to insert.
\param refChild The reference node, i.e. the node before which the new node must be inserted.
\return The node being inserted.
\exception DOMException::HIERARCHY_REQUEST_ERR this node is of a type that does not allow children of the type of the newChild node, or if the node to append is one of this node�s ancestors or this node itself, or if this node is of type Document and the DOM application attempts to append a second DocumentType or Element node.
\exception DOMException::WRONG_DOCUMENT_ERR newChild was created from a different document than the one that created this node.
\exception DOMException::NOT_FOUND_ERR refChild is not a child of this node.
\note Introduced in DOM Level 3.
*/
        virtual NodeSP insertBefore(NodeSP newChild, NodeSP refChild = NodeSP());

/*! \brief Check if the specified namespace URI is the default namespace.
\param namespaceURI The namespace URI to look for.
\return True if the specified namespace URI is the default namespace, false otherwise.
\note Introduced in DOM Level 3.
*/
        virtual bool isDefaultNamespace(const DOMString &namespaceURI);

/*! \brief Test whether two nodes are equal.

This method tests for equality of nodes, not sameness (i.e. whether the two nodes are references to the same object) which can be tested with Node::isSameNode().
All nodes that are the same will also be equal, though the reverse may not be true.

Two nodes are equal if and only if the following conditions are satisfied:
\li The two nodes are of the same type.
\li The following string attributes are equal: nodeName, localName, namespaceURI, prefix, nodeValue.
This is: they are both null, or they have the same length and are character for character identical.
\li The attributes NamedNodeMaps are equal.
This is: they are both null, or they have the same length and for each node that exists in one map there is a node that exists in the other map and is equal, although not necessarily at the same index.
\li The childNodes NodeLists are equal.
This is: they are both null, or they have the same length and contain equal nodes at the same index.
Note that normalization can affect equality; to avoid this, nodes should be normalized before being compared.

For two DocumentType nodes to be equal, the following conditions must also be satisfied:
\li The following string attributes are equal: publicId, systemId, internalSubset.
\li The entities NamedNodeMaps are equal.
\li The notations NamedNodeMaps are equal.

On the other hand, the following do not affect equality: the ownerDocument, baseURI, and parentNode attributes, the specified attribute for Attr nodes, the schemaTypeInfo attribute for Attr and Element nodes, the Text::isElementContentWhitespace attribute for Text nodes, as well as any user data or event listeners registered on the nodes.

\note As a general rule, anything not mentioned in the description above is not significant in consideration of equality checking.
Note that future versions of this specification may take into account more attributes and implementations conform to this specification are expected to be updated accordingly.

\warning In this DOM implementation, attributes are represented as boost::any variants. This prohibits validation of attribute value equality.

\param arg The node to compare equality with.
\return True if the nodes are equal, false otherwise.
\note Introduced in DOM Level 3.
*/
        virtual bool isEqualNode(NodeSP arg);

/*! \brief Test whether this node is the same node as the given one.

This method provides a way to determine whether two Node references returned by the implementation reference the same object.
When two Node references are references to the same object, even if through a proxy, the references may be used completely interchangeably, such that all attributes have the same values and calling the same DOM method on either reference always has exactly the same effect.

\param other The node to tests against.
\return True if the nodes are the same, false otherwise.
\note Introduced in DOM Level 3.
*/
        virtual bool isSameNode(NodeSP other);

/*! \brief Test whether the DOM implementation implements a specific feature and that feature is supported by this node.
\param feature The name of the feature to test.
\param version The version number of the feature to test.
\return True if the specified feature is supported on this node, false otherwise.
\note Introduced in DOM Level 2.
*/
        virtual bool isSupported(const DOMString &feature, const DOMString &version = DOMString());

/*! \brief Look up the namespace URI associated to the given prefix, starting from this node.
\param prefix The prefix to look for. If this parameter is empty, the method will return the default namespace URI if any.
\return Associated namespace URI if found, empty otherwise.
*/
        virtual DOMString lookupNamespaceURI(const DOMString &prefix);

/*! \brief Look up the prefix associated to the given namespace URI, starting from this node.

The default namespace declarations are ignored by this method.

\param namespaceURI The namespace URI to look for.
\return An associated namespace prefix if found or empty if none is found.
*/
        virtual DOMString lookupPrefix(const DOMString &namespaceURI);

/*! \brief Normalize the node.

Puts all Text nodes in the full depth of the sub-tree underneath this Node, including attribute nodes, into a "normal" form where only structure (e.g. elements, comments, processing instructions, CDATA sections, and entity references) separates Text nodes, i.e. there are neither adjacent Text nodes nor empty Text nodes.
This can be used to ensure that the DOM view of a document is the same as if it were saved and re-loaded, and is useful when operations (such as XPointer lookups) that depend on a particular document tree structure are to be used.
If the parameter "normalize-characters" of the DOMConfiguration object attached to the ownerDocument is true, this method will also fully normalize the characters of the Text nodes.

\note In cases where the document contains CDATASections, the normalize operation alone may not be sufficient, since XPointers do not differentiate between Text nodes and CDATASection nodes.
\note Modified in DOM Level 3.
*/
        // TODO: virtual void normalize(void);

/*! \brief Remove the child node indicated by oldChild from the list of children.

\param oldChild The node being removed.
\return The node removed.
\exception NOT_FOUND_ERR oldChild is not a child of this node.
\note Modified in DOM Level 3.
*/
        virtual NodeSP removeChild(NodeSP oldChild);

/*! \brief Replace the child node oldChild with newChild in the list of children.

If newChild is a DocumentFragment object, oldChild is replaced by all of the DocumentFragment children, which are inserted in the same order.
If the newChild is already in the tree, it is first removed.

\param newChild The new node to put in the child list.
\param oldChild The node being replaced in the list.
\return The node replaced.
\exception DOMException::HIERARCHY_REQUEST_ERR this node is of a type that does not allow children of the type of the newChild node, or if the node to append is one of this node's ancestors or this node itself, or if this node is of type Document and the DOM application attempts to append a second DocumentType or Element on the Document node.
\exception DOMException::WRONG_DOCUMENT_ERR newChild was created from a different document than the one that created this node.
\exception DOMException::NOT_FOUND_ERR oldChild is not a child of this node.
\note Modified in DOM Level 3.
*/
        virtual NodeSP replaceChild(NodeSP newChild, NodeSP oldChild);

/*! \brief Associate an object to a key on this node.

The object can later be retrieved from this node by calling getUserData with the same key.

\param key The key to associate the object to.
\param data The object to associate to the given key, or empty to remove any existing association to that key.
\param handler The handler to associate to that key, or empty.
\return The DOMUserData previously associated to the given key on this node, or empty if there was none.
\note Introduced in DOM Level 3.
*/
        virtual DOMUserDataSP setUserData(const DOMString &key, DOMUserDataSP data, UserDataHandlerSP handler = UserDataHandlerSP());
    
/*! \brief Returns the read-only status of the node.

A node is read-only if it or any of its ancestors are read-only.

\return The read-only status of the node.
\note Introduced in this DOM implementation.
*/
        virtual bool isReadOnly(void);

        virtual ElementList getChildElements(void);
        virtual NodeSP findNode(Node *ptr);
        virtual ccl::uint32_t getNodeIndex(NodeSP node);
        virtual ccl::uint32_t getElementIndex(NodeSP node);
        virtual NodeSP getNodeAt(ccl::uint32_t index);
        virtual ElementSP getElementAt(ccl::uint32_t index);

    };

}
