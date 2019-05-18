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
/*! \brief Provides sfa::Matrix
\author Joshua Anghel <janghel@cognitics.net>
\date September 8, 2011
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "Point.h"
#include "Quat.h"

namespace sfa {
    class Point;
    class Quat;
/*!    \class sfa::Matrix Matrix.h Matrix.h
    \brief Matrix

    Defines a sfa friendly matrix class for transforming sfa::Points. A Matrix is a 4x4 matrix represented only by 12 values
    (the last row is always 0 0 0 1). 

    A Matrix can be create easily from a Quaterion. Though creating Quaternions and understanding a quaternions meaning is
    easier than understanding a matrix (and it is more memory efficient), but a Matrix can transform a Point much faster
    than a Quat can, so when speed is needed, use a Matrix.

    The Push methods can be used to incrimentally build up a Matrix transform. The order you call Push... in is the order the
    operations will be applied on any points in. REMEMBER THIS ORDER...IT DOES MATTER.

    The Matrix class is initialized (and can be reset to using clear) the identity matrix, but can be set to a zero matrix using
    zero().

    Matrix multiplication is supported by the * operator and to transform a point just multiply the matrix by that point.

    Example

\code
    //Create a quaternion for the rotation 
    Quat rotation_quat;
    rotation_quat.makeRotation(2*3.1415,1.0,4.0,2.6);    //    Random rotation
    
//    This example, translates a point by -5,-5,-5, scales, rotates, and translates the point back by 5,5,5 (in that order)

    //Create a Matrix (stack)
    Matrix matrix;
    //    Push a translate
    matrix.PushTranslate(Point(-5,-5,-5));
    //    Push a scale
    matrix.PushScale(5);
    //    Push a Rotation
    matrix.PushRotation(rotation_quat);
    //    Push the translate back
    matrix.PushTranslate(Point(5,5,5));
    
    //    Transform a Point
    Point original(1,2,3);
    Point newPoint = matrix*original;
\endcode
*/
    class Matrix 
    {
    private:
        double data[12];

    public:
        Matrix(void);
        Matrix(const Quat& quat);
        Matrix( double a1, double a2, double a3, double a4,
                double b1, double b2, double b3, double b4,
                double c1, double c2, double c3, double c4 );
        Matrix(const double args[]);
        Matrix(const Matrix& matrix);

        ~Matrix(void) { }

//!    Reset to the identity matrix
        void clear(void);
//!    Reset to a zero matrix
        void zero(void);

        void set(const Quat& quat);
        void set(    double a1, double a2, double a3, double a4,
                    double b1, double b2, double b3, double b4,
                    double c1, double c2, double c3, double c4 );
        void set(const double args[]);
        void set(const Matrix& matrix);

        double& operator()(int i, int j);
        double operator()(int i, int j) const;

        double* getData(void);
        const double* getData(void) const;

        sfa::Point getScale(void) const;
        sfa::Point getTranslation(void) const;
        Quat getRotation(void) const;

        Matrix operator*(const Matrix& matrix) const;
        Point operator*(const Point& point) const;

        void PushScale(double scale);
        void PushScale(double xscale, double yscale, double zscale);
        void PushScale(const Point& scale);
        void PushTranslate(double dx, double dy, double dz);
        void PushTranslate(const Point& trans);
        void PushRotate(double x, double y, double z, double angle);
        void PushRotate(const Point& axis, double angle);
        void PushRotate(const Point& from, const Point& to);
        void PushRotate(const Quat& quat);
        void PushMatrix(const Matrix& matrix);

    //    Create a transformation from S1 to S2 -> requires S1 and S2 to have at least 4 (non-colinear) points each
    //    Returns TRUE if a valid transform is found, FALSE otherwise
        bool CreateTransform(const std::vector<Point>& S1, const std::vector<Point>& S2);
        
    /*! Invert the matrix.
     *  \return TRUE if matrix was inverted successfully, FALSE otherwise.
     */
        bool invert(void);

        bool operator==(const Matrix &rhs) const;
    };

}