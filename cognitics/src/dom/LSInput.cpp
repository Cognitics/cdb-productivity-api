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

#pragma warning ( disable : 4244 )        // possible loss of data

#include "dom/LSInput.h"

namespace dom
{
    struct LSInput::_LSInput
    {
        LSInputSP sourceInput;
        std::locale locale;
        LSReader::pos_type cstart;
        LSInputStream::pos_type bstart;
        DOMString::iterator sstart;
        DOMString::iterator stringDataPosition;
        bool failed;

        _LSInput(void) : failed(true) { }

    };

    LSInput::~LSInput(void)
    {
        if(!_data->failed)
            return;
        if(characterStream)
            characterStream->seekg(_data->cstart);
        else if(byteStream)
            byteStream->seekg(_data->bstart);
        else if(stringData)
            _data->stringDataPosition = _data->sstart;
    }

    LSInput::LSInput(void) : characterStream(NULL), byteStream(NULL), stringData(NULL), _data(std::shared_ptr<_LSInput>(new _LSInput))
    {
    }

    LSInput::LSInput(LSInputSP input) : characterStream(NULL), byteStream(NULL), stringData(NULL), _data(std::shared_ptr<_LSInput>(new _LSInput))
    {
        if(input->characterStream)
        {
            characterStream = input->characterStream;
            _data->cstart = characterStream->tellg();
            return;
        }
        if(input->byteStream)
        {
            byteStream = input->byteStream;
            _data->bstart = byteStream->tellg();
            return;
        }
        if(input->stringData)
        {
            stringData = input->stringData;
            _data->sstart = input->_data->sstart;
            _data->stringDataPosition = _data->sstart;
            return;
        }
    }

    bool LSInput::pass(void)
    {
        _data->failed = false;
        return true;
    }

    bool LSInput::fail(void)
    {
        _data->failed = true;
        return false;
    }

    DOMChar LSInput::peek(void)
    {
        if(characterStream)
            return characterStream->peek();
        if(byteStream)
            return std::use_facet<std::ctype<DOMChar> >(_data->locale).widen(byteStream->peek());
        if(stringData)
        {
            /*
            if(!stringDataPosition._Has_container())
            {
                stringDataPosition = stringData->begin();
                sstart = stringDataPosition;
            }
            return *stringDataPosition;
            */
        }
        return 0;
    }

    DOMChar LSInput::get(void)
    {
        if(characterStream)
            return characterStream->get();
        if(byteStream)
            return std::use_facet<std::ctype<DOMChar> >(_data->locale).widen(byteStream->get());
        if(stringData)
        {
            /*
            if(!stringDataPosition._Has_container())
            {
                stringDataPosition = stringData->begin();
                sstart = stringDataPosition;
            }
            DOMChar c = *stringDataPosition;
            ++stringDataPosition;
            return c;
            */
        }
        return 0;
    }

    bool LSInput::eof(void)
    {
        if(characterStream)
            return characterStream->eof();
        if(byteStream)
            return byteStream->eof();
        if(stringData)
        {
            /*
            if(!stringDataPosition._Has_container())
            {
                stringDataPosition = stringData->begin();
                sstart = stringDataPosition;
            }
            return (stringDataPosition == stringData->end());
            */
        }
        return true;
    }

    bool LSInput::ch(const DOMChar c)
    {
        if(eof())
            return false;
        if(peek() == c)
        {
            get();
            return true;
        }
        return false;
    }

    bool LSInput::range(const DOMChar c1, const DOMChar c2)
    {
        if(eof())
            return false;
        const DOMChar c = peek();
        if((c >= c1) && (c <= c2))
        {
            get();
            return true;
        }
        return false;
    }

    bool LSInput::in(const std::string &s)
    {
        const DOMChar c = peek();
        const char *sp = &s[0];
        while(*sp)
        {
            if(c == *sp)
            {
                get();
                return true;
            }
            ++sp;
        }
        return false;
    }

    bool LSInput::not_in(const std::string &s)
    {
        const DOMChar c = peek();
        const char *sp = &s[0];
        while(*sp)
        {
            if(c == *sp)
                return false;
            ++sp;
        }
        get();
        return true;
    }

    bool LSInput::str(const std::string &s)
    {
        LSReader::pos_type p = characterStream->tellg();
        const char *sp = &s[0];
        while(*sp)
        {
            if(eof() || (get() != *sp))
            {
                characterStream->seekg(p);
                return false;
            }
            ++sp;
        }
        return true;
    }

    bool LSInput::not_str(const std::string &s)
    {
        LSReader::pos_type p = characterStream->tellg();
        const char *sp = &s[0];
        while(*sp)
        {
            if(eof() || (get() != *sp))
            {
                characterStream->seekg(p);
                return true;
            }
            ++sp;
        }
        characterStream->seekg(p);
        return false;
    }

    DOMString LSInput::copy(int startOffset, int endOffset)
    {
        if(characterStream)
        {
            LSReader::pos_type pos = characterStream->tellg();
            LSReader::pos_type begin = _data->cstart;
            LSReader::pos_type end = pos;
            begin += startOffset;
            end += endOffset;
            int size = end - begin;
            if(size <= 0)
                return DOMString();
            DOMString result;
            result.resize(size);
            characterStream->seekg(begin);
            characterStream->get(&result[0], size + 1, 0);
            characterStream->seekg(pos);
            return result;
        }
        if(byteStream)
        {
            LSInputStream::pos_type pos = byteStream->tellg();
            LSInputStream::pos_type begin = _data->bstart;
            LSInputStream::pos_type end = pos;
            begin += startOffset;
            end += endOffset;
            int size = end - begin;
            std::string result;
            result.resize(size);
            byteStream->seekg(begin);
            byteStream->get(&result[0], size + 1);
            byteStream->seekg(pos);
            return result;
        }
        if(stringData)
        {
            DOMString::iterator begin = _data->sstart;
            DOMString::iterator end = _data->stringDataPosition;
            begin += startOffset;
            end += endOffset;
            return DOMString(begin, end);
        }
        return DOMString();
    }



}

