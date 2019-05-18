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
/*! \file dom/NodeFilter.h
\headerfile dom/NodeFilter.h
\brief Provides dom::NodeFilter.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 1.2
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::NodeFilter NodeFilter.h NodeFilter.h
\brief Traversal Node Filter

Filters are objects that know how to "filter out" nodes.
If a NodeIterator or TreeWalker is given a NodeFilter, it applies the filter before it returns the next node.
If the filter says to accept the node, the traversal logic returns it; otherwise, traversal looks for the next node and pretends that the node that was rejected was not there.

The DOM does not provide any filters.
NodeFilter is just an interface that users can implement to provide their own filters.

NodeFilters do not need to know how to traverse from node to node, nor do they need to know anything about the data structure that is being traversed.
This makes it very easy to write filters, since the only thing they have to know how to do is evaluate a single node.
One filter may be used with a number of different kinds of traversals, encouraging code reuse.

\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 1.2 [p21]
*/
    class NodeFilter : public DOMObject, public boost::noncopyable
    {
    private:
        struct _NodeFilter;
        _NodeFilter *_data; 

    public:
        virtual ~NodeFilter(void);
        NodeFilter(unsigned long whatToShow = SHOW_ALL);

        static const int FILTER_ACCEPT    = 1;
        static const int FILTER_REJECT    = 2;
        static const int FILTER_SKIP    = 3;

        static const int SHOW_ALL                        = 0xFFFF;
        static const int SHOW_ELEMENT                    = 0x0001;
        static const int SHOW_ATTRIBUTE                    = 0x0002;
        static const int SHOW_TEXT                        = 0x0004;
        static const int SHOW_CDATA_SECTION                = 0x0008;
        static const int SHOW_ENTITY_REFERENCE            = 0x0010;
        static const int SHOW_ENTITY                    = 0x0020;
        static const int SHOW_PROCESSING_INSTRUCTION    = 0x0040;
        static const int SHOW_COMMENT                    = 0x0080;
        static const int SHOW_DOCUMENT                    = 0x0100;
        static const int SHOW_DOCUMENT_TYPE                = 0x0200;
        static const int SHOW_DOCUMENT_FRAGMENT            = 0x0400;
        static const int SHOW_NOTATION                    = 0x0800;

        virtual unsigned long getWhatToShow(void);

/* \brief Test whether a specified node is visible in the logical view of a TreeWalker or NodeIterator.

This function will be called by the implementation of TreeWalker and NodeIterator; it is not normally called directly from user code.
(Though you could do so if you wanted to use the same filter to guide your own application logic.)
/param n The node to check to see if it passes the filter or not.
\return A constant to determine whether the node is accepted (FILTER_ACCEPT), rejected (FILTER_REJECT), or skipped (FILTER_SKIP).
*/
        virtual unsigned short acceptNode(NodeSP n);

    };

}
