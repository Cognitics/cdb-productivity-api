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
/*! \file dom/UserDataHandler.h
\headerfile dom/UserDataHandler.h
\brief Provides dom::UserDataHandler.
\author Aaron Brinton <abrinton@cognitics.net>
\date 02 October 2009
\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4
*/
#pragma once

#include "dom/base.h"

namespace dom
{
/*! \class dom::UserDataHandler UserDataHandler.h UserDataHandler.h
\brief User Data Handler

When associating an object to a key on a node using Node::setUserData() the application can provide a handler that gets called when the node the object is associated to is being cloned, imported, or renamed.
This can be used by the application to implement various behaviors regarding the data it associates to the DOM nodes.
This interface defines that handler.

\sa Document Object Model (DOM) Level 3 Core Specification (http://www.w3.org/TR/DOM-Level-3-Core/) 1.4 [p99]
\note Introduced in DOM Level 3.
*/
    class UserDataHandler
    {
    public:
        virtual ~UserDataHandler(void) { }
        UserDataHandler(void) { }

        static const unsigned short NODE_CLONED        = 1;
        static const unsigned short NODE_IMPORTED    = 2;
        static const unsigned short NODE_DELETED    = 3;
        static const unsigned short NODE_RENAMED    = 4;
        static const unsigned short NODE_ADOPTED    = 5;

/*! \brief Called whenever the node for which this handler is registered is imported or cloned.

The following operations are defined:
\li NODE_ADOPTED: The node is adopted, using Document::adoptNode().
\li NODE_CLONED: The node is cloned, using Node::cloneNode().
\li NODE_DELETED: The node is deleted.
\note This may not be supported or may not be reliable in certain environments, such as Java, where the implementation has no real control over when objects are actually deleted.
\li NODE_IMPORTED: The node is imported, using Document::importNode().
\li NODE_RENAMED: The node is renamed, using Document::renameNode().

\warning DOM applications must not raise exceptions in a UserDataHandler. The effect of throwing exceptions from the handler is DOM implementation dependent.
\param operation The type of operation that is being performed on the node.
\param key The key for which this handler is being called.
\param data The data for which this handler is being called.
\param src The node being cloned, adopted, imported, or renamed. This is empty when the node is being deleted.
\param dst The node newly created if any, or empty.
*/
        virtual void handle(unsigned short operation, const DOMString &key, DOMUserDataSP data, NodeSP src, NodeSP dst) = 0;

    };

}
