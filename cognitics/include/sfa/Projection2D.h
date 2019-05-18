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
/*! \brief Provides sfa::Projection2D
\author Josh Anghel <janghel@cognitics.net>
\date 8 September 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Point.h"
#include "LineString.h"
#include "Polygon.h"
#include "PolyhedralSurface.h"
#include "GeometryCollection.h"
#include "Quat.h"
#include "Matrix.h"

namespace sfa {

/*!\class sfa::Projection2D Projection2D.h Projection2D.h
\brief Projection2D

Creates and maintains an affine transformation. This allows 2D geometries in 3D space to be represented as purely 2D geometries 
while maintaining any analytical results found.

Any operation performed on a geometry after it is transformed to a new coordinate system will be valid. If it is a geometric 
operation (ie intersection, union, etc) any Geometry resulting from that operation can be transformed back to yeild the correct
result in the original coordinate system.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class Projection2D
    {
    protected:
        Point        origin;
        Point        e1;
        Point        e2;
        Point        norm;

    public:

/*!\brief createBasis
Constructs and stores an orthonormal basis in the Projection2D object for transformations.
The Points cannot intersect and cannot be collinear.
\param p1 First Point
\param p2 Second Point
\param p3 Third Point
*/
        void createBasis(const Point* p1, const Point* p2, const Point* p3);
        void createBasis(const Point* origin, const Point* normal);
        /*!\brief createBasis
Constructs and stores an orthonormal basis in the Projection2D object for transformations.
The Points cannot intersect and cannot be collinear.
\param polygon
\return True if a basis can be created
*/
        bool createBasis(const Polygon* polygon);

/*!\brief Construcst a new Projection2D with an undefined basis. You must call createBasis before calling any other methods.*/
        Projection2D() {}

/*!\brief Construcst a new Projection2D with a basis represented by the three points.*/
        Projection2D(const Point* p1, const Point* p2, const Point* p3);
        Projection2D(const Point* origin, const Point* normal);

/*!\brief Constuct a new Projection2D with a basis represented by three non-collinear points of the Polygon given.*/
        Projection2D(const Polygon* poly);

        ~Projection2D(void) { }

/*!\brief transformPointTo2D
Transforms a 3D
\param p Point to transform
\return A The transformed Point
*/
        Point transformPointTo2D(const Point& p) const;
        Point* transformPointTo2D(const Point* p) const;
        Point transformPointTo3D(const Point& p) const;
        Point* transformPointTo3D(const Point* p) const;

/*!\brief transformLineTo2D
Transforms a 3D LineString to 2D by itterating a basis transformation over all the points.
\param l LineString to transform
\return The transformed LineString
*/
        LineString* transformLineTo2D(const LineString* l) const;
        LineString* transformLineTo3D(const LineString* l) const;

/*!\brief transformPolygonTo2D
Transforms a 3D Polygon to 2D by itterating a basis transformation over all the LineStrings
\param poly Polygon to transform
\return The transformed Polygon
*/
        Polygon* transformPolygonTo2D(const Polygon* poly) const;
        Polygon* transformPolygonTo3D(const Polygon* poly) const;

        GeometryCollection* transformCollectionTo2D(const GeometryCollection* geometries) const;
        GeometryCollection* transformCollectionTo3D(const GeometryCollection* geometries) const;

        PolyhedralSurface* transformSurfaceTo2D(const PolyhedralSurface* surface) const;
        PolyhedralSurface* transformSurfaceTo3D(const PolyhedralSurface* surface) const;

        Geometry* transformGeometryTo2D(const Geometry* geometry) const;
        Geometry* transformGeometryTo3D(const Geometry* geometry) const;

        const Point* getOrigin(void) const;
        void setOrigin(const Point& o);
        const Point* getNormal(void) const;
        void setNormal(const Point& n);
        const Point* getBasisVector(int i) const;
        void setBasisVector(const Point& v, int i);

        void applyRotation(const Quat& quat);

        Matrix getMatrix(void) const;

/*!    \brief Scales the 3D projection. This is equivalent to a pre-projection translate. */
        void scale2D(double x, double y);

/*!    \brief Scales the 3D projection. This is equivalent to a post-projection translate. */
        void scale3D(double x, double y, double z);

/*!    \brief Translates the origin of the 2D projection. This is equivalent to a pre-projection translate. */
        void trans2D(double x, double y);

/*!    Translates the origin of the 3D projection. This is equivalent to a post-projection translate. */
        void trans3D(double x, double y, double z);
/*!    \brief Invert the basis so that one looks at the other side of the plane. Effectively flips all orientations. */
        void invert(void);
    };

}