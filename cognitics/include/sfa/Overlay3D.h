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
/*! \brief Provides sfa::OverlayPlane and sfa::Overlay3D.
\author Josh Anghel <janghel@cognitics.net>
\date 22 November 2010
*/
#pragma once

#include "Overlay.h"
#include "Projection2D.h"
#include "PlanarGraph.h"
#include "PlanarGraphOperation.h"

namespace sfa {

/*!    \class sfa::OverlayPlane Overlay3D.h Overlay3D.h
\brief OverlayPlane

Provides an Overlay class with the ability to tranform 3D geometries to a projection on a 2D plane
*/
    class OverlayPlane : public PlanarGraph
    {
    public:
        OverlayPlane(Projection2D projection);
        Geometry* computeOverlay(OverlayOperation op);
    };

/*!    \class sfa::Overlay3D Overlay3D.h Overlay3D.h
\brief Overlay3D

Any given sfa::Geometry can be represented as a collection of 2D primatives. These 2D primatives can then
be collected into their respective planes. By treating each plane as a 2D environment and created an isomorphic
projection from 3D space onto that plane we can thus reduce any 3D geometry to a collection of 2D geometries.
By intersecting the planes first and transfering any portion of the geometries on each plane to the other, we
can then construct the resulting 3D overlay by itterating through each plane and constructing its overlay, 
projecting each result back into 3D coordinates, and then assembling them all into a GeometryCollection.

This class is used exactly the same way as the 2D sfa::Overlay counterpart. Only instead of returning a 2D
Geometry representing the overlay of the geometries xy projections, it will return the fully construct 3D
Geometry. Note that some 3D Geometries may intersect in the xy plane, but have no intersection in 3D space
- ie two planes identical in the xy plane, but parallel and separated in the z plane will be equal to the
Overlay class but disjoint to the Overlay3D class.

See sfa::Overlay for usage.

Note that this is a 3D algorithm and thus will handle geometries in 3D. Geometries do not need to be valid in 2D xy
space, but they do need to be valid in the xyz space, or incorrect results will be reported.
*/
    class Overlay3D : public PlanarGraphOperation
    {
    public:
        Overlay3D(const Geometry* a, const Geometry* b);
        ~Overlay3D(void) {}

        virtual PlanarGraph* createPlanarGraph(Projection2D proj);
        virtual Geometry* computeOverlay(OverlayOperation op);
    };

}