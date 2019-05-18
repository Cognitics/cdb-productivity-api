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
/*! \brief Provides dom::LSOutput.
\author Aaron Brinton <abrinton@cognitics.net>
\date 12 November 2009
\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \brief LS Output

This interface represents an output destination for data.

This interface allows an application to encapsulate information about an output destination in a single object, which may include a URI, a byte stream (possibly with a specified encoding), a base URI, and/or a character stream.

The exact definitions of a byte stream and a character stream are binding dependent.

The application is expected to provide objects that implement this interface whenever such objects are needed.
The application can either provide its own objects that implement this interface, or it can use the generic factory method DOMImplementationLS.createLSOutput() to create objects that implement this interface.

The LSSerializer will use the LSOutput object to determine where to serialize the output to.
The LSSerializer will look at the different outputs specified in the LSOutput in the following order to know which one to output to, the first one that is not null and not an empty string will be used:
-# LSOutput.characterStream
-# LSOutput.byteStream
-# LSOutput.systemId

LSOutput objects belong to the application.
The DOM implementation will never modify them (though it may make copies and modify the copies, if necessary).

\sa Document Object Model (DOM) Level 3 Load and Save Specification (http://www.w3.org/TR/DOM-Level-3-LS/) 1.3 [p36]
*/
    class LSOutput : public DOMObject, public boost::noncopyable
    {
    public:
        LSWriter *characterStream;
        LSOutputStream *byteStream;
        DOMString systemId;
        DOMString encoding;

        virtual ~LSOutput(void);
        LSOutput(void);

        bool write(const DOMString &text);

    };

}
