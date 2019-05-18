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

#include "dom/CharacterData.h"
#include "dom/DOMException.h"

namespace dom
{
    struct CharacterData::_CharacterData
    {
        DOMString data;
    };

    CharacterData::~CharacterData(void)
    {
        delete _data;
    }

    CharacterData::CharacterData(void) : _data(new _CharacterData)
    {
    }

    DOMString CharacterData::getNodeValue(void)
    {
        return getData();
    }

    DOMString CharacterData::getTextContent(void)
    {
        return getData();
    }

    void CharacterData::setTextContent(const DOMString &textContent)
    {
        setData(textContent);
    }

    NodeSP CharacterData::cloneNode(bool deep)
    {
        CharacterDataSP clone = std::dynamic_pointer_cast<CharacterData>(Node::cloneNode(deep));
        clone->_data->data = _data->data;
        return clone;
    }

    NodeSP CharacterData::appendChild(NodeSP newChild)
    {
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP CharacterData::insertBefore(NodeSP newChild, NodeSP refChild)
    {
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    NodeSP CharacterData::replaceChild(NodeSP newChild, NodeSP oldChild)
    {
        throw DOMException(HIERARCHY_REQUEST_ERR);
    }

    DOMString CharacterData::getData(void)
    {
        return _data->data;
    }

    void CharacterData::setData(const DOMString &data)
    {
        this->_data->data = data;
    }

    unsigned long CharacterData::getLength(void)
    {
        return (unsigned long)_data->data.size();
    }

    DOMString CharacterData::substringData(unsigned long offset, unsigned long count)
    {
        try
        {
            return _data->data.substr(offset, count);
        }
        catch(std::out_of_range e)
        {
            throw DOMException(INDEX_SIZE_ERR);
        }
    }

    void CharacterData::appendData(const DOMString &arg)
    {
        _data->data.append(arg);
    }

    void CharacterData::insertData(unsigned long offset, const DOMString &arg)
    {
        _data->data.insert(offset, arg);
    }

    void CharacterData::deleteData(unsigned long offset, unsigned long count)
    {
        _data->data.erase(offset, count);
    }

    void CharacterData::replaceData(unsigned long offset, unsigned long count, const DOMString &arg)
    {
        _data->data.replace(offset, count, arg);
    }


}