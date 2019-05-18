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
/*! \brief Provides sfa::Relate.
\author Josh Anghel <janghel@cognitics.net>
\date 4 November 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Geometry.h"
#include "de9im.h"
#include "RelateOperation.h"

namespace sfa {

/*! \class sfa::Relate Relate.h Relate.h
\brief Relate

Acts as an optimized wrapper to create and interpret a de9im matrix. If an optimized algorithm (Point equals, intersects, etc)
cannot be found, an intersection matrix will be created and stored using the created RelateOperation. Note that a matrix will
only be created once (and it will only be created if it has no other method of checking for the relation) until the object is
destroyed. So if you wish to check for several spatial relationships between two geometries, create a Relate object from those
two Geometries and use it instead. If the Geometry relational query methods are used, it could result in the same intersection
matrix being created several times (which is the most computationally intensive part of the relational query system).

Note that this system is currently only defined for 2D relations. 3D relational matrix queries are currently being implemented
and will be tied into this class (along with an option for choosing 3D over 2D) soon.

Because of the dynamic optimized handling of the matrix, since it is only computed and stored when it is first needed, some calls
to the query functions may take much longer than previous calls, that is because the matrix needed to be constructed. Note that 
if the same method is called again, the time needed will be much lower, since the matrix has been stored in memory.

Usage:
\code
    sfa::GeometrySP a, b;
    bool result;

//    For 2D analysis
    Relate op(a,b);
//    Or for 3D analysis
    Relate op(a,b,true);

    result = op.equals();
    result = op.disjoint();
    result = op.intersects();
    result = op.touches();
    //...

    result = op.relate("F*T120*F");

//    The de9im matrix can be computed manually if a call has not yet triggered it's computation by
    op.computeIM();

//    And if you need to analyse or print the matrix yourself, simple call
    sfa::de9imSP myMatrix = op.getMatrix();
\endcode

When passing an argument to the Relate::relate method, the following syntax should be used to create a string of exactly 9 characters

    0    -    The highest dimensional intersection between the two regions is a Point
    1    -    The highest dimensional intersection between the two regions is a LineString
    2    -    The highest dimensional intersection between the two regions is a Polygon
    T    -    The highest dimensional intersection between the two regions is at least a Point (ie >= 0)
    F    -    The two regions are disjoint and do not intersect
    *    -    The dimensional intersection between the two regions is irrelivant and can be anything

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class Relate
    {
    protected:
        bool                _is3D;
        const Geometry*        A;
        const Geometry*        B;
        de9im                intersectionMatrix;
        bool                created;
        RelateOperation*    computer;

    public:
        Relate(const Geometry* a, const Geometry* b, bool is3D = false);
        ~Relate(void);

//!    \brief See Geometry::equals()
        bool equals(void);

//!    See Geometry::disjoint()
        bool disjoint(void);

//! See Geometry::intersects()
        bool intersects(void);

//! See Geometry::touches()
        bool touches(void);

//! See Geoemtry::crosses()
        bool crosses(void);

//! See Geometry::within()
        bool within(void);

//! See Geometry::contains()
        bool contains(void);

//! See Geometry::overlaps()
        bool overlaps(void);

//! See Geometry::relate()
        bool relate(const std::string &matrix);

//! Forces a (re-)computation of the intersection matrix if it is not already stored in memory
        void computeIM(void);

//! Returns the intersection matrix. Computes the matrix if it is not already created.
        de9im getMatrix();
    };

}