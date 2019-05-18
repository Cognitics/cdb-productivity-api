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

#include "sfa/GeometryCollection.h"
#include "sfa/Point.h"
#include "sfa/MultiPoint.h"
#include "sfa/Curve.h"
#include "sfa/LineString.h"
#include "sfa/MultiCurve.h"
#include "sfa/MultiLineString.h"
#include "sfa/Surface.h"
#include "sfa/Polygon.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/MultiSurface.h"
#include "sfa/MultiPolygon.h"

#include "sfa/ConvexHull.h"
#include "sfa/ConvexHull3D.h"

#include <sstream>

//#pragma optimize( "", off )

namespace sfa
{
    GeometryCollection::~GeometryCollection(void)
    {
        clearGeometries();
    }

    GeometryCollection::GeometryCollection(void)
    {
    }

    GeometryCollection::GeometryCollection(const GeometryCollection& collection)
    {
        for(GeometryList::const_iterator it = collection.geometries.begin(), end = collection.geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
    }

    GeometryCollection::GeometryCollection(const GeometryCollection* collection)
    {
        for(GeometryList::const_iterator it = collection->geometries.begin(), end = collection->geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
    }

    GeometryCollection& GeometryCollection::operator=(const GeometryCollection& rhs)
    {
        clearGeometries();
        for(GeometryList::const_iterator it = rhs.geometries.begin(), end = rhs.geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
        return *this;
    }

    void GeometryCollection::setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform)
    {
        if(mathTransform)
        {
            for(GeometryList::iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
                (*it)->setCoordinateSystem(coordinateSystem, mathTransform);
        }
        Geometry::setCoordinateSystem(coordinateSystem, mathTransform);
    }

    int GeometryCollection::getDimension(void) const
    {
        int dimension = 0;
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            int dim = (*it)->getDimension();
            if(dim > dimension)
                dimension = dim;
        }
        return dimension;
    }

    std::string GeometryCollection::getGeometryType(void) const
    {
        return "GeometryCollection";
    }

    Geometry* GeometryCollection::getEnvelope(void) const
    {
        if(isEmpty()) return NULL;

        double minX, maxX, minY, maxY, minZ, maxZ, minM, maxM;
        minX = maxX = minY = maxY = minZ = maxZ = minM = maxM = 0;
        LineString* line = dynamic_cast<LineString*>(getGeometryN(1)->getEnvelope());
        Point* point1 = line->getPointN(0);
        Point* point2 = line->getPointN(1);

        minX = std::min<double>(point1->X(),point2->X());
        maxX = std::max<double>(point1->X(),point2->X());
        minY = std::min<double>(point1->Y(),point2->Y());
        maxY = std::max<double>(point1->Y(),point2->Y());
        minZ = std::min<double>(point1->Z(),point2->Z());
        maxZ = std::max<double>(point1->Z(),point2->Z());
        minM = std::min<double>(point1->M(),point2->M());
        maxM = std::max<double>(point1->M(),point2->M());

        delete line;

        for(int i = 2; i <= getNumGeometries(); i++)
        {
            LineString* line = dynamic_cast<LineString* >(getGeometryN(i)->getEnvelope());
            if (!line) continue;

            Point* min = line->getPointN(0);
            Point* max = line->getPointN(1);
            if(min->X() < minX)
                minX = min->X();
            if(max->X() > maxX)
                maxX = max->X();
            if(min->Y() < minY)
                minY = min->Y();
            if(max->Y() > maxY)
                maxY = max->Y();
            if(line->is3D())
            {
                if(min->Z() < minZ)
                    minZ = min->Z();
                if(max->Z() > maxZ)
                    maxZ = max->Z();
            }
            if(line->isMeasured())
            {
                if(min->M() < minM)
                    minM = min->M();
                if(max->M() > maxM)
                    maxM = max->M();
            }

            delete line;
        }

        Point* minPoint = new Point(minX, minY);
        Point* maxPoint = new Point(maxX, maxY);
        if(is3D())
        {
            minPoint->setZ(minZ);
            maxPoint->setZ(maxZ);
        }
        if(isMeasured())
        {
            minPoint->setM(minM);
            maxPoint->setM(maxM);
        }

        LineString* envelope = new LineString();
        envelope->addPoint(minPoint);
        envelope->addPoint(maxPoint);
        return envelope;
    }

    bool GeometryCollection::isEmpty(void) const
    {
        return geometries.empty();
    }

    bool GeometryCollection::isSimple(void) const
    {
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            if(!(*it)->isSimple())
                return false;
        }
        return true;
    }

    bool GeometryCollection::is3D(void) const
    {
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            if((*it)->is3D())
                return true;
        }
        return false;
    }

    bool GeometryCollection::isMeasured(void) const
    {
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            if((*it)->isMeasured())
                return true;
        }
        return false;
    }

    Geometry* GeometryCollection::getBoundary(void) const
    {
        //    SymDifference itteration of Boundaries will result in a mod 2 rule.
        Geometry* result = NULL;
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            Geometry* nextBoundary = (*it)->getBoundary();
            if (nextBoundary) 
            {
                Geometry* newResult = nextBoundary->symDifference(result);
                if (result) delete result;
                delete nextBoundary;
                result = newResult;
            }
        }
        if (!result)
            return NULL;
        if (result->isEmpty())
        {
            delete result;
            return NULL;
        }
        else return result;
    }

    Geometry* GeometryCollection::convexHull(void) const
    {
        MultiPoint isomorph;
        //transform the collection into a MultiPoint
        for (GeometryList::const_iterator it = geometries.begin(); it != geometries.end(); ++it)
        {
            if (dynamic_cast<Point*>(*it))
            {
                isomorph.addGeometry(new Point(dynamic_cast<Point*>(*it)));
            }
            else if (dynamic_cast<LineString*>(*it))
            {
                LineString* line = dynamic_cast<LineString*>(*it);
                for (int i = 0; i < line->getNumPoints(); i++)
                    isomorph.addGeometry(new Point(line->getPointN(i)));
            }
            else if (dynamic_cast<Polygon*>(*it))
            {
                Polygon* polygon = dynamic_cast<Polygon*>(*it);
                LineString* line = polygon->getExteriorRing();

                for (int i = 0; i < line->getNumPoints(); i++)
                    isomorph.addGeometry(new Point(line->getPointN(i)));
            }
            else if (dynamic_cast<PolyhedralSurface*>(*it))
            {
                PolyhedralSurface* surface = dynamic_cast<PolyhedralSurface*>(*it);
            
                for (int j = 0; j < surface->getNumPatches(); j++)
                {
                    Polygon* patch = surface->getPatchN(j);
                    LineString* line = patch->getExteriorRing();

                    for (int i = 0; i < line->getNumPoints(); i++)
                        isomorph.addGeometry(new Point(line->getPointN(i)));
                }
            }
        }
        return GrahamHull::apply(&isomorph);

        return NULL;
    }

    Geometry* GeometryCollection::convexHull3D(void) const
    {
        MultiPoint isomorph;
        //transform the collection into a MultiPoint
        for (GeometryList::const_iterator it = geometries.begin(); it != geometries.end(); ++it)
        {
            if (dynamic_cast<Point*>(*it))
            {
                isomorph.addGeometry(new Point(dynamic_cast<Point*>(*it)));
            }
            else if (dynamic_cast<LineString*>(*it))
            {
                LineString* line = dynamic_cast<LineString*>(*it);
                for (int i = 0; i < line->getNumPoints(); i++)
                    isomorph.addGeometry(new Point(line->getPointN(i)));
            }
            else if (dynamic_cast<Polygon*>(*it))
            {
                Polygon* polygon = dynamic_cast<Polygon*>(*it);
                LineString* line = polygon->getExteriorRing();

                for (int i = 0; i < line->getNumPoints(); i++)
                    isomorph.addGeometry(new Point(line->getPointN(i)));
            }
            else if (dynamic_cast<PolyhedralSurface*>(*it))
            {
                PolyhedralSurface* surface = dynamic_cast<PolyhedralSurface*>(*it);
            
                for (int j = 0; j < surface->getNumPatches(); j++)
                {
                    Polygon* patch = surface->getPatchN(j);
                    LineString* line = patch->getExteriorRing();

                    for (int i = 0; i < line->getNumPoints(); i++)
                        isomorph.addGeometry(new Point(line->getPointN(i)));
                }
            }
        }
        return ConvexHull3D::apply(&isomorph);
    }

    int GeometryCollection::getNumGeometries(void) const
    {
        return int(geometries.size());
    }

    Geometry* GeometryCollection::getGeometryN(int n) const
    {
        return geometries.at(n - 1);
    }

    void GeometryCollection::clearGeometries(void)
    {
        for (GeometryList::iterator it = geometries.begin(), end = geometries.end(); it != end; it++)
            delete (*it);
        geometries.clear();
    }

    void GeometryCollection::addGeometry(Geometry* geometry)
    {
        geometries.push_back(geometry);
    }

    void GeometryCollection::insertGeometry(int pos, Geometry* geometry)
    {
        geometries.insert(geometries.begin() + pos, geometry);
    }

    void GeometryCollection::removeGeometry(Geometry* geometry)
    {
        bool deleted_geom = false;
        for(GeometryList::iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            if(*it == geometry)
            {
                if (!deleted_geom)
                {
                    delete (*it);
                    deleted_geom = true;
                }
                it = geometries.erase(it);
                // Erasing an item from the middle of an iterator loop isn't a
                it = geometries.begin();
                end = geometries.end();
                if (it == end)
                    break;
            }
        }
    }

    void GeometryCollection::removeGeometry(int pos)
    {
        if (pos >= 0 && pos < int(geometries.size()))
            delete geometries[pos];
        geometries.erase(geometries.begin() + pos);
    }

    bool GeometryCollection::isValid(void) const
    {
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            if(!(*it)->isValid())
                return false;
        }
        return true;
    }

    WKBGeometryType GeometryCollection::getWKBGeometryType(bool withZ, bool withM) const
    {
        if (withZ && withM && is3D() && isMeasured())
            return wkbGeometryCollectionZM;
        if (withZ && is3D())
            return wkbGeometryCollectionZ;
        if (withM && isMeasured())
            return wkbGeometryCollectionM;
        return wkbGeometryCollection;
    }

    void GeometryCollection::toText(std::ostream &os, bool tag, bool withZ, bool withM) const
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
            tag = (getGeometryType() == "GeometryCollection");    // this just saves having to reimplement this for all the other multi geometries
            for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
            {
                if(it != geometries.begin())
                    os << ",";
                (*it)->toText(os, tag, withZ, withM);
            }
        }
        os << ")";
    }

    void GeometryCollection::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
    //    Move into Geometry List
        is.ignore(1024,'(');

    //    Check for empty case
        if (is.peek() == 'E' || is.peek() == 'e')
        {
            is.ignore(1024,')'); // Move out of Geometry List
            return;
        }

    //    Parse Geometries using their tags
        while (is.good())
        {
            Geometry*    geometry = NULL;
            std::string tag;
            is >> tag;

        //    Convert to lower case
            for (int i = 0; i < int(tag.length()); i++) tag[i] = tolower(tag[i]);

        //    Parse tag
            if (tag.compare("point")==0) geometry = new Point;
            else if (tag.compare("linestring")==0) geometry = new LineString;
            else if (tag.compare("polygon")==0 || tag.compare("triangle") == 0) geometry = new Polygon;
            else if (tag.compare("multipoint")==0) geometry = new MultiPoint;
            else if (tag.compare("multilinestring")==0) geometry = new MultiLineString;
            else if (tag.compare("multipolygon")==0) geometry = new MultiPolygon;
            else if (tag.compare("geometrycollection")==0) geometry = new GeometryCollection;
            else if (tag.compare("polyhedralsurface")==0 || tag.compare("tin")==0 || tag.compare("polyhdedron")==0) geometry = new PolyhedralSurface;
            else throw std::runtime_error("GeometryCollection::fromText Unrecognized Geometry tag!");

            geometry->fromText(is,true,withZ,withM);
            addGeometry(geometry);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }

    void GeometryCollection::toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const
    {
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> numGeometries(ccl::uint32_t(geometries.size()));
            os << numGeometries;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> numGeometries(ccl::uint32_t(geometries.size()));
            os << numGeometries;
        }
        for(GeometryList::const_iterator it = geometries.begin(), end = geometries.end(); it != end; ++it)
        {
            os << (char)byteOrder;
            if(byteOrder == wkbXDR)
            {
                ccl::BigEndian<ccl::uint32_t> geometryType((*it)->getWKBGeometryType(withZ, withM));
                os << geometryType;
            }
            else
            {
                ccl::LittleEndian<ccl::uint32_t> geometryType((*it)->getWKBGeometryType(withZ, withM));
                os << geometryType;
            }
            (*it)->toBinary(os, byteOrder, withZ, withM);
        }
    }

    void GeometryCollection::fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM)
    {
        ccl::uint32_t numGeometries;
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> tempGeometries;
            is >> tempGeometries;
            numGeometries = tempGeometries;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> tempGeometries;
            is >> tempGeometries;
            numGeometries = tempGeometries;
        }
        for(ccl::uint32_t i = 0; i < numGeometries; ++i)
        {
            byteOrder = (WKBByteOrder)is.get();
            ccl::uint32_t geometryType;
            if(byteOrder == wkbXDR)
            {
                ccl::BigEndian<ccl::uint32_t> tempType;
                is >> tempType;
                geometryType = tempType;
            }
            else
            {
                ccl::LittleEndian<ccl::uint32_t> tempType;
                is >> tempType;
                geometryType = tempType;
            }
            Geometry* geometry = NULL;
            switch(geometryType)
            {
                case wkbPoint:
                case wkbPointZ:
                case wkbPointM:
                case wkbPointZM:
                    geometry = new Point();
                    break;
                case wkbLineString:
                case wkbLineStringZ:
                case wkbLineStringM:
                case wkbLineStringZM:
                    geometry = new LineString();
                    break;
                case wkbPolygon:
                case wkbPolygonZ:
                case wkbPolygonM:
                case wkbPolygonZM:
                case wkbTriangle:
                case wkbTriangleZ:
                case wkbTriangleM:
                case wkbTriangleZM:
                    geometry = new Polygon();
                    break;
                case wkbMultiPoint:
                case wkbMultiPointZ:
                case wkbMultiPointM:
                case wkbMultiPointZM:
                    geometry = new MultiPoint();
                    break;
                case wkbMultiLineString:
                case wkbMultiLineStringZ:
                case wkbMultiLineStringM:
                case wkbMultiLineStringZM:
                    geometry = new MultiLineString();
                    break;
                case wkbMultiPolygon:
                case wkbMultiPolygonZ:
                case wkbMultiPolygonM:
                case wkbMultiPolygonZM:
                    geometry = new MultiPolygon();
                    break;
                case wkbGeometryCollection:
                case wkbGeometryCollectionZ:
                case wkbGeometryCollectionM:
                case wkbGeometryCollectionZM:
                    geometry = new GeometryCollection();
                    break;
                case wkbPolyhedralSurface:
                case wkbPolyhedralSurfaceZ:
                case wkbPolyhedralSurfaceM:
                case wkbPolyhedralSurfaceZM:
                case wkbTIN:
                case wkbTINZ:
                case wkbTINM:
                case wkbTINZM:
                    geometry = new PolyhedralSurface();
                    break;
            }
            if(geometry)
            {
                geometry->fromBinary(is, byteOrder, withZ, withM);
                addGeometry(geometry);
            }
        }
    }

    bool GeometryCollection::isParentOf(const GeometryBase *child) const
    {
        if(!child)
            return false;
        for(int i = 0, c = getNumGeometries(); i < c; ++i)
        {
            if(getGeometryN(i) == child)
                return true;
        }
        return false;
    }

    GeometryBase *GeometryCollection::getParentOf(const GeometryBase *child) const
    {
        if(isParentOf(child))
            return const_cast<GeometryCollection *>(this);
        for(int i = 0, c = getNumGeometries(); i < c; ++i)
        {
            GeometryBase *match = getGeometryN(i)->getParentOf(child);
            if(match)
                return match;
        }
        return NULL;
    }

    int GeometryCollection::getNumChildren(void) const
    {
        return getNumGeometries();
    }

}