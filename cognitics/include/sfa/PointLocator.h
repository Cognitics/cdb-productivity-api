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
/*! \brief Provides sfa::PointLocator
\author Joshua Anghel <janghel@cognitics.net>
\date 14 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "Label.h"
#include "Point.h"
#include "LineString.h"

namespace sfa {

/*!\class PointLocator PointLocator.h PointLocator.h
\brief PointLocator

Provides methods to find the Location of a Point on a given Geometry.

Usage:
\code
    sfa::GeometrySP geometry;
    sfa::PointSP point;
    sfa::Location loc;

//    Calling the static method will automatically create a PointLocator class and initialize the search, returning the correct location
//    based on the most efficient way to handled whatever geometry is given.
    loc = sfa::PointLocator::apply(point,Geometry);
\endcode

Note that this is a 2D method. It will only use the x and y values of the input Geometries, effectively flattening them when in use.

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class PointLocator
    {
    protected:
        PointLocator(void): loc(EXTERIOR) {}
        ~PointLocator(void) {}

        int LineBoundaryCount;
        int AreaBoundaryCount;

        Location loc;

/*!\brief PointOnLine
\param p Point to test
\param line LineString to test
\return Returns True if p is found to be on line.
*/
        bool PointOnLine(const Point* p, const LineString* line);

/*!\brief updateLocation
Updates the current known location of this point using the mod-2 boundary rule.
\param l Location to update from
*/
        void updateLocation(Location l);

/*!\brief Point_Point
A Point is located on the INTERIOR of another Point if they are equals, otherwise it lies in the EXTERIOR.
\param p Point to find Location for
\param geom Point to test Location on
\return Location of p on geom
*/
        Location Point_Point(const Point* p, const Geometry* geom);

/*!\brief Point_LineString
A Point is located on the BOUNDARY of a LineString if it is equal to one and only one of the endPoints. Otherwise
it is on the INTERIOR if it lies on the LineString and the EXTERIOR otherwise.
\param p Point to find Location for
\param geom LineString to test Location on
\return Location of p on geom
*/
        Location Point_LineString(const Point* p, const Geometry* geom);

/*!\brief Point_Polygon
A Point is located on the BOUNDARY of a Polygon if it lies on one of the LineStrings of the Polygon, otherwise it 
lies on the INTERIOR if it is inside and EXTERIOR if not.
\param p Point to find Location for
\param geom Polygon to test Location on
\return Location of p on geom
*/
        Location Point_Polygon(const Point* p, const Geometry* geom);

/*!\brief Point_Geometry
Determines what type of Geometry geom is and returns the value given from the appropriate method.
\param p Point to find Location for
\param geom Geometry to test Location on
\return Location of p on geom
*/
        Location Point_Geometry(const Point* p, const Geometry* geom);
        
/*!\brief Point_Multi
Updates the current known Location of the Point by itterating through each geometry of the GeometryCollection given.
\param p Point to find Location for
\param geom GeometryCollection to test Location on
\return Location of p on geom
*/
        Location Point_Multi(const Point* p, const Geometry* geom);

        Location Point_PolySurface(const Point* p, const Geometry* geom);

    public:
/*!\brief intersects
Optimized method of finding if a Point intersects another Geometry.
\param p Point to test for intersetion
\param geom Geometry to test for intersection
\return True if p intersects geom
*/
        static bool intersects(const Point* p, const Geometry* geom);

/*!\brief apply
Finds the Location of p on Geometry geom
\param p Point to find Location for
\param geom Geometry to find Location on
\return Location of p on Geometry geom
*/
        static Location apply(const Point* p, const Geometry* geom);
    };

}