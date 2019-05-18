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
#include "sfa/Relate.h"
#include "sfa/Intersector.h"
#include "sfa/Intersector3D.h"

namespace sfa {
    
    Relate::Relate(const Geometry* a, const Geometry* b, bool is3D)
    {
        _is3D = is3D;
        A = a;
        B = b;
        computer = RelateOperation::getRelateComputer(a,b,is3D);
        created = false;
    }

    Relate::~Relate(void)
    {
        delete computer;
    }

    bool Relate::equals(void)
    {
        if (!A || !B) return false;

        //    Can't be equal if the dimensions are different
        if (A->getDimension() != B->getDimension()) return false;
        /*
            Override for points...others equals checks are not robust...they could fail in cases where
            linestrings are overdifined or when polygons have rings that are permuted.
        */
        int typeA = A->getWKBGeometryType(false,false);
        int typeB = B->getWKBGeometryType(false,false);
        if (typeA == wkbPoint) return _is3D ? A->equals3D(B) : A->equals(B);
        else if (typeB == wkbPoint) return _is3D ? B->equals3D(A) : B->equals(A);
        else  
        {
            if (!created) computeIM();
        //    If one is a multiPoint, it has no BOUNDARY, so it cannot intersect at the BOUNDARY in any way
            if (typeA == wkbMultiPoint || typeB == wkbMultiPoint) return intersectionMatrix.compare("TFFFFFFFT");
            if (A->getGeometryType() == "LinearRing" || B->getGeometryType() == "LinearRing") return intersectionMatrix.compare("TFFFFFFFT");
            else return intersectionMatrix.compare("TFFFTFFFT");
        }
    }

    bool Relate::disjoint(void)
    {
        return !intersects();
    }

    bool Relate::intersects(void)
    {
        if (!A || !B) return false;
        if(A->isEmpty() || B->isEmpty()) return false;
        return _is3D ? Intersector3D::apply(A,B) : Intersector::apply(A,B);
    }

    bool Relate::touches(void)
    {
        if (!A || !B) return false;
        if (!created) computeIM();
        return (intersectionMatrix.compare("FT*******") ||
                intersectionMatrix.compare("F**T*****") ||
                intersectionMatrix.compare("F***T****"));
    }

    bool Relate::crosses(void)
    {
        if (!A || !B) return false;
        if (!created) computeIM();
        int dimA = A->getDimension();
        int dimB = B->getDimension();

        if ( (dimA == 0 && dimB == 1) || (dimA == 0 && dimB == 2) || (dimA == 1 && dimB == 2) )
        {
            return intersectionMatrix.compare("T*T******");
        }
        else if (dimA == 1 && dimB == 1)
        {
            return intersectionMatrix.compare("0********");
        }
        else return false;
    }

    bool Relate::within(void)
    {
        if (!A || !B) return false;
        if (!created) computeIM();
        return intersectionMatrix.compare("T*F**F***");
    }

    bool Relate::overlaps(void)
    {
        if (!A || !B) return false;
        if (!created) computeIM();
        int dimA = A->getDimension();
        int dimB = B->getDimension();

        if ( (dimA == 0 && dimB == 0) || (dimA == 2 && dimB == 2) )
        {
            return intersectionMatrix.compare("T*T***T**");
        }
        else if (dimA == 1 && dimB == 1)
        {
            return intersectionMatrix.compare("1*T***T**");
        }
        else return false;
    }

    bool Relate::contains(void)
    {
        if (!A || !B) return false;
        if (!created) computeIM();
        intersectionMatrix.transpose();
        bool result = within();
        intersectionMatrix.transpose();
        return result;
    }

    bool Relate::relate(const std:: string &matrix)
    {
        if (!A || !B) return false;
        if (!created) computeIM();
        return intersectionMatrix.compare(matrix);
    }

    void Relate::computeIM(void)
    {
        if (!A || !B) return;
        intersectionMatrix = computer->computeIM();
        created = true;
    }

    de9im Relate::getMatrix(void)
    {
        if (!created) computeIM();
        return intersectionMatrix;
    }
}