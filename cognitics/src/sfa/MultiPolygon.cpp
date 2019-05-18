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

#include "sfa/MultiPolygon.h"
#include "sfa/Polygon.h"

#include <sstream>
#include <exception>
#include <stdexcept>

namespace sfa
{
    MultiPolygon::~MultiPolygon(void)
    {
    }

    MultiPolygon::MultiPolygon(void)
    {
    }

    MultiPolygon::MultiPolygon(const MultiPolygon& polygons) : MultiSurface(polygons)
    {
    }

    MultiPolygon::MultiPolygon(const MultiPolygon* polygons) : MultiSurface(polygons)
    {
    }

    MultiPolygon& MultiPolygon::operator =(const MultiPolygon& rhs)
    {
        clearGeometries();
        for(GeometryList::const_iterator it = rhs.geometries.begin(), end = rhs.geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
        return *this;
    }

    std::string MultiPolygon::getGeometryType(void) const
    {
        return "MultiPolygon";
    }

    WKBGeometryType MultiPolygon::getWKBGeometryType(bool withZ, bool withM) const
    {
        if (withZ && withM && is3D() && isMeasured())
            return wkbMultiPolygonZM;
        if (withZ && is3D())
            return wkbMultiPolygonZ;
        if (withM && isMeasured())
            return wkbMultiPolygonM;
        return wkbMultiPolygon;
    }

    void MultiPolygon::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
    //    Move into Polygon List
        is.ignore(1024,'(');

    //    Check for emtpy case
        if (is.peek() == 'E' || is.peek() == 'e')
        {
            is.ignore(1024,')'); // Move out of Polygon List
            return;
        }

    //    Parse Polygon, the check for ) will also remove the commas
        while (is.good())
        {
            Polygon* polygon = new Polygon;
            polygon->fromText(is,false,withZ,withM);
            addGeometry(polygon);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }

    void MultiPolygon::addGeometry(Geometry* geometry)
    {
        if((geometry->getGeometryType().compare("Polygon") == 0) || (geometry->getGeometryType().compare("Triangle") == 0))
        {
            GeometryCollection::addGeometry(geometry);
            return;
        }
        throw std::runtime_error("MultiPolygon::addGeometry(): attempted to add invalid geometry");
    }

    void MultiPolygon::insertGeometry(int pos, Geometry* geometry)
    {
        if((geometry->getGeometryType().compare("Polygon") == 0) || (geometry->getGeometryType().compare("Triangle") == 0))
        {
            GeometryCollection::insertGeometry(pos, geometry);
            return;
        }
        throw std::runtime_error("MultiPolygon::insertGeometry(): attempted to add invalid geometry");
    }

    double MultiPolygon::getArea(void) const
    {
        double area = 0;
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            area += dynamic_cast<Polygon*>((*it))->getArea();
        }
        return area;
    }

    Point* MultiPolygon::getCentroid(void) const
    {
        if (geometries.size() == 0) return NULL;

        //The centroid of a multipolygon can be described by the average of all the centroids
        double x = 0;
        double y = 0;
        double z = 0;

        for (GeometryList::const_iterator it = geometries.begin(); it != geometries.end(); ++it)
        {
            Point* polygonCentroid = dynamic_cast<Polygon*>((*it))->getCentroid();
            x += polygonCentroid->X();
            y += polygonCentroid->Y();
            z += polygonCentroid->Z();
            delete polygonCentroid;
        }
        x = x/double(geometries.size());
        y = y/double(geometries.size());
        z = z/double(geometries.size());
        return new Point(x,y,z);
    }

    Point* MultiPolygon::getPointOnSurface(void) const
    {
        return dynamic_cast<Polygon*>(geometries.front())->getPointOnSurface();
    }

}