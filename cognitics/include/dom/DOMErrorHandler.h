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
/*! \file dom/DOMErrorHandler.h
\headerfile dom/DOMErrorHandler.h
\brief Provides dom::DOMErrorHandler.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/DOMError.h"

namespace dom
{
/*! \class dom::DOMErrorHandler DOMErrorHandler.h DOMErrorHandler.h
\brief Error Handler

DOMErrorHandler is a callback interface that the DOM implementation can call when reporting errors that happens while processing XML data, or when doing some other processing (e.g. validating a document).
A DOMErrorHandler object can be attached to a Document using the "error-handler" on the DOMConfiguration interface.
If more than one error needs to be reported during an operation, the sequence and numbers of the errors passed to the error handler are implementation dependent.

The application that is using the DOM implementation is expected to implement this interface.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p105]
\note Introduced in DOM Level 3.
*/
    class DOMErrorHandler
    {
    public:
        virtual ~DOMErrorHandler(void) { }
        DOMErrorHandler(void) { }

/*! \brief Handle an error.

This method is called on the error handler when an error occurs.

If an exception is thrown from this method, it is considered to be equivalent of returning true.

\param error The error object that describes the error. This object may be reused by the DOM implementation across multiple calls to the handleError() method.
\return If the handleError method returns false, the DOM implementation should stop the current processing when possible.
If the method returns true, the processing may continue depending on DOMError::severity.
*/
        virtual bool handleError(DOMError error) = 0;

    };

}
