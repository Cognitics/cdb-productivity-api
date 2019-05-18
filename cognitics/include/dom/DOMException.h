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
/*! \brief Provides dom::DOMException.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include <exception>
#include <stdexcept>

namespace dom
{
    const unsigned short INDEX_SIZE_ERR                    = 1;
    const unsigned short DOMSTRING_SIZE_ERR                = 2;
    const unsigned short HIERARCHY_REQUEST_ERR            = 3;
    const unsigned short WRONG_DOCUMENT_ERR                = 4;
    const unsigned short INVALID_CHARACTER_ERR            = 5;
    const unsigned short NO_DATA_ALLOWED_ERR            = 6;
    const unsigned short NO_MODIFICATION_ALLOWED_ERR    = 7;
    const unsigned short NOT_FOUND_ERR                    = 8;
    const unsigned short NOT_SUPPORTED_ERR                = 9;
    const unsigned short INUSE_ATTRIBUTE_ERR            = 10;
    const unsigned short INVALID_STATE_ERR                = 11;
    const unsigned short SYNTAX_ERR                        = 12;
    const unsigned short INVALID_MODIFICATION_ERR        = 13;
    const unsigned short NAMESPACE_ERR                    = 14;
    const unsigned short INVALID_ACCESS_ERR                = 15;
    const unsigned short VALIDATION_ERR                    = 16;
    const unsigned short TYPE_MISMATCH_ERR                = 17;

/*! \class dom::DOMException DOMException.h DOMException.h
\brief Exception

Exception Codes:
\li <tt>INDEX_SIZE_ERR</tt> - Index or size is negative, or greater than the allowed value.
\li <tt>DOMSTRING_SIZE_ERR</tt> - Specified range of text does not fit into a DOMString.
\li <tt>HIERARCHY_REQUEST_ERR</tt> - Node inserted somewhere it doesn't belong.
\li <tt>WRONG_DOCUMENT_ERR</tt> - Node is used in a different document than the one that created it (that doesn't support it).
\li <tt>INVALID_CHARACTER_ERR</tt> - Invalid or illegal character specified.
\li <tt>NO_DATA_ALLOWED_ERR</tt> - Data specified for a node which does not support data.
\li <tt>NO_MODIFICATION_ALLOWED_ERR</tt> - Attempt made to modify an object where modifications are not allowed.
\li <tt>NOT_FOUND_ERR</tt> - Attempt made to reference a node in a context where it does not exist.
\li <tt>NOT_SUPPORTED_ERR</tt> - Implementation does not support the requested type of object or operation.
\li <tt>INUSE_ATTRIBUTE_ERR</tt> - Attempt made to add an attribute that is already in use elsewhere.
\li <tt>INVALID_STATE_ERR</tt> - [DOM2] Attempt made to use an object that is not, or is no longer, usable.
\li <tt>SYNTAX_ERR</tt> - [DOM2] Invalid or illegal string specified.
\li <tt>INVALID_MODIFICATION_ERR</tt> - [DOM2] Attempt made to modify the type of the underlying object.
\li <tt>NAMESPACE_ERR</tt> - [DOM2] Attempt made to create or change an object in a way which is incorrect with regard to namespaces.
\li <tt>INVALID_ACCESS_ERR</tt> - [DOM2] Parameter or operation not supported by the underlying object.
\li <tt>VALIDATION_ERR</tt> - [DOM3] Node invalid with respect to partial validity.
\li <tt>TYPE_MISMATCH_ERR</tt> - [DOM3] Object is incompatible with the expected type of the parameter associated to the object.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p31]
*/

    class DOMException : public std::exception
    {
    public:
        unsigned short code;    //!< type of error generated

        DOMException(unsigned short code) : code(code) { }

        virtual const char *what() const throw()
        {
            switch(code)
            {
                case INDEX_SIZE_ERR:                return "INDEX_SIZE_ERR: Index or size is negative, or greater than the allowed value.";
                case DOMSTRING_SIZE_ERR:            return "DOMSTRING_SIZE_ERR: Specified range of text does not fit into a DOMString.";
                case HIERARCHY_REQUEST_ERR:            return "HIERARCHY_REQUEST_ERR: Node inserted somewhere it doesn't belong.";
                case WRONG_DOCUMENT_ERR:            return "WRONG_DOCUMENT_ERR: Node is used in a different document than the one that created it (that doesn't support it).";
                case INVALID_CHARACTER_ERR:            return "INVALID_CHARACTER_ERR: Invalid or illegal character specified.";
                case NO_DATA_ALLOWED_ERR:            return "NO_DATA_ALLOWED_ERR: Data specified for a node which does not support data.";
                case NO_MODIFICATION_ALLOWED_ERR:    return "NO_MODIFICATION_ALLOWED_ERR: Attempt made to modify an object where modifications are not allowed.";
                case NOT_FOUND_ERR:                    return "NOT_FOUND_ERR: Attempt made to reference a node in a context where it does not exist.";
                case NOT_SUPPORTED_ERR:                return "NOT_SUPPORTED_ERR: Implementation does not support the requested type of object or operation.";
                case INUSE_ATTRIBUTE_ERR:            return "INUSE_ATTRIBUTE_ERR: Attempt made to add an attribute that is already in use elsewhere.";
                case INVALID_STATE_ERR:                return "INVALID_STATE_ERR: Attempt made to use an object that is not, or is no longer, usable.";
                case SYNTAX_ERR:                    return "SYNTAX_ERR: Invalid or illegal string specified.";
                case INVALID_MODIFICATION_ERR:        return "INVALID_MODIFICATION_ERR: Attempt made to modify the type of the underlying object.";
                case NAMESPACE_ERR:                    return "NAMESPACE_ERR: Attempt made to create or change an object in a way which is incorrect with regard to namespaces.";
                case INVALID_ACCESS_ERR:            return "INVALID_ACCESS_ERR: Parameter or operation not supported by the underlying object.";
                case VALIDATION_ERR:                return "VALIDATION_ERR: Node invalid with respect to partial validity.";
                case TYPE_MISMATCH_ERR:                return "TYPE_MISMATCH_ERR: Object is incompatible with the expected type of the parameter associated to the object.";
            }
            return "";
        }
    };

}
