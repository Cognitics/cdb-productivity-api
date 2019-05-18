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
/*! \brief Provides sfa::Geometry.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include <cts/cts.h>
#include <ccl/ccl.h>
#if (_MSC_VER >= 1700)
#include <memory>
#else
#include <boost/tr1/memory.hpp>
#endif
#include <string>
#include <vector>

namespace sfa
{

    extern double SFA_EPSILON;

    class Geometry;
    typedef std::shared_ptr<Geometry> GeometrySP;
    typedef std::vector<Geometry *> GeometryList;

    enum WKBByteOrder
    {
        wkbXDR = 0,
        wkbNDR = 1
    };

    enum WKBGeometryType
    {
        wkbUnknownSFA            =  0,

        wkbPoint                =  1,
        wkbLineString            =  2,
        wkbPolygon                =  3,
        wkbTriangle                = 17,
        wkbMultiPoint            =  4,
        wkbMultiLineString        =  5,
        wkbMultiPolygon            =  6,
        wkbGeometryCollection    =  7,
        wkbPolyhedralSurface    = 15,
        wkbTIN                    = 16,

        wkbPointZ                = 1001,
        wkbLineStringZ            = 1002,
        wkbPolygonZ                = 1003,
        wkbTriangleZ            = 1017,
        wkbMultiPointZ            = 1004,
        wkbMultiLineStringZ        = 1005,
        wkbMultiPolygonZ        = 1006,
        wkbGeometryCollectionZ    = 1007,
        wkbPolyhedralSurfaceZ    = 1015,
        wkbTINZ                    = 1016,

        wkbPointM                = 2001,
        wkbLineStringM            = 2002,
        wkbPolygonM                = 2003,
        wkbTriangleM            = 2017,
        wkbMultiPointM            = 2004,
        wkbMultiLineStringM        = 2005,
        wkbMultiPolygonM        = 2006,
        wkbGeometryCollectionM    = 2007,
        wkbPolyhedralSurfaceM    = 2015,
        wkbTINM                    = 2016,

        wkbPointZM                = 3001,
        wkbLineStringZM            = 3002,
        wkbPolygonZM            = 3003,
        wkbTriangleZM            = 3017,
        wkbMultiPointZM            = 3004,
        wkbMultiLineStringZM    = 3005,
        wkbMultiPolygonZM        = 3006,
        wkbGeometryCollectionZM    = 3007,
        wkbPolyhedralSurfaceZM    = 3015,
        wkbTINZM                = 3016
    };

    class GeometryBase
    {
    public:
        virtual ~GeometryBase(void);
        GeometryBase(void);
        virtual std::string getGeometryType(void) const = 0;
        virtual WKBGeometryType getWKBGeometryType(bool withZ = false, bool withM = false) const = 0;
        virtual bool isParentOf(const GeometryBase *child) const = 0;
        virtual GeometryBase *getParentOf(const GeometryBase *child) const = 0;
        virtual int getNumChildren(void) const = 0;
    };

    typedef std::shared_ptr<GeometryBase> GeometryBaseSP;

/*! \class sfa::Geometry Geometry.h Geometry.h
\brief Geometry

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.2
*/
    class Geometry : public GeometryBase, public std::enable_shared_from_this<Geometry>
    {
    private:
        cts::CS_CoordinateSystem* coordinateSystem;

    public:
        virtual ~Geometry(void);
        Geometry(void);

        Geometry* copy(void) const;
        static GeometrySP getShared(Geometry* geometry);

/*! \brief Get the spatial reference system for this geometric object.

This is a diversion from the OGC specification to incorporate the OGC Coordinate Transformation Services CS_CoordinateSystem definition.

In this implementation, there is no getSRID(), getCoordinateDimension(), or getSpatialDimension().

\return A shared pointer to a cts::CS_CoordinateSystem, or empty if no spatial reference system is associated with this geometric object.
*/
        virtual cts::CS_CoordinateSystem* getCoordinateSystem(void) const;

/*! \brief Set the spatial reference system for this geometric object.
\param coordinateSystem A shared pointer to a cts::CS_CoordinateSystem.
\param mathTransform The cts::MathTransform instance for the transformation if necessary.
*/
        virtual void setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform = NULL);

/*! \brief Set the epsilon to use in comparison of double types in the SFA.

Due to the high number of computations performed in the sfa and the limitations of linear interpolation, rounding errors in doubles can occur and when 
compounded can cause standard comparisons to give undersired results. As a result, a custom epsilon is used in comparison of values. 

The default epsilon is 6e-14 and any functions using this custom epsilon will state so in their documentation. Note that this value was set with values
of order of magnitude 1 in mind. If dealing with numbers orders of magnitude higher or lower, the SFA_EPSILON should be increased or decreased respectively,
so as to maintain its ability to give a slight buffer to the end of the significant digit range of the doubles being used in the SFA.

If dealing with very small or large numbers and errors or oddities arise in the resulting data, try increased (if using large numbers) or decreasing (if 
using small numbers) the SFA_EPSILON, till the errors are no longer present.
\param newEpsilon The new epsilong to set.
*/
        static void setEpsilon(double newEpsilon);

/*! \brief Get the dimension of this geometric object.
\return The inherent dimension of this geometric object, which must be less than or equal to the coordinate dimension.
In non-homogeneous collections, this will return the largest topological dimension of the contained objects.
*/
        virtual int getDimension(void) const = 0;

/*! \brief Get the name of the instantiable subtype.
\return The name of the instantiable subtype of Geometry of which this geometric object is an instantiable member.
The name of the subtype of Geometry is returned as a string.
*/
        virtual std::string getGeometryType(void) const = 0;
        virtual WKBGeometryType getWKBGeometryType(bool withZ = false, bool withM = false) const = 0;

/*! \brief Get the minimum bounding box.
\return The minimum bounding box for this Geometry, returned as a Geometry.
The polygon is defined by the corner points of the bounding box [(MINX, MINY), (MAXX, MINY), (MAXX, MAXY), (MINX, MAXY), (MINX, MINY)].
Minimums for Z and M may be added.

Minimums for Z have been added.

The simplest representation of an Envelope is as two direct positions, one containing all the minimums, and another all the maximums.
In some cases, this coordinate will be outside the range of validity for the Spatial Reference System.
*/
        virtual Geometry* getEnvelope(void) const = 0;

/*! \brief Exports this geometric object to a specific Well-known Text Representation of Geometry.
\return The WKT text for this geometry.
*/
        virtual std::string asText(bool tag = true) const;
        virtual void toText(std::ostream &os, bool tag, bool withZ, bool withM) const;
        virtual void fromText(std::istream &is, bool tag, bool withZ, bool withM) = 0;

/*! \brief Exports this geometric object to a specific Well-known Binary Representation of Geometry.
\return The well known binary for this geometry.
*/
        virtual ccl::binary asBinary(WKBByteOrder byteOrder = wkbXDR) const;
        virtual void toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const = 0;
        virtual void fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM) = 0;

/*! \brief Check if this geometric object is the emtpy geometry.
\return True if this geometric object represents the empty point set for the coordinate space; false otherwise.
*/
        virtual bool isEmpty(void) const = 0;

/*! \brief Check if this geometric object has no anomalous geometric points.
\return True if this geometric object has no anomalous geometric points, such as self intersection or self tangency; false otherwise.
The description of each instantiable geometric class will include the specific conditions that cause an instance of that class to be classified as not simple.
*/
        virtual bool isSimple(void) const = 0;

/*! \brief Check if this geometric object has z coordinate values.
\return True if this geometric object has z coordinate values; false otherwise.
*/
        virtual bool is3D(void) const = 0;

/*! \brief Check if this geometric object has m coordinate values.
\return True if this geometric object has m coordinate values; false otherwise.
*/
        virtual bool isMeasured(void) const = 0;

/*! \brief Get the combinatorial boundary of this geometric object.
\return The closure of the combinatorial boundary of this geometric object.
Because the result of this function is a closure, and hence topologically closed, the resulting boundary can be represented using representational Geometry primitives.
*/
        virtual Geometry* getBoundary(void) const = 0;

//***********************************************************************
// query

/*! \brief Determine if this geometry is spacially equal to another geometry.
\return True if this geometry is spacially equal to another geometry.
*/
        virtual bool equals(const Geometry* another) const;

/*!    \brief Determine if this geometry is spacially equal to another geometry. (This method does not ignore z values)
\return True if this geometry is spacially equal to another geometry.
*/
        virtual bool equals3D(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially disjoint from another geometry.
\return True if this geometry is spacially disjoint from another geometry.
*/
        virtual bool disjoint(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially disjoint from another geometry. (This method does not ignore z values)
\return True if this geometry is spacially disjoint from another geometry.
*/
        virtual bool disjoint3D(const Geometry* another) const;

/*! \brief Determine if this geometry spacially intersects another geometry.
\return True if this geometry is spacially intersects another geometry.
*/
        virtual bool intersects(const Geometry* another) const;

/*! \brief Determine if this geometry spacially intersects another geometry. (This method does not ignore z values)
\return True if this geometry is spacially intersects another geometry.
*/
        virtual bool intersects3D(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially touches another geometry.
\return True if this geometry is spacially touches another geometry.
*/
        virtual bool touches(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially touches another geometry. (This method does not ignore z values)
\return True if this geometry is spacially touches another geometry.
*/
        virtual bool touches3D(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially crosses another geometry.
\return True if this geometry is spacially crosses another geometry.
*/
        virtual bool crosses(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially crosses another geometry. (This method does not ignore z values)
\return True if this geometry is spacially crosses another geometry.
*/
        virtual bool crosses3D(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially within another geometry.
\return True if this geometry is spacially within another geometry.
*/
        virtual bool within(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially within another geometry. (This method does not ignore z values)
\return True if this geometry is spacially within another geometry.
*/
        virtual bool within3D(const Geometry* another) const;

/*! \brief Determine if this geometry spacially contains another geometry.
\return True if this geometry spacially contains another geometry.
*/
        virtual bool contains(const Geometry* another) const;

/*! \brief Determine if this geometry spacially contains another geometry. (This method does not ignore z values)
\return True if this geometry spacially contains another geometry.
*/
        virtual bool contains3D(const Geometry* another) const;

/*! \brief Determine if this geometry spacially overlaps another geometry.
\return True if this geometry spacially overlaps another geometry.
*/
        virtual bool overlaps(const Geometry* another) const;

/*! \brief Determine if this geometry spacially overlaps another geometry. (This method does not ignore z values)
\return True if this geometry spacially overlaps another geometry.
*/
        virtual bool overlaps3D(const Geometry* another) const;

/*! \brief Determine if this geometry is spacially related to another geometry. See sfa::Relate.
\return True if this geometric object is spatially related to another geometry by testing for intersections between the interior,
boundary and exterior of the two geometric objects as specified by the values in the intersection matrix.
This returns false if all the tested intersections are empty except exterior (this) intersect exterior (another).
*/
        virtual bool relate(const Geometry* another, const std::string &matrix) const;

/*! \brief Determine if this geometry is spacially related to another geometry. See sfa::Relate. (This method does not ignore z values) 
\return True if this geometric object is spatially related to another geometry by testing for intersections between the interior,
boundary and exterior of the two geometric objects as specified by the values in the intersection matrix.
This returns false if all the tested intersections are empty except exterior (this) intersect exterior (another).
*/
        virtual bool relate3D(const Geometry* another, const std::string &matrix) const;

/*! \brief Get a geometry collection that matches the specified m coordinate value.
\return A derived geometry collection value that matches the specified m coordinate value.
*/
        virtual Geometry* locateAlong(double mValue) const;

/*! \brief Get a geometry collection that matches the specified range of m coordinate values inclusively.
\return A derived geometry collection that matches specified range of m coordinate values inclusively.
*/
        virtual Geometry* locateBetween(double mStart, double mEnd) const;

//***********************************************************************
// analysis

/*! \brief Get the shortest distance between any two points in the two geometric objects.S
\return The shortest distance between any two points in the two geometric objects as calculated in the spatial reference system of this geometric object.
Because the geometries are closed, it is possible to find a point on each geometric object involved, such that the distance between these 2 points is the returned distance between their geometric objects.
*/
        virtual double distance(const Geometry* another) const;

/*! \brief Get the shortest distance between any two points in the two geometric objects. See sfa::Distance. (This method does not ignore z values - because of this, two geometric objects may have a non-zero distance even though
they might overlap in the xy plane.
\return The shortest distance between any two points in the two geometric objects as calculated in the spatial reference system of this geometric object.
Because the geometries are closed, it is possible to find a point on each geometric object involved, such that the distance between these 2 points is the returned distance between their geometric objects.
*/
        virtual double distance3D(const Geometry* another) const;

/*! \brief Get a geometric object that represents all points whose distance from this geometric object is less than or equal to the passed distance.

Extra parameters may be specified to allow customized interpolation, variable EndCapPolicy options for LineString, and optional Union of multiple overlapping results.
\param distance See sfa::Buffer.
\param interpolationN See sfa::Buffer.
\param EndcapPolicy see sfa::Buffer.
\param UnionPolicy see sfa::Buffer.
\return A geometric object that represents all points whose distance from this geometric object is less than or equal to the passed distance.
Calculations are in the spatial reference system of this geometric object.
Because of the limitations of linear interpolation, there will often be some relatively small error in this distance, but it should be near the resolution of the coordinates used.
*/
        virtual Geometry* buffer(double distance, int EndcapPolicy = 0, bool UnionPolicy = false, int interpolationN = 4) const;

/*!    \brief  Get a geometric object that represents all points whose distance is roughly equal to the passed distance (in full 3D). The result will be a PolyhedralSurface representing that point collection.
\param distance See sfa::Buffer3D.
\param depth See sfa::Buffer3D.
\return A geometric object that represents all points whose distance is roughly equal to the passed distance (in full 3D).
Calculations are in the spatial reference system of this geometric object.
Because of the limitations of linear interpolation, there will often be some relatively small error in this distance, but it should be near the resolution of the coordinates used.
*/
        virtual Geometry* buffer3D(double distance, int depth = 4) const;

/*! \brief Get a geometric object that represents the convex hull of this geometric object.
\return A geometric object that represents the convex hull of this geometric object.
Convex hulls, being dependent on straight lines, can be accurately represented in linear interpolations for any geometry restricted to linear interpolations.
*/
        virtual Geometry* convexHull(void) const = 0;

/*! \brief Get a geometric object that represents the convex hull of this geometric object. (This method does not ignore z values - because of this, the result may be a Polyhedral Surface (TIN) instead of a single Polygon)
\return A geometric object that represents the convex hull of this geometric object.
Convex hulls, being dependent on straight lines, can be accurately represented in linear interpolations for any geometry restricted to linear interpolations.
*/
        virtual Geometry* convexHull3D(void) const = 0;

/*! \brief Get a geometric object that represents the point set intersection of this geometric object with another geometry.

There is a second version of this with a boolean parameter specifying if Z-values should be preserved.
This option is intended for clipping a polygon to a bounding box while preserving existing Z-values.
It expects the clipping polygon to have no Z values (use clearZ() or don't set them; setting it to 0 isn't adequate).
Clip edges are linearly interpolated, so use for other reasons may have undesirable elevations set.

\return A geometric object that represents the point set intersection of this geometric object with another geometry.
*/
        virtual Geometry* intersection(const Geometry* another) const;
        virtual Geometry* intersection(const Geometry* another, bool saveZ) const;

/*! \brief Get a geometric object that represents the point set intersection of this geometric object with another geometry. (This method does not ignore z values)
\return A geometric object that represents the point set intersection of this geometric object with another geometry.
*/
        virtual Geometry* intersection3D(const Geometry* another) const;

/*! \brief Get a geometric object that represents the point set union of this geometric object with another geometry.
\return A geometric object that represents the point set union of this geometric object with another geometry.
*/
        virtual Geometry* Union(const Geometry* another) const;

/*! \brief Get a geometric object that represents the point set union of this geometric object with another geometry. (This method does not ignore z values)
\return A geometric object that represents the point set union of this geometric object with another geometry.
*/
        virtual Geometry* Union3D(const Geometry* another) const;

/*! \brief Computes a Geometry representing the points making up this Geometry that do not make up other. This method returns the closure of the resultant Geometry. 
\return A geometric object that represents the point set difference of this geometric object with another geometry.
*/
        virtual Geometry* difference(const Geometry* another) const;

/*! \brief Computes a Geometry representing the points making up this Geometry that do not make up other. This method returns the closure of the resultant Geometry. (This method does not ignore z values)
\return A geometric object that represents the point set difference of this geometric object with another geometry.
*/
        virtual Geometry* difference3D(const Geometry* another) const;

/*! \brief Returns a set combining the points in this Geometry not in other, and the points in other not in this Geometry.
\return A geometric object that represents the point set symmetric difference of this geometric object with another geometry.
*/
        virtual Geometry* symDifference(const Geometry* another) const;

/*! \brief Returns a set combining the points in this Geometry not in other, and the points in other not in this Geometry. (This method does not ignore z values)
\return A geometric object that represents the point set symmetric difference of this geometric object with another geometry.
*/
        virtual Geometry* symDifference3D(const Geometry* another) const;

//***********************************************************************
// extensions

/*! \brief Identify if the geometry contents are valid for the geometry type.
\return True if valid; false otherwise.
\note This is an extension to the specification.
*/
        virtual bool isValid(void) const = 0;
        //virtual bool isValid3D(void) = 0;
        static std::string getGeometryTypeText(WKBGeometryType type);

        static WKBGeometryType sGetWKBGeometryType(WKBGeometryType type, bool withZ = false, bool withM = false);
    };

}

