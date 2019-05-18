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
/*! \file dom/Text.h
\headerfile dom/Text.h
\brief Provides dom::Text.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/CharacterData.h"

namespace dom
{
/*! \class dom::Text Text.h Text.h
\brief Text

The Text interface inherits from CharacterData and represents the textual content (termed character data in XML) of an Element or Attr.
If there is no markup inside an element's content, the text is contained in a single object implementing the Text interface that is the only child of the element.
If there is markup, it is parsed into the information items (elements, comments, etc.) and Text nodes that form the list of children of the element.

When a document is first made available via the DOM, there is only one Text node for each block of text.
Users may create adjacent Text nodes that represent the contents of a given element without any intervening markup, but should be aware that there is no way to represent the separations between these nodes in XML or HTML, so they will not (in general) persist between DOM editing sessions.
The Node::normalize() method merges any such adjacent Text objects into a single node for each block of text.

No lexical check is done on the content of a Text node and, depending on its position in the document, some characters must be escaped during serialization using character references; e.g. the characters "<&" if the textual content is part of an element or of an attribute, the character sequence "]]>" when part of an element, the quotation mark character " or the apostrophe character ' when part of an attribute.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p95]
*/
    class Text : public CharacterData
    {
    private:
        struct _Text;
        _Text *_data;

    public:
        virtual ~Text(void);
        Text(void);

        unsigned short getNodeType(void);
        DOMString getNodeName(void);

/*! \brief Get whether this text node contains element content whitespace.

The text node is determined to contain whitespace in element content during the load of the document or if validation occurs while using Document::normalizeDocument().

\return Whether this text node contains element content whitespace.
\note Introduced in DOM Level 3.
*/
        // TODO: virtual bool isElementContentWhitespace(void);

/*! \brief Get all text of this and logically adjacent text nodes to this node, concatenated in document order.
\return The concatenated text of this and logically adjacent text nodes.
\note Introduced in DOM Level 3.
*/
        // TODO: virtual DOMString getWholeText(void);

/*! \brief Break this node into two nodes at the specified offset, keeping both in the tree as siblings.

After being split, this node will contain all the content up to the offset point.
A new node of the same type, which contains all the content at and after the offset point, is returned.
If the original node had a parent node, the new node is inserted as the next sibling of the original node.
When the offset is equal to the length of this node, the new node has no data.

\param offset The offset at which to split, starting from 0.
\return The new node, of the same type as this node.
*/
        // TODO: virtual TextSP splitText(unsigned long offset);

/*! \brief Replace the text of the current node and all logically adjacent text nodes with the specified text.

All logically adjacent text nodes are removed including the current node unless it was the recipient of the replacement text.

This method returns the node which received the replacement text. The returned node is:
\li empty, when the replacement text is the empty string;
\li the current node, except when the current node is read-only;
\li a new Text node of the same type (Text or CDATASection) as the current node inserted at the location of the replacement.

\param content The content of the replacing Text node.
\note Introduced in DOM Level 3.
*/
        // TODO: virtual TextSP replaceWholeText(const DOMString &content);

    };

}
