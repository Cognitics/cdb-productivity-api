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
#include "sfa/Projection2D.h"
#include "sfa/PointMath.h"
#include "sfa/RingMath.h"
#include <cmath>

namespace sfa {

    Projection2D::Projection2D(const Point* p1, const Point* p2, const Point* p3)
    {
        createBasis(p1,p2,p3);
    }

    Projection2D::Projection2D(const Point* origin, const Point* normal)
    {
        createBasis(origin,normal);
    }

    Projection2D::Projection2D(const Polygon* polygon)
    {
        createBasis(polygon);
    }

    bool Projection2D::createBasis(const Polygon* poly)
    {
        LineString* ring = poly->getExteriorRing();
        Point* p1 = ring->getPointN(0);
        Point* p2 = ring->getPointN(1);

        for (int i = 2; i < ring->getNumPoints(); i++)
        {
            if (!Collinear3D(p1,p2,ring->getPointN(i)))
            {
                createBasis(p1,p2,ring->getPointN(i));
                return true;
            }
        }
        
        return false;
    }


    void Projection2D::createBasis(const Point* p1, const Point* p2, const Point* p3)
    {
        Point v1 = *p2 - *p1;
        Point v2 = *p3 - *p1;
        Point n = v1.cross(v2);

        createBasis(p1,&n);
    }

    void Projection2D::createBasis(const Point* origin, const Point* normal)
    {
        norm = normal;
        norm.normalize();

        this->origin = *origin;

    //    XY Plane, standard basis
        if ( (abs(normal->X()) < SFA_EPSILON) && (abs(normal->Y()) < SFA_EPSILON) )
        {
            e1 = Point(1,0,0);
            e2 = norm.cross(e1);
        }
    //    XZ plane
        else if ( (abs(normal->X()) < SFA_EPSILON) && (abs(normal->Z()) < SFA_EPSILON) )
        {
            e2 = Point(0,0,1);
            e1 = e2.cross(norm);
        }
    //    YZ plane
        else if ( (abs(normal->Y()) < SFA_EPSILON) && (abs(normal->Z()) < SFA_EPSILON) )
        {
            e2 = Point(0,0,1);
            e1 = e2.cross(norm);
        }
    //    Force first vector to lie in the XY plane
        else
        {
        //    Try to find at least one vector in the XY plane, by inverting normal
            e1 = Point(norm.Y(), -norm.X());
            e1.normalize();
            e2 = norm.cross(e1);
            e2.normalize();

            if (e2.Z() < 0)
            {
                e2 = e2*-1;
                e1 = e1*-1;
            }
        }
    }

    Point* Projection2D::transformPointTo2D(const Point* p) const
    {
        Point temp = *p - origin;
        double x = temp.dot(e1);
        double y = temp.dot(e2);
        double z = temp.dot(norm); 
        Point* result = new Point(x,y);
        if (z) result->setZ(z);
        if (p->isMeasured()) result->setM(p->M());
        return result;
    }

    Point Projection2D::transformPointTo2D(const Point& p) const
    {
        Point temp = p - origin;
        double x = temp.dot(e1);
        double y = temp.dot(e2);
        double z = temp.dot(norm); 
        Point result(x,y);
        if (z) result.setZ(z);
        if (p.isMeasured()) result.setM(p.M());
        return result;
    }

    Point* Projection2D::transformPointTo3D(const Point* p) const
    {            
        double x = origin.X() + e1.X()*p->X() + e2.X()*p->Y() + norm.X()*p->Z();
        double y = origin.Y() + e1.Y()*p->X() + e2.Y()*p->Y() + norm.Y()*p->Z();
        double z = origin.Z() + e1.Z()*p->X() + e2.Z()*p->Y() + norm.Z()*p->Z();
        Point* result = new Point(x,y,z);
        if (p->isMeasured()) result->setM(p->M());
        return result;
    }

    Point Projection2D::transformPointTo3D(const Point &p) const
    {            
        double x = origin.X() + e1.X()*p.X() + e2.X()*p.Y() + norm.X()*p.Z();
        double y = origin.Y() + e1.Y()*p.X() + e2.Y()*p.Y() + norm.Y()*p.Z();
        double z = origin.Z() + e1.Z()*p.X() + e2.Z()*p.Y() + norm.Z()*p.Z();

        Point result(x,y,z);
        if (p.isMeasured()) result.setM(p.M());
        return result;
    }

    LineString* Projection2D::transformLineTo2D(const LineString* l) const
    {
        LineString* result = new LineString;
        for (int i = 0; i < l->getNumPoints(); i++)
        {
            result->addPoint(transformPointTo2D(l->getPointN(i)));
        }
        return result;
    }

    LineString* Projection2D::transformLineTo3D(const LineString* l) const
    {
        LineString* result = new LineString;
        for (int i = 0; i < l->getNumPoints(); i++)
        {
            result->addPoint(transformPointTo3D(l->getPointN(i)));
        }
        return result;
    }

    Polygon* Projection2D::transformPolygonTo2D(const Polygon* poly) const
    {
        Polygon* result = new Polygon;
        result->addRing(transformLineTo2D(poly->getExteriorRing()));
        for (int i = 0; i < poly->getNumInteriorRing(); i++)
        {
            result->addRing(transformLineTo2D(poly->getInteriorRingN(i)));
        }
        //check if the polygon has the same orientation as the original
        if (ComputeOrientation(result) != ComputeOrientation(poly))
        {
            result->reverse();
        }
        return result;
    }

    Polygon* Projection2D::transformPolygonTo3D(const Polygon* poly) const
    {
        Polygon* result = new Polygon;
        result->addRing(transformLineTo3D(poly->getExteriorRing()));
        for (int i = 0; i < poly->getNumInteriorRing(); i++)
        {
            result->addRing(transformLineTo3D(poly->getInteriorRingN(i)));
        }
        return result;
    }

    GeometryCollection* Projection2D::transformCollectionTo2D(const GeometryCollection* geometries) const
    {
        GeometryCollection* result = new GeometryCollection;
        for (int i = 1; i < geometries->getNumGeometries()+1; i++)
            result->addGeometry(transformGeometryTo2D(geometries->getGeometryN(i)));
        return result;
    }

    GeometryCollection* Projection2D::transformCollectionTo3D(const GeometryCollection* geometries) const
    {
        GeometryCollection* result = new GeometryCollection;
        for (int i = 1; i < geometries->getNumGeometries()+1; i++)
            result->addGeometry(transformGeometryTo3D(geometries->getGeometryN(i)));
        return result;
    }

    PolyhedralSurface* Projection2D::transformSurfaceTo2D(const PolyhedralSurface* surface) const
    {
        PolyhedralSurface* result = new PolyhedralSurface;
        for (int i = 0; i < surface->getNumPatches(); i++)
            result->addPatch(transformPolygonTo2D(surface->getPatchN(i)));
        return result;
    }

    PolyhedralSurface* Projection2D::transformSurfaceTo3D(const PolyhedralSurface* surface) const
    {
        PolyhedralSurface* result = new PolyhedralSurface;
        for (int i = 0; i < surface->getNumPatches(); i++)
            result->addPatch(transformPolygonTo3D(surface->getPatchN(i)));
        return result;
    }

    Geometry* Projection2D::transformGeometryTo2D(const Geometry* geom) const
    {
        int type = geom->getWKBGeometryType(false,false);
        switch (type)
        {
        case wkbPoint:
            return transformPointTo2D(static_cast<const Point*>(geom));
        case wkbLineString:
            return transformLineTo2D(static_cast<const LineString*>(geom));
        case wkbPolygon:
        case wkbTriangle:
            return transformPolygonTo2D(static_cast<const Polygon*>(geom));
        case wkbPolyhedralSurface:
        case wkbTIN:
            return transformSurfaceTo2D(static_cast<const PolyhedralSurface*>(geom));
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            return transformCollectionTo2D(static_cast<const GeometryCollection*>(geom));
        default:
            throw std::runtime_error("Projection2D::transformGeometrTo2D() Invalid Geometry Type!");
        }
    }

    Geometry* Projection2D::transformGeometryTo3D(const Geometry* geom) const
    {
        int type = geom->getWKBGeometryType(false,false);
        switch (type)
        {
        case wkbPoint:
            return transformPointTo3D(static_cast<const Point*>(geom));
        case wkbLineString:
            return transformLineTo3D(static_cast<const LineString*>(geom));
        case wkbPolygon:
        case wkbTriangle:
            return transformPolygonTo3D(static_cast<const Polygon*>(geom));
        case wkbPolyhedralSurface:
        case wkbTIN:
            return transformSurfaceTo3D(static_cast<const PolyhedralSurface*>(geom));
        case wkbMultiPoint:
        case wkbMultiLineString:
        case wkbMultiPolygon:
        case wkbGeometryCollection:
            return transformCollectionTo3D(static_cast<const GeometryCollection*>(geom));
        default:
            throw std::runtime_error("Projection2D::transformGeometrTo3D() Invalid Geometry Type!");
        }
    }

    const Point* Projection2D::getOrigin(void) const
    {
        return &origin;
    }

    void Projection2D::setOrigin(const Point& o)
    {
        origin = o;
    }

    const Point* Projection2D::getNormal(void) const
    {
        return &norm;
    }

    void Projection2D::setNormal(const Point& n)
    {
        norm = n;
    }

    const Point* Projection2D::getBasisVector(int l) const
    {
        if (l == 0)
            return &e1;
        else if (l == 1)
            return &e2;
        else
            return NULL;
    }

    void Projection2D::setBasisVector(const Point& v, int l)
    {
        if (l == 0)
            e1 = v;
        else if (l == 1)
            e2 = v;
    }

    void Projection2D::applyRotation(const Quat& quat)
    {
        e1 = quat*e1;
        e2 = quat*e2;
        norm = quat*norm;
    }

    Matrix Projection2D::getMatrix(void) const
    {
        Matrix translate(1,0,0,-origin.X(),
                        0,1,0,-origin.Y(),
                        0,0,1,-origin.Z());
        Matrix mat(    e1.X(), e2.X(), norm.X(), 0,
                        e1.Y(), e2.Y(), norm.Y(), 0,
                        e1.Z(), e2.Z(), norm.Z(), 0 );
        return mat*translate;
    }

    void Projection2D::scale2D(double x, double y)
    {
        e1 *= x;
        e2 *= y;
    }

    void Projection2D::scale3D(double x, double y, double z)
    {
        e1 = Point( e1.X()*x, e1.Y()*y, e1.Z()*z );
        e2 = Point( e2.X()*x, e2.Y()*y, e2.Z()*z );
        norm = Point( norm.X()*x, norm.Y()*y, norm.Z()*z );
    }

    void Projection2D::trans2D(double x, double y)
    {
        origin += e1*x + e2*y;
    }

    void Projection2D::trans3D(double x, double y, double z)
    {
        origin += Point(x,y,z);
    }

    void Projection2D::invert(void)
    {
        e1 *= -1;
        norm *= -1;
    }

}