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
/*! \brief Provides dom::Document.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 1.2, 2.13
*/
#pragma once

#include "dom/Node.h"
#include "dom/NodeFilter.h"

namespace dom
{
/*! \brief Document

The Document interface represents the entire HTML or XML document.
Conceptually, it is the root of the document tree, and provides the primary access to the document's data.
Since elements, text nodes, comments, processing instructions, etc. cannot exist outside the context of a Document, the Document interface also contains the factory methods needed to create these objects.
The Node objects created have a ownerDocument attribute which associates them with the Document within whose context they were created.

\note This class is instantiated via DOMImplementation::createDocument().
\sa DOMImplementation
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p41]
\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 1.2 [p27], 2.13 [p53]
*/
    class Document : public Node
    {
    private:
        struct _Document;
        _Document *_data;

    public:
        virtual ~Document(void);
        Document(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual DOMString getTextContent(void);
        virtual void setTextContent(const DOMString &textContent);
        virtual bool isDefaultNamespace(const DOMString &namespaceURI);
        virtual DOMString lookupNamespaceURI(const DOMString &prefix);
        virtual DOMString lookupPrefix(const DOMString &namespaceURI);
        virtual NodeSP cloneNode(bool deep);
        virtual NodeSP appendChild(NodeSP newChild);
        virtual NodeSP insertBefore(NodeSP newChild, NodeSP refChild = NodeSP());
        virtual NodeSP replaceChild(NodeSP newChild, NodeSP oldChild);


/*! \brief Get the Document Type Declaration associated with this document.

For XML documents without a document type declaration this returns empty.
This provides direct access to the DocumentType node, child node of this Document.
This node can be set at document creation time and later changed through the use of child node manipulation methods, such as Node::insertBefore(), or Node::replaceChild().

\return The Document Type Declaration associated with this document.
\sa DocumentType
\note Modified in DOM Level 3.
*/
        virtual DocumentTypeSP getDoctype(void);

/*! \brief Get the Element associated with this document.

This is a convenience attribute that allows direct access to the child node that is the document element of the document.

\return The Element associated with this document.
\sa Element
*/
        virtual ElementSP getDocumentElement(void);

/*! \brief Get the documentURI.

The location of the document or empty if undefined or if the Document was created using DOMImplementation::createDocument().
No lexical checking is performed when setting this attribute; this could result in a empty value returned when using Node::baseURI.

\return The documentURI.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getDocumentURI(void);

/*! \brief Set the documentURI.

No lexical checking is performed when setting this attribute; this could result in an empty value returned when using Node.getBaseURI().

\note Introduced in DOM Level 3.
*/
        virtual void setDocumentURI(const DOMString &documentURI);

/*! \brief Get the DOMConfiguration.
\return The configuration used when normalizeDocument() is invoked.
\note Introduced in DOM Level 3.
*/
        virtual DOMConfigurationSP getDomConfig(void);

/*! \brief Get the DOMImplementation object that handles this document.
\return The DOMImplementation object that handles this document.
*/
        virtual DOMImplementationSP getImplementation(void);
        virtual void setImplementation(DOMImplementationSP implementation);

/*! \brief Get the document input encoding.

An attribute specifying the encoding used for this document at the time of the parsing.
This is empty when it is not known, such as when the Document was created in memory.

\return The document input encoding.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getInputEncoding(void);
        virtual void setInputEncoding(const DOMString &inputEncoding);

/*! \brief Get the error checking enforcement attribute.

An attribute specifying whether error checking is enforced or not.
When set to false, the implementation is free to not test every possible error case normally defined on DOM operations, and not raise any DOMException on DOM operations or report errors while using normalizeDocument().
In case of error, the behavior is undefined.
This attribute is true by default.

\return The error checking enforcement attribute.
\note Introduced in DOM Level 3.
*/
        virtual bool getStrictErrorChecking(void);

/*! \brief Set the error checking enforcement attribute.

\sa getStrictErrorChecking()
\note Introduced in DOM Level 3.
*/
        virtual void setStrictErrorChecking(bool strictErrorChecking);

/*! \brief Get the XML encoding of the document.

An attribute specifying, as part of the XML declaration, the encoding of this document.
This is empty when unspecified or when it is not known, such as when the Document was created in memory.

\return The XML encoding of the document.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getXmlEncoding(void);
        virtual void setXmlEncoding(const DOMString &xmlEncoding);

/*! \brief Get the XML standalone attribute of the document.

An attribute specifying, as part of the XML declaration, whether this document is standalone.
This is false when unspecified.

\return The XML standalone attribute of the document.
\note Introduced in DOM Level 3.
*/
        virtual bool getXmlStandalone(void);

/*! \brief Set the XML standalone attribute of the document.
\sa getXmlStandalone()
\note Introduced in DOM Level 3.
*/
        virtual void setXmlStandalone(bool xmlStandalone);

/*! \brief Get the XML version of the document.

An attribute specifying, as part of the XML declaration, the version number of this document.
If there is no declaration and if this document supports the "XML" feature, the value is "1.0".
If this document does not support the "XML" feature, the value is always empty.
Changing this attribute will affect methods that check for invalid characters in XML names.
Applications should invoke normalizeDocument() in order to check for invalid characters in the Nodes that are already part of this Document.

\return The XML version of the document.
\note Introduced in DOM Level 3.
*/
        virtual DOMString getXmlVersion(void);

/*! \brief Set the XML version attribute of the document.

This will throw a NOT_SUPPORTED_ERR exception for any value except "1.0".

\exception NOT_SUPPORTED_ERR version set to a value that is not supported by this implementation.
\sa getXmlVersion()
\note Introduced in DOM Level 3.
*/
        virtual void setXmlVersion(const DOMString &xmlVersion);

/*! \brief Adopt node from another document (move).

Attempts to adopt a node from another document to this document.
If supported, it changes the ownerDocument of the source node, its children, as well as the attached attribute nodes if there are any.
If the source node has a parent it is first removed from the child list of its parent.
This effectively allows moving a subtree from one document to another (unlike importNode() which create a copy of the source node instead of moving it).
When it fails, applications should use importNode() instead.
Note that if the adopted node is already part of this document (i.e. the source and target document are the same), this method still has the effect of removing the source node from
the child list of its parent, if any.

The following list describes the specifics for each type of node:
\li ATTRIBUTE_NODE: The ownerElement attribute is set to empty and the specified flag is set to true on the adopted Attr. The descendants of the source Attr are recursively adopted.
\li DOCUMENT_FRAGMENT_NODE: The descendants of the source node are recursively adopted.
\li DOCUMENT_NODE: Document nodes cannot be adopted.
\li DOCUMENT_TYPE_NODE: DocumentType nodes cannot be adopted.
\li ELEMENT_NODE: Specified attribute nodes of the source element are adopted. Default attributes are discarded, though if the document being adopted into defines default attributes for this element name, those are assigned. The descendants of the source element are recursively adopted.
\li ENTITY_NODE: Entity nodes cannot be adopted.
\li ENTITY_REFERENCE_NODE: Only the EntityReference node itself is adopted, the descendants are discarded, since the source and destination documents might have defined the entity differently. If the document being imported into provides a definition for this entity name, its value is assigned.
\li NOTATION_NODE: Notation nodes cannot be adopted.
\li PROCESSING_INSTRUCTION_NODE: Can be adopted. No specifics.
\li TEXT_NODE: Can be adopted. No specifics.
\li CDATA_SECTION_NODE: Can be adopted. No specifics.
\li COMMENT_NODE: Can be adopted. No specifics.

\note Since it does not create new nodes unlike the importNode() method, this method does not raise an INVALID_CHARACTER_ERR exception, and applications should use the normalizeDocument() method to check if an imported name is not an XML name according to the XML version in use.

\param source The node to move into this document.
\return The adopted node, or empty if this operation fails, such as when the source node comes from a different implementation.
\exception NOT_SUPPORTED_ERR the source node is of type DOCUMENT, DOCUMENT_TYPE.
\note Introduced in DOM Level 3.
*/
        virtual NodeSP adoptNode(NodeSP source);

/*! \brief Create an Attr node for the given name.

Note that the Attr instance can then be set on an Element using the setAttributeNode() method.
To create an attribute with a qualified name and namespace URI, use the createAttributeNS() method.

\param name The name of the attribute.
\return A new Attr object with nodeName set to name, and localName, prefix, and namespaceURI set to empty. The value of the attribute is the empty string.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\sa Attr, createAttributeNS()
*/
        virtual AttrSP createAttribute(const DOMString &name);

/*! \brief Create an Attr node for the given qualified name and namespace URI.

Per XML Namespaces, applications must use the value empty as the namespaceURI parameter for methods if they wish to have no namespace.

\param namespaceURI The namespace URI of the attribute to create.
\param qualifiedName The qualified name of the attribute to instantiate.
\return A new Attr object.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\exception NAMESPACE_ERR Raised if the qualifiedName is a malformed qualified name, if the qualifiedName has a prefix and the namespaceURI is empty, if the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace", if the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from "http://www.w3.org/2000/xmlns/", or if the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the qualifiedName nor its prefix is "xmlns".
\sa Attr
\note Introduced in DOM Level 2.
*/
        virtual AttrSP createAttributeNS(const DOMString &namespaceURI, const DOMString &qualifiedName);

/*! \brief Create a CDATASection node whose value is the specified string.

\param data The data for the CDATASection contents.
\return The new CDATASection object.
\sa CDATASection
*/
        virtual CDATASectionSP createCDATASection(const DOMString &data);

/*! \brief Create a Comment node given the specified string.

\param data The data for the node.
\return The new Comment object.
\sa Comment
*/
        virtual CommentSP createComment(const DOMString &data);

/*! \brief Create an empty DocumentFragment object.

\return A new DocumentFragment object.
\sa DocumentFragment
*/
        virtual DocumentFragmentSP createDocumentFragment(void);

/*! \brief Create an element of the type specified.

Note that the instance returned implements the Element interface, so attributes can be specified directly on the returned object.
In addition, if there are known attributes with default values, Attr nodes representing them are automatically created and attached to the element.
To create an element with a qualified name and namespace URI, use the createElementNS() method.

CGL adds the attempt to create an element object dynamically using the CGL DOM implementation element factory.
If no factory returns a valid element, the default Element class is instantiated.

\param tagName The name of the element type to instantiate.
\return A new Element object with the nodeName attribute set to tagName, and localName, prefix, and namespaceURI set to empty.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\sa Element 
*/
        virtual ElementSP createElement(const DOMString &tagName);

/*! \brief Create an element for the given qualified name and namespace URI.

Per XML Namespaces, applications must use the value empty as the namespaceURI parameter for methods if they wish to have no namespace.

CGL adds the attempt to create an element object dynamically using the CGL DOM implementation element factory.
If no factory returns a valid element, the default Element class is instantiated.

\param namespaceURI The namespace URI of the element to create.
\param qualifiedName The qualified name of the element type to instantiate.
\return A new element object.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\exception NAMESPACE_ERR Raised if the qualifiedName is a malformed qualified name, if the qualifiedName has a prefix and the namespaceURI is empty, if the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace", if the qualifiedName or its prefix is "xmlns" and the namespaceURI is different from "http://www.w3.org/2000/xmlns/", or if the namespaceURI is "http://www.w3.org/2000/xmlns/" and neither the qualifiedName nor its prefix is "xmlns".
\sa Element
\note Introduced in DOM Level 2.
*/
        virtual ElementSP createElementNS(const DOMString &namespaceURI, const DOMString &qualifiedName);

/*! \brief Create an EntityReference object.

In addition, if the referenced entity is known, the child list of the EntityReference node is made the same as that of the corresponding Entity node.

\note If any descendant of the Entity node has an unbound namespace prefix, the corresponding descendant of the created EntityReference node is also unbound; (its namespaceURI is empty).
The DOM Level 2 and 3 do not support any mechanism to resolve namespace prefixes in this case.

\param name The name of the entity to reference.
\return A new EntityReference object.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\sa EntityReference
*/
        virtual EntityReferenceSP createEntityReference(const DOMString &name);

/*! \brief Create a ProcessingInstruction node given the specified name and data strings.

\param target The target part of the processing instruction.
\param data The data for the node.
\return The new ProcessingInstruction object.
\exception INVALID_CHARACTER_ERR the specified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\sa ProcessingInstruction
*/
        virtual ProcessingInstructionSP createProcessingInstruction(const DOMString &target, const DOMString &data);

/*! \brief Create a Text node given the specified string.

\param data The data for the node.
\return The new Text object.
\sa Text
*/
        virtual TextSP createTextNode(const DOMString &data);

/*! \brief Get the Element that has an ID attribute with the given value.

If no such element exists, this returns empty.
If more than one element has an ID attribute with that value, what is returned is undefined.

\note Attributes with the name "ID" or "id" are not of type ID unless so defined.

param elementId The unique id value for an element.
return The matching element or empty if there is none.
\sa Element, Attr::isId()
\note Introduced in DOM Level 2.
*/
        // TODO: virtual ElementSP getElementById(const DOMString &elementId);

/*! \brief Get a NodeList of all the Elements in document order with a given tag name and are contained in the document.

\param tagname The name of the tag to match on. The special value "*" matches all tags.
\return A new NodeList object containing all the matched Elements.
\sa Element, NodeList
*/
        virtual NodeList getElementsByTagName(const DOMString &tagname);

/*! \brief Get a NodeList of all the ELements with a given local name and namespace URI in document order.

\param namespaceURI The namespace URI of the elements to match on. The special value "*" matches all namespaces.
\param localName The local name of the elements to match on. The special value "*" matches all local names.
\return A new NodeList object containing all the matched Elements.
\sa Element, NodeList
\note Introduced in DOM Level 2.
*/
        virtual NodeList getElementsByTagNameNS(const DOMString &namespaceURI, const DOMString &localName);

/*! \brief Import node from another document (copy).

Imports a node from another document to this document, without altering or removing the source node from the original document; this method creates a new copy of the source node.
The returned node has no parent; (parentNode is empty).

For all nodes, importing a node creates a node object owned by the importing document, with attribute values identical to the source node�s nodeName and nodeType, plus the attributes related to namespaces (prefix, localName, and namespaceURI).
As in the cloneNode operation, the source node is not altered.
User data associated to the imported node is not carried over.
However, if any UserDataHandlers have been specified along with the associated data these handlers will be called with the appropriate parameters before this method returns.

Additional information is copied as appropriate to the nodeType, attempting to mirror the behavior expected if a fragment of XML or HTML source was copied from one document to another, recognizing that the two documents may have different DTDs in the XML case.

The following list describes the specifics for each type of node:
\li ATTRIBUTE_NODE: The ownerElement attribute is set to empty and the specified flag is set to true on the generated Attr. The descendants of the source Attr are recursively imported and the resulting nodes reassembled to form the corresponding subtree. Note that the deep parameter has no effect on Attr nodes; they always carry their children with them when imported.
\li DOCUMENT_FRAGMENT_NODE: If the deep option was set to true, the descendants of the source DocumentFragment are recursively imported and the resulting nodes reassembled under the imported DocumentFragment to form the corresponding subtree. Otherwise, this simply generates an empty DocumentFragment.
\li DOCUMENT_NODE: Document nodes cannot be imported.
\li DOCUMENT_TYPE_NODE: DocumentType nodes cannot be imported.
\li ELEMENT_NODE: Specified attribute nodes of the source element are imported, and the generated Attr nodes are attached to the generated Element. Default attributes are not copied, though if the document being imported into defines default attributes for this element name, those are assigned. If the importNode deep parameter was set to true, the descendants of the source element are recursively imported and the resulting nodes reassembled to form the corresponding subtree.
\li ENTITY_NODE: Entity nodes can be imported, however in the current release of the DOM the DocumentType is readonly. Ability to add these imported nodes to a DocumentType will be considered for addition to a future release of the DOM. On import, the publicId, systemId, and notationName attributes are copied. If a deep import is requested, the descendants of the the source Entity are recursively imported and the resulting nodes reassembled to form the corresponding subtree.
\li ENTITY_REFERENCE_NODE: Only the EntityReference itself is copied, even if a deep import is requested, since the source and destination documents might have defined the entity differently. If the document being imported into provides a definition for this entity name, its value is assigned.
\li NOTATION_NODE: Notation nodes can be imported, however in the current release of the DOM the DocumentType is readonly. Ability to add these imported nodes to a DocumentType will be considered for addition to a future release of the DOM. On import, the publicId and systemId attributes are copied. Note that the deep parameter has no effect on this type of nodes since they cannot have any children.
\li PROCESSING_INSTRUCTION_NODE: The imported node copies its target and data values from those of the source node. Note that the deep parameter has no effect on this type of nodes since they cannot have any children.
\li TEXT_NODE, CDATA_SECTION_NODE, COMMENT_NODE: These three types of nodes inheriting from CharacterData copy their data and length attributes from those of the source node. Note that the deep parameter has no effect on these types of nodes since they cannot have any children.

\param importedNode The node to import.
\param deep If true, recursively import the subtree under the specified node; if false, import only the node itself, as explained above. This has no effect on nodes that cannot have any children, and on Attr, and EntityReference nodes.
\return The imported node that belongs to this Document.
\exception NOT_SUPPORTED_ERR the type of node being imported is not supported.
\exception INVALID_CHARACTER_ERR one of the imported names is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\note Introduced in DOM Level 2.
*/
        virtual NodeSP importNode(NodeSP importedNode, bool deep);

/*! \brief Normalize the document (as if going through a save and load cycle).

This method acts as if the document was going through a save and load cycle, putting the document in a "normal" form.
As a consequence, this method updates the replacement tree of EntityReference nodes and normalizes Text nodes, as defined in the method Node::normalize().

Otherwise, the actual result depends on the features being set on the domConfig object and governing what operations actually take place.
Noticeably this method could also make the document namespace well-formed according to the algorithm described in Namespace Normalization, check the character normalization, remove the CDATASection nodes, etc. See DOMConfiguration for details.

Mutation events, when supported, are generated to reflect the changes occurring on the document.

If errors occur during the invocation of this method, such as an attempt to update a read-only node or a Node::nodeName contains an invalid character according to the XML version in use, errors or warnings (DOMError::SEVERITY_ERROR or DOMError::SEVERITY_WARNING) will be reported using the DOMErrorHandler object associated with the "error-handler" parameter.
Note this method might also report fatal errors (DOMError.SEVERITY_FATAL_ERROR) if an implementation cannot recover from an error.

\note Introduced in DOM Level 3.
*/
        // TODO: virtual void normalizeDocument(void);

/*! \brief Rename an existing node of type ELEMENT_NODE or ATTRIBUTE_NODE.

When possible this simply changes the name of the given node, otherwise this creates a new node with the specified name and replaces the existing node with the new node as described below.

If simply changing the name of the given node is not possible, the following operations are performed:
\li a new node is created
\li any registered event listener is registered on the new node
\li any user data attached to the old node is removed from that node
\li the old node is removed from its parent if it has one
\li the children are moved to the new node
\li if the renamed node is an Element its attributes are moved to the new node
\li the new node is inserted at the position the old node used to have in its parent�s child nodes list if it has one
\li the user data that was attached to the old node is attached to the new node.

When the node being renamed is an Element only the specified attributes are moved, default attributes originated from the DTD are updated according to the new element name.
In addition, the implementation may update default attributes from other schemas.
Applications should use normalizeDocument() to guarantee these attributes are up-to-date.
When the node being renamed is an Attr that is attached to an Element, the node is first removed from the Element attributes map.
Then, once renamed, either by modifying the existing node or creating a new one as described above, it is put back.

param n The node to rename.
param namespaceURI The new namespace URI.
param qualifiedName The new qualified name.
return The renamed node. This is either the specified node or the new node that was created to replace the specified node.
\exception NOT_SUPPORTED_ERR the type of the specified node is neither ELEMENT_NODE nor ATTRIBUTE_NODE, or the implementation does not support the renaming of the document element.
\exception INVALID_CHARACTER_ERR the new qualified name is not an XML name according to the XML version in use specified in the xmlVersion attribute.
\exception NAMESPACE_ERR the qualifiedName is a malformed qualified name, the qualifiedName has a prefix and the namespaceURI is empty, or the qualifiedName has a prefix that is "xml" and the namespaceURI is different from "http://www.w3.org/XML/1998/namespace" [XML Namespaces]. Also raised, when the node being renamed is an attribute, if the qualifiedName, or its prefix, is "xmlns" and the namespaceURI is different from "http://www.w3.org/2000/xmlns/".
*/
        // TODO: virtual NodeSP renameNode(NodeSP n, const DOMString &namespaceURI, const DOMString &qualifiedName);

/*! \brief Create a new NodeIterator over the subtree rooted at the specified node.
\param root The node which will be iterated together with its children. The iterator is initially positioned just before this node. The whatToShow flags and the filter, if any, are not considered when setting this position. The root must not be empty.
\param whatToShow This flag specifies which node types may appear in the logical view of the tree presented by the iterator. See the description of NodeFilter for the set of possible SHOW_ values.
\param filter The NodeFilter to be used with this TreeWalker, or empty to indicate no filter.
\param entityReferenceExpansion The value of this flag determines whether entity reference nodes are expanded.
\note This is part of DOM2-Traversal.
*/
        virtual NodeIteratorSP createNodeIterator(NodeSP root, unsigned long whatToShow = NodeFilter::SHOW_ALL, NodeFilterSP filter = NodeFilterSP(), bool entityReferenceExpansion = false);

/*! \brief Create a new TreeWalker over the subtree rooted at the specified node.
\param root The node which will serve as the root for the TreeWalker. The whatToShow flags and the NodeFilter are not considered when setting this value; any node type will be accepted as the root. The currentNode of the TreeWalker is initialized to this node, whether or not it is visible. The root functions as a stopping point for traversal methods that look upward in the document structure, such as parentNode and nextNode. The root must not be null.
\param whatToShow This flag specifies which node types may appear in the logical view of the tree presented by the tree-walker. See the description of NodeFilter for the set of possible SHOW_ values.
\param filter The NodeFilter to be used with this TreeWalker, or empty to indicate no filter.
\param entityReferenceExpansion If this flag is false, the contents of EntityReference nodes are not presented in the logical view.
\note This is part of DOM2-Traversal.
*/
        virtual TreeWalkerSP createTreeWalker(NodeSP root, unsigned long whatToShow = NodeFilter::SHOW_ALL, NodeFilterSP filter = NodeFilterSP(), bool entityReferenceExpansion = false);

/*! \brief Create a new Node Range.

The initial state of the Range returned from this method is such that both of its boundary-points are positioned at the beginning of the corresponding Document, before any content.
The Range returned can only be used to select content associated with this Document, or with DocumentFragments and Attrs for which this Document is the ownerDocument.

\note This is part of DOM2-Range.
*/
        // TODO: virtual RangeSP createRange(void);

    };

}
