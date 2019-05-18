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
/*! \file dom/ProcessingInstruction.h
\headerfile dom/ProcessingInstruction.h
\brief Provides dom::ProcessingInstruction.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::ProcessingInstruction ProcessingInstruction.h dom/ProcessingInstruction.h
\brief Processing Instruction

The ProcessingInstruction interface represents a "processing instruction", used in XML as a way to keep processor-specific information in the text of the document.

No lexical check is done on the content of a processing instruction and it is therefore possible to have the character sequence "?>" in the content, which is illegal a processing instruction per section 2.6 of XML 1.0.
The presence of this character sequence must generate a fatal error during serialization.

\note This class is instantiated via Document::createProcessingInstruction().
\sa Document, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5 [p118]
*/
    class ProcessingInstruction : public Node
    {
    private:
        struct _ProcessingInstruction;
        _ProcessingInstruction *_data;

    public:
        virtual ~ProcessingInstruction(void);
        ProcessingInstruction(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);
        virtual DOMString getNodeValue(void);
        virtual DOMString getTextContent(void);
        virtual void setTextContent(const DOMString &textContent);
        virtual NodeSP cloneNode(bool deep);
        virtual NodeSP appendChild(NodeSP newChild);
        virtual NodeSP insertBefore(NodeSP newChild, NodeSP refChild = NodeSP());
        virtual NodeSP replaceChild(NodeSP newChild, NodeSP oldChild);

/*! \brief Get the target of this processing instruction.

XML defines this as being the first token following the markup that begins the processing instruction.

\return The target of this processing instruction.
*/
        virtual DOMString getTarget(void);
        virtual void setTarget(const DOMString &target);

/*! \brief Get the content of this processing instruction.

This is from the first non white space character after the target to the character immediately preceding the ?>.

\return The content of this processing instruction.
*/
        virtual DOMString getData(void);

/*! \brief Set the content of this processing instruction.
\param data The content to set.
*/
        virtual void setData(const DOMString &data);

    };

}
