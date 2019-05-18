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

#include "dom/Range.h"
#include "dom/Node.h"
#include "dom/DocumentFragment.h"

namespace dom
{
    struct Range::_Range
    {
    };

    Range::~Range(void)
    {
    }

    Range::Range(void) : _data(std::shared_ptr<_Range>(new _Range))
    {
    }

/*
    NodeSP Range::getStartContainer(void)
    {
        throw std::runtime_error("Range::getStartContainer(): not yet implemented");
    }

    long Range::getStartOffset(void)
    {
        throw std::runtime_error("Range::getStartOffset(): not yet implemented");
    }

    NodeSP Range::getEndContainer(void)
    {
        throw std::runtime_error("Range::getEndContainer(): not yet implemented");
    }

    long Range::getEndOffset(void)
    {
        throw std::runtime_error("Range::getEndOffset(): not yet implemented");
    }

    bool Range::isCollapsed(void)
    {
        throw std::runtime_error("Range::isCollapsed(): not yet implemented");
    }

    NodeSP Range::getCommonAncestorContainer(void)
    {
        throw std::runtime_error("Range::getCommonAncestorContainer(): not yet implemented");
    }

    void Range::setStart(NodeSP refNode, long offset)
    {
        throw std::runtime_error("Range::setStart(): not yet implemented");
    }

    void Range::setEnd(NodeSP refNode, long offset)
    {
        throw std::runtime_error("Range::setEnd(): not yet implemented");
    }

    void Range::setStartBefore(NodeSP refNode)
    {
        throw std::runtime_error("Range::setStartBefore(): not yet implemented");
    }

    void Range::setStartAfter(NodeSP refNode)
    {
        throw std::runtime_error("Range::setStartAfter(): not yet implemented");
    }

    void Range::setEndBefore(NodeSP refNode)
    {
        throw std::runtime_error("Range::setEndBefore(): not yet implemented");
    }

    void Range::setEndAfter(NodeSP refNode)
    {
        throw std::runtime_error("Range::setEndAfter(): not yet implemented");
    }

    void Range::collapse(bool toStart)
    {
        throw std::runtime_error("Range::collapse(): not yet implemented");
    }

    void Range::selectNode(NodeSP refNode)
    {
        throw std::runtime_error("Range::selectNode(): not yet implemented");
    }

    void Range::selectNodeContents(NodeSP refNode)
    {
        throw std::runtime_error("Range::selectNodeContents(): not yet implemented");
    }

    unsigned short Range::compareBoundaryPoints(unsigned short how, RangeSP sourceRange)
    {
        throw std::runtime_error("Range::compareBoundaryPoints(): not yet implemented");
    }

    void Range::deleteContents(void)
    {
        throw std::runtime_error("Range::deleteContents(): not yet implemented");
    }

    DocumentFragmentSP Range::extractContents(void)
    {
        throw std::runtime_error("Range::extractContents(): not yet implemented");
    }

    DocumentFragmentSP Range::cloneContents(void)
    {
        throw std::runtime_error("Range::cloneContents(): not yet implemented");
    }

    void Range::insertNode(NodeSP newNode)
    {
        throw std::runtime_error("Range::insertNode(): not yet implemented");
    }

    void Range::surroundContents(NodeSP newParent)
    {
        throw std::runtime_error("Range::surroundContents(): not yet implemented");
    }

    RangeSP Range::cloneRange(void)
    {
        throw std::runtime_error("Range::cloneRange(): not yet implemented");
    }

    DOMString Range::toString(void)
    {
        throw std::runtime_error("Range::toString(): not yet implemented");
    }

    void Range::detach(void)
    {
        throw std::runtime_error("Range::detach(): not yet implemented");
    }
*/


}