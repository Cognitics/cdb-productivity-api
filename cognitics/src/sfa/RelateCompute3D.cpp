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
#include "sfa/RelateCompute3D.h"
#include "sfa/PointMath.h"

namespace sfa {

//    RelatePlane  -------------------------------------------------------------------------------

    RelatePlane::RelatePlane(Projection2D projection) : PlanarGraph(projection)
    {
    }

    de9im RelatePlane::computeIM(void)
    {
        RelateCompute op(this);
        op.setGeometryGraph(this->getGeometryGraph(0),0);
        op.setGeometryGraph(this->getGeometryGraph(1),1);
        de9im matrix = op.computeIM();
        op.setGeometryGraph(NULL,0);
        op.setGeometryGraph(NULL,1);
        return matrix;
    }

//    Overlay3D  ---------------------------------------------------------------------------------

    void RelateCompute3D::addPoint(const Geometry* point, int arg)
    {
        intersectionMatrix.setAtLeast(arg==0 ? INTERIOR : EXTERIOR, arg==0 ? EXTERIOR : INTERIOR, 0);
    }

    PlanarGraph* RelateCompute3D::createPlanarGraph(Projection2D proj)
    {
        return new RelatePlane(proj);
    }

    RelateCompute3D::RelateCompute3D(const Geometry* a, const Geometry* b)
    {
        A = a;
        B = b;
        initialized = false;
        //initialize(a,b);
    }

    de9im RelateCompute3D::computeIM(void)
    {
        if (!initialized)
            initialize(A,B);

        for (int i = 0; i < int(planes.size()); i++)
            intersectionMatrix.setAtLeast(static_cast<RelatePlane*>(planes[i])->computeIM());

        intersectionMatrix.setAtLeast(EXTERIOR,EXTERIOR,3);
        return intersectionMatrix;
    }

}