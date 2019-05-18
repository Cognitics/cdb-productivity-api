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
#include "sfa/Quat.h"
#include <math.h>

namespace sfa {

    Quat::Quat(double x, double y, double z, double w)
    {
        set(x,y,z,w);
    }

    Quat::Quat(const Point& point)
    {
        set(point);
    }

    Quat::Quat(double angle, const Point& axis)
    {
        makeRotation(angle,axis);
    }

    Quat::Quat(const Point& from, const Point& to)
    {
        makeRotation(from,to);
    }

    Quat::Quat(const Quat& quat)
    {
        set(quat._v[0],quat._v[1],quat._v[2],quat._v[3]);
    }

    Quat& Quat::operator=(const Quat& rhs)
    {
        set(rhs._v[0],rhs._v[1],rhs._v[2],rhs._v[3]);
        return *this;
    }

    bool Quat::operator==(const Quat& rhs) const
    {
        return    _v[0] == rhs._v[0] &&
                _v[1] == rhs._v[1] &&
                _v[2] == rhs._v[2] &&
                _v[3] == rhs._v[3];
    }

    bool Quat::operator!=(const Quat& rhs) const
    {
        return !(*this == rhs);
    }

    Point Quat::asPoint(void) const
    {
        return Point(_v[0],_v[1],_v[2],_v[3]);
    }

    void Quat::set(double x, double y, double z, double w)
    {
        _v[0] = x;
        _v[1] = y;
        _v[2] = z;
        _v[3] = w;
    }

    void Quat::set(const sfa::Point& point)
    {
        set(point.X(),point.Y(),point.Z(),point.M());
    }

    double& Quat::operator[](int i) { return _v[i]; }
    double Quat::operator[](int i) const { return _v[i]; }

    double& Quat::x(void) { return _v[0]; }
    double& Quat::y(void) { return _v[1]; }
    double& Quat::z(void) { return _v[2]; }
    double& Quat::w(void) { return _v[3]; }

    double Quat::x(void) const { return _v[0]; }
    double Quat::y(void) const { return _v[1]; }
    double Quat::z(void) const { return _v[2]; }
    double Quat::w(void) const { return _v[3]; }

    bool Quat::zero(void) const
    {
        return _v[0] == 0 && _v[1] == 0 && _v[2] == 0 && _v[3] == 1.0;
    }

    Quat Quat::operator*(double rhs) const
    {
        return Quat(_v[0]*rhs,_v[1]*rhs,_v[2]*rhs,_v[3]*rhs);
    }

    Quat& Quat::operator*=(double rhs)
    {
        _v[0] *= rhs;
        _v[1] *= rhs;
        _v[2] *= rhs;
        _v[3] *= rhs;
        return *this;
    }

    Quat Quat::operator/(double rhs) const
    {
        return (*this)*(1.0/rhs);
    }

    Quat& Quat::operator/=(double rhs)
    {
        return (*this)*=(1.0/rhs);
    }

    Quat Quat::operator*(const Quat& rhs) const
    {
        return Quat(    rhs._v[3]*_v[0] + rhs._v[0]*_v[3] + rhs._v[1]*_v[2] - rhs._v[2]*_v[1],
                        rhs._v[3]*_v[1] - rhs._v[0]*_v[2] + rhs._v[1]*_v[3] + rhs._v[2]*_v[0],
                        rhs._v[3]*_v[2] + rhs._v[0]*_v[1] - rhs._v[1]*_v[0] + rhs._v[2]*_v[3],
                        rhs._v[3]*_v[3] - rhs._v[0]*_v[0] - rhs._v[1]*_v[1] - rhs._v[2]*_v[2] );
    }

    Quat& Quat::operator*=(const Quat& rhs)
    {
        double x = rhs._v[3]*_v[0] + rhs._v[0]*_v[3] + rhs._v[1]*_v[2] - rhs._v[2]*_v[1];
        double y = rhs._v[3]*_v[1] - rhs._v[0]*_v[2] + rhs._v[1]*_v[3] + rhs._v[2]*_v[0];
        double z = rhs._v[3]*_v[2] + rhs._v[0]*_v[1] - rhs._v[1]*_v[0] + rhs._v[2]*_v[3];
        _v[3] = rhs._v[3]*_v[3] - rhs._v[0]*_v[0] - rhs._v[1]*_v[1] - rhs._v[2]*_v[2];
    
        _v[2] = z;
        _v[1] = y;
        _v[0] = x;
    
        return (*this);
    }

    Quat Quat::operator/(const Quat& rhs) const
    {
        return (*this)*rhs.inverse();
    }

    Quat& Quat::operator/=(const Quat& rhs)
    {
        return (*this) *= rhs.inverse();
    }

    Quat Quat::operator+(const Quat& rhs) const
    {
        return Quat(_v[0] + rhs._v[0], _v[1] + rhs._v[1], _v[2] + rhs._v[2], _v[3] + rhs._v[3]);
    }

    Quat& Quat::operator+=(const Quat& rhs)
    {
        _v[0] += rhs._v[0];
        _v[1] += rhs._v[1];
        _v[2] += rhs._v[2];
        _v[3] += rhs._v[3];
        return *this;
    }

    Quat Quat::operator-(const Quat& rhs) const
    {
        return Quat(_v[0] - rhs._v[0], _v[1] - rhs._v[1], _v[2] - rhs._v[2], _v[3] - rhs._v[3]);
    }

    Quat& Quat::operator-=(const Quat& rhs) 
    {
        _v[0] -= rhs._v[0];
        _v[1] -= rhs._v[1];
        _v[2] -= rhs._v[2];
        _v[3] -= rhs._v[3];
        return *this;
    }

    double Quat::length(void) const
    {
        return sqrt(length2());
    }

    double Quat::length2(void) const
    {
        return _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] + _v[3]*_v[3];
    }

    Quat Quat::conj(void) const
    {
        return Quat( -_v[0], -_v[1], -_v[2], _v[3]);
    }

    Quat Quat::inverse(void) const
    {
        return conj() / length2();
    }

    void Quat::makeRotation(double angle, double x, double y, double z)
    {
        double length = x*x + y*y + z*z;

        if (length < SFA_EPSILON)
        {
            *this = Quat();
            return;
        }

        double inverse = 1.0/length;
        double cos_half_angle = cos(angle * 0.5);
        double sin_half_angle = sin(angle * 0.5);

        _v[0] = x * sin_half_angle * inverse;
        _v[1] = y * sin_half_angle * inverse;
        _v[2] = z * sin_half_angle * inverse;
        _v[3] = cos_half_angle;
    }

    void Quat::makeRotation(double angle, const Point& axis)
    {
        makeRotation(angle, axis.X(), axis.Y(), axis.Z());
    }

    void Quat::makeRotation(const Point& from, const Point& to)
    {
        Point source = from;
        Point dest = to;

        source.normalize();
        dest.normalize();

        double dotPlus1 = 1.0 + source.dot(dest);

        if (dotPlus1 < SFA_EPSILON)
        {
            if (abs(source.X()) < 0.6)
            {
                double norm = sqrt(1.0 - source.X()*source.X());
                set( 0.0 , source.Z()/norm , -source.Y()/norm , 0.0 );
            }
            else if (abs(source.Y()) < 0.6)
            {
                double norm = sqrt(1.0 - source.Y()*source.Y());
                set( -source.Z()/norm , 0.0 , source.X()/norm , 0.0 );
            }
            else
            {
                double norm = sqrt(1.0 - source.Z()*source.Z());
                set( source.Y()/norm , -source.X()/norm , 0.0 , 0.0 );
            }
        }
        else
        {
            double s = sqrt(0.5*dotPlus1);
            Point temp = source.cross(dest) / (2.0*s);
            _v[0] = temp.X();
            _v[1] = temp.Y();
            _v[2] = temp.Z();
            _v[3] = s;
        }
    }

    void Quat::getRotation(double &angle, double &x, double &y, double &z) const
    {
        double sin_half_angle = sqrt( _v[0]*_v[0] + _v[1]*_v[1] + _v[2]*_v[2] );

        angle = 2.0*atan2(sin_half_angle,_v[3]);
        if ( abs(sin_half_angle) > SFA_EPSILON)
        {
            x = _v[0];
            y = _v[1];
            z = _v[2];
        }
        else
        {
            x = 0.0;
            y = 0.0;
            z = 1.0;
        }
    }

    void Quat::getRotation(double &angle, Point &axis) const
    {
        double x, y, z;
        getRotation(angle,x,y,z);
        axis = Point(x,y,z);
    }

    Point Quat::operator*(const Point& v) const
    {
    //    nVidia SDK implementation from osg
        Point uv, uuv;
        Point qvec(_v[0],_v[1],_v[2]);
        uv = qvec.cross(v);
        uuv = qvec.cross(uv);
        uv *= (2.0 * _v[3]);
        uuv *= 2.0;
        return v + uv + uuv;
    }
    
}