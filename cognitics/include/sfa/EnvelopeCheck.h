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
/*! \brief Provides sfa::EnvelopeCheck
\author Josh Anghel <janghel@cognitics.net>
\date 2 September 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Geometry.h"

namespace sfa {

/*!\class EnvelopeCheck EnvelopeCheck.h EnvelopeCheck.h
\brief EnvelopeCheck

Provides a test to check if the envelopes of two Geometries intersect.
Uses SFA_EPSILON in comparisons.

Usage:
\code
    GeometrySP a, b;
    bool result;

//    To check if    the Geometries envelopes intersect in the xy plane only
    result = sfa::EnvelopeCheck::apply(a,b);

//    To check if the Geometries envelopes intersect in full 3D xyz space use
    result = sfa::EnvelopeCheck::apply3D(a,b);
\endcode
*/
    class EnvelopeCheck
    {
    protected:
        EnvelopeCheck(void){}
        ~EnvelopeCheck(void){}

    public:

//!\brief Envelope intersection test using x and y
        static bool apply(const Geometry* a, const Geometry* b);
//!\brief Envelope intersection test using x, y, and z
        static bool apply3D(const Geometry* a, const Geometry* b);
    };

}