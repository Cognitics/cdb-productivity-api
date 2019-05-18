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
/*! \brief Provides sfa::Curve.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"
#include "Point.h"

namespace sfa
{
    class Curve;
    typedef std::shared_ptr<Curve> CurveSP;

/*! \class sfa::Curve Curve.h Curve.h
\brief Curve

A Curve is a 1-dimensional geometric object usually stored as a sequence of Points, with the subtype of Curve specifying the form of the interpolation between Points.
This specification defines only one subclass of Curve, LineString, which uses linear interpolation between Points.

A Curve is simple if it does not pass through the same Point twice with the possible exception of the two end points.

A Curve is closed if its start Point is equal to its end Point.

The boundary of a closed Curve is empty.

A Curve that is simple and closed is a Ring.

The boundary of a non-closed Curve consists of its two end Points.

A Curve is defined as topologically closed, that is, it contains its endpoints.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.6
*/
    class Curve : public Geometry
    {
    protected:
        PointList points;

    public:
        virtual ~Curve(void);
        Curve(void);
        Curve(const Curve& curve);
        Curve(const Curve* curve);
        Curve& operator=(const Curve& rhs);

        // accessors
        virtual void clearPoints(void);
//!    Takes ownership of the Point.
        virtual void addPoint(Point* point);
//!    Copies the Point.
        virtual void addPoint(const Point& point);
//!    Copies the Point.

        /**
         * @fn    virtual void Curve::insertPoint(int pos, Point* point);
         *
         * @brief    Inserts a point at the specified position.
         *
         * @param    pos                 The position in the curve that the new point will be stored at.
         * @param [in,out]    point    The point. The curve takes ownership of the point.
         */
        virtual void insertPoint(int pos, Point* point);
/**
         * @fn    virtual void Curve::insertPoint(int pos, Point* point);
         *
         * @brief    Inserts a point at the specified position.
         *
         * @param    pos                 The position in the curve that the new point will be stored at.
         * @param [in,out]    point    The point. A copy is made and inserted into the curve.
         */
        virtual void insertPoint(int pos, const Point& point);
//!    Copies the Point.
        virtual void removePoint(Point* point);
        virtual void removePoint(int pos);

//!    Reverse the orientation of this ring.
        virtual void reverse(void);

        // OGC Geometry
        virtual void setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform = NULL);
        virtual int getDimension(void) const;
        virtual Geometry* getEnvelope(void) const;
        virtual bool isEmpty(void) const;
        virtual bool isSimple(void) const;
        virtual bool is3D(void) const;
        virtual bool isMeasured(void) const;
        virtual Geometry* getBoundary(void) const;

        virtual Geometry* locateBetween(double mStart, double mEnd) const;

        // OGC Curve
        virtual double getLength(void) const;
//!    Returns pointer owned by this Curve. Never delete the returned Point.
        virtual Point* getStartPoint(void) const;
//!    Returns pointer owned by this Curve. Never delete the returned Point.
        virtual Point* getEndPoint(void) const;
        virtual bool isClosed(void) const;
        virtual bool isRing(void) const;

    };

}
