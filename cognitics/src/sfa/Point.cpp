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
//#pragma optimize( "", off )
#include "sfa/Point.h"
#include "sfa/Polygon.h"
#include "sfa/MultiPoint.h"
#include <math.h>
#include <sstream>

namespace sfa
{
    Point::~Point(void)
    {
    }

    Point::Point(void) : x(0.0), y(0.0), z(0.0), m(0.0), hasZ(false), hasM(false)
    {
    }

    Point::Point(double x, double y) : x(x), y(y), z(0.0), m(0.0), hasZ(false), hasM(false)
    {
    }

    Point::Point(double x, double y, double z) : x(x), y(y), z(z), m(0.0), hasZ(true), hasM(false)
    {
    }

    Point::Point(double x, double y, double z, double m) : x(x), y(y), z(z), m(m), hasZ(true), hasM(true)
    {
    }

    Point::Point(const Point& other) : x(other.x), y(other.y), z(other.z), m(other.m), hasZ(other.hasZ), hasM(other.hasM)
    {
    }

    Point::Point(const Point* other) : x(other->x), y(other->y), z(other->z), m(other->m), hasZ(other->hasZ), hasM(other->hasM)
    {
    }

    double Point::X(void) const
    {
        return x;
    }

    double Point::Y(void) const
    {
        return y;
    }

    double Point::Z(void) const
    {
        return z;
    }

    double Point::M(void) const
    {
        return m;
    }

    void Point::setX(double x)
    {
        this->x = x;
    }

    void Point::setY(double y)
    {
        this->y = y;
    }

    void Point::setZ(double z)
    {
        hasZ = true;
        this->z = z;
    }

    void Point::setM(double m)
    {
        hasM = true;
        this->m = m;
    }

    Point &Point::operator=(const Point &p)
    {
        setX(p.x);
        setY(p.y);
        if(p.hasZ)
            setZ(p.z);
        else
            clearZ();
        if(p.hasM)
            setM(p.m);
        else
            clearM();
        return *this;
    }

    bool Point::operator==(const Point &p) const
    {
        return (x == p.x) && (y == p.y) && (!hasZ || (z == p.z));// && (!hasM || (m == p.m));
    }

    bool Point::operator!=(const Point &p) const
    {
        return !(*this == p);
    }

    Point &Point::operator+=(const Point &p)
    {
        x += p.x;
        y += p.y;
        if(hasZ || p.hasZ)
            setZ(z + p.z);
        return *this;
    }

    Point &Point::operator-=(const Point &p)
    {
        x -= p.x;
        y -= p.y;
        if(hasZ || p.hasZ)
            setZ(z - p.z);
        return *this;
    }

    Point &Point::operator*=(double value)
    {
        x *= value;
        y *= value;
        if(hasZ)
            z *= value;
        return *this;
    }

    Point &Point::operator/=(double value)
    {
        double inv = 1.0 / value;
        x *= inv;
        y *= inv;
        if(hasZ)
            z *= inv;
        return *this;
    }

    Point Point::operator+(const Point &p) const
    {
        Point result = *this;
        result += p;
        if(hasM || p.hasM)
            result.setM(m);
        return result;
    }

    Point Point::operator-(const Point &p) const
    {
        Point result = *this;
        result -= p;
        if(hasM || p.hasM)
            result.setM(m);
        return result;
    }

    Point Point::operator-(void) const
    {
        Point result;
        result.setX(-x);
        result.setY(-y);
        if(hasZ)
            result.setZ(-z);
        if(hasM)
            result.setM(m);
        return result;
    }

    Point Point::operator*(double value) const
    {
        Point result = *this;
        result *= value;
        if(hasM)
            result.setM(m);
        return result;
    }

    Point Point::operator/(double value) const
    {
        Point result = *this;
        result /= value;
        if(hasM)
            result.setM(m);
        return result;
    }

    void Point::clearZ(void)
    {
        hasZ = false;
        z = 0.0;
    }

    void Point::clearM(void)
    {
        hasM = false;
        m = 0.0;
    }

    void Point::zero(void)
    {
        x = y = 0;
        if(hasZ)
            z = 0;
    }

    double Point::length(void) const
    {
        if (hasZ)
            return sqrt((x*x) + (y*y) + (z*z));
        else
            return sqrt((x*x) + (y*y));
        /*
        if(hasM && hasZ)
            return sqrt((x*x)+(y*y)+(z*z)+(m*m));
        if(hasM && !hasZ)
            return sqrt((x*x)+(y*y)+(m*m));
        return hasZ ? sqrt((x*x)+(y*y)+(z*z)) : sqrt((x*x)+(y*y));
        */
    }

    double Point::length2(void) const
    {
        if (hasZ)
            return x*x + y*y + z*z;
        else
            return x*x + y*y;

            /*
        if (hasM && hasZ)
            return x*x + y*y + z*z + m*m;
        if (hasM && !hasZ)
            return x*x + y*y + m*m;
        return hasZ ? x*x + y*y + z*z : x*x + y*y;
        */
    }

    double Point::length2D(void) const {
        return sqrt(x*x + y*y);
    }

    Point* Point::nearest(Point* p1, Point* p2) const
    {
        if(!p1 || !p2)
            return p1 ? p1 : p2;
        if(p1->is3D() && p2->is3D())
            return (distance3D(p1) < distance3D(p2)) ? p1 : p2;
        else
            return (distance(p1) < distance(p2)) ? p1 : p2;
    }

    void Point::setCoordinateSystem(cts::CS_CoordinateSystem* coordinateSystem, cts::CT_MathTransform *mathTransform)
    {
        if(mathTransform)
        {
            cts::PT_CoordinatePoint p;
            p.push_back(X());
            p.push_back(Y());
            if(hasZ)
                p.push_back(Z());
            cts::PT_CoordinatePoint result = mathTransform->transform(p);
            if(result.size() > 0)
            {
                setX(result[0]);
                setY(result[1]);
                if(hasZ)
                    setZ(result[2]);
            }
        }
        Geometry::setCoordinateSystem(coordinateSystem, mathTransform);
    }

    int Point::getDimension(void) const
    {
        return 0;
    }

    std::string Point::getGeometryType(void) const
    {
        return "Point";
    }

    WKBGeometryType Point::getWKBGeometryType(bool withZ, bool withM) const
    {
        if (withZ && withM && is3D() && isMeasured())
            return wkbPointZM;
        if (withZ && is3D())
            return wkbPointZ;
        if (withM && isMeasured())
            return wkbPointM;
        return wkbPoint;
    }

    Geometry* Point::getEnvelope(void) const
    {
        LineString* geometry = new LineString;
        geometry->addPoint(*this);
        geometry->addPoint(*this);
        return geometry;
    }

    bool Point::isEmpty(void) const
    {
        return false;
    }

    bool Point::isSimple(void) const
    {
        return true;
    }

    bool Point::is3D(void) const
    {
        return hasZ;
    }

    bool Point::isMeasured(void) const
    {
        return hasM;
    }

    Geometry* Point::getBoundary(void) const
    {
        return NULL;
    }

    bool Point::equals(const Geometry* another) const
    {
        if (!another) return false;
        if (another->getWKBGeometryType(false,false) == wkbGeometryCollection)
        {
            const GeometryCollection* mp = static_cast<const GeometryCollection*>(another);
            if (mp->isEmpty()) return false;
            for (int i = 1; i < mp->getNumGeometries()+1; i++)
            {
                if (!equals(mp->getGeometryN(i))) return false;
            }
            return true;
        }
        else if (another->getWKBGeometryType(false,false) != wkbPoint)
            return false;

        const Point* other = static_cast<const Point*>(another);
        double dx = this->X() - other->X();
        double dy = this->Y() - other->Y();
        return dx*dx + dy*dy < SFA_EPSILON*SFA_EPSILON;
        //return sqrt(dx*dx + dy*dy) < SFA_EPSILON;
    }

    bool Point::equals3D(const Geometry* another) const
    {
        if (another->getWKBGeometryType(false,false) == wkbGeometryCollection)
        {
            const GeometryCollection* mp = static_cast<const GeometryCollection*>(another);
            if (mp->isEmpty()) return false;
            for (int i = 1; i < mp->getNumGeometries()+1; i++)
            {
                if (!equals3D(mp->getGeometryN(i))) return false;
            }
            return true;
        }
        else if (another->getWKBGeometryType(false,false) != wkbPoint)
            return false;

        const Point* other = static_cast<const Point*>(another);
        Point d = *this - *other;
        return d.length() < SFA_EPSILON;
    }

    Geometry* Point::locateBetween(double mStart, double mEnd) const
    {
        MultiPoint* result = new MultiPoint;
        if (!hasM) return result;
        if ( m <= mEnd && m >= mStart) result->addGeometry(new Point(this));
        return result;
    }

    Geometry* Point::convexHull(void) const
    {
        return new Point(this);
    }

    Geometry* Point::convexHull3D(void) const
    {
        return new Point(this);
    }

    bool Point::isValid(void) const
    {
        return true;
    }

    void Point::toText(std::ostream &os, bool tag, bool withZ, bool withM) const
    {
        if(tag)
        {
            Geometry::toText(os, tag, withZ, withM);
            os << " (";
        }
        os << x << " " << y;
        if(withZ)
            os << " " << z;
        if(withM)
            os << " " << m;
        if(tag)
            os << ")";
    }

    void Point::fromText(std::istream &is, bool tag, bool withZ, bool withM)
    {
        hasZ = withZ;
        hasM = withM;

    //    Remove tag if there is one
        if (tag) is.ignore(1024,'(');

        is >> x >> y;
        if (withZ) is >> z;
        if (withM) is >> m;
        
    //    Remove the end paren if taged
        if (tag) is.ignore(1024,')');
    }

    void Point::toBinary(std::ostream &os, WKBByteOrder byteOrder, bool withZ, bool withM) const
    {
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<double> dx(x);
            ccl::BigEndian<double> dy(y);
            ccl::BigEndian<double> dz(z);
            ccl::BigEndian<double> dm(m);
            os << dx << dy;
            if(withZ)
                os << dz;
            if(withM)
                os << dm;
        }
        else
        {
            ccl::LittleEndian<double> dx(x);
            ccl::LittleEndian<double> dy(y);
            ccl::LittleEndian<double> dz(z);
            ccl::LittleEndian<double> dm(m);
            os << dx << dy;
            if(withZ)
                os << dz;
            if(withM)
                os << dm;
        }
    }

    void Point::fromBinary(std::istream &is, WKBByteOrder byteOrder, bool withZ, bool withM)
    {
        if(byteOrder == wkbXDR)
        {
            ccl::BigEndian<double> dx;
            is >> dx;
            setX(dx);
            ccl::BigEndian<double> dy;
            is >> dy;
            setY(dy);
            if(withZ)
            {
                ccl::BigEndian<double> dz;
                is >> dz;
                setZ(dz);
            }
            if(withM)
            {
                ccl::BigEndian<double> dm;
                is >> dm;
                setM(dm);
            }
        }
        else
        {
            ccl::LittleEndian<double> dx;
            is >> dx;
            setX(dx);
            ccl::LittleEndian<double> dy;
            is >> dy;
            setY(dy);
            if(withZ)
            {
                ccl::LittleEndian<double> dz;
                is >> dz;
                setZ(dz);
            }
            if(withM)
            {
                ccl::LittleEndian<double> dm;
                is >> dm;
                setM(dm);
            }
        }
    }

    bool Point::isParentOf(const GeometryBase *child) const
    {
        return false;
    }

    GeometryBase *Point::getParentOf(const GeometryBase *child) const
    {
        return NULL;
    }

    int Point::getNumChildren(void) const
    {
        return 0;
    }

    double Point::dot(const Point &other) const
    {
        if(hasZ)
        {
            return ((other.X() * this->X())+(other.Y() * this->Y())+(other.Z() * this->Z()));
        }
        else
        {
            return ((other.X() * this->X())+(other.Y() * this->Y()));
        }
    }

    double Point::dot2D(const Point& other) const {
        return other.X()*X() + other.Y()*Y();
    }

    Point Point::cross(const Point &other) const
    {
        Point ret;
        ret.setX((this->Y()*other.Z()) - (this->Z()*other.Y()));
        ret.setY((this->Z()*other.X()) - (this->X()*other.Z()));
        ret.setZ((this->X()*other.Y()) - (this->Y()*other.X()));

        return ret;
    }

    double Point::normalize()
    {
        double len = length();
        if(len > 0)
        {
            double inv = 1.0f/len;
            x *= inv;
            y *= inv;
            z *= inv;
            m *= inv;
        }
        return len;
    }

    bool Point::operator<(const Point& p) const
    {
        if (x<p.x) 
            return true;
        else if (x>p.x) 
            return false;
        if (y<p.y) 
            return true;
        else if (y>p.y) 
            return false;
        if (z<p.z) 
            return true;
        else if (z>p.z) 
            return false;
        return (m<p.m);
    }

    double Point::distance2D2(const Point &other) const
    {        
        double dx = X() - other.X();
        double dy = Y() - other.Y();
        return (dx*dx + dy*dy);        
    }

    void Point::transform(const sfa::Matrix &xform)
    {
        *this = xform * (*this);        
    }
}