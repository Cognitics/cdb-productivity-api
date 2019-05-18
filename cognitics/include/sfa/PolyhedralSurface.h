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
/*! \brief Provides sfa::PolyhedralSurface.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Surface.h"
#include "Polygon.h"
#include "MultiPolygon.h"

namespace sfa
{
    class PolyhedralSurface;
    typedef std::shared_ptr<PolyhedralSurface> PolyhedralSurfaceSP;

/*! \class sfa::PolyhedralSurface PolyhedralSurface.h PolyhedralSurface.h
\brief PolyhedralSurface

A PolyhedralSurface is a contiguous collection of polygons, which share common boundary segments.
For each pair of polygons that �touch�, the common boundary shall be expressible as a finite collection of LineStrings.
Each such LineString shall be part of the boundary of at most 2 Polygon patches.
A TIN (triangulated irregular network) is a PolyhedralSurface consisting only of Triangle patches.

For any two polygons that share a common boundary, the �top� of the polygon shall be consistent.
This means that when two LinearRings from these two Polygons traverse the common boundary segment, they do so in opposite directions.
Since the Polyhedral surface is contiguous, all polygons will be thus consistently oriented.
This means that a non-oriented surface (such as M�bius band) shall not have single surface representations.
They may be represented by a MultiSurface.

If each such LineString is the boundary of exactly 2 Polygon patches, then the PolyhedralSurface is a simple, closed polyhedron and is topologically isomorphic to the surface of a sphere.
By the Jordan Surface Theorem (Jordan�s Theorem for 2-spheres), such polyhedrons enclose a solid topologically isomorphic to the interior of a sphere; the ball.
In this case, the �top� of the surface will either point inward or outward of the enclosed finite solid.
If outward, the surface is the exterior boundary of the enclosed surface.
If inward, the surface is the interior of the infinite complement of the enclosed solid.
A Ball with some number of voids (holes) inside can thus be presented as one exterior boundary shell, and some number in interior boundary shells.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.12
*/
    class PolyhedralSurface : public Surface
    {
    protected:
        PolygonList patches;

    public:
        virtual ~PolyhedralSurface(void);
        PolyhedralSurface(void);
        PolyhedralSurface(const PolyhedralSurface& surface);
        PolyhedralSurface(const PolyhedralSurface* surface);
        PolyhedralSurface& operator=(const PolyhedralSurface& rhs);

        // accessors
        virtual void clearPatches(void);
//!    Takes ownership of the Polygon
        virtual void addPatch(Polygon* patch);
//!    Copies Polygon.
        virtual void addPatch(const Polygon& patch);
//!    Takes ownership of the Polygon
        virtual void insertPatch(int pos, Polygon* patch);
//!    Copies Polygon.
        virtual void insertPatch(int pos, const Polygon& patch);
        virtual void removePatch(Polygon* patch);
        virtual void removePatch(int pos);

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

        // OGC PolyhedralSurface
        virtual int getNumPatches(void) const;
//!    Never delete returned Polygon.
        virtual Polygon* getPatchN(int n) const;
        virtual MultiPolygon* getBoundingPolygons(const Polygon* p) const;
        virtual bool isClosed(void) const;

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

    };

}
