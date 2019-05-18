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
/*! \brief Provides dom::LSException.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include <exception>

namespace dom
{
    const unsigned short PARSE_ERR        = 81;
    const unsigned short SERIALIZE_ERR    = 82;

/*! \brief LS Exception

Exception Codes:
\li <tt>PARSE_ERR</tt> - An attempt was made to load a document, or an XML Fragment, using LSParser and the processing has been stopped.
\li <tt>SERIALIZE_ERR</tt> - An attempt was made to serialize a Node using LSSerializer and the processing has been stopped.

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p11]
*/

    class LSException : public std::exception
    {
    public:
        unsigned short code;    //!< type of error generated

        LSException(unsigned short code) : code(code) { }

        virtual const char *what() const throw()
        {
            switch(code)
            {
                case PARSE_ERR:
                    return "PARSE_ERR: An attempt was made to load a document, or an XML Fragment, using LSParser and the processing has been stopped.";
                case SERIALIZE_ERR:
                    return "SERIALIZE_ERR: An attempt was made to serialize a Node using LSSerializer and the processing has been stopped.";
            }
            return "";
        }
    };

}