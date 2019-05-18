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
/*! \page ccl_page Cognitics Core Library (CCL)

\section Description

The Cognitics Core Library provides a set of definitions, types, and tools for simplifying development in a cross-platform environment.

The library consists of the following components:
\li \ref cstdint_page
\li \ref utf_page
\li \ref binary_page
\li \ref endian_page
\li \ref bitfield_page
\li \ref bindstream_page
\li \ref log_page
\li \ref md5_page
\li \ref timer_page

\section Usage

\code
#include <ccl.h>
\endcode

*/
#pragma once

#define COGNITICS_CCL_VERSION 1.0

#include "cstdint.h"
#include "uuid.h"
#include "datetime.h"
#include "binary.h"
#include "LittleEndian.h"
#include "BigEndian.h"
#include "LSBitField.h"
#include "MSBitField.h"
#include "BindStream.h"
#include "Number.h"
#include "Variant.h"
#include "Op.h"
#include "Key.h"
#include "Value.h"
#include "Action.h"
#include "LogStream.h"
#include "ObjLog.h"
#include "md5.h"
#include "Timer.h"
#include "FileInfo.h"
#include "inspection.h"

//! \namespace ccl Cognitics Core Library
namespace ccl { }

