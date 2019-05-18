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
/*! \file dom/Comment.h
\headerfile dom/Comment.h
\brief Provides dom::Comment.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/CharacterData.h"

namespace dom
{
/*! \class dom::Comment Comment.h Comment.h
\brief Comment

This interface inherits from CharacterData and represents the content of a comment, i.e. all the characters between the starting '<!--' and ending '-->'.
Note that this is the definition of a comment in XML, and, in practice, HTML, although some HTML tools may implement the full SGML comment structure.

No lexical check is done on the content of a comment and it is therefore possible to have the character sequence "--" (double-hyphen) in the content, which is illegal in a comment per section 2.5 of XML 1.0.
The presence of this character sequence must generate a fatal error during serialization.

\note This class is instantiated via Document::createTextNode().
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p99]
*/
    class Comment : public CharacterData
    {
    private:
        struct _Comment;
        _Comment *_data;

    public:
        virtual ~Comment(void);
        Comment(void);

        virtual unsigned short getNodeType(void);
        virtual DOMString getNodeName(void);

    };

}
