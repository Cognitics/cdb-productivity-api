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

#include "dom/NodeIterator.h"
#include "dom/Node.h"
#include "dom/NodeFilter.h"

namespace dom
{
    struct NodeIterator::_NodeIterator
    {
        NodeSP root;
        NodeFilter whatToShowFilter;
        NodeFilterSP filter;
        bool entityReferenceExpansion;

        _NodeIterator(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion)
            : root(root), whatToShowFilter(whatToShow), filter(filter), entityReferenceExpansion(entityReferenceExpansion) { }
    };

    NodeIterator::~NodeIterator(void)
    {
    }

    NodeIterator::NodeIterator(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion)
        : _data(std::shared_ptr<_NodeIterator>(new _NodeIterator(root, whatToShow, filter, entityReferenceExpansion)))
    {
    }

    NodeSP NodeIterator::getRoot(void)
    {
        return _data->root;
    }

    unsigned long NodeIterator::getWhatToShow(void)
    {
        return _data->whatToShowFilter.getWhatToShow();
    }

    NodeFilterSP NodeIterator::getFilter(void)
    {
        return _data->filter;
    }

    bool NodeIterator::expandEntityReferences(void)
    {
        return _data->entityReferenceExpansion;
    }

/*
    NodeSP NodeIterator::getNextNode(void)
    {
        throw std::runtime_error("NodeIterator::getNextNode(): not yet implemented");
    }
*/

/*
    NodeSP NodeIterator::getPreviousNode(void)
    {
        throw std::runtime_error("NodeIterator::getPreviousNode(): not yet implemented");
    }
*/

/*
    void NodeIterator::detach(void)
    {
        throw std::runtime_error("NodeIterator::detach(): not yet implemented");
    }
*/



}