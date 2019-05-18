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
/*! \file dom/RangeException.h
\headerfile dom/RangeException.h
\brief Provides dom::RangeException.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 2.13
*/
#pragma once

#include <exception>

namespace dom
{
    const unsigned short BAD_BOUNDARYPOINTS_ERR        = 1;
    const unsigned short INVALID_NODE_TYPE_ERR        = 2;

/*! \class dom::RangeException RangeException.h RangeException.h
\brief Range Exception

Exception Codes:
\li BAD_BOUNDARYPOINTS_ERR - the boundary-points of a Range do not meet specific requirements.
\li INVALID_NODE_TYPE_ERR - the container of an boundary-point of a Range is being set to either a node of an invalid type or a node with an ancestor of an invalid type.

\sa Document Object Model (DOM) Level 2 Traversal and Range Specification (http://www.w3.org/TR/DOM-Level-2-Traversal-Range/) 2.13 [p53]
*/

    class RangeException : public std::exception
    {
    public:
        unsigned short code;    //!< type of error generated

        RangeException(unsigned short code) : code(code) { }

        virtual const char *what() const throw()
        {
            switch(code)
            {
                case BAD_BOUNDARYPOINTS_ERR:
                    return "BAD_BOUNDARYPOINTS_ERR: the boundary-points of a Range do not meet specific requirements.";
                case INVALID_NODE_TYPE_ERR:
                    return "INVALID_NODE_TYPE_ERR: the container of an boundary-point of a Range is being set to either a node of an invalid type or a node with an ancestor of an invalid type.";
            }
            return "";
        }
    };

}