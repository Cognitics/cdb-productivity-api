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
/*! \file dom/DOMError.h
\headerfile dom/DOMError.h
\brief Provides dom::DOMError.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/base.h"

// #define in WinError.h
#undef SEVERITY_ERROR

namespace dom
{
/*! \class dom::DOMError DOMError.h DOMError.h
\brief Error

DOMError is an interface that describes an error.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p104]
\note Introduced in DOM Level 3.
*/
    class DOMError
    {
    private:
        struct _DOMError;
        _DOMError *_data;

    public:
        virtual ~DOMError(void);
        DOMError(void);

        static const unsigned short SEVERITY_WARNING        = 1;
        static const unsigned short SEVERITY_ERROR            = 2;
        static const unsigned short SEVERITY_FATAL_ERROR    = 3;

/*! \brief Get the severity of the error.

The following severity indicators may be returned:
\li SEVERITY_ERROR: The severity of the error described by the DOMError is error.
A SEVERITY_ERROR may not cause the processing to stop if the error can be recovered, unless DOMErrorHandler::handleError() returns false.
\li SEVERITY_FATAL_ERROR: The severity of the error described by the DOMError is fatal error.
A SEVERITY_FATAL_ERROR will cause the normal processing to stop.
The return value of DOMErrorHandler::handleError() is ignored unless the implementation chooses to continue, in which case the behavior becomes undefined.
\li SEVERITY_WARNING: The severity of the error described by the DOMError is warning.
A SEVERITY_WARNING will not cause the processing to stop, unless DOMErrorHandler::handleError() returns false.

\return The severity of the error.
*/
        virtual unsigned short getSeverity(void);
        virtual void setSeverity(unsigned short severity);

/*! \brief Get a string describing the error that occurred.
\return A string describing the error that occurred.
*/
        virtual DOMString getMessage(void);
        virtual void setMessage(const DOMString &message);

/*! \brief Get which related data is expected from getRelatedData().

Users should refer to the specification of the error in order to find its DOMString type and relatedData definitions if any.

\note As an example, Document::normalizeDocument() does generate warnings when the "split-cdata-sections" parameter is in use.
Therefore, the method generates a SEVERITY_WARNING with type "cdata-sections-splitted" and the first CDATASection node in document order resulting from the split is returned by the relatedData attribute.

\return Which related data is expected from getRelatedData().
*/
        virtual DOMString getType(void);
        virtual void setType(const DOMString &type);

/*! \brief Get the related exception if any.
\return The related exception or empty if there is none.
*/
        virtual DOMObjectSP getRelatedException(void);
        virtual void setRelatedException(DOMObjectSP relatedException);

/*! \brief Get the related type dependent data if any.
\return The related type dependent data or empty if there is none.
*/
        virtual DOMObjectSP getRelatedData(void);
        virtual void setRelatedData(DOMObjectSP relatedData);

/*! \brief Get the location of the error.
\return The location of the error.
*/
        virtual DOMLocatorSP getLocation(void);
        virtual void setLocation(DOMLocatorSP location);

    };

}
