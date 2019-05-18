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
#include "sfa/Polygon.h"

#include "sfa/Projection2D.h"
#include "sfa/RingMath.h"
#include "sfa/ConvexHull.h"

#include <sstream>

namespace sfa
{
    Polygon::~Polygon(void)
    {
        clearRings();
    }

    Polygon::Polygon(void)
    {
    }

    Polygon::Polygon(const Polygon& polygon)
    {
        for (LineStringList::const_iterator it = polygon.rings.begin(), end = polygon.rings.end(); it != end; it++)
            addRing(new LineString(*it));
    }

    Polygon::Polygon(const Polygon* polygon)
    {
        for (LineStringList::const_iterator it = polygon->rings.begin(), end = polygon->rings.end(); it != end; it++)
            addRing(new LineString(*it));
    }

    Polygon& Polygon::operator =(const Polygon& rhs)
    {
        clearRings();
        for (LineStringList::const_iterator it = rhs.rings.begin(), end = rhs.rings.end(); it != end; it++)
            addRing(new LineString(*it));
        return *this;
    }

    void Polygon::clearRings(void)
    {
        for (LineStringList::iterator it = rings.begin(), end = rings.end(); it != end; it++)
            delete (*it);
        rings.clear();
    }

    void Polygon::addRing(LineString* ring)
    {
        rings.push_back(ring);
    }

    void Polygon::addRing(const LineString& ring)
    {
        addRing(new LineString(&ring));
    }

    void Polygon::insertRing(int pos, LineString* ring)
    {
        rings.insert(rings.begin() + pos, ring);
    }

    void Polygon::insertRing(int pos, const LineString& ring)
    {
        insertRing(pos,new LineString(&ring));
    }

    void Polygon::insertRing(int pos, LineStringSP ring)
    {
        insertRing(pos,new LineString(ring.get()));
    }

    void Polygon::removeRing(LineString* ring)
    {
        for(LineStringList::iterator it = rings.begin(), end = rings.end(); it != end; ++it)
        {
            if(*it == ring)
            {
                delete (*it);
                it = rings.erase(it);
                --it;
            }
        }
    }

    void Polygon::removeRing(int pos)
    {
        if (pos >= 0 && pos < int(rings.size()))
            delete rings[pos];
        rings.erase(rings.begin() + pos);
    }

    void Polygon::reverse(void)
    {
        for (LineStringList::iterator it = rings.begin(), end = rings.end(); it != end; ++it)
            (*it)->reverse();
    }

    void Polygon::setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform)
    {
        if(mathTransform)
        {
            for(LineStringList::iterator it = rings.begin(), end = rings.end(); it != end; ++it)
                (*it)->setCoordinateSystem(coordinateSystem, mathTransform);
        }
        Geometry::setCoordinateSystem(coordinateSystem, mathTransform);
    }

    std::string Polygon::getGeometryType(void) const
    {
        /*
        //no interior rings and only 3 points on the exterior ring
        if (rings.size() == 1)
        {
            if (rings.front()->getNumPoints() == 4 && rings.front()->isClosed() ) return "Triangle";
        }
        */
        return "Polygon";
    }

    Geometry* Polygon::getEnvelope(void) const
    {
        return rings.front()->getEnvelope();
    }

    bool Polygon::isEmpty(void) const
    {
        return rings.empty();
    }

    bool Polygon::isSimple(void) const
    {
        //test for self intersections
        for (int i = 0; i < int(rings.size()); i++)
        {
            for (int j = i + 1; j < int(rings.size()); j++)
            {
                if (!rings[i]->isSimple())            return false;
                if (rings[i]->intersects(rings[j]))    return false;
            }
        }
        return true;
    }

    bool Polygon::is3D(void) const
    {
        for(LineStringList::const_iterator it = rings.begin(), end = rings.end(); it != end; ++it)
        {
            if((*it)->is3D())
                return true;
        }
        return false;
    }

    bool Polygon::isMeasured(void) const
    {
        for(LineStringList::const_iterator it = rings.begin(), end = rings.end(); it != end; ++it)
        {
            if((*it)->isMeasured())
                return true;
        }
        return false;
    }

    Geometry* Polygon::getBoundary(void) const
    {
        if (rings.size() == 0)
            return NULL;

        MultiLineString* result = new MultiLineString;
        for (LineStringList::const_iterator it = rings.begin(); it != rings.end(); ++it)
        {
            result->addGeometry(new LineString(*it));
        }
        return result;
    }

    double Polygon::getArea(void) const
    {
        return GetPolygonArea(this);
    }

    double Polygon::getArea3D(void) const
    {
        Projection2D proj;
        if(!proj.createBasis(this))
            return 0;
        Polygon* temp = proj.transformPolygonTo2D(this);
        double result = temp->getArea();
        delete temp;
        return result;
        
    }

    Point* Polygon::getCentroid(void) const
    {
        return new Point(GetPolygonCentroid(this));
    }

    Point* Polygon::getCentroid3D(void) const
    {
        Projection2D proj;
        if(!proj.createBasis(this))
            return NULL;
        Polygon* temp = proj.transformPolygonTo2D(this);
        Point* result = temp->getCentroid();
        Point* result3D = proj.transformPointTo3D(result);
        delete result;
        delete temp;
        return result3D;
    }

    Point* Polygon::getPointOnSurface(void) const
    {
        if(rings.size() == 0) return NULL;

        //try the center of the bounding box
        LineString* envelope = static_cast<LineString*>(getEnvelope());
        Point center(    0.5*(envelope->getPointN(1)->X() + envelope->getPointN(0)->X()),
                        0.5*(envelope->getPointN(1)->Y() + envelope->getPointN(0)->Y()),
                        0.5*(envelope->getPointN(1)->Z() + envelope->getPointN(0)->Z()) );
        delete envelope;
        if (intersects(&center))
            return new Point(center);

    /*
        If that fails...run an intersection of the diagonal with the polygon and return a point that is 
        in along the first resulting LineString.
    */

        GeometryCollection* resultingDiagonal = dynamic_cast<GeometryCollection*>(intersection(getEnvelope()));

        if (resultingDiagonal)
        {
            LineString* lineFrag = NULL;
            for (int i = 1; i <= resultingDiagonal->getNumGeometries(); i++)
            {
                lineFrag = dynamic_cast<LineString*>(resultingDiagonal->getGeometryN(i));
                if (lineFrag)
                    break;
            }

            delete resultingDiagonal;

            if (lineFrag)
            {
                return new Point(    0.5*(lineFrag->getPointN(1)->X() + lineFrag->getPointN(0)->X()),
                                    0.5*(lineFrag->getPointN(1)->Y() + lineFrag->getPointN(0)->Y()),
                                    0.5*(lineFrag->getPointN(1)->Z() + lineFrag->getPointN(0)->Z()) );
            }
        }

        //if that fails, use the corner (if this step is needed, a rounding error was most likely encountered)
        return new Point(rings.front()->getPointN(0));
    }

    LineString* Polygon::getExteriorRing(void) const
    {
        if (rings.size() == 0)
            return NULL;
        return rings.front();
    }

    int Polygon::getNumInteriorRing(void) const
    {
        if (rings.size() <= 0) return 0;
        else return (int(rings.size()) - 1);
    }

    LineString* Polygon::getInteriorRingN(int n) const
    {
        if ((n+1) < 0 || (n+1) > int(rings.size()))
            throw std::runtime_error("Polygon::getInteriorRingN(): index out of bounds!");
        else return rings[n+1];
    }

    Geometry* Polygon::convexHull(void) const
    {
        return MelkmanHull::apply(getExteriorRing());
    }

    Geometry* Polygon::convexHull3D(void) const
    {
        Projection2D proj;
        if(!proj.createBasis(this))
        {
            return NULL;
        }
        Polygon* temp = proj.transformPolygonTo2D(this);
        Geometry* result = temp->convexHull();
        Geometry* result3D = proj.transformGeometryTo3D(result);
        delete result;
        delete temp;
        return result3D;
    }

    bool Polygon::isValid(void) const
    {
        if(rings.size() == 0) return true;
        if (!isSimple()) return false;
        if (!IsPolygonConsistent(this)) return false;
        if (ComputeOrientation(getExteriorRing()) != ORIENTATION_C_CW) return false;
        return true;
    }

    WKBGeometryType Polygon::getWKBGeometryType(bool withZ, bool withM) const
    {
        /*
        if(getGeometryType() == "Triangle")
        {
            if (withZ && withM && is3D() && isMeasured())
                return wkbTriangleZM;
            if (withZ && is3D())
                return wkbTriangleZ;
            if (withM && isMeasured())
                return wkbTriangleM;
            return wkbTriangle;
        }
        */
        if (withZ && withM && is3D() && isMeasured())
            return wkbPolygonZM;
        if (withZ && is3D())
            return wkbPolygonZ;
        if (withM && isMeasured())
            return wkbPolygonM;
        return wkbPolygon;
    }

    void Polygon::toText(std::ostream &os, bool tag, bool withZ, bool withM) const
    {
        if(tag)
        {
            Geometry::toText(os, tag, withZ, withM);
            os << " ";
        }
        os << "(";
        if(isEmpty())
        {
            os << "EMPTY";
        }
        else
        {
            for(LineStringList::const_iterator it = rings.begin(), end = rings.end(); it != end; ++it)
            {
                if(it != rings.begin())
                    os << ",";
                (*it)->toText(os, false, withZ, withM);
            }
        }
        os << ")";
    }

    void Polygon::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
    //    Move into LineString List
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
            addRing(line);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }

    void Polygon::toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const
    {
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> numRings(ccl::uint32_t(rings.size()));
            os << numRings;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> numRings(ccl::uint32_t(rings.size()));
            os << numRings;
        }
        for(LineStringList::const_iterator it = rings.begin(), end = rings.end(); it != end; ++it)
        {
            (*it)->toBinary(os, byteOrder, withZ, withM);
        }
    }

    void Polygon::fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM)
    {
        ccl::uint32_t numRings;
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> tempRings;
            is >> tempRings;
            numRings = tempRings;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> tempRings;
            is >> tempRings;
            numRings = tempRings;
        }
        for(ccl::uint32_t i = 0; i < numRings; ++i)
        {
            LineString* ring = new LineString();
            ring->fromBinary(is, byteOrder, withZ, withM);
            addRing(ring);
        }
    }

    bool Polygon::isParentOf(const GeometryBase *child) const
    {
        if(!child)
            return false;
        if(getExteriorRing() == child)
            return true;
        for(int i = 0, c = getNumInteriorRing(); i < c; ++i)
        {
            if(getInteriorRingN(i) == child)
                return true;
        }
        return false;
    }

    GeometryBase *Polygon::getParentOf(const GeometryBase *child) const
    {
        if(isParentOf(child))
            return const_cast<Polygon *>(this);
        GeometryBase *match = (getExteriorRing()) ? getExteriorRing()->getParentOf(child) : NULL;
        if(match)
            return match;
        for(int i = 0, c = getNumInteriorRing(); i < c; ++i)
        {
            match = getInteriorRingN(i)->getParentOf(child);
            if(match)
                return match;
        }
        return NULL;
    }

    int Polygon::getNumChildren(void) const
    {
        int c = (getExteriorRing()) ? 1 : 0;
        return c + getNumInteriorRing();
    }


    void Polygon::clean(double minDistance2)
    {
        this->getExteriorRing()->clean();
    }

    void Polygon::close()
    {
        for (LineStringList::iterator it = rings.begin(), end = rings.end(); it != end; ++it)
        {
            sfa::LineString *ls = *it;
            if (ls->getNumPoints() < 2)
                return;
            sfa::Point *first = ls->getPointN(0);
            sfa::Point *last = ls->getPointN(ls->getNumPoints()-1);
            sfa::Point diff = (*last) - (*first);
            if (diff.length2()>SFA_EPSILON)
            {
                ls->addPoint(*first);
            }
        }
    }

    void Polygon::transform(const sfa::Matrix &xform)
    {
        for (LineStringList::iterator it = rings.begin(), end = rings.end(); it != end; ++it)
        {
            sfa::LineString *ls = *it;
            ls->transform(xform);
        }
    }

    std::list<sfa::LineString *> Polygon::getRingList(void)
    {
        std::list<sfa::LineString *> ring_list;
        ring_list.insert(ring_list.end(), rings.begin(), rings.end());
        return ring_list;
    }

}