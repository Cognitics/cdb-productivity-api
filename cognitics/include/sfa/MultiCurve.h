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
/*! \brief Provides sfa::MultiCurve.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "GeometryCollection.h"

namespace sfa
{
    class MultiCurve;
    typedef std::shared_ptr<MultiCurve> MultiCurveSP;

/*! \class sfa::MultiCurve MultiCurve.h MultiCurve.h
\brief MultiCurve

A MultiCurve is a 1-dimensional GeometryCollection whose elements are Curves.

MultiCurve is a non-instantiable class in this specification; it defines a set of methods for its subclasses and is included for reasons of extensibility.

A MultiCurve is simple if and only if all of its elements are simple and the only intersections between any two elements occur at Points that are on the boundaries of both elements.

The boundary of a MultiCurve is obtained by applying the �mod 2� union rule: A Point is in the boundary of a MultiCurve if it is in the boundaries of an odd number of elements of the MultiCurve. 

A MultiCurve is closed if all of its elements are closed.
The boundary of a closed MultiCurve is always empty.

A MultiCurve is defined as topologically closed.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.7
*/
    class MultiCurve : public GeometryCollection
    {
    public:
        virtual ~MultiCurve(void);
        MultiCurve(void);
        MultiCurve(const MultiCurve& curves);
        MultiCurve(const MultiCurve* curves);
        MultiCurve& operator=(const MultiCurve& rhs);

        virtual int getDimension(void);
        virtual Geometry* getBoundary(void);

        virtual bool isClosed(void);
        virtual double getLength(void);

    };

}
