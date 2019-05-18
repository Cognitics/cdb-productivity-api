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

#include "dom/TreeWalker.h"
#include "dom/DOMException.h"
#include "dom/Node.h"
#include "dom/NodeFilter.h"

namespace dom
{
    struct TreeWalker::_TreeWalker
    {
        NodeSP root;
        NodeFilter whatToShowFilter;
        NodeFilterSP filter;
        bool entityReferenceExpansion;
        NodeSP currentNode;

        _TreeWalker(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion)
            : root(root), whatToShowFilter(whatToShow), filter(filter), entityReferenceExpansion(entityReferenceExpansion) { }

        unsigned short acceptNode(NodeSP n)
        {
            if(!entityReferenceExpansion && (n->getNodeType() == Node::ENTITY_REFERENCE_NODE))
                return NodeFilter::FILTER_REJECT;
            unsigned short result = whatToShowFilter.acceptNode(n);
            if(result != NodeFilter::FILTER_ACCEPT)
                return result;
            if(filter)
                result = filter->acceptNode(n);
            return result;
        }

        NodeSP getParentNode(NodeSP n)
        {
            if(n == root)
                return NodeSP();
            NodeSP target = n->getParentNode();
            if(!target)
                return NodeSP();
            if(acceptNode(target) == NodeFilter::FILTER_ACCEPT)
            {
                currentNode = target;
                return currentNode;
            }
            return getParentNode(target);
        }

        NodeSP getFirstChild(NodeSP n)
        {
            NodeList childNodes = n->getChildNodes();
            for(NodeList::iterator it = childNodes.begin(), end = childNodes.end(); it != end; ++it)
            {
                if(acceptNode(*it) == NodeFilter::FILTER_SKIP)
                {
                    if(getFirstChild(*it))
                        return currentNode;
                }
                if(acceptNode(*it) == NodeFilter::FILTER_ACCEPT)
                {
                    currentNode = *it;
                    return currentNode;
                }
            }
            return NodeSP();
        }

        NodeSP getLastChild(NodeSP n)
        {
            NodeList childNodes = n->getChildNodes();
            for(NodeList::reverse_iterator it = childNodes.rbegin(), end = childNodes.rend(); it != end; ++it)
            {
                if(acceptNode(*it) == NodeFilter::FILTER_SKIP)
                {
                    if(getLastChild(*it))
                        return currentNode;
                }
                if(acceptNode(*it) == NodeFilter::FILTER_ACCEPT)
                {
                    currentNode = *it;
                    return currentNode;
                }
            }
            return NodeSP();
        }

    };

    TreeWalker::~TreeWalker(void)
    {
    }

    TreeWalker::TreeWalker(NodeSP root, unsigned long whatToShow, NodeFilterSP filter, bool entityReferenceExpansion)
        : _data(std::shared_ptr<_TreeWalker>(new _TreeWalker(root, whatToShow, filter, entityReferenceExpansion)))
    {
    }

    NodeSP TreeWalker::getRoot(void)
    {
        return _data->root;
    }

    unsigned long TreeWalker::getWhatToShow(void)
    {
        return _data->whatToShowFilter.getWhatToShow();
    }

    NodeFilterSP TreeWalker::getFilter(void)
    {
        return _data->filter;
    }

    bool TreeWalker::expandEntityReferences(void)
    {
        return _data->entityReferenceExpansion;
    }

    NodeSP TreeWalker::getCurrentNode(void)
    {
        return _data->currentNode;
    }

    void TreeWalker::setCurrentNode(NodeSP currentNode)
    {
        if(!currentNode)
            throw DOMException(NOT_SUPPORTED_ERR);
        _data->currentNode = currentNode;
    }

    NodeSP TreeWalker::getParentNode(void)
    {
        return _data->getParentNode(_data->currentNode);
    }

    NodeSP TreeWalker::getFirstChild(void)
    {
        return _data->getFirstChild(_data->currentNode);
    }

    NodeSP TreeWalker::getLastChild(void)
    {
        return _data->getLastChild(_data->currentNode);
    }

    NodeSP TreeWalker::getPreviousSibling(void)
    {
        NodeSP parent = _data->currentNode->getParentNode();
        if(!parent)
            return NodeSP();
        NodeList childNodes = parent->getChildNodes();
        NodeList::reverse_iterator it = std::find(childNodes.rbegin(), childNodes.rend(), _data->currentNode);
        if(it == childNodes.rend())
            return NodeSP();
        ++it;
        while(it != childNodes.rend())
        {
            if(_data->acceptNode(*it) == NodeFilter::FILTER_SKIP)
            {
                if(_data->getLastChild(*it))
                    return _data->currentNode;
            }
            if(_data->acceptNode(*it) == NodeFilter::FILTER_ACCEPT)
            {
                _data->currentNode = *it;
                return _data->currentNode;
            }
        }
        return NodeSP();
    }

    NodeSP TreeWalker::getNextSibling(void)
    {
        NodeSP parent = _data->currentNode->getParentNode();
        if(!parent)
            return NodeSP();
        NodeList childNodes = parent->getChildNodes();
        NodeList::iterator it = std::find(childNodes.begin(), childNodes.end(), _data->currentNode);
        if(it == childNodes.end())
            return NodeSP();
        ++it;
        while(it != childNodes.end())
        {
            if(_data->acceptNode(*it) == NodeFilter::FILTER_SKIP)
            {
                if(_data->getFirstChild(*it))
                    return _data->currentNode;
            }
            if(_data->acceptNode(*it) == NodeFilter::FILTER_ACCEPT)
            {
                _data->currentNode = *it;
                return _data->currentNode;
            }
        }
        return NodeSP();
    }

/*
    NodeSP TreeWalker::getPreviousNode(void)
    {
        throw std::runtime_error("TreeWalker::getPreviousNode(): not yet implemented");
    }
*/

/*
    NodeSP TreeWalker::getNextNode(void)
    {
        throw std::runtime_error("TreeWalker::getNextNode(): not yet implemented");
    }
*/

}