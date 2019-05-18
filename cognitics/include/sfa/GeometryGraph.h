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
/*! \brief Provides sfa::GeometryGraph
\author Josh Anghel <janghel@cognitics.net>
\date 4 November 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Geometry.h"
#include "PointNode.h"
#include "EdgeNode.h"

namespace sfa {

/*! \brief GeometryGraph

A GeometryGraph is an object that represents a sfa::Geometry as a collection of PointNodes and EdgeNodes. This allows for
an efficiently traversable data structure for performing operations involving the interaction of multiple Geometries.

Since PointNodes and EdgeNodes are Labeled, a GeometryGraph is capable of fully describing each partition of space created
by its PointNodes and EdgeNodes as INTERIOR, BOUNDARY, or EXTERIOR to one or more Geoemtries.
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class GeometryGraph
    {
    protected:
        const Geometry* parent;
        int                arg;

        PointNodeList    points;            //    Owned by this
        PointNodeList    boundaryPoints;    //    Duplicates for fast processing only

        EdgeNodeList    edges;            //    Owned by this

/*
    A simple graph is one constructed from a geometry asserted to be simple or contain
    no boundary: Point, MultiPoint, LinearRing, Polygon, or LineString
*/
        bool simple;
        bool saveZ;

    public:
//!    Create a GeometryGraph from a Geometry and give it the Geometry ID id. id must be 0 or 1.
        GeometryGraph(const Geometry* geom, int id, bool saveZ = false);
        ~GeometryGraph(void);

/*!    Add a single Geometry to this GeometryGraph
\param geom Geometry to add to this GeometrGraph.
*/
        void addGeometry(const Geometry* geom);

/*! Create a PointNode representing a Point and add it to this GeometryGraph.
\param geom Geometry to add to this GeometrGraph.
*/
        void addPoint(const Geometry* geom);

/*!    Create an EdgeNode representing a LineString and add it to this GeometryGraph.
\param geom Geometry to add to this GeometrGraph.
*/
        void addLineString(const Geometry* geom);

/*!    Create an EdgeNode representing a LinearRing and add it to this GeometryGraph.
\param geom Geometry to add to this GeometrGraph.
*/
        void addLinearRing(const Geometry* geom);

/*!    Create a EdgeNode representing a LinearRing of a Polygon and add it to this GeometryGraph
\param left Location that is to the left (with respect to the orientation) of this LinearRing.
\param right Location that is to the right (with respect to the orientation) of this LinearRing.
\param geom Geometry to add to this GeometrGraph.
*/
        void addPolygonRing(const Geometry* geom, Location left, Location right);

/*!    Create EdgeNodes representing a Polygon boundary and its holes add add them to this GeometryGraph.
\param geom Geometry to add to this GeometrGraph.
*/
        void addPolygon(const Geometry* geom);

/*!    Adds a GeometryCollection to this GeometryGraph by itterated through its children with addGeometry.
\param geom GeometryCollection to add to this GeometryGraph.
*/
        void addGeometryCollection(const Geometry* geom);

/*!    Adds a MultiPoint to this GeometryGraph by itterating through its child Points with addPoint.
\param geom MultiPoint to add to this GeometryGraph.
*/
        void addMultiPoint(const Geometry* geom);

/*!    Adds a MultiLineString to this GeometryGraph by itterating through its child LineStrings with addLineString and addLinearRing.
\param geom MultiLineString to add to this GeometryGraph.
*/
        void addMultiLineString(const Geometry* geom);

/*!    Adds a PolyhedralSurface to this GeometryGraph by itterating through its child Polygons (patches) with addPolygon.
\param geom PolyhedralSurface to add to this GeometryGraph.
*/
        void addPolySurface(const Geometry* geom);

/*!    Adds a MultiPolygon to this GeometryGraph by itterating through its child Polygons with addPolygon.
\param geom MultiPolygon to add to this GeometryGraph.
*/
        void addMultiPolygon(const Geometry* geom);

/*!    Adds an EdgeNode to this GeometryGraph.
\param edge EdgeNode to add to this GeometryGraph.
*/
        void addEdge(EdgeNode* edge);

/*!    Adds a PointNode to this GeoemtryGraph.
\param point PointNode to add to this GeometryGraph.
*/
        void addPoint(PointNode* point);

        void addIsolatedPoint(PointNode* a);

/*! \brief Adds a PointNode that lies on a BOUNDARY to this GeometryGraph using the Mod-2 rule.

Attempt to add a BOUNDARY PointNode, if the PointNode is already in the PointNodeList if will not add the PointNode,
but it will update the Location of the currently added PointNode using the Mod-2 rule.
    
If the current PointNode is also a BOUNDARY PointNode, it will be Labeled as INTERIOR. If the current PointNode
is not a BOUNDARY PointNode, it will now become    a BOUNDARY PointNode.
\param a BOUNDARY PointNode to attemp to add to the PointNodeList.
*/
        void addBoundaryPoint(PointNode* a);

//!    Computes the intersections of this GeometryGraph with itself. Only performed for non-simple Geometries.
        void computeSelfIntersections(void);

//! Returns a PointNodeList of all PointNodes in this GeometryGraph.
        PointNodeList getPointNodes(void);

//!    Returns a PointNodeList of all PointNodes in this GeometryGraph that do not intersect another Geometry and are not part of any EdgeNode.
        PointNodeList getIsolatedPoints(void);

//!    Returns an EdgeNodeList of all EdgeNodes in this GeometryGraph.
        EdgeNodeList getEdgeNodes(void);

/*!    Adds all EdgeNodes in this GeometryGraph to either the intersecting or isolated EdgeNodeLists passed.
\param intersecting EdgeNodeList to place all EdgeNodes found in this GeometryGraph that have intersection on them.
\param isolated EdgeNodeList to place all EdgeNodes found in this GeometryGraph that do not have any intersections on them.
*/
        void getEdgeNodes(EdgeNodeList &intersecting, EdgeNodeList &isolated);

//!    Returns an EdgeNodeList of all isolated EdgeNodes in this GeometryGraph that have no intersections on them.
        EdgeNodeList getIsolatedEdges(void);

//!    Returns the parent Geometry that this GeometryGraph was constructed from.
        const Geometry* getGeometry(void);

    };

}