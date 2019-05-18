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
/*! \file dom/LSInput.h
\headerfile dom/LSInput.h
\brief Provides dom::LSInput.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::LSInput LSInput.h LSInput.h
\brief LS Input

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p22]
*/
    class LSInput : public DOMObject, boost::noncopyable
    {
    private:
        struct _LSInput;
        std::shared_ptr<_LSInput> _data; 

    public:
        LSReader *characterStream;
        LSInputStream *byteStream;
        DOMString *stringData;
        DOMString systemId;
        DOMString publicId;
        DOMString baseURI;
        DOMString encoding;
        bool certifiedText;

        virtual ~LSInput(void);
        LSInput(void);
        LSInput(LSInputSP input);

        bool pass(void);
        bool fail(void);
        LSInputSP pos(void);
        DOMChar peek(void);
        DOMChar get(void);
        bool eof(void);
        bool ch(const DOMChar c);
        bool range(const DOMChar c1, const DOMChar c2);
        bool in(const std::string &s);
        bool not_in(const std::string &s);
        bool str(const std::string &s);
        bool not_str(const std::string &s);
        DOMString copy(int startOffset = 0, int endOffset = 0);

    };

}
