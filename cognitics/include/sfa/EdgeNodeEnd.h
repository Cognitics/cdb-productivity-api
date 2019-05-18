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
/*! \brief Provides sfa::EdgeNodeEnd.
\author Josh Anghel <janghel@cognitics.net>
\date 4 November 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "EdgeNode.h"
#include "LineString.h"

namespace sfa {

/*! \class sfa::EdgeNodeEnd EdgeNodeEnd.h EdgeNodeEnd.h
\brief EdgeNodeEnd

An EdgeNodeEnd represents a a directed end point of a non-intersecting EdgeNode that starts at a single PointNode and moves away from this node. Every EdgeNodeEnd has a symetrically linked
EdgeNodeEdge that begins at the other end of the parent EdgeNode and terminates at this EdgeNodeEnds end point. EdgeNodeEnds are comparable by sorting about
the central point in a clockwise direction. Because EdgeNodeEnds represent EdgeNodes that are non-intersecting (except at end points), the partitions in space they create
when placed ends together (EdgeGroup) can be used to complete labels of nearby EdgeNodeEnds.
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
    class EdgeNodeEnd
    {
        friend class EdgeNode;

    protected:
        bool            saveZ;        // preserve Z-values

        EdgeNode*        parent;        //    Parent EdgeNode -> Owns this Edge
        bool            away;
        PointNode*        point;

        Label            left;
        Label            on;
        Label            right;

        double            dx;
        double            dy;

        EdgeNodeEnd*    next;        //    Next minimal ring path
        EdgeNodeEnd*    sym;
        bool            inResult;
        bool            visited;

/*!    Defines the quadrant that this EdgeNodeEnd lies in with respect to the center point. Quadrants are defined clockwise from +y axis.
    Quadrant 1 includes the + y axis, Quadrant 2 includes the + x axis, Quadrant 3 includes the - y axis, and Quadrant 4 includes the - x axis.
*/
        int                Quadrant;

        EdgeNodeEnd(EdgeNode* e, bool away, bool saveZ);

    public:
        ~EdgeNodeEnd(void) {}

        EdgeNode*        getParent(void) const;
        EdgeNode*        getRootParent(void) const;    //    Equivalent to parent->parent

        Label            getLeft(void) const;
        Location        getLeft(int n) const;
        void            setLeft(Label label);
        void            setLeft(int n, Location l);
        Label            getOn(void) const;
        Location        getOn(int n) const;
        void            setOn(Label label);
        void            setOn(int n, Location l);
        Label            getRight(void) const;
        Location        getRight(int n) const;
        void            setRight(Label label);
        void            setRight(int n, Location l);

        PointNode*        getOrigin(void) const;
        void            setOriginLabel(const Label& label);
        double            getDx(void) const;
        double            getDy(void) const;

        void            setNext(EdgeNodeEnd* next);
        EdgeNodeEnd*    getNext(void) const;

        bool            isInResult(void) const;
        void            setInResult(bool flag);

        bool            isVisited(void) const;
        void            setVisited(bool flag);
        void            visit(void);

        EdgeNodeEnd*    getSym(void) const;
        void            setSym(EdgeNodeEnd* sym);

//!    \return Returns True if this EdgeNodeEnd represents a line (its left and right Labels are the same value for at least one Geometry), False otherwise.
        bool            isLine(void) const;
//!    \return Returns True if this EdgeNodeEnd represents an area (its left and right Labels are of different values for at least one Geometry), False otherwise.
        bool            isArea(void) const;

//!    \return Returns True if the parent EdgeNode is equal to that of the other parent EdgeNode.
        bool            equals(const EdgeNodeEnd* another) const;

/*!    Compares the EdgeNodeEnd. The EdgeNodeEnd that is more clockwise about the center will be greater. It is assumed that both EdgeNodeEnds will have the same center (origin).
\param other EdgeNodeEnd to compare this EdgeNodeEnd to.
\return 1 if this EdgeNodeEnd is clockwise relative to the other, -1 if this EdgeNodeEdge is counterclockwise relative to the other, and 0 if the EdgeNodeEnds both radiate in the same direction.
*/
        int                compare(const EdgeNodeEnd* other) const;

/*!    Adds all Points in this EdgeNodeEnds parent Edge to the end of the given line. Does not; however, add the first point.
\param line LineString to append Points to.
*/
        void            appendToLineString(LineString* line) const;

/*!    Forces the Labels to the given values if the given values are not UNKNOWN.
\param left Label to use in updating the left Label of this EdgeNodeEnd.
\param on Label to use in updating the Label along this EdgeNodeEnd.
\param right Label to use in updating the right Label of this EdgeNodeEnd.
*/
        void            setLabels(const Label& left, const Label& on, const Label& right);

/*!    Updates an intersection matrix based on the information in this EdgeNodeEnd
\param matrix DE9IM matrix to update values for from this EdgeNodeEnd.
*/
        void            updateIM(de9im* matrix) const;
        void            updateIM(de9im& matrix) const;

    };

}