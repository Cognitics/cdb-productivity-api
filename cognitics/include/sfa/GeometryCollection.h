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
/*! \brief Provides sfa::GeometryCollection.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"

namespace sfa
{
    class GeometryCollection;
    typedef std::shared_ptr<GeometryCollection> GeometryCollectionSP;

/*! \class sfa::GeometryCollection GeometryCollection.h GeometryCollection.h
\brief GeometryCollection

A GeometryCollection is a geometric object that is a collection of some number of geometric objects.

All the elements in a GeometryCollection shall be in the same Spatial Reference System.
This is also the Spatial Reference System for the GeometryCollection.

GeometryCollection places no other constraints on its elements.
Subclasses of GeometryCollection may restrict membership based on dimension and may also place other constraints on the degree of spatial overlap between elements.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.3
*/
    class GeometryCollection : public Geometry
    {
    protected:
        GeometryList geometries;

    public:
        virtual ~GeometryCollection(void);
        GeometryCollection(void);
        GeometryCollection(const GeometryCollection& collection);
        GeometryCollection(const GeometryCollection* collection);
        GeometryCollection& operator=(const GeometryCollection& rhs);

        virtual void setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform = NULL);
        virtual int getDimension(void) const;
        virtual std::string getGeometryType(void) const;
        virtual Geometry* getEnvelope(void) const;
        virtual bool isEmpty(void) const;
        virtual bool isSimple(void) const;
        virtual bool is3D(void) const;
        virtual bool isMeasured(void) const;
        virtual Geometry* getBoundary(void) const;

// Analysis
        virtual Geometry* convexHull(void) const;
        virtual Geometry* convexHull3D(void) const;

//! Get the number of geometries in this GeometryCollection.
        virtual int getNumGeometries(void) const;

//! Get the Nth geometry in this GeometryCollection. Never delete this Geometry.
        virtual Geometry* getGeometryN(int n) const;

        virtual void clearGeometries(void);
//!    Takes ownership of the Geometry.
        virtual void addGeometry(Geometry* geometry);
//!    Takes ownership of the Geometry.
        virtual void insertGeometry(int pos, Geometry* geometry);
        virtual void removeGeometry(Geometry* geometry);
        virtual void removeGeometry(int pos);

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
