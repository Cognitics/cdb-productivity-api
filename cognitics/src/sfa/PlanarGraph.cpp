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
#include "sfa/PlanarGraph.h"

namespace sfa {

    PlanarGraph::PlanarGraph(Projection2D projection) : proj(projection)
    {
        geo[0] = &collections[0];
        geo[1] = &collections[1];
    }

    PlanarGraph::PlanarGraph(const PlanarGraph& other)
    {
        geo[0] = other.geo[0];
        geo[1] = other.geo[1];
    }

    GeometryGraph* PlanarGraph::getGeometryGraph(int id)
    {
        return arg[id];
    }

    void PlanarGraph::addGeometry(const Geometry* geom, int id)
    {
        collections[id].addGeometry(geom->copy());
    }

    void PlanarGraph::add3DGeometry(const Geometry* geom, int id)
    {
        Geometry* temp = proj.transformGeometryTo2D(geom);
        addGeometry(temp,id);
        delete temp;
    }

    Projection2D PlanarGraph::getProjection(void) 
    {
        return proj;
    }

}