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
/*! \file dom/DOMLocator.h
\headerfile dom/DOMLocator.h
\brief Provides dom::DOMLocator.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::DOMLocator DOMLocator.h DOMLocator.h
\brief Locator

DOMLocator is an interface that describes a location (e.g. where an error occurred).

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p106]
\note Introduced in DOM Level 3.
*/
    class DOMLocator
    {
    private:
        struct _DOMLocator;
        _DOMLocator *_data;

    public:
        virtual ~DOMLocator(void);
        DOMLocator(void);

/*! \brief Get the line number this locator is pointing to.
\return The line number this locator is pointing to or -1 if there is no line number available.
*/
        virtual long getLineNumber(void);
        virtual void setLineNumber(long lineNumber);

/*! \brief Get the column number this locator is pointing to.
\return The column number this locator is pointing to or -1 if there is no column number available.
*/
        virtual long getColumnNumber(void);
        virtual void setColumnNumber(long columnNumber);

/*! \brief Get the byte offset into the input source this locator is pointing to.
\return The byte offset into the input source this locator is pointing to or -1 if there is no byte offset available.
*/
        virtual long getByteOffset(void);
        virtual void setByteOffset(long byteOffset);

/*! \brief Get the UTF-16 offset into the input source this locator is pointing to.
\return The UTF-16 offset into the input source this locator is pointing to or -1 if there is no UTF-16 offset available.
*/
        virtual long getUtf16Offset(void);
        virtual void setUtf16Offset(long utf16Offset);

/*! \brief Get the node this locator is pointing to.
\return The node this locator is pointing to, or empty if no node is available.
*/
        virtual NodeSP getRelatedNode(void);
        virtual void setRelatedNode(NodeSP relatedNode);

/*! \brief Get the URI this locator is pointing to.
\return The node this URI is pointing to, or empty if no URI is available.
*/
        virtual DOMString getUri(void);
        virtual void setUri(const DOMString &uri);

    };

}
