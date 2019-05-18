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
/*! \brief Provides sfa::Overlay.
\author Josh Anghel <janghel@cognitics.net>
\date 4 November 2010
*/
#pragma once

#include "GraphOperation.h"
#include "EdgeNodeEnd.h"
#include "EdgeGroupBuilder.h"
#include "PolyhedralSurface.h"

namespace sfa {

    enum OverlayOperation
    {
        OVERLAY_INTERSECTION    =    0,
        OVERLAY_UNION            =    1,
        OVERLAY_DIFFERENCE        =    2,
        OVERLAY_SYMDIFFERENCE    =    3
    };

/*! \class sfa::Overlay Overlay.h Overlay.h
\brief Overlay

An Overlap is two interconnected GeometryGraphs which is constructed of linked EdgeNodes. These links allow
a boolean set operation to be performed on the GeometryGraphs. The supported operations are OVERLAY_INTERSECTION, OVERLAY_UNION,
OVERLAY_DIFFERENCE, and OVERLAY_SYMDIFFERENCE.

Usage:
\code
    sfa::GeometrySP a;
    sfa::GeometrySP b;
    sfa::GeometrySP result;

//    Use the overlay operations from the sfa::Geometry class
    result = a->intersection(b);    
    result = a->Union(b);
    result = a->difference(b);
    result = a->symDifference(b);

//    Or call the operations from a sfa::Overlay directly
    sfa::Overlay operation(a,b);
    result = operation.computeOverlay(OVERLAY_INTERSECTION);
    result = operation.computeOverlay(OVERLAY_UNION);
    result = operation.computeOverlay(OVERLAY_DIFFERENCE);
    result = operation.computeOverlay(OVERLAY_SYMDIFFERENCE);
\endcode

The benefit of using the Overlay class directly is that multiple OverlayOperations can be performed faster on the same two geometries since the GeometryGraphs
will only be generated and linked once when the sfa::Overlay object is created.

Note that this is a 2D method. It will only use the x and y values of the input Geometries, effectively flattening them. Also, the resulting geometry will be a 2D
geometry in the xy plane.
*/
    class Overlay : public GraphOperation
    {
    protected:
        char ProcessingError;    //    0x00 if no errors
                                //    0x01 if there was an error in generating a ring.

        virtual bool errorOccured(void) const;

        EdgeGroupBuilder* egb;

        EdgeNodeEndList    includedAreaEdges;    //    Not owned by this
        EdgeNodeEndList    includedLineEdges;    //    Not owned by this
        PointNodeList    includedPoints;        //    Not owned by this

        virtual bool isInResult(Label l, OverlayOperation op);
        virtual bool isInResult(Location l0, Location l1, OverlayOperation op);

        virtual void resetEdges(void);

    //    Consider separating divisions first, might save some time
        virtual void gatherIncludedAreaEdges(OverlayOperation op);
        virtual void gatherIncludedLineEdges(OverlayOperation op);
        virtual void gatherIncludedPoints(OverlayOperation op);

//!\brief Constructs LinearRings from the linked EdgeNodeEnds in includedAreaEdges.
        virtual LineStringList getMinimalRings(void);

//!\brief Constructs LineStrings from the EdgeNodeEnds in includedLineEdges
        virtual LineStringList    getLines(OverlayOperation op);

//!\brief Constructs Polygons from the EdgeNodeEnds in includedAreaEdges
        virtual PolygonList constructPolygons(LineStringList rings);

        virtual Geometry* handleEmpty(OverlayOperation op, bool A, bool B);
        virtual Geometry* handleDisjoint(OverlayOperation op);

    public:
        bool saveZ;            // if set to true, the intersection will attempt to preserve the Z-values of the input geometry; default is false

        Overlay(void) : saveZ(false) {}
        Overlay(const Geometry* a, const Geometry* b);
        Overlay(const GraphOperation* operation);
        ~Overlay(void);

//!\brief Performs the OverlayOperation op on the two geometries.
        virtual Geometry* computeOverlay(OverlayOperation op);

    };

}