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
/* \brief Provides sfa::Distance.
\author Joshua Anghel <janghel@cognitics.net>
\date 1 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Point.h"
#include "LineString.h"
#include "Polygon.h"
#include "PolyhedralSurface.h"
#include "GeometryCollection.h"

namespace sfa {

/*!\class sfa::Distance Distance.h Distance.h
\brief Distance

Uses comparable distances to find the closest distance between two Geometries. A comparable distance is defined as the
square of the actual distance. If a < b then sqrt(a)) < sqrt(b), so a comparable distance will do in order
to find the shortest distance, and then the sqrt can be computed. This saves the time of continually computing the sqrt
needlessly.

Currently does not support distance about a great sphere. SRF detection must be tied into the sfa first in order to account
for great sphere distances more accuratley. Currently, only linear interpolation in cartesian reference frames is used.

Note: If the distance at one point ever drops to below SFA_EPSILON, 0 is returned immediately, as the Geometries could not
be any closer (from the perspective of the algorithms in the sfa since they view SFA_EPSILON as 0.
*/
    class Distance
    {
    public:

/*! \brief Returns the comparable distance between Points a and b using only x and y.
\param a Point a.
\param b Point b.
\return The comparable distance between a and b.
*/
        virtual double comparablePointPoint(const Point* a, const Point* b);

/*! \brief Returns the comparable distance between a Point and a Line using only x and y.
\param a Point a.
\param p1 First Point of the Line.
\param p2 Second Point of the Line.
\return The shortest comparable distance between point a and the Line p1-p2.
*/
        virtual double comparablePointLine(const Point* a, const Point* p1, const Point* p2);

/*! \brief Returns the comparable distance between two Lines. This may in some cases be shorter than the distance
between the end points and the other Line.

A simple approach is to compute the point-line distance 4 times for each end point, but this can be slow, so we use
a slightly more advanced method.

Note:: Uses SFA_EPSILON in comparisons.
\param a First Point of first Line.
\param b Second Point of first Line.
\param c First Point of second Line.
\param d Second Point of second Line.
\return The shorest comparable distance between the two Lines.
*/
        virtual double comparableLineLine(const Point* a, const Point* b, const Point* c, const Point* d);

        virtual double comparablePointLineString(const Point* a, const LineString* other);
        virtual double comparablePointPolygon(const Point* a, const Polygon* other);
        virtual double comparablePointPolyhedralSurface(const Point* a, const PolyhedralSurface* other);
        virtual double comparablePointCollection(const Point* a, const GeometryCollection* other);
        virtual double comparablePoint(const Point* point, const Geometry* other);

        virtual double comparableLineLineString(const Point* a, const Point* b, const LineString* other);
        virtual double comparableLinePolygon(const Point* a, const Point* b, const Polygon* other);
        virtual double comparableLinePolyhedralSurface(const Point* a, const Point* b, const PolyhedralSurface* other);
        virtual double comparableLineCollection(const Point* a, const Point* b, const GeometryCollection* other);
        virtual double comparableLine(const Point* a, const Point* b, const Geometry* other);

        virtual double comparableLineString(const LineString* line, const Geometry* other);
        virtual double comparablePolygon(const Polygon* polygon, const Geometry* other);
        virtual double comparablePolyhedralSurface(const PolyhedralSurface* surface, const Geometry* other);
        virtual double comparableCollection(const GeometryCollection* collection, const Geometry* other);
    
        Distance(void) {}
        ~Distance(void) {}

/*!\fn Distance::apply(GeometrySP a, GeometrySP b)
\brief Returns the shortest distance between a and b

Finds the shortest distance between a and b by using comaprable distances. This saves computation time as the sqrt operation can be costly when
performed so many times needlessly.

Assumes linear interpolation.
\param a First Geometry
\param b Second Geometry
\return Returns the shortest distance between a and b
*/
        virtual double apply(const Geometry* a, const Geometry* b);
    };

}