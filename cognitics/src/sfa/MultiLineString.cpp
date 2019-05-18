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

#include "sfa/MultiLineString.h"
#include "sfa/LineString.h"

#include <sstream>
#include <exception>
#include <stdexcept>

namespace sfa
{
    MultiLineString::~MultiLineString(void)
    {
    }

    MultiLineString::MultiLineString(void)
    {
    }

    MultiLineString::MultiLineString(const MultiLineString& lines) : MultiCurve(lines)
    {
    }

    MultiLineString::MultiLineString(const MultiLineString* lines) : MultiCurve(lines)
    {
    }

    MultiLineString& MultiLineString::operator=(const MultiLineString& rhs)
    {
        clearGeometries();
        for(GeometryList::const_iterator it = rhs.geometries.begin(), end = rhs.geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
        return *this;
    }

    std::string MultiLineString::getGeometryType(void) const
    {
        return "MultiLineString";
    }

    WKBGeometryType MultiLineString::getWKBGeometryType(bool withZ, bool withM) const
    {
        if (withZ && withM && is3D() && isMeasured())
            return wkbMultiLineStringZM;
        if (withZ && is3D())
            return wkbMultiLineStringZ;
        if (withM && isMeasured())
            return wkbMultiLineStringM;
        return wkbMultiLineString;
    }

    void MultiLineString::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
    //    Move into LineString list
        is.ignore(1024,'(');

    //    Check for emtpy case 
        if (is.peek() == 'E' || is.peek() == 'e')
        {
            is.ignore(1024,')'); // Move out of LineString List
            return;
        }

    //    Parse LineStrings, the check for ) will also remove the commas
        while (is.good())
        {
            LineString* line = new LineString;
            line->fromText(is,false,withZ,withM);
            addGeometry(line);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }
    
    void MultiLineString::addGeometry(Geometry* geometry)
    {
        if((geometry->getGeometryType().compare("LineString") == 0) || (geometry->getGeometryType().compare("Line") == 0) || (geometry->getGeometryType().compare("LinearRing") == 0))
        {
            GeometryCollection::addGeometry(geometry);
            return;
        }
        throw std::runtime_error("MultiLineString::addGeometry(): attempted to add invalid geometry");
    }

    void MultiLineString::insertGeometry(int pos, Geometry* geometry)
    {
        if((geometry->getGeometryType().compare("LineString") == 0) || (geometry->getGeometryType().compare("Line") == 0) || (geometry->getGeometryType().compare("LinearRing") == 0))
        {
            GeometryCollection::insertGeometry(pos, geometry);
            return;
        }
        throw std::runtime_error("MultiLineString::insertGeometry(): attempted to add invalid geometry");
    }

}