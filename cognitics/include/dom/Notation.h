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
/*! \file dom/Notation.h
\headerfile dom/Notation.h
\brief Provides dom::Notation.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::Notation Notation.h Notation.h
\brief Notation

This interface represents a notation declared in the DTD.
A notation either declares, by name, the format of an unparsed entity (see section 4.7 of the XML 1.0 specification), or is used for formal declaration of processing instruction targets (see section 2.6 of the XML 1.0 specification).
The nodeName attribute inherited from Node is set to the declared name of the notation.

A Notation node does not have any parent.

\note DOM Level 3 doesn't support editing Notation nodes; they are therefore read-only.
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5 [p116]
*/
    class Notation : public Node
    {
    private:
        struct _Notation;
        _Notation *_data;

    public:
        virtual ~Notation(void);
        Notation(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual DOMString getTextContent(void);
        virtual void setTextContent(const DOMString &textContent);
        virtual NodeSP cloneNode(bool deep);
        virtual bool isDefaultNamespace(const DOMString &namespaceURI);
        virtual DOMString lookupNamespaceURI(const DOMString &prefix);
        virtual DOMString lookupPrefix(const DOMString &namespaceURI);
        virtual bool isReadOnly(void);

/*! \brief Get the public identifier of this notation.
\return The public identifier of this notation, or empty if not specified.
*/
        virtual DOMString getPublicId(void);
        virtual void setPublicId(const DOMString &publicId);

/*! \brief Get the system identifier of this notation.
\return The system identifier of this notation, or empty if not specified.
*/
        virtual DOMString getSystemId(void);
        virtual void setSystemId(const DOMString &systemId);

    };

}
