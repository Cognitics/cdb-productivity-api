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
/*! \brief Provides sfa::ConvexHull3D.
\author Joshua Anghel <janghel@cognitics.net>
\date 23 December 2010
*/
#pragma once

#include "Point.h"
#include "MultiPoint.h"

namespace sfa {

/*!    \class sfa::ConvexHull3D ConvexHull3D.h ConvexHull3D.h
\brief ConvexHull3D

Provides an incrimental algorithm for the generation of a 3D Convex Hull for a collection of Points.
Any Geometry's convex hull can be found in 3D by converting the geometry to a point collection and 
then passing that collection to this algorithm.

To ensure more efficient run times, this is a randomized incrimental algorithm, random points from the
collection are added to the hull one at a time if they are found to be on the outside of the hull and 
the result of this hull algorithm will be either a TIN if the result is not degenerate. If the resulting
hull is a degenerate case, then a Point or LineString may be returned. This method is robust and should
handle any collection of points. There can be collinear, duplicate, insufficient number, etc of Points
and the correct hull will always be returned in its lowest dimensional state.

Usage:
\code
//    The convex hull can be constructed one of two ways.
    GeometrySP geometry;    //    Some sfa::Geometry
    GeometrySP hull;        //    resulting hull

//    Calling the ConvexHull3D method from the geometry object
    hull = geometry->convexHull3D();

//    Using the ConvexHull3D class directly
    hull = ConvexHull3D::apply(geometry);
\endcode
*/
    class ConvexHull3D
    {
    protected:

        struct HullPoint;
        struct HullEdge;
        struct HullFace;

        struct HullPoint
        {
            Point            point;
            bool            processed;
            HullEdge*        coneEdge;

            HullPoint(void);
        };

        struct HullEdge
        {
            HullEdge*        sym;
            HullPoint*        ends[2];
            HullFace*        left;
            HullFace*        right;
            HullEdge*        prev;
            HullEdge*        next;
            bool            toDelete;

            HullEdge(void);
        };

        struct HullFace
        {
            Point            points[3];
            bool            visible;
        };

        const MultiPoint*            multiPoint;
        std::vector<HullPoint*>        hull_points;
        std::vector<HullEdge*>        hull_edges;
        std::vector<HullFace*>        hull_faces;

        int completed;

        void reset(void);

/*!    \brief Set the MultiPoint collection to use.
    \param points MultiPoint to construct hull for*/
        void setPoints(const MultiPoint* points);

/*!    \brief Make a HullEdge and its symetric edge and add them to the hull_edges vector.
\param p0 First HullPoint to use for the edges.
\param p1 Second HullPoint to use for the edges.
\returns The first HullEdgeSP, which is linked to its symetric edge.*/
        HullEdge* makeEdge(HullPoint* p0, HullPoint* p1);

/*!    \brief Returns Volume of the pyramid created by a triangle and point. This is used to determine which side of the triangle a point lies on.
\param f HullFace to test HullPoint against.
\param p HullPoint to test against the HullFace.
\return 0, 1, or -1 if the HullPoint lies on, inside, or outside of the HullFace respectively.*/
        int volumeSign(HullFace* f, HullPoint* p);

/*!    \brief Create a new HullFace from the given HullEdge to the HullPoint p using any pre-existing edges while creating new ones if needed.
\param e Already constructed HullEdge base.
\param p Point to create new HullEdges to from the base ends.
\return The fully constructed and linked resulting HullFace.*/
        HullFace* LinkFaceEdge(HullEdge* e, HullPoint* p);

/*!    \brief Adds a single HullPoint to the hull.
\param p HullPoint to add to the hull.
\return True if the HullPoint was added, False if the HullPoint was inside or on the hull and thus not included.*/
        bool addOne(HullPoint* p);

//    Handle the degenerate collinear case - all points lie along the same line
        Geometry* handleCollinear(void);

//    Handle coplanar case, by translating to a 2D plane and applying the GrahamHull algorithm
        Geometry* handleCoplanar(void);

//    initialize starter hull and discover any degerate cases
        int initialize(void);

//    Construct the hull using the set MultiPoint collection
        Geometry* constructHull(void);

//    Cleaning functions for removing hidden faces and reseting point flags
        void cleanPoints(void);
        void cleanEdges(void);
        void cleanFaces(void);
        
        void clean(void);

        ConvexHull3D(void) {}
        ~ConvexHull3D(void) {}

    public:

/*!    \brief Apply the incrimental ConvexHull3D algorithm on a MultiPoint.
\param points MultiPoint collection to construct the hull for
\return Returns a Geometry representing the convex hull of the MultiPoint in 3D space.*/
        static Geometry* apply(const MultiPoint* points);
    };

}