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

#include "dom/DOMLocator.h"

namespace dom
{
    struct DOMLocator::_DOMLocator
    {
        long lineNumber;
        long columnNumber;
        long byteOffset;
        long utf16Offset;
        NodeSP relatedNode;
        DOMString uri;
    };

    DOMLocator::~DOMLocator(void)
    {
        delete _data;
    }

    DOMLocator::DOMLocator(void) : _data(new _DOMLocator)
    {
    }

    long DOMLocator::getLineNumber(void)
    {
        return _data->lineNumber;
    }

    void DOMLocator::setLineNumber(long lineNumber)
    {
        _data->lineNumber = lineNumber;
    }

    long DOMLocator::getColumnNumber(void)
    {
        return _data->columnNumber;
    }

    void DOMLocator::setColumnNumber(long columnNumber)
    {
        _data->columnNumber = columnNumber;
    }

    long DOMLocator::getByteOffset(void)
    {
        return _data->byteOffset;
    }

    void DOMLocator::setByteOffset(long byteOffset)
    {
        _data->byteOffset = byteOffset;
    }

    long DOMLocator::getUtf16Offset(void)
    {
        return _data->utf16Offset;
    }

    void DOMLocator::setUtf16Offset(long utf16Offset)
    {
        _data->utf16Offset = utf16Offset;
    }

    NodeSP DOMLocator::getRelatedNode(void)
    {
        return _data->relatedNode;
    }

    void DOMLocator::setRelatedNode(NodeSP relatedNode)
    {
        _data->relatedNode = relatedNode;
    }

    DOMString DOMLocator::getUri(void)
    {
        return _data->uri;
    }

    void DOMLocator::setUri(const DOMString &uri)
    {
        _data->uri = uri;
    }

}