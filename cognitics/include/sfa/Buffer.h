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
/*! \brief Provides sfa::Buffer.
\author Joshua Anghel <janghel@cognitics.net>
\date 27 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"
#include "LineString.h"
#include "Point.h"

namespace sfa {

/*!\class sfa::Buffer Buffer.h Buffer.h
\brief Buffer
*/
    class Buffer
    {
    public:
        static const int ROUND_ENDS            = 0;
        static const int FLATTEN_ENDS        = 1;
        //static const int SQUARE_ENDS        = 2;

        static const int UNION_RESULTS        = 0;
        static const int DISJOINT_RESULTS    = 1;

        static const double    pi;

    protected:
    //    Settings and    
        int                    end_policy;
        int                    union_policy;

        double                distance;

        unsigned int        quad_segments;
        double                angle;
        double                sin_angle;
        double                cos_angle;

    //    Working Space
        std::vector<Point>    points;

        void addPoint(const Point& point);
        void closePoints(void);
        
        Point                s0, s1, s2;            //    Previous segment
        Point                offset00, offset01;    //    Previous offset
        Point                offset10, offset11; //    Next offset
        
        void addTurn(const Point& p, const Point& p0, const Point& p1, int direction);
        void computeOffset(const Point& p0, const Point& p1, Point& o0, Point& o1);
        
        void addSegment(const Point& p);
        void addLineEnd(const Point& p0, const Point& p1);

/*! \brief Converts a convolution to a final result using ray tracing to determine if intersections occur due to loops.

Loops in a convolution are the result of non convex corners. These can arise if the result is a non-convex polygon, or if there are
holes in the result. The difference is that a hole will always have a clockwise loop, while a counter-clockwise loop is the result of
uneeded convolution edges, and is discarded.
\param points PointList representing a convolution of a Minkowski sum.
\return A Polygon representing the true Minkowski sum  of the convolution (also known as the outer boundary of the convolution).
*/
    public:
        Geometry* de_convolute(const std::vector<Point>& points);

    protected:
        Geometry* bufferPoint(const Geometry* point);
        Geometry* bufferLineString(const Geometry* line);
        Geometry* bufferPolygon(const Geometry* polygon);
        Geometry* bufferPolyhedralSurface(const Geometry* surface);
        Geometry* bufferCollection(const Geometry* collection);
        Geometry* bufferGeometry(const Geometry* geom);

    public:
        Buffer(unsigned int quadrantSegments = 4, int endPolicy = ROUND_ENDS, int unionPolicy = UNION_RESULTS);
        ~Buffer(void) {}

        int getEndPolicy(void) const;
        void setEndPolicy(int policy);

        int getUnionPolicy(void) const;
        void setUnionPolicy(int policy);

        unsigned int getQuadrantSegments(void) const;
        void setQuadrantSegments(int n);

/*! \brief Returns a Geometry representing the Geometry geom buffered by a distance d.
\param geom Geometry to buffer.
\param d Distance to buffer by.
\return The buffer of geom.
*/
        Geometry* apply(const Geometry* geom, double d);
    };

}