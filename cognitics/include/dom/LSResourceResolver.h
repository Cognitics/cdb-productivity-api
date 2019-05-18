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
/*! \file dom/LSResourceResolver.h
\headerfile dom/LSResourceResolver.h
\brief Provides dom::LSResourceResolver.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::LSResourceResolver LSResourceResolver.h LSResourceResolver.h
\brief LS Resource Resolver

LSResourceResolver provides a way for applications to redirect references to external resources.

Applications needing to implement custom handling for external resources can implement this interface and register their implementation by setting the "resource-resolver" parameter of DOMConfiguration objects attached to LSParser and LSSerializer.
It can also be register on DOMConfiguration objects attached to Document if the "LS" feature is supported.

The LSParser will then allow the application to intercept any external entities, including the external DTD subset and external parameter entities, before including them.
The top-level document entity is never passed to the resolveResource method.

Many DOM applications will not need to implement this interface, but it will be especially useful for applications that build XML documents from databases or other specialized input sources, or for applications that use URNs.

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p24]
*/
    class LSResourceResolver
    {
    public:
        virtual ~LSResourceResolver(void) { }
        LSResourceResolver(void) { }

/*! \brief Allow the application to resolve external resources.

The LSParser will call this method before opening any external resource, including the external DTD subset, external entities referenced within the DTD, and external entities referenced within the document element (however, the top-level document entity is not passed to this method).
The application may then request that the LSParser resolve the external resource itself, that it use an alternative URI, or that it use an entirely different input source.

Application writers can use this method to redirect external system identifiers to secure and/or local URI, to look up public identifiers in a catalogue, or to read an entity from a database or other input source (including, for example, a dialog box).

\param type The type of the resource being resolved. For XML resources (i.e. entities), applications must use the value "http://www.w3.org/TR/REC-xml". For XML Schema, applications must use the value "http://www.w3.org/2001/XMLSchema". Other types of resources are outside the scope of this specification and therefore should recommend an absolute URI in order to use this method.
\param namespaceURI The namespace of the resource being resolved, e.g. the target namespace of the XML Schema when resolving XML Schema resources.
\param publicId The public identifier of the external entity being referenced, or empty if no public identifier was supplied or if the resource is not an entity.
\param systemId The system identifier, a URI reference, of the external resource being referenced, or empty if no system identifier was supplied.
\param baseURI The absolute base URI of the resource being parsed, or empty if there is no base URI.
\return LSInput object describing the new input source, or empty to request that the parser open a regular URI connection to the resource.
*/
        virtual LSInputSP resolveResource(const DOMString &type, const DOMString &namespaceURI, const DOMString &publicId, const DOMString &systemId, const DOMString &baseURI) = 0;

    };

}
