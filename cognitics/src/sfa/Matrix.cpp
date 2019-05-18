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
#include <math.h>
#include "sfa/Matrix.h"
#include <cmath>

namespace sfa {

    Matrix::Matrix(void) { clear(); }
    Matrix::Matrix(const Quat& quat) { set(quat); }

    Matrix::Matrix( double a1, double a2, double a3, double a4,
                    double b1, double b2, double b3, double b4,
                    double c1, double c2, double c3, double c4 )
    { set (a1,a2,a3,a4,b1,b2,b3,b4,c1,c2,c3,c4); }

    Matrix::Matrix(const double args[]) { set(args); }
    Matrix::Matrix(const Matrix& matrix) { set(matrix); }

    void Matrix::clear(void) 
    {
        zero();
        data[0] = data[5] = data[10] = 1.0;
    }

    void Matrix::zero(void)
    {
        for (int i = 0; i < 12; i++)
            data[i] = 0.0;
    }

    void Matrix::set(const Quat& quat)
    {
        double x2 = quat.x()*quat.x();
        double y2 = quat.y()*quat.y();
        double z2 = quat.z()*quat.z();
        double xy = quat.x()*quat.y();
        double xz = quat.x()*quat.z();
        double yz = quat.y()*quat.z();
        double xw = quat.x()*quat.w();
        double yw = quat.y()*quat.w();
        double zw = quat.z()*quat.w();

        data[0] = 1.0 - 2.0*(y2 + z2); data[1] = 2.0*(xy - zw); data[2] = 2.0*(xz + yw); data[3] = 0.0;
        data[4] = 2.0*(xy + zw); data[5] = 1.0 - 2.0*(x2 + z2); data[6] = 2.0*(yz - xw); data[7] = 0.0;
        data[8] = 2.0*(xz - yw); data[9] = 2.0*(yz + xw); data[10] = 1.0 - 2.0*(x2 + y2); data[11] = 0.0;
    }

    void Matrix::set(    double a1, double a2, double a3, double a4,
                        double b1, double b2, double b3, double b4,
                        double c1, double c2, double c3, double c4 )
    {
        data[0] = a1; data[1] = a2; data[2] = a3; data[3] = a4;
        data[4] = b1; data[5] = b2; data[6] = b3; data[7] = b4;
        data[8] = c1; data[9] = c2; data[10] = c3; data[11] = c4;
    }

    void Matrix::set(const double args[])
    {
        for (int i = 0; i < 12; i++)
            data[i] = args[i];
    }

    void Matrix::set(const Matrix& matrix)
    {
        for (int i = 0; i < 12; i++)
            data[i] = matrix.data[i];
    }

    double& Matrix::operator()(int i, int j)
    {
        return data[i%4 + j];
    }

    double Matrix::operator()(int i, int j) const 
    {
        return data[i*4 + j];
    }

    double* Matrix::getData(void) 
    {
        return data;
    }

    const double* Matrix::getData(void) const
    {
        return data;
    }

    sfa::Point Matrix::getScale(void) const
    {
        sfa::Point col1(data[0*4 + 0],data[1*4 + 0],data[2*4 + 0]);
        sfa::Point col2(data[0*4 + 1],data[1*4 + 1],data[2*4 + 1]);
        sfa::Point col3(data[0*4 + 2],data[1*4 + 2],data[2*4 + 2]);
        return sfa::Point(col1.length(),col2.length(),col3.length());
    }

    sfa::Point Matrix::getTranslation(void) const
    {
        return sfa::Point(data[0*4 + 3], data[1*4 + 3], data[2*4 + 3]);
    }

    Quat Matrix::getRotation(void) const
    {
        Quat q;

        sfa::Point scale = getScale();
        double a[3][3] = {    {data[0]/scale.X(), data[1]/scale.X(), data[2]/scale.X()},
                            {data[4]/scale.Y(), data[5]/scale.Y(), data[6]/scale.Y()},
                            {data[9]/scale.Z(), data[9]/scale.Z(), data[10]/scale.Z()} };

        double trace = a[0][0] + a[1][1] + a[2][2];
        if (trace > 0) {
            double s = 0.5 / sqrt(trace + 1.0);
            q[3] = 0.25 / s;
            q[0] = (a[2][1] - a[1][2])*s;
            q[1] = (a[0][2] - a[2][0])*s;
            q[2] = (a[1][0] - a[0][1])*s;
        } else {
            if ( a[0][0] > a[1][1] && a[0][0] > a[2][2] ) {
                double s = 2.0 * sqrtf( 1.0 + a[0][0] - a[1][1] - a[2][2]);
                q[3] = (a[2][1] - a[1][2] ) / s;
                q[0] = 0.25 * s;
                q[1] = (a[0][1] + a[1][0] ) / s;
                q[2] = (a[0][2] + a[2][0] ) / s;
            } else if (a[1][1] > a[2][2]) {
                double s = 2.0 * sqrtf( 1.0 + a[1][1] - a[0][0] - a[2][2]);
                q[3] = (a[0][2] - a[2][0] ) / s;
                q[0] = (a[0][1] + a[1][0] ) / s;
                q[1] = 0.25 * s;
                q[2] = (a[1][2] + a[2][1] ) / s;
            } else {
                double s = 2.0 * sqrtf( 1.0 + a[2][2] - a[0][0] - a[1][1] );
                q[3] = (a[1][0] - a[0][1] ) / s;
                q[0] = (a[0][2] + a[2][0] ) / s;
                q[1] = (a[1][2] + a[2][1] ) / s;
                q[2] = 0.25 * s;
            }
        }

        return q;
    }

    Matrix Matrix::operator*(const Matrix& matrix) const 
    {
        Matrix result;
        result.data[0] = data[0]*matrix.data[0] + data[1]*matrix.data[4] + data[2]*matrix.data[8];
        result.data[1] = data[0]*matrix.data[1] + data[1]*matrix.data[5] + data[2]*matrix.data[9];
        result.data[2] = data[0]*matrix.data[2] + data[1]*matrix.data[6] + data[2]*matrix.data[10];
        result.data[3] = data[0]*matrix.data[3] + data[1]*matrix.data[7] + data[2]*matrix.data[11] + data[3];

        result.data[4] = data[4]*matrix.data[0] + data[5]*matrix.data[4] + data[6]*matrix.data[8];
        result.data[5] = data[4]*matrix.data[1] + data[5]*matrix.data[5] + data[6]*matrix.data[9];
        result.data[6] = data[4]*matrix.data[2] + data[5]*matrix.data[6] + data[6]*matrix.data[10];
        result.data[7] = data[4]*matrix.data[3] + data[5]*matrix.data[7] + data[6]*matrix.data[11] + data[7];

        result.data[8] = data[8]*matrix.data[0] + data[9]*matrix.data[4] + data[10]*matrix.data[8];
        result.data[9] = data[8]*matrix.data[1] + data[9]*matrix.data[5] + data[10]*matrix.data[9];
        result.data[10] = data[8]*matrix.data[2] + data[9]*matrix.data[6] + data[10]*matrix.data[10];
        result.data[11] = data[8]*matrix.data[3] + data[9]*matrix.data[7] + data[10]*matrix.data[11] + data[11];
        return result;
    }

    Point Matrix::operator*(const Point& point) const 
    {
        Point result;
        result.setX( point.X()*data[0] + point.Y()*data[1] + point.Z()*data[2] + data[3] );
        result.setY( point.X()*data[4] + point.Y()*data[5] + point.Z()*data[6] + data[7] );
        result.setZ( point.X()*data[8] + point.Y()*data[9] + point.Z()*data[10] + data[11] );
        return result;
    }

    void Matrix::PushScale(double scale)
    {
        for (int i = 0; i < 12; i++)
            data[i] *= scale;
    }

    void Matrix::PushScale(double xscale, double yscale, double zscale)
    {
        Matrix temp (    xscale, 0, 0, 0,
                        0, yscale, 0, 0,
                        0, 0, zscale, 0        );
        set( temp*(*this) );
    }

    void Matrix::PushScale(const Point& scale) 
    {
        PushScale(scale.X(),scale.Y(),scale.Z());
    }

    void Matrix::PushTranslate(double dx, double dy, double dz)
    {
        data[3] += dx;
        data[7] += dy;
        data[11] += dz;
    }

    void Matrix::PushTranslate(const Point& trans)
    {
        PushTranslate(trans.X(),trans.Y(),trans.Z());
    }    

    void Matrix::PushRotate(double x, double y, double z, double angle)
    {
        Quat temp;
        temp.makeRotation(angle,x,y,z);
        PushRotate(temp);
    }

    void Matrix::PushRotate(const Point& axis, double angle)
    {
        Quat temp;
        temp.makeRotation(angle,axis);
        PushRotate(temp);
    }

    void Matrix::PushRotate(const Point& from, const Point& to)
    {
        Quat temp;
        temp.makeRotation(from,to);
        PushRotate(temp);
    }

    void Matrix::PushRotate(const Quat& quat)
    {
        Matrix result = Matrix(quat)*(*this);
        set(result);
    }

    void Matrix::PushMatrix(const Matrix& matrix)
    {
        Matrix result = matrix*(*this);
        set(result);
    }

    bool Matrix::CreateTransform(const std::vector<Point>& S1, const std::vector<Point>& S2)
    {
        double M[4][8] = {    {S1[0].X(), S1[1].X(), S1[2].X(), S1[3].X(), 1, 0, 0, 0},
                            {S1[0].Y(), S1[1].Y(), S1[2].Y(), S1[3].Y(), 0, 1, 0, 0},
                            {S1[0].Z(),    S1[1].Z(), S1[2].Z(), S1[3].Z(), 0, 0, 1, 0},
                            {1,         1,         1,         1,         0, 0, 0, 1} };

    //    Invert Matrix of points from S1
        for (int i=0; i<4; i++) 
        {
        //    Find a row with the max entry
            int max = i;
            for (int j=i+1; j<4; j++) 
            {
                if (abs(M[j][i]) > abs(M[max][i]))
                    max = j;
            }
        //    Swap for that row
            for (int j=0; j<8; j++)
                std::swap(M[max][j],M[i][j]);

        //    Normalize row
            double temp = M[i][i];
        //    Cannot solve
            if (temp == 0)
                return false;
            for (int j=i; j<8; j++)
                M[i][j] /= temp;

        //    Reduce other rows
            for (int j=0; j<4; j++)
            {
                if (i!=j)
                {
                    double temp = M[j][i];
                    for (int k=i; k<8; k++)
                        M[j][k] -= temp*M[i][k];
                }
            }
        }

        double temp[4];
    //    Multiply on the right side of S2
        for (int i=0; i<4; i++)
        {
        //    Row 1
            data[0 + i] = M[0][4 + i]*S2[0].X() + M[1][4 + i]*S2[1].X() + M[2][4 + i]*S2[2].X() + M[3][4 + i]*S2[3].X();
        //    Row 2
            data[4 + i] = M[0][4 + i]*S2[0].Y() + M[1][4 + i]*S2[1].Y() + M[2][4 + i]*S2[2].Y() + M[3][4 + i]*S2[3].Y();
        //    Row 3
            data[8 + i] = M[0][4 + i]*S2[0].Z() + M[1][4 + i]*S2[1].Z() + M[2][4 + i]*S2[2].Z() + M[3][4 + i]*S2[3].Z();

            temp[i] = M[0][4 + i] + M[1][4 + i] + M[2][4 + i] + M[3][4 + i];
        }

        return true;
    }

    bool Matrix::invert(void)
    {
    //  Fast inverse optimized for inverting an affine transformation.
        double inv[12], det;
        
        inv[0] = data[5]*data[10] - data[9]*data[6];
        inv[4] = -data[4]*data[10] + data[8]*data[6];
        inv[8] = data[4]*data[9] - data[8]*data[5];
        inv[1] = -data[1]*data[10] + data[9]*data[2];
        inv[5] = data[0]*data[10] - data[8]*data[2];
        inv[9] = -data[0]*data[9] +data[8]*data[1];
        inv[2] = data[1]*data[6] - data[5]*data[2];
        inv[6] = -data[0]*data[6] + data[4]*data[2];
        inv[10] = data[0]*data[5] - data[4]*data[1];
        
        inv[3] = -data[1] * data[6] * data[11] +
        data[1] * data[7] * data[10] +
        data[5] * data[2] * data[11] -
        data[5] * data[3] * data[10] -
        data[9] * data[2] * data[7] +
        data[9] * data[3] * data[6];
        
        inv[7] = data[0] * data[6] * data[11] -
        data[0] * data[7] * data[10] -
        data[4] * data[2] * data[11] +
        data[4] * data[3] * data[10] +
        data[8] * data[2] * data[7] -
        data[8] * data[3] * data[6];
        
        inv[11] = -data[0] * data[5] * data[11] +
        data[0] * data[7] * data[9] +
        data[4] * data[1] * data[11] -
        data[4] * data[3] * data[9] -
        data[8] * data[1] * data[7] +
        data[8] * data[3] * data[5];

        det = data[0]*inv[0] + data[1]*inv[4] + data[2]*inv[8];
        
        if (det == 0)
            return false;
        
        det = 1.0 / det;
        
        for (int i = 0; i < 12; i++)
            data[i] = inv[i] * det;
        
        return true;
    }
    
    bool Matrix::operator==(const Matrix &rhs) const
    {
        for(int i = 0; i < 12; ++i)
        {
            if(rhs.data[i] != data[i])
                return false;
        }
        return true;
    }

}
