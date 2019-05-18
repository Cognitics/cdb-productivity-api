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
/*! \file dom/CharacterData.h
\headerfile dom/CharacterData.h
\brief Provides dom::CharacterData.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/Node.h"

namespace dom
{
/*! \class dom::CharacterData CharacterData.h CharacterData.h
\brief Character Data

The CharacterData interface extends Node with a set of attributes and methods for accessing character data in the DOM.
For clarity this set is defined here rather than on each object that uses these attributes and methods.
No DOM objects correspond directly to CharacterData, though Text and others do inherit the interface from it.
All offsets in this interface start from 0.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p78]
*/
    class CharacterData : public Node
    {
    private:
        struct _CharacterData;
        _CharacterData *_data;

    public:
        virtual ~CharacterData(void);
        CharacterData(void);

        virtual DOMString getNodeValue(void);
        virtual DOMString getTextContent(void);
        virtual void setTextContent(const DOMString &textContent);
        virtual NodeSP cloneNode(bool deep);
        virtual NodeSP appendChild(NodeSP newChild);
        virtual NodeSP insertBefore(NodeSP newChild, NodeSP refChild = NodeSP());
        virtual NodeSP replaceChild(NodeSP newChild, NodeSP oldChild);

/*! \brief Get the data.
\return The data.
*/
        virtual DOMString getData(void);

/*! \brief Set the data.
\param data The data to assign.
*/
        virtual void setData(const DOMString &data);

/*! \brief Get the length of the data.

This just returns data.size().

\return Length of the data string.
*/
        virtual unsigned long getLength(void);

/*! \brief Extract a string of data from the character data.

This is implemented as a wrapper for data.substr().

\param offset Offset to extract from.
\param count Number of characters to extract.
\return Extracted string.
\exception INDEX_SIZE_ERR the specified offset is out of range.
*/
        virtual DOMString substringData(unsigned long offset, unsigned long count);

/*! \brief Append the string to the end of the character data of the node.

This is implemented as a wrapper for data.append().

\param arg The DOMString to append.
*/
        virtual void appendData(const DOMString &arg);

/*! \brief Insert string into the character data at the specified offset.

This is implemented as a wrapper for data.insert().

\param offset Offset to begin insert.
\param arg String to insert.
*/
        virtual void insertData(unsigned long offset, const DOMString &arg);

/*! \brief Remove a range of characters from the character data at the specified offset.

This is implemented as a wrapper for data.erase().

\param offset The offset from which to start removing.
\param count Number of characters to delete.
If the sum of offset and count exceeds length then all characters from offset to the end of the data are deleted.
*/
        virtual void deleteData(unsigned long offset, unsigned long count);

/*! \brief Replace a range of characters with the specified string from the character data at the specified offset.

This is implemented as a wrapper for data.replace().

\param offset Offset to begin replace.
\param count Number of characters to replace.
\param arg String to insert in place of the replaced characters.
*/
        virtual void replaceData(unsigned long offset, unsigned long count, const DOMString &arg);

    };

}
