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
/*! \brief Provides sfa::Quat.
\author Josh Anghel <janghel@cognitics.net>
\date 26 May 2011
*/
#pragma once
#include "Point.h"

namespace sfa {
    class Point;
/*!    \class sfa::Quat Quat.h Quat.h
    \brief Quat

    Provides a Quaternion class. Used to represent an orientation in 3D space.
*/
    class Quat
    {
        double _v[4];

    public:
        Quat(double x = 0.0, double y = 0.0, double z = 0.0, double w = 1.0);
    //!    Uses point.M() for w
        Quat(const Point& point);
    //!    See makeRotation(double,const Point&)
        Quat(double angle, const Point& axis);
    //!    See makeRotation(const Point&,const Point&)
        Quat(const Point& from, const Point& to);
        Quat(const Quat& quat);
        ~Quat(void) { }

        Quat& operator=(const Quat& rhs);
        bool operator==(const Quat& rhs) const;
        bool operator!=(const Quat& rhs) const;
        
    //!    point.M() is used for w.
        Point asPoint(void) const;

        void set(double x, double y, double z, double w);
    //!    point.M() is used for w.
        void set(const Point& point);
        
        double& operator[](int i);
        double operator[](int i) const;
        
        double& x(void);
        double& y(void);
        double& z(void);
        double& w(void);
        double x(void) const;
        double y(void) const;
        double z(void) const;
        double w(void) const;

    //!    True if this rotation is a null rotation
        bool zero(void) const;

    //!    Scalar operators
        Quat operator*(double rhs) const;
        Quat& operator*=(double rhs);
        Quat operator/(double rhs) const;
        Quat& operator/=(double rhs);

    //!    Vector operators (Not all operators behave like a standard vector)
        Quat operator*(const Quat& rhs) const;
        Quat& operator*=(const Quat& rhs);
        Quat operator/(const Quat& rhs) const;
        Quat& operator/=(const Quat& rhs);
        Quat operator+(const Quat& rhs) const;
        Quat& operator+=(const Quat& rhs);
        Quat operator-(const Quat& rhs) const;
        Quat& operator-=(const Quat& rhs);

    //!    Standard length and length squared of a 4-vector.
        double length(void) const;
        double length2(void) const;

    //!    Conjugate of a Quat
        Quat conj(void) const;
    //!    Multiplicative inverse.
        Quat inverse(void) const;

    /*!    Methods for creating a Quaternion from rotation angles and axis vectors,
     *    or a Quaternion by taking the rotation required to transform one vector
     *    into another.
     */
        void makeRotation(double angle, double x, double y, double z);
        void makeRotation(double angle, const Point& axis);
        void makeRotation(const Point& from, const Point& to);
        
    //!    Converts this Quaternion into a representation of rotation axis and angle.
        void getRotation(double& angle, double& x, double& y, double& z) const;
        void getRotation(double& angle, Point& axis) const;
        
    //!    Rotate a vector by this quaternion
        Point operator*(const Point& v) const;
    };


}