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
/*! \file dom/CDATASection.h
\headerfile dom/CDATASection.h
\brief Provides dom::CDATASection.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5
*/
#pragma once

#include "dom/Text.h"

namespace dom
{
/*! \class dom::CDATASection CDATASection.h CDATASection.h
\brief CDATA Section

CDATA sections are used to escape blocks of text containing characters that would otherwise be regarded as markup.
The only delimiter that is recognized in a CDATA section is the "]]>" string that ends the CDATA section.
CDATA sections cannot be nested.
Their primary purpose is for including material such as XML fragments, without needing to escape all the delimiters.

The CharacterData::data attribute holds the text that is contained by the CDATA section.
Note that this may contain characters that need to be escaped outside of CDATA sections and that, depending on the character encoding ("charset") chosen for serialization, it may be impossible to write out some characters as part of a CDATA section.

The CDATASection interface inherits from the CharacterData interface through the Text interface.
Adjacent CDATASection nodes are not merged by use of the normalize method of the Node interface.

No lexical check is done on the content of a CDATA section and it is therefore possible to have the character sequence "]]>" in the content, which is illegal in a CDATA section per section 2.7 of XML 1.0.
The presence of this character sequence must generate a fatal error during serialization or the cdata section must be splitted before the serialization (see also the parameter "split-cdata-sections" in the DOMConfiguration interface).

\note Because no markup is recognized within a CDATASection, character numeric references cannot be used as an escape mechanism when serializing.
Therefore, action needs to be taken when serializing a CDATASection with a character encoding where some of the contained characters cannot be represented.
Failure to do so would not produce well-formed XML.
One potential solution in the serialization process is to end the CDATA section before the character, output the character using a character reference or entity reference, and open a new CDATA section for any further characters in the text node.
Note, however, that some code conversion libraries at the time of writing do not return an error or exception when a character is missing from the encoding, making the task of ensuring that data is not corrupted on serialization more difficult.

\note This class is instantiated via Document::createCDATASection().
\sa Document, Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.5 [p114]
*/
    class CDATASection : public Text
    {
    private:
        struct _CDATASection;
        _CDATASection *_data;

    public:
        virtual ~CDATASection(void);
        CDATASection(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);

    };

}
