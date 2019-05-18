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
#include "sfa/MultiPoint.h"
#include "sfa/Point.h"

#include <sstream>
#include <exception>
#include <stdexcept>

namespace sfa
{
    MultiPoint::~MultiPoint(void)
    {
    }

    MultiPoint::MultiPoint(void)
    {
    }

    MultiPoint::MultiPoint(const MultiPoint& points) : GeometryCollection(points)
    {
    }

    MultiPoint::MultiPoint(const MultiPoint* points) : GeometryCollection(points)
    {
    }

    MultiPoint& MultiPoint::operator =(const MultiPoint& rhs)
    {
        clearGeometries();
        for(GeometryList::const_iterator it = rhs.geometries.begin(), end = rhs.geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
        return *this;
    }

    int MultiPoint::getDimension(void) const
    {
        return 0;
    }

    std::string MultiPoint::getGeometryType(void) const
    {
        return "MultiPoint";
    }

    Geometry* MultiPoint::getBoundary(void) const
    {
        return NULL;
    }

    void MultiPoint::addGeometry(Geometry* geometry)
    {
        if(geometry->getGeometryType().compare("Point") != 0)
            throw std::runtime_error("MultiPoint::addGeometry(): attempted to add invalid geometry");
        GeometryCollection::addGeometry(geometry);
    }

    void MultiPoint::insertGeometry(int pos, Geometry* geometry)
    {
        if(geometry->getGeometryType().compare("Point") != 0)
            throw std::runtime_error("MultiPoint::insertGeometry(): attempted to add invalid geometry");
        GeometryCollection::insertGeometry(pos, geometry);
    }

    WKBGeometryType MultiPoint::getWKBGeometryType(bool withZ, bool withM) const
    {
        if (withZ && withM && is3D() && isMeasured())
            return wkbMultiPointZM;
        if (withZ && is3D())
            return wkbMultiPointZ;
        if (withM && isMeasured())
            return wkbMultiPointM;
        return wkbMultiPoint;
    }

    void MultiPoint::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
    //    Move into Point List
        is.ignore(1024,'(');

    //    Check for emtpy case
        if (is.peek() == 'E' || is.peek() == 'e')
        {
            is.ignore(1024,')'); // Move out of Point List
            return;
        }

    //    Parse Points, the check for ) will also remove the commas
        while (is.good())
        {
            Point* point = new Point;
            point->fromText(is,false,withZ,withM);
            addGeometry(point);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }

}