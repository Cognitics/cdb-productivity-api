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
/* \brief Provides sfa::Distance3D.
\author Joshua Anghel <janghel@cognitics.net>
\date 1 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Distance.h"

namespace sfa {

/*! \class sfa::Distance3D Distance3D.h Distance3D.h
\brief Distance3D

Provides a distance operation in full 3D coordinates.

Inherits the geometry handling from Distance, but overloads the elementary distance operations: Point-Point, Point-Line,
and Line-Line to allow for full 3D comparisions.
*/
    class Distance3D : public Distance
    {
    public:
//! Finds the comparable distance between two Points using x, y, and z
        virtual double comparablePointPoint(const Point* a, const Point* b);
//! Finds the comparable distance between a Point and Line using x, y, and z
        virtual double comparablePointLine(const Point* a, const Point* p1, const Point* p2);
//! Finds the comparable distance between two Lines using x, y, and z
        virtual double comparableLineLine(const Point* a, const Point* b, const Point* c, const Point* d);
//!    Finds the comparable distance between a Point and a Polygon.
        virtual double comparablePointPolygon(const Point* a, const Polygon* polygon);

        Distance3D(void) {}
        ~Distance3D(void) {}
    };

}