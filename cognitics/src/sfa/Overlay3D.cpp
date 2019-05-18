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
#include "sfa/Overlay3D.h"
#include "sfa/PointMath.h"

namespace sfa {

//    OverlayPlane  ------------------------------------------------------------------------------

    OverlayPlane::OverlayPlane(Projection2D projection) : PlanarGraph(projection)
    {
    }

    Geometry* OverlayPlane::computeOverlay(OverlayOperation op)
    {
        Overlay operation(this);
        operation.setGeometryGraph(this->getGeometryGraph(0),0);
        operation.setGeometryGraph(this->getGeometryGraph(1),1);
        Geometry* geometry = operation.computeOverlay(op);
        operation.setGeometryGraph(NULL,0);
        operation.setGeometryGraph(NULL,1);
        return geometry;
    }

//    Overlay3D  ---------------------------------------------------------------------------------

    PlanarGraph* Overlay3D::createPlanarGraph(Projection2D proj)
    {
        return new OverlayPlane(proj);
    }

    Overlay3D::Overlay3D(const Geometry* a, const Geometry* b)
    {
        initialize(a,b);
    }

    Geometry* Overlay3D::computeOverlay(OverlayOperation op)
    {
        GeometryCollection* result = new GeometryCollection;

        Geometry* next = NULL;
        for (int i = 0; i < int(planes.size()); i++)
        {
            next = static_cast<OverlayPlane*>(planes[i])->computeOverlay(op);
            if (next) 
            {
                result->addGeometry(planes[i]->getProjection().transformGeometryTo3D(next));
                delete next;
            }
        }

        if (op != OVERLAY_INTERSECTION)
        {
            for (int i = 0; i < int(isolatedPoints[0].size()); i++)
                result->addGeometry(isolatedPoints[0][i]);

            if (op != OVERLAY_DIFFERENCE)
            {
                for (int i = 0; i < int(isolatedPoints[1].size()); i++)
                    result->addGeometry(isolatedPoints[1][i]);
            }
        }

        if (result->isEmpty()) return NULL;
        else if (result->getNumGeometries() == 1)
        {
            Geometry* temp = result->getGeometryN(1)->copy(); 
            delete result;
            return temp;
        }
        else return result;
    }


}