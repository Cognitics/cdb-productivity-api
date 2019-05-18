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
/*! \file dom/LSSerializer.h
\headerfile dom/LSSerializer.h
\brief Provides dom::LSSerializer.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::LSSerializer LSSerializer.h LSSerializer.h
\brief LS Serializer

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p29]
*/
    class LSSerializer : public DOMObject, public boost::noncopyable
    {
    private:
        struct _LSSerializer;
        std::shared_ptr<_LSSerializer> _data; 

    public:
        virtual ~LSSerializer(void);
        LSSerializer(void);

        // TODO: virtual DOMConfiguration getDOMConfig(void);
        virtual DOMString getNewLine(void);
        virtual void setNewLine(const DOMString &newLine);
        virtual DOMString getIndent(void);
        virtual void setIndent(const DOMString &indent);
        virtual LSSerializerFilterSP getFilter(void);
        virtual void setFilter(LSSerializerFilterSP filter);
        virtual bool write(NodeSP nodeArg, LSOutputSP destination, bool recursive = true, int depth = 0);
        // TODO: virtual bool writeToURI(NodeSP nodeArg, const DOMString &uri);
        virtual DOMString writeToString(NodeSP nodeArg, bool recursive = true);

    };

}
