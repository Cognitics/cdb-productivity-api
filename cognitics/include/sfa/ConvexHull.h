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
/*! \brief Provides sfa::MelkmanHull and sfa::GrahamHull
\author Joshua Anghel <janghel@cognitics.net>
\date 1 October 2010
*/
#pragma once

#include "LineString.h"
#include "MultiPoint.h"

namespace sfa {

/*!\class sfa::MelkmanHull ConvexHull.h ConvexHull.h
\brief MelkmanHull

Uses the Melkman algorithm for finding the convex hull of a 2D polyline (LineString, LinearRing, or Polygon) in O(n) time.
Creates a DEQueue that stores all the points of the current hull with the most recent added point at the bottom and top of
the DEQueue. The next point of each line is tested to be in one of 4 possible Locations and then the DEQueue is backtracked 
to the correct location to insert the next point.

Usage:
\code
    sfa::LineStringSP geometry;
    sfa::GeometrySP hull;

//    Calling the convexHull function from a sfa::GeometrySP that is castable to a LineString or Polygon will call the MelkmaHull algorithm.
    hull = geometry->convexHull();

//    Or to use the MelkmanHull algorithm directly call
    hull = sfa::MelkmanHull::apply(geometry);
\endcode

There is no benefit in calling the MelkmanHull algorithm directly, and is in fact limited only to polylines, so calling the convexHull
method in the Geometry class is more practical.

Note that this is a 2D method. It will only use the x and y values of the input Geometries, effectively flattening them. Also, the resulting geometry will be a 2D
geometry in the xy plane.
*/
    class MelkmanHull
    {
    protected:
        MelkmanHull(void) {}
        ~MelkmanHull(void) {}

/*!\brief Tests if the point p is to the left of the line created by p1 and p2

Uses SFA_EPSILON.
\param p PointSP to test
\param p1 First PointSP on the line
\param p2 Second PointSP on the line
\return 0 if p is along the line of p1 and p2, -1 if it is to the left, and 1 if it is to the right
*/
        static int isLeft(const Point& p, const Point& p1, const Point& p2);

    public:

/*!\brief MelkmanHull Algorithm

Initializes the algorithm to find the convex hull of a LineString or a LinearRing.
\param a LineStringSP to find the hull of
\return The convex hull of a
*/
        static Geometry* apply(const LineString* a);
    };

/*!\class sfa::GrahamHull ConvexHull.h ConvexHull.h
\brief GrahamHull

Uses the Graham Scan algorithm to find the convex hull of a 2D set of points in O(n log n) time.
Works on any collection of points and does not require any assumptions. Any collinear or duplicate points are
handled. If all the points lie along the same line, a LineString of that length is returned instead of a polygon.

Since the running time is optimized for a general collection of points, if the geometry can be represented as a
LineString or Polygon in 2D, the Melkman algorithm should be used instead.

Usage:
\code
    sfa::GeometrySP geometry;
    sfa::GeometrySP hull;

//    Calling the convexHull function from a sfa::GeometrySP that is not castable to a LineString or Polygon will call the GrahamHull algorithm.
    hull = geometry->convexHull();

//    Or the GrahamHull algorithm can be called directly by transforming any geometry into a MultiPoint
    sfa::MultiPointSP multiPoint;
    hull = sfa::GrahamHull::apply(multiPoint);
\endcode

There is no need to call the GrahamHull algorithm directly. In fact, if calling the convexHull method from the sfa::Geometry class
is always prefered because if a Geometry cannot be cast to a LineString or Polygon, it will be automatically converted into a
MultiPoint and passed to the GrahamHull algorithm.
*/
    class GrahamHull
    {
    protected:

/*!\struct sfa::GrahamHull::GrahamPoint ConvexHull.h ConvexHull.h
\brief GrahamPoint

Contains a PointSP and boolean flag. The flag will be set to true if the Point is found to be the smaller of two
collinear points. The compress function uses these flags to delete points from the list.
*/
        struct GrahamPoint
        {
            Point p;
            bool toDelete;

            GrahamPoint(const Point& point): toDelete(false), p(point) {}
            ~GrahamPoint(void) {}
        };

        std::vector<GrahamPoint> _P;
        std::vector<GrahamPoint*> P;
        std::vector<Point> hull;
        int num;
        
//!    Find the lowest right Point in this convex hull
        void findLowest(void);

/*!\brief Compares two GrahamPoints for sorting

Compares two GrahamPoints. This comparison sorts points by taking the cross product of the lines formed with 
the pivot point. If two points are found to be collinear, the point that is closest to the pivot is flagged
for deletion.

\param a First GrahamPoint to compare
\param b Second GrahamPoint to compare
\return -1 if a is radially less than b, 0 if they are radially equal, and 1 if a is radially greater than b
*/
        int comparePoints(GrahamPoint* a, GrahamPoint* b);

//!    Swaps two GrahamPoints in the array.
        void swap(int i, int j);

//!    Quicksort the GrahamPoint list
        void quickSort(int low, int high);
        void sort(void);

//!    Compress the sorted array and remove duplicates and collienar points.
        void compress(void);

//!    Compute the hull
        void compute(void);

    public:

        GrahamHull(const std::vector<Point>& points);
        GrahamHull(const MultiPoint* a);
        ~GrahamHull(void) {}

//!    Constructs and returns a Geometry representing this ConvexHull
        Geometry* getHullGeometry(void);

//!    Returns the ordered points of this ConvexHull
        std::vector<Point> getHullPoints(void);

//!    Returns the convex hull of a MultiPoint
        static Geometry* apply(const MultiPoint* a);

//!    Returns the convex hull of a collection of Points
        static std::vector<Point> apply(const std::vector<Point>& points);

    };

}