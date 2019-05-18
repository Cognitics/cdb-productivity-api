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
/*! \brief Provides sfa::Buffer3D.
\author Josh Anghel <janghel@cognitics.net>
\date 31 January 2010
*/
#pragma once
#include "Point.h"
#include "PolyhedralSurface.h"

namespace sfa {

/*!    \class sfa::Buffer3D Buffer3D.h Buffer3D.h
\brief Buffer3D

This class currently still uses shared_ptr internally, and will be changed in the long run to not longer need them.
Also, this class currently has a maximum depth restricted by memory since the entire grid is created and stored. This
will be changed so that only 3 "layers" of the grid are created are stored at any time on a cache. This will allow
approximately the same processing time, but it will allow much larger datasets to be handled while keeping memory usage
to a minimum.

Provides a Marching Cubes algorithm that uses the distance function to construct a isosurface that can be used
to represend a 3D Buffer or offset.

The space surrounding the geometry is broken into a grid of cubes. Each cube vertex is evaluated using the distance
function. Based on which vertex points on a cube are within the threshold value, various premade trianglulations 
using points along the edges of the cube are used. In order to allow a more accurate surface, the points used on
the edges are found using linear interpolation of the two vertices which make up the edge.

The controllable parameter depth is used to determine the spatial resolution of the resulting surface by controlling
how many times the space around the geometry is partitioned. 1 being 2x2x2, 2 being 4x4x4, 3 being 8x8x8, and so on.
The max depth that a Buffer can be constructed is with depth 8. This can result in up to 256x256x256 cubes in some
cases, and can take an intensive amount of time and memory, but yeilds very clean results. The default depth is 4.

Also note that the depth you wish to use must be comparable to the distance you wish to buffer the geometry by.
If your geometry spans roughly 10 units in one direction, in order to offset by an amount 1, the depth must be at least 
4. If the depth is chosen to be less than that, the cubes will begin to be much wider than the distance needed, resulting
in an insufficient density of vertex points to create the buffer surface.
*/
    class Buffer3D
    {
    protected:

//    Structures --------------------------------------------------------------------

        enum NeighborID
        {
            NORTH    = 0,
            SOUTH    = 1,
            EAST    = 2,
            WEST    = 3,
            UP        = 4,
            DOWN    = 5
        };

        struct        vertex;
        struct        edge;
        struct        cube;

        typedef        std::shared_ptr<vertex> vertexSP;
        typedef        std::shared_ptr<edge> edgeSP;
        typedef        std::shared_ptr<cube> cubeSP;

/*!    \struct sfa::Buffer3D::vertex Buffer3D.h Buffer3D.h
\brief Buffer3D::vertex

Structure that contains a Point and its value as a +/- double above or below the set threshold.
Used to represent the corners of a cube used by Buffer3D to perform a Marching Cubes operation.
*/
        struct vertex
        {
            PointSP        point;
            double        value;

            vertex(PointSP p, double v) : point(p), value(v) {}
            ~vertex(void) {}
        };

/*!    \struct sfa::Buffer3D::edge Buffer3D.h Buffer3D.h
\brief Buffer3D::edge

Structure that contains a Point and its surface normal. This Point is found by linear interpolation
from two nearby Buffer3D::vertex Points.
Used to represent the edges of a cube used by Buffer3D to perform a Marching Cubes operation.
*/
        struct edge
        {
            PointSP        point;
            PointSP        norm;

/*!    Computes the Point and normal for this edge from the linear interpolation of the two passed Buffer3D::vertex.
\param v0 First Buffer3D::vertex.
\param v1 Second Buffer3D::vertex.
*/
            void        Interpolate(vertexSP v0, vertexSP v1);
        };

/*!    \struct sfa::Buffer3D::cube Buffer3D.h Buffer3D.h
\brief Buffer3D::cube

Structure that contains a integer coordinate for this cubes location in a grid, 8 Buffer3D::vertex points,
and 12 Buffer3D::edges. Does not contain information on neighboors since its neighboors can be easily found
by using the neighboring intex coordinates.
*/
        struct cube
        {
            ccl::uint8_t    x, y, z;
            vertexSP    verts[8];
            edgeSP        edges[12];

/*! The case index of a Buffer3D::cube is the 8-bit binary number |v7|v6|v5|v4|v3|v2|v1|v0| where vn is 1 if
the value of that Buffer3D::vertex is greater than or equal to 0 and 0 otherwise.
\return The case number of this Buffer3D::cube.
*/
            ccl::uint8_t    CaseIndex(void);

/*! Computes the interpolated points and normals of each edge of the cube if they have not already be computed
by a neighbooring Buffer3D::cube.
*/
            void        ComputeEdges(void);

            cube(ccl::uint8_t _x, ccl::uint8_t _y, ccl::uint8_t _z): x(_x), y(_y), z(_z) {}
            ~cube(void) {}
        };

//    Lookup Tables -----------------------------------------------------------------

        static const ccl::uint8_t    Ambiguous_Cases[60];
        static const ccl::int8_t    Triangles_N[256][15];
        static const ccl::int8_t    Triangles_A[256][15];

//    Buffer Parameters -------------------------------------------------------------

        const Geometry*    parent;
        double        threshold;
        Point*        origin;
        double        cube_width;
        int            size;    

//    Grid --------------------------------------------------------------------------

        std::map<ccl::uint32_t,cubeSP> grid_map;

//    Triangles ---------------------------------------------------------------------

        PolyhedralSurface* triangles;

//    Functions ---------------------------------------------------------------------
/*!    \brief Performs a hash of 3 8-bit integer coordinates to create a 32-bit unsigned int key.

This key is created by combining 4 8-bit blocks in this manner: [ blank ][ z ][ y ][ x ]
Since there is no clipping, this hash will ensure that no two coordinates have the same key.
\param x X 8-bit unsigned int index.
\param y Y 8-bit unsigned int index.
\param z Z 8-bit unsigned int index.
\return 32-bit unsigned int key cooresponding to these coordinates.
*/
        ccl::uint32_t    Hash(ccl::uint8_t x, ccl::uint8_t y, ccl::uint8_t z);

/*! Performs Hash on the coodinates of the passed Buffer3D::cube.
\param _cube Buffer3D::cube to find the Hash key for.
\return Hash key of the passed Buffer3D::cube.
*/
        ccl::uint32_t    Hash(cubeSP _cube);

/*!    Performs Hash on the expected coordinates of the passed Buffer3D::cube's neighbor. The neighboor to find the key for is found using the passed NeighborID.
\param _cube Buffer3D::cube to find the neighbor for.
\param id NeighborID to find for _cube.
\return Hash key of the passed Buffer3D::cube's neighbor.
*/
        ccl::uint32_t    NeighborHash(cubeSP _cube, NeighborID id);

/*!    Finds the Buffer3D::cube with the passed coordinates in the grid using the Hash of the coodinates as a key.
\param x X 8-bit unsigned int index.
\param y Y 8-bit unsigned int index.
\param z Z 8-bit unsigned int index.
\return Returns the Buffer3D::cube with those coordinates or a blank cubeSP() if that cube does not exist.
*/
        cubeSP        GetCube(ccl::uint8_t x, ccl::uint8_t y, ccl::uint8_t z);

/*! Finds the Buffer3D::cube that represents the Buffer3D::NeighborID of the passed Buffer3D::cube using the key found by NeighborHash.
\param _cube Buffer3D::cube to find a neighbor for.
\param id NeighborID to find for _cube.
\return The Buffer3D::cube representing the NeighborID of the passed Buffer3D::cube, or cubeSP() if that cube does not exist.
*/
        cubeSP        GetNeighbor(cubeSP _cube, NeighborID id);

/*!    Evaluates the distance function between the passed Geometry and Point and offsets that value by the set threshold. The result if a +/- value representing
if the Point is inside or outside the (threshold) buffer of a Geometry.
\param geometry Parent Geometry to test distance from.
\param point Point to evaluate distance to fromt to.
\return The threshold offset distance between the passed Geometry and Point.
*/
        double        Evaluate(const Geometry* geometry, PointSP point);

/*!    Creates a new Point at the passed coordinates.
\param x X 8-bit unsigned int index.
\param y Y 8-bit unsigned int index.
\param z Z 8-bit unsigned int index.
\return A Point located at the passed coordinates.
*/
        PointSP        CreatePoint(ccl::uint8_t x, ccl::uint8_t y, ccl::uint8_t z);

/*!    Creates a new Buffer3D::vertex at the passed coordinates.
\param x X 8-bit unsigned int index.
\param y Y 8-bit unsigned int index.
\param z Z 8-bit unsigned int index.
\return A Buffer3D::vertex at the passed coordinates.
*/
        vertexSP    CreateVertex(ccl::uint8_t x, ccl::uint8_t y, ccl::uint8_t z);

/*!    Creates a new Buffer3D::cube at the passed coordinates.
\param x X 8-bit unsigned int index.
\param y Y 8-bit unsigned int index.
\param z Z 8-bit unsigned int index.
\return A Buffer3D::cube at the passed coordinates.
*/
        cubeSP        CreateCube(ccl::uint8_t x, ccl::uint8_t y, ccl::uint8_t z);

/*!    Re-evaluates the value of a Buffer3D::vertex using the passed Geometry and replaces its value if and only if it is less than the currently stored value.
\param vert Buffer3D::vertex to re-evaluate.
\param geometry Geometry to re-evaluate the passed Buffer3D::vertex against.
*/
        void        UpdateVertex(vertexSP vert, const Geometry* geometry);

/*!    Adds a Geometry to the grid and creates any Buffer3D::cubes that are within the threshold distance of the envelope of the passed Geometry.
\param geometry Geometry to add.
*/
        void        AddGeometry(const Geometry* geometry);

/*!    Tests if a Buffer3D::cube is an ambiguous case or not. (This test if performed by using a binary seach of a sorted array containing known ambiguous case index values).
\param _cube Buffer3D::cube to test for ambiguity.
\return True if the Buffer3D::cube has an ambiguous case index.
*/
        bool        IsAmbiguous(cubeSP _cube);

/*!    Creates a series of Triangles representing the interpolated surface that passes through the passed Buffer3D::cube using the passed edges noted by their index values.
\param _cube Buffer3D::cube to pull edges from.
\param edges Array of index values to edges of _cube to create Triangles from.
*/
        void        CreateTriangles(cubeSP _cube, const ccl::int8_t edges[15]);

    public:
        Buffer3D(const Geometry* geometry, double distance, int depth = 4);
        ~Buffer3D(void);

/*!    Perform the marching cubes algorithm to construct a PolyhedralSurface from the cube grid.
\return A PolyhedralSurface representing a surface whoes points are all as close to equal to the set threshold distance away form the parent Geometry.
*/
        Geometry* apply(void);
    };

}