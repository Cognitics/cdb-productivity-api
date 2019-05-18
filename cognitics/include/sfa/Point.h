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
/*! \brief Provides sfa::Point.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"
#include "Matrix.h"

namespace sfa
{
    class Quat;
    class Matrix;
    class Point;
    typedef std::shared_ptr<Point> PointSP;
    typedef std::vector<Point*> PointList;

/*! \class sfa::Point Point.h Point.h
\brief Point

A Point is a 0-dimensional geometric object and represents a single location in coordinate space.
A Point has an x-coordinate value, a y-coordinate value.
If called for by the associated Spatial Reference System, it may also have coordinate values for z and m.

The boundary of a Point is the empty set.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.4
*/
    class Point : public Geometry
    {
    private:
        double x;
        double y;
        double z;
        double m;
        bool hasZ;
        bool hasM;

    public:
        virtual ~Point(void);
        Point(void);
        Point(double x, double y);
        Point(double x, double y, double z);
        Point(double x, double y, double z, double m);
        Point(const Point& other);
        Point(const Point* other);

        // accessors
        double X(void) const;
        double Y(void) const;
        double Z(void) const;
        double M(void) const;
        void setX(double x);
        void setY(double y);
        void setZ(double z);
        void setM(double m);

        Point &operator=(const Point &p);
        bool operator==(const Point &p) const;
        bool operator!=(const Point &p) const;
        bool operator<(const Point& p) const;
        Point &operator+=(const Point &p);
        Point &operator-=(const Point &p);
        Point &operator*=(double value);
        Point &operator/=(double value);
        Point operator+(const Point &p) const;
        Point operator-(const Point &p) const;
        Point operator-(void) const;
        Point operator*(double value) const;
        Point operator/(double value) const;

/*! \brief Clear the Point's Z-value.

Calling this method effectively flattens the Point, marking it 2D rather than 3D.

\note This is an extension to the specification.
*/
        void clearZ(void);

/*! \brief Clear the Point's M-value.

Calling this method removes the measured value from the Point.

\note This is an extension to the specification.
*/
        void clearM(void);

        //! Zero the point.
        void zero(void);

        //! Distance from origin [0,0(,0)].
        double length(void) const;
        //!    Distance squared from origin (dot product with itself).
        double length2(void) const;
        //! Distance from origin in 2D only
        double length2D(void) const;

        //! Returns the nearest of two points.
        Point* nearest(Point* p1, Point* p2) const;

        virtual void setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform = NULL);
        virtual int getDimension(void) const;
        virtual std::string getGeometryType(void) const;
        virtual Geometry* getEnvelope(void) const;
        virtual bool isEmpty(void) const;
        virtual bool isSimple(void) const;
        virtual bool is3D(void) const;
        virtual bool isMeasured(void) const;
        virtual Geometry* getBoundary(void) const;

        virtual bool equals(const Geometry* another) const;
        virtual bool equals3D(const Geometry* another) const;
        virtual Geometry* locateBetween(double mStart, double mEnd) const;

        virtual Geometry* convexHull(void) const;
        virtual Geometry* convexHull3D(void) const;

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

        double dot(const Point &other) const;
        double dot2D(const Point& other) const;
        Point cross(const Point &other) const;
        double normalize();
        double distance2D2(const Point &other) const;

        void transform(const sfa::Matrix &xform);
    };

}
