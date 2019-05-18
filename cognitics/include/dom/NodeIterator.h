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
/*! \file dom/NodeIterator.h
\headerfile dom/NodeIterator.h
\brief Provides dom::NodeIterator.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 1.2
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::NodeIterator NodeIterator.h NodeIterator.h
\brief Traversal Node Iterator

\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 1.2 [p19]
*/
    class NodeIterator : public DOMObject, public boost::noncopyable
    {
    private:
        struct _NodeIterator;
        std::shared_ptr<_NodeIterator> _data; 

    public:
        virtual ~NodeIterator(void);
        NodeIterator(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion);

        virtual NodeSP getRoot(void);
        virtual unsigned long getWhatToShow(void);
        virtual NodeFilterSP getFilter(void);
        virtual bool expandEntityReferences(void);
        // TODO: virtual NodeSP getNextNode(void);
        // TODO: virtual NodeSP getPreviousNode(void);
        // TODO: virtual void detach(void);

    };

}
