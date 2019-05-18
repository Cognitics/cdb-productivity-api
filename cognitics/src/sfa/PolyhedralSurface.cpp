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

#include "sfa/PolyhedralSurface.h"
#include "sfa/MultiPoint.h"

#include "sfa/PointMath.h"
#include "sfa/PatchUnion.h"
#include "sfa/ConvexHull.h"
#include "sfa/ConvexHull3D.h"

#include <sstream>

namespace sfa
{
    PolyhedralSurface::~PolyhedralSurface(void)
    {
        clearPatches();
    }

    PolyhedralSurface::PolyhedralSurface(void)
    {
    }

    PolyhedralSurface::PolyhedralSurface(const PolyhedralSurface& surface)
    {
        for (PolygonList::const_iterator it = surface.patches.begin(), end = surface.patches.end(); it != end; it++)
            addPatch(new Polygon(*it));
    }

    PolyhedralSurface::PolyhedralSurface(const PolyhedralSurface* surface)
    {
        for (PolygonList::const_iterator it = surface->patches.begin(), end = surface->patches.end(); it != end; it++)
            addPatch(new Polygon(*it));
    }

    PolyhedralSurface& PolyhedralSurface::operator =(const PolyhedralSurface& rhs)
    {
        clearPatches();
        for (PolygonList::const_iterator it = rhs.patches.begin(), end = rhs.patches.end(); it != end; it++)
            addPatch(new Polygon(*it));
        return *this;
    }

    void PolyhedralSurface::clearPatches(void)
    {
        for (PolygonList::iterator it = patches.begin(), end = patches.end(); it != end; it++)
            delete (*it);

        patches.clear();
    }

    void PolyhedralSurface::addPatch(Polygon* patch)
    {
        patches.push_back(patch);
    }

    void PolyhedralSurface::addPatch(const Polygon& patch)
    {
        addPatch(new Polygon(&patch));
    }

    void PolyhedralSurface::insertPatch(int pos, Polygon* patch)
    {
        patches.insert(patches.begin() + pos, patch);
    }

    void PolyhedralSurface::insertPatch(int pos, const Polygon& patch)
    {
        insertPatch(pos,new Polygon(&patch));
    }

    void PolyhedralSurface::removePatch(Polygon* patch)
    {
        for(PolygonList::iterator it = patches.begin(), end = patches.end(); it != end; ++it)
        {
            if(*it == patch)
            {
                delete (*it);
                it = patches.erase(it);
                --it;
            }
        }
    }

    void PolyhedralSurface::removePatch(int pos)
    {
        if (pos >= 0 && pos < int(patches.size()))
            delete patches[pos];

        patches.erase(patches.begin() + pos);
    }

    void PolyhedralSurface::setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform)
    {
        if(mathTransform)
        {
            for(PolygonList::iterator it = patches.begin(), end = patches.end(); it != end; ++it)
                (*it)->setCoordinateSystem(coordinateSystem, mathTransform);
        }
        Geometry::setCoordinateSystem(coordinateSystem, mathTransform);
    }

    std::string PolyhedralSurface::getGeometryType(void) const
    {
        //Tin if all the polygons are triangles, simple and valid are other tests all together
        for (PolygonList::const_iterator it = patches.begin(); it != patches.end(); ++it)
        {
            if ((*it)->getWKBGeometryType(false,false) != wkbTriangle) return "PolyhedralSurface";
            //if ((*it)->getGeometryType() != "Triangle") return "PolyhedralSurface";
        }
        return "TIN";
    }

    Geometry* PolyhedralSurface::getEnvelope(void) const
    {
        if(isEmpty())
            return NULL;

        double minX, maxX, minY, maxY, minZ, maxZ, minM, maxM;
        minX = maxX = minY = maxY = minZ = maxZ = minM = maxM = 0;

        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
        {
            LineString* line = dynamic_cast<LineString*>((*it)->getEnvelope());
            if (!line) continue;

            Point* min = (line->getPointN(0));
            Point* max = (line->getPointN(1));
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

    bool PolyhedralSurface::isEmpty(void) const
    {
        return patches.empty();
    }

    bool PolyhedralSurface::isSimple(void) const
    {
        for (int i = 0; i < int(patches.size()); i++)
        {
            for (int j = i + 1; j < int(patches.size()); j++)
            {
                if (!patches[i]->isSimple())            return false;
                if (patches[i]->overlaps(patches[j]))    return false;
            }
        }
        return true;
    }

    bool PolyhedralSurface::is3D(void) const
    {
        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
        {
            if((*it)->is3D())
                return true;
        }
        return false;
    }

    bool PolyhedralSurface::isMeasured(void) const
    {
        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
        {
            if((*it)->isMeasured())
                return true;
        }
        return false;
    }

    Geometry* PolyhedralSurface::getBoundary(void) const
    {
        return PatchUnion::apply(patches);
    }

    Geometry* PolyhedralSurface::convexHull(void) const
    {
        MultiPoint mPoint;

        for (PolygonList::const_iterator it = patches.begin(); it != patches.end(); ++it)
        {
            //only need the exterior ring to make a convex hull
            LineString* ring = (*it)->getExteriorRing();
            for (int i = 0; i < ring->getNumPoints(); i++)
            {
                mPoint.addGeometry(new Point(ring->getPointN(i)));
            }
        }

        return GrahamHull::apply(&mPoint);
    }

    Geometry* PolyhedralSurface::convexHull3D(void) const
    {
        MultiPoint mPoint;

        for (PolygonList::const_iterator it = patches.begin(); it != patches.end(); ++it)
        {
            //only need the exterior ring to make a convex hull
            LineString* ring = (*it)->getExteriorRing();
            for (int i = 0; i < ring->getNumPoints(); i++)
            {
                mPoint.addGeometry(new Point(ring->getPointN(i)));
            }
        }

        return ConvexHull3D::apply(&mPoint);
    }

    double PolyhedralSurface::getArea(void) const
    {
        double area = 0;
        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
            area += (*it)->getArea();
        return area;
    }

    double PolyhedralSurface::getArea3D(void) const
    {
        double area = 0;
        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
            area += (*it)->getArea3D();
        return area;
    }

    Point* PolyhedralSurface::getCentroid(void) const
    {
        //The centroid of a polyhedral surface can be described as the centroid of all the
        //centroids of its polygons. This reduces to the centroid of a multipoint, which is
        //the average location of the points
        double x = 0;
        double y = 0;

        for (PolygonList::const_iterator patch = patches.begin(); patch != patches.end(); ++patch)
        {
            Point* patchCentroid = (*patch)->getCentroid();
            x += patchCentroid->X();
            y += patchCentroid->Y();
            delete patchCentroid;
        }
        x = x/double(patches.size());
        y = y/double(patches.size());
        return new Point(x,y);
    }

    Point* PolyhedralSurface::getCentroid3D(void) const
    {
        //The centroid of a polyhedral surface can be described as the centroid of all the
        //centroids of its polygons. This reduces to the centroid of a multipoint, which is
        //the average location of the points
        double x = 0;
        double y = 0;
        double z = 0;

        for (PolygonList::const_iterator patch = patches.begin(); patch != patches.end(); ++patch)
        {
            Point* patchCentroid = (*patch)->getCentroid3D();
            x += patchCentroid->X();
            y += patchCentroid->Y();
            z += patchCentroid->Z();
            delete patchCentroid;
        }
        x = x/double(patches.size());
        y = y/double(patches.size());
        z = z/double(patches.size());
        return new Point(x,y,z);
    }

    Point* PolyhedralSurface::getPointOnSurface(void) const
    {
        return patches.front()->getPointOnSurface();
    }

    int PolyhedralSurface::getNumPatches(void) const
    {
        return int(patches.size());
    }

    Polygon* PolyhedralSurface::getPatchN(int n) const
    {
        if (n >= 0 && n < int(patches.size()))
            return patches[n];
        throw std::runtime_error("PolyhedralSurface::getPointN(): index out of bounds");
    }

    MultiPolygon* PolyhedralSurface::getBoundingPolygons(const Polygon* p) const
    {
        //firstly, we must find the index values of all polygons equal to p in the polyhedral surface
        int index = -1;
        for (int i = 0; i < getNumPatches(); i++)
        {
            if (patches[i]->equals(p)) index = i;
        }

        if (index == -1) return NULL; //no p found in the surface

        MultiPolygon* result = new MultiPolygon;
        LineString* ring = p->getExteriorRing();
        for (int i = 0; i < ring->getNumPoints(); i++)
        {
            //for each point on the polygon, check all patches, if a patch has one point that is equal to this one,
            //add it to the result and move on
            for (PolygonList::const_iterator patch = patches.begin(); patch != patches.end(); ++patch)
            {
                bool touches = false;
                LineString* patchRing = (*patch)->getExteriorRing();
                for (int j = 0; j < patchRing->getNumPoints(); j++)
                {
                    if (patchRing->getPointN(j)->equals(ring->getPointN(i)))
                    {
                        touches = true;
                        break;
                    }
                }
                if (touches) result->addGeometry(new Polygon(*patch));
            }
        }
        if (result->isEmpty()) 
        {
            delete result;
            return NULL;
        }
        else return result;
    }

    bool PolyhedralSurface::isClosed(void) const
    { 
        if (!getBoundary()) return true;
        else return false;
    }

    bool PolyhedralSurface::isValid(void) const
    {
        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
        {
            if(!(*it)->isValid())
                return false;
        }

        //check that no point is on an edge
        for(PolygonList::const_iterator A = patches.begin(), end = patches.end(); A != end; ++A)
        {
            for(PolygonList::const_iterator B = patches.begin(), end = patches.end(); B != end; ++B)
            {
                if ((*A)==(*B)) continue;

            /*
                For each point on A, check against all the edges of B to see if the current
                vertex of A is between two vertex on B, but no equal to the endpoints. 
                This means that there is an edge shared by more than two polygons, and thus
                the polyhedral surface is not valid.
            */
                LineString* Aring = (*A)->getExteriorRing();
                LineString* Bring = (*B)->getExteriorRing();

                for (int i = 0; i < Aring->getNumPoints() - 1; i++)
                {
                    for (int j = 0; j < Bring->getNumPoints() - 1; j++)
                    {
                        if (CollinearAndExclusiveBetween(Bring->getPointN(j),Bring->getPointN(j+1),Aring->getPointN(i)))
                            return false;
                    }
                }

            }
        }

        return true;
    }

    WKBGeometryType PolyhedralSurface::getWKBGeometryType(bool withZ, bool withM) const
    {
        if(getGeometryType() == "TIN")
        {
            if (withZ && withM && is3D() && isMeasured())
                return wkbTINZM;
            if (withZ && is3D())
                return wkbTINZ;
            if (withM && isMeasured())
                return wkbTINM;
            return wkbTIN;
        }
        if (withZ && withM && is3D() && isMeasured())
            return wkbPolyhedralSurfaceZM;
        if (withZ && is3D())
            return wkbPolyhedralSurfaceZ;
        if (withM && isMeasured())
            return wkbPolyhedralSurfaceM;
        return wkbPolyhedralSurface;
    }
    
    void PolyhedralSurface::toText(std::ostream &os, bool tag, bool withZ, bool withM) const
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
            for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
            {
                if(it != patches.begin())
                    os << ",";
                (*it)->toText(os, false, withZ, withM);
            }
        }
        os << ")";
    }

    void PolyhedralSurface::fromText(std::istream &is, bool tag, bool withZ, bool withM)
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
            addPatch(polygon);
            while (is.peek() == ' ') is.ignore(1);
            if (is.get() == ')') return;
        }
    }

    void PolyhedralSurface::toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const
    {
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> numPatches(ccl::uint32_t(patches.size()));
            os << numPatches;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> numPatches(ccl::uint32_t(patches.size()));
            os << numPatches;
        }
        for(PolygonList::const_iterator it = patches.begin(), end = patches.end(); it != end; ++it)
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
    
    void PolyhedralSurface::fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM)
    {
        ccl::uint32_t numPatches;
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<ccl::uint32_t> tempPatches;
            is >> tempPatches;
            numPatches = tempPatches;
        }
        else
        {
            ccl::LittleEndian<ccl::uint32_t> tempPatches;
            is >> tempPatches;
            numPatches = tempPatches;
        }
        for(ccl::uint32_t i = 0; i < numPatches; ++i)
        {
            byteOrder = (WKBByteOrder)is.get();
            ccl::uint32_t geometryType;        // this isn't used here since all patches are polygons, but it's in the binary stream
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
            Polygon* patch = new Polygon();
            patch->fromBinary(is, byteOrder, withZ, withM);
            addPatch(patch);
        }
    }

    bool PolyhedralSurface::isParentOf(const GeometryBase *child) const
    {
        if(!child)
            return false;
        for(int i = 0, c = getNumPatches(); i < c; ++i)
        {
            if(getPatchN(i) == child)
                return true;
        }
        return false;
    }

    GeometryBase *PolyhedralSurface::getParentOf(const GeometryBase *child) const
    {
        if(isParentOf(child))
            return const_cast<PolyhedralSurface *>(this);
        for(int i = 0, c = getNumPatches(); i < c; ++i)
        {
            GeometryBase *match = getPatchN(i)->getParentOf(child);
            if(match)
                return match;
        }
        return NULL;
    }

    int PolyhedralSurface::getNumChildren(void) const
    {
        return getNumPatches();
    }

}