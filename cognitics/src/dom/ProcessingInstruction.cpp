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

#include "dom/ProcessingInstruction.h"
#include "dom/DOMException.h"

namespace dom
{
    struct ProcessingInstruction::_ProcessingInstruction
    {
        DOMString target;
        DOMString data;
    };

    ProcessingInstruction::~ProcessingInstruction(void)
    {
        delete _data;
    }

    ProcessingInstruction::ProcessingInstruction(void) : _data(new _ProcessingInstruction)
    {
    }

    unsigned short ProcessingInstruction::getNodeType(void)
    {
        return PROCESSING_INSTRUCTION_NODE;
    }

    DOMString ProcessingInstruction::getNodeName(void)
    {
        return getTarget();
    }

    DOMString ProcessingInstruction::getNodeValue(void)
    {
        return getData();
    }

    DOMString ProcessingInstruction::getTextContent(void)
    {
        return getData();
    }

    void ProcessingInstruction::setTextContent(const DOMString &textContent)
    {
        setData(textContent);
    }

    NodeSP ProcessingInstruction::cloneNode(bool deep)
    {
        ProcessingInstructionSP clone = std::dynamic_pointer_cast<ProcessingInstruction>(Node::cloneNode(deep));
        clone->_data->target = _data->target;
        clone->_data->data = _data->data;
        return clone;
    }

    NodeSP ProcessingInstruction::appendChild(NodeSP newChild)
    {
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP ProcessingInstruction::insertBefore(NodeSP newChild, NodeSP refChild)
    {
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP ProcessingInstruction::replaceChild(NodeSP newChild, NodeSP oldChild)
    {
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    DOMString ProcessingInstruction::getTarget(void)
    {
        return _data->target;
    }

    void ProcessingInstruction::setTarget(const DOMString &target)
    {
        _data->target = target;
    }

    DOMString ProcessingInstruction::getData(void)
    {
        return _data->data;
    }

    void ProcessingInstruction::setData(const DOMString &data)
    {
        _data->data = data;
    }

}