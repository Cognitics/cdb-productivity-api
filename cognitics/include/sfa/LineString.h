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
/*! \brief Provides sfa::LineString.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Curve.h"

namespace sfa
{
    class LineString;
    typedef std::shared_ptr<LineString> LineStringSP;
    typedef std::vector<LineString*> LineStringList;

/*! \class sfa::LineString LineString.h LineString.h
\brief LineString

A LineString is a Curve with linear interpolation between Points.
Each consecutive pair of Points defines a Line segment.

A Line is a LineString with exactly 2 Points.

A LinearRing is a LineString that is both closed and simple.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.7
*/
    class LineString : public Curve
    {
    public:
        virtual ~LineString(void);
        LineString(void);
        LineString(const LineString& linestring);
        LineString(const LineString* linestring);
        LineString& operator=(const LineString& rhs);

        // OGC Geometry
        virtual std::string getGeometryType(void) const;
        virtual Geometry* convexHull(void) const;
        virtual Geometry* convexHull3D(void) const;

//! Get the number of Points in this LineString.
        virtual int getNumPoints(void) const;

//! Get the specified Point n in this LineString. Never delete this Point.
        virtual Point* getPointN(int n) const;

//! Get the length of the LineString using linear interpolation.
        virtual double getLength(void) const;

        virtual double getLength2D(void) const;

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

        //! Interpolates axis value(s) using an existing axis point.
        /*! This is using the parametric line equation:

            x = x1 + t(x2-x1)
            y = y1 + t(y2-y1)
            z = z1 + t(z2-z1)

            these return false if b.from - a.from is zero
        */
        bool interpolateXfromY(Point* p, int segment = 0) const;
        bool interpolateXfromZ(Point* p, int segment = 0) const;
        bool interpolateYfromX(Point* p, int segment = 0) const;
        bool interpolateYfromZ(Point* p, int segment = 0) const;
        bool interpolateZfromX(Point* p, int segment = 0) const;
        bool interpolateZfromY(Point* p, int segment = 0) const;
        bool interpolateX(Point *p, int segment = 0) const;
        bool interpolateY(Point *p, int segment = 0) const;
        bool interpolateZ(Point *p, int segment = 0) const;

        /*! Add colinear points so no segment is longer than maxDist
            \param maxDist the maximum distance between points
            \param tolerance if an existing point is maxDist + maxDist*tolerance of the previous point, don't insert another point
        */
        int addColinearPoints(double maxDist, double tolerance=0.0f);
        int addDeviatedColinearPoints(double mean, double stddev = 0.0f, double mindist = 1.0f);
        //! Remove colinear points that are more than epsilon distance from the line
        int removeColinearPoints(double epsilon);
        int removeColinearPoints(double epsilon, double zEpsilon);

        //! 
        /*! \brief Calculates the nearest point on the line to the searchPoint
\param a Point a.
\param searchPoint The point to search for (input)
\param outputPoint The point on the line nearest to searchPoint
\return The integrer index the point in the line string where the intersection was found
*/
        int getNearestPointOnLine(const sfa::Point &searchPoint, sfa::Point &outputPoint, int startPt = 0, int endPt = -1);
        int getNearestPointOnLine2D(const sfa::Point &searchPoint, sfa::Point &outputPoint, int startPt = 0, int endPt = -1);

        void reverse();
        //! 
        /*! \brief Calculates the x/y/z point along the length of the linestring
            \param s The s value along the length of the linestring
            \param pt The sfa::Point reference that will receive the x/y/z value. If the linestring is in 3d, the point position will still include a z value
            \param use3D if true, the s value will be calculated using the full 3d length of the line, otherwise it will be calculated in 2d.
            \param afterIdx out receives the index of the point coming immediately before the s value
            \return True if s is <= the length of the line (in 2d or 3d depending on the value of use3D)
        */
        bool getPointAtS(double &s, sfa::Point &pt, int &afterIdx, bool use3D=true);

        //!
        /*! \brief Cuts a portion of each end of the linestring off according to the parameters specified.
                   Operates in 2D
        */
        bool trimEnds(double offset0, double offsetN);

        void clean(double minDistance2=1e-4);

        void transform(const sfa::Matrix &xform);
    };

}
