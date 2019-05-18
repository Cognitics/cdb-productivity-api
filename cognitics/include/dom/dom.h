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
/*! \page dom_page Cognitics Document Object Model Implementation (DOM)

\section Description

\section Usage

\section Notes

*/
#pragma once

#define COGNITICS_DOM_VERSION 1.0

// Core 3.0
#include "dom/base.h"
#include "dom/DOMException.h"
#include "dom/DOMImplementation.h"
#include "dom/DOMConfiguration.h"
#include "dom/Node.h"
#include "dom/DocumentFragment.h"
#include "dom/Document.h"
#include "dom/Attr.h"
#include "dom/Element.h"
#include "dom/CharacterData.h"
#include "dom/Comment.h"
#include "dom/Text.h"
#include "dom/TypeInfo.h"
#include "dom/DOMLocator.h"
#include "dom/DOMError.h"
#include "dom/DOMErrorHandler.h"
#include "dom/UserDataHandler.h"

// XML 3.0
#include "dom/DocumentType.h"
#include "dom/CDATASection.h"
#include "dom/Notation.h"
#include "dom/Entity.h"
#include "dom/EntityReference.h"
#include "dom/ProcessingInstruction.h"

// Traversal 2.0
#include "dom/NodeIterator.h"
#include "dom/NodeFilter.h"
#include "dom/TreeWalker.h"

// Range 2.0
#include "dom/RangeException.h"
#include "dom/Range.h"

// LS 3.0
#include "dom/LSException.h"
#include "dom/LSParser.h"
#include "dom/LSInput.h"
#include "dom/LSSerializer.h"
#include "dom/LSOutput.h"
#include "dom/LSParserFilter.h"
#include "dom/LSSerializerFilter.h"
//#include "dom/LSParserEvent.h"    // p28, extends DOM3 Events
//#include "dom/LSLoadEvent.h"        // p28, extends DOM3 Events

//! \namespace dom Cognitics Document Object Model
namespace dom { }
