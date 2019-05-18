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
/*! \file dom/LSParserFilter.h
\headerfile dom/LSParserFilter.h
\brief Provides dom::LSParserFilter.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::LSParserFilter LSParserFilter.h LSParserFilter.h
\brief LS Parser Filter

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p25]
*/
    class LSParserFilter : public DOMObject
    {
    public:
        virtual ~LSParserFilter(void) { }
        LSParserFilter(void) { }

        static const int FILTER_ACCEPT        = 1;
        static const int FILTER_REJECT        = 2;
        static const int FILTER_SKIP        = 3;
        static const int FILTER_INTERRUPT    = 4;

        virtual unsigned long getWhatToShow(void) = 0;
        virtual unsigned short startElement(ElementSP elementArg) = 0;
        virtual unsigned short acceptNode(NodeSP nodeArg) = 0;

    };

}
