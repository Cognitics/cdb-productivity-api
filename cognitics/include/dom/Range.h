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
/*! \file dom/Range.h
\headerfile dom/Range.h
\brief Provides dom::Range.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 2.13
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::Range Range.h Range.h
\brief Range

\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 2.13 [p42]
*/
    class Range : public DOMObject
    {
    private:
        struct _Range;
        std::shared_ptr<_Range> _data; 

    public:
        virtual ~Range(void);
        Range(void);

        static const unsigned short START_TO_START    = 0;
        static const unsigned short START_TO_END    = 1;
        static const unsigned short END_TO_END        = 2;
        static const unsigned short END_TO_START    = 3;

/* TODO:
        virtual NodeSP getStartContainer(void);
        virtual long getStartOffset(void);
        virtual NodeSP getEndContainer(void);
        virtual long getEndOffset(void);
        virtual bool isCollapsed(void);
        virtual NodeSP getCommonAncestorContainer(void);

        virtual void setStart(NodeSP refNode, long offset);
        virtual void setEnd(NodeSP refNode, long offset);
        virtual void setStartBefore(NodeSP refNode);
        virtual void setStartAfter(NodeSP refNode);
        virtual void setEndBefore(NodeSP refNode);
        virtual void setEndAfter(NodeSP refNode);
        virtual void collapse(bool toStart);
        virtual void selectNode(NodeSP refNode);
        virtual void selectNodeContents(NodeSP refNode);
        virtual unsigned short compareBoundaryPoints(unsigned short how, RangeSP sourceRange);
        virtual void deleteContents(void);
        virtual DocumentFragmentSP extractContents(void);
        virtual DocumentFragmentSP cloneContents(void);
        virtual void insertNode(NodeSP newNode);
        virtual void surroundContents(NodeSP newParent);
        virtual RangeSP cloneRange(void);
        virtual DOMString toString(void);
        virtual void detach(void);
*/

    };


}
