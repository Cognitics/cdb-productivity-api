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
/*! \file dom/DOMImplementation.h
\headerfile dom/DOMImplementation.h
\brief Provides dom::DOMImplementation.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::DOMImplementation DOMImplementation.h dom/DOMImplementation.h
\brief Cognitics DOM Implementation

This class assigns itself as the implementation object for the Core (2.0, 3.0), XML (2.0, 3.0), Traversal (2.0), Range (2.0), LS (3.0), and XMLVersion features.
The setFeature() method may be used to replace these entries if desired.

Cognitics has extended the DOM implementation with the ability to dynamically generate element classes through factory functions.
This functionality is represented as the Factory (1.0, 2.0, 3.0) feature.

\code
DOMImplementationSP DOM(new DOMImplementation());
DocumentSP doc = DOM->createDocument();
\endcode

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p37]
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
    class DOMImplementation : public DOMObject, public boost::noncopyable
    {
    private:
        struct _DOMImplementation;
        _DOMImplementation *_data; 

    public:
        static DocumentSP readXML(const DOMString &filename, DOMImplementationSP domImplementation = DOMImplementationSP());
        static bool writeXML(const DOMString &filename, DocumentSP document, DOMImplementationSP domImplementation = DOMImplementationSP());
        static bool writeXML(const DOMString &filename, ElementSP element, DOMImplementationSP domImplementation = DOMImplementationSP());

        static bool writeXMLToString(std::string &str, DocumentSP document, DOMImplementationSP domImplementation = DOMImplementationSP());
        static bool writeXMLToString(std::string &str, ElementSP element, DOMImplementationSP domImplementation = DOMImplementationSP());
        virtual ~DOMImplementation(void);
        DOMImplementation(void);

        static const unsigned short MODE_SYNCHRONOUS    = 1;
        static const unsigned short MODE_ASYNCHRONOUS    = 2;

/*! \brief Set the specialized implementation object for the specified feature and version.
\param feature The name of the feature to associate.
\param version The version of the feature to associate.
\param featureObject The object to associate with the specified feature and version.
*/
        void setFeature(const DOMString &feature, const DOMString &version, DOMObjectSP featureObject);

/*! \brief Test if the DOM implementation supports a specific feature and version.
\param feature The name of the feature to test.
\param version The version number of the feature to test.
\return True if the feature is implemented in the specified version, false otherwise.
*/
        bool hasFeature(const DOMString &feature, const DOMString &version = DOMString());

/*! \brief Get a specialized API object for the specified feature and version.

This method returns a specialized object which implements the specialized APIs of the specified feature and version.
The specialized object may also be obtained by using binding-specific casting methods but is not necessarily expected to.
This method also allows the implementation to provide specialized objects which do not support the DOMImplementation interface.

\param feature The name of the feature requested. Note that any plus sign "+" prepended to the name of the feature will be ignored since it is not significant in the context of this method.
\param version The version number of the feature to test.
\return Returns an object which implements the specialized APIs of the specified feature and version, if any, or empty if there is no object which implements interfaces associated with that feature.
*/
        DOMObjectSP getFeature(const DOMString &feature, const DOMString &version = DOMString());

/*! \brief Create an empty DocumentType.

Creates an empty DocumentType node.
Entity declarations and notations are not made available.
Entity reference expansions and default attribute additions do not occur.

\param qualifiedName The qualified name of the document type to be created.
\param publicId The external subset public identifier.
\param systemId The external subset system identifier.
\return A new DocumentType node with Node::ownerDocument set to empty.
\exception DOMException::INVALID_CHARACTER_ERR qualifiedName is not a valid XML name.
\exception DOMException::NAMESPACE_ERR qualifiedName is malformed.
*/
        virtual DocumentTypeSP createDocumentType(const DOMString &qualifiedName, const DOMString &publicId = DOMString(), const DOMString &systemId = DOMString());

/*! \brief Create a Document object of the specified type.

Creates a DOM Document object of the specified type with its document element.

\param namespaceURI The namespace URI of the document element to create or empty.
\param qualifiedName The qualified name of the document element to create or empty.
\param doctype Type of document to be created or empty.
\return A new Document object with document element. If the namespaceURI, qualifiedName, and doctype are empty, the returned Document is empty with no document element.
\exception DOMException::WRONG_DOCUMENT_ERR doctype has already been used with a different document or was created from a different implementation.
\exception DOMException::INVALID_CHARACTER_ERR qualifiedName is not a valid XML name.
\exception DOMException::NAMESPACE_ERR qualifiedName is malformed, qualifiedName has a prefix but namespaceURI is empty, qualifiedName is empty but namespaceURI is not empty, or qualifiedName has an "xml" prefix and namespaceURI is not "http://www.w3.org/XML/1998/namespace".
*/
        virtual DocumentSP createDocument(const DOMString &namespaceURI = DOMString(), const DOMString &qualifiedName = DOMString(), DocumentTypeSP doctype = DocumentTypeSP());

        virtual LSParserSP createLSParser(unsigned short mode = MODE_SYNCHRONOUS, const DOMString &schemaType = DOMString());
        virtual LSSerializerSP createLSSerializer(void);
        virtual LSInputSP createLSInput(void);
        // TODO: virtual LSOutputSP createLSOutput(void);

/*! \brief Create a new Element

This is called from Document->createElementNS() to instantiate a new element using an element factory function.
It should not be called directly, but is available on the public interface for classes derived from Element.

\note This is a Cognitics extension.
*/
        virtual ElementSP createElementNS(const DOMString &namespaceURI, const DOMString &qualifiedName);

/*! \brief Set the element factory function for the specified element tag name.
\param tagName The element tag name to match for factory lookups.
\param func The factory function instantiated for createElementNS() calls on the associated tag name.
\note This is a Cognitics extension.
*/
        virtual void setFactory(const DOMString &tagName, FactoryFunc func);

    };

}
