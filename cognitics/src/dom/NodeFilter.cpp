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

#include "dom/NodeFilter.h"
#include "dom/Node.h"

namespace dom
{
    struct NodeFilter::_NodeFilter
    {
        unsigned long whatToShow;

        _NodeFilter(unsigned long whatToShow) : whatToShow(whatToShow) { }
    };

    NodeFilter::~NodeFilter(void)
    {
        delete _data;
    }

    NodeFilter::NodeFilter(unsigned long whatToShow) : _data(new _NodeFilter(whatToShow))
    {
    }

    unsigned long NodeFilter::getWhatToShow(void)
    {
        return _data->whatToShow;
    }

    unsigned short NodeFilter::acceptNode(NodeSP n)
    {
        if(_data->whatToShow && SHOW_ALL)
            return FILTER_ACCEPT;
        switch(n->getNodeType())
        {
            case Node::ELEMENT_NODE:                return (_data->whatToShow && SHOW_ELEMENT) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::ATTRIBUTE_NODE:                return (_data->whatToShow && SHOW_ATTRIBUTE) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::TEXT_NODE:                    return (_data->whatToShow && SHOW_TEXT) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::CDATA_SECTION_NODE:            return (_data->whatToShow && SHOW_CDATA_SECTION) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::ENTITY_REFERENCE_NODE:        return (_data->whatToShow && SHOW_ENTITY_REFERENCE) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::ENTITY_NODE:                    return (_data->whatToShow && SHOW_ENTITY) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::PROCESSING_INSTRUCTION_NODE:    return (_data->whatToShow && SHOW_PROCESSING_INSTRUCTION) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::COMMENT_NODE:                return (_data->whatToShow && SHOW_COMMENT) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::DOCUMENT_NODE:                return (_data->whatToShow && SHOW_DOCUMENT) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::DOCUMENT_TYPE_NODE:            return (_data->whatToShow && SHOW_DOCUMENT_TYPE) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::DOCUMENT_FRAGMENT_NODE:        return (_data->whatToShow && SHOW_DOCUMENT_FRAGMENT) ? FILTER_ACCEPT : FILTER_SKIP;
            case Node::NOTATION_NODE:                return (_data->whatToShow && SHOW_NOTATION) ? FILTER_ACCEPT : FILTER_SKIP;
        };
        return FILTER_SKIP;
    }

}