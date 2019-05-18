/*************************************************************************
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
/*! \page cts_page Cognitics Coordinate Transformation Services (CTS)

\section Description

This is the Cognitics implementation of the OpenGIS Coordinate Transformation Services (CTS).

It includes:
- PT: Positioning
- CS: Coordinate Systems
- CT: Coordinate Transformations

\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00)

\section Usage

\section Notes

*/
#pragma once

#define COGNITICS_CTS_VERSION 1.0

// PT
#include "pt.h"

// CS
#include "CS_CoordinateSystemFactory.h"
#include "CS_CoordinateSystemAuthorityFactory.h"

// CT
#include "CT_CoordinateTransformationAuthorityFactory.h"
#include "CT_CoordinateTransformationFactory.h"
#include "CT_MathTransformFactory.h"

// Cognitics
#include "ProjInfo.h"
#include "WGS84Orthographic.h"
#include "WGS84ToOrthographicMathTransform.h"
#include "WGS84FromOrthographicMathTransform.h"
#include "FlatEarthProjection.h"
#include "WGS84ToFlatEarthMathTransform.h"
#include "WGS84FromFlatEarthMathTransform.h"

//! \namespace cts Cognitics Coordinate Transformation Services
namespace cts { }

