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
/*! \brief Provides sfa::Polygon.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Surface.h"
#include "MultiLineString.h"
#include "Matrix.h"
#include <list>

namespace sfa
{
    class Polygon;
    typedef std::shared_ptr<Polygon> PolygonSP;
    typedef std::vector<Polygon*> PolygonList;

/*! \class sfa::Polygon Polygon.h Polygon.h
\brief Polygon

A Polygon is a planar Surface defined by 1 exterior boundary and 0 or more interior boundaries.
Each interior boundary defines a hole in the Polygon.
A Triangle is a polygon with 3 distinct, non-collinear vertices and no interior boundary.

The exterior boundary LinearRing defines the �top� of the surface which is the side of the surface from which the exterior boundary appears to traverse the boundary in a counter clockwise direction.
The interior LinearRings will have the opposite orientation, and appear as clockwise when viewed from the �top�.

The assertions for Polygons (the rules that define valid Polygons) are as follows:
a) Polygons are topologically closed.
b) The boundary of a Polygon consists of a set of LinearRings that make up its exterior and interior boundaries.
c) No two Rings in the boundary cross and the Rings in the boundary of a Polygon may intersect at a Point but only as a tangent.
d) A Polygon may not have cut lines, spikes or punctures.
e) The interior of every Polygon is a connected point set.
f) The exterior of a Polygon with 1 or more holes is not connected.
Each hole defines a connected component of the exterior.

In the above assertions, interior, closure and exterior have the standard topological definitions.
The combination of (a) and (c) makes a Polygon a regular closed Point set.
Polygons are simple geometric objects.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.11
*/
    class Polygon : public Surface
    {
    protected:
        LineStringList rings;

    public:
        virtual ~Polygon(void);
        Polygon(void);
        Polygon(const Polygon& polygon);
        Polygon(const Polygon* polygon);
        Polygon& operator=(const Polygon& rhs);

        // accessors
        virtual void clearRings(void);
//!    Takes ownership of LineString.
        virtual void addRing(LineString* ring);
//!    Copies LineString.
        virtual void addRing(const LineString& ring);
//!Takes ownership of LineString.
        virtual void insertRing(int pos, LineString* ring);
//!    Copies LineString.
        virtual void insertRing(int pos, const LineString& ring);
//!    Copies LineString
        virtual void insertRing(int pos, LineStringSP ring);
        virtual void removeRing(LineString* ring);
        virtual void removeRing(int pos);

//!    Reverse the orientation of all rings in this Polygon
        virtual void reverse(void);

        // OGC Geometry
        virtual void setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform = NULL);
        virtual std::string getGeometryType(void) const;
        virtual Geometry* getEnvelope(void) const;
        virtual bool isEmpty(void) const;
        virtual bool isSimple(void) const;
        virtual bool is3D(void) const;
        virtual bool isMeasured(void) const;
        virtual Geometry* getBoundary(void) const;

        virtual Geometry* convexHull(void) const;
        virtual Geometry* convexHull3D(void) const;

        // OGC Surface
        virtual double getArea(void) const;
        virtual double getArea3D(void) const;
        virtual Point* getCentroid(void) const;
        virtual Point* getCentroid3D(void) const;
        virtual Point* getPointOnSurface(void) const;

        // OGC Polygon
//!    Never delete this LineString.
        virtual LineString* getExteriorRing(void) const;
        virtual int getNumInteriorRing(void) const;
//!    Never delete this LineString.
        virtual LineString* getInteriorRingN(int n) const;

        // extensions
        virtual bool isValid(void) const;
        virtual WKBGeometryType getWKBGeometryType(bool withZ = false, bool withM = false) const;
        virtual void toText(std::ostream &os, bool tag, bool withZ, bool withM) const;
        virtual void fromText(std::istream &is, bool tag, bool withZ, bool withM);
        virtual void toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const;
        virtual void fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM);
        virtual bool isParentOf(const GeometryBase *child) const;
        virtual GeometryBase *getParentOf(const GeometryBase *child) const;
        virtual int getNumChildren(void) const;
        //! Remove extra points and line segments that double back
        void clean(double minDistance2=1e-4);

        //! Close the rings if they are not already closed (i.e. make sure the first and last vertex are the same)
        //! Does nothing if the first and last point are the same, or the distance squared is less than SFA_EPSILON
        void close();

        void transform(const sfa::Matrix &xform);

        std::list<sfa::LineString *> getRingList(void);
    };

}
