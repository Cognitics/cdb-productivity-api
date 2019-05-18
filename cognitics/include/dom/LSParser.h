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
/*! \file dom/LSParser.h
\headerfile dom/LSParser.h
\brief Provides dom::LSParser.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::LSParser LSParser.h LSParser.h
\brief LS Parser

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p14]
*/
    class LSParser : public DOMObject, public boost::noncopyable
    {
    private:
        struct _LSParser;
        std::shared_ptr<_LSParser> _data; 

    public:
        virtual ~LSParser(void);
        LSParser(DOMImplementationSP implementation);

        static const int ACTION_APPEND_AS_CHILDREN    = 1;
        static const int ACTION_REPLACE_CHILDREN    = 2;
        static const int ACTION_INSERT_BEFORE        = 3;
        static const int ACTION_INSERT_AFTER        = 4;
        static const int ACTION_REPLACE                = 5;

        // TODO: virtual DOMConfiguration getDOMConfig(void);
        // TODO: virtual LSParserFilterSP getFilter(void);
        // TODO: virtual void setFilter(LSParserFilterSP filter);
        // TODO: virtual bool isAsync(void);
        // TODO: virtual bool isBusy(void);
        virtual DocumentSP parse(LSInputSP input);
        // TODO: virtual DocumentSP parseURI(const DOMString &uri);
        virtual NodeSP parseWithContext(LSInputSP input, NodeSP contextArg, unsigned short action);
        // TODO: virtual void abort(void);

    };

}
