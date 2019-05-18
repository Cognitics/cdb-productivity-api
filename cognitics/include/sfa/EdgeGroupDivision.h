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
/*! \brief Provides sfa::EdgeGroupDivision
\author Joshua Anghel <janghel@cognitics.net>
\date 13 October 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "EdgeNodeEnd.h"

namespace sfa {

/*!\class EdgeGroupDivision EdgeGroupDivision.h EdgeGroupDivision.h
\brief EdgeGroupDivision

Represents a collection of EdgeNodeEnds that not only belong to the same EdgeGroup, but who are all incident away from
the group along the same direction. Thus these edges are merged into each other. This structure is important, because
we cannot keep duplicate edges in the labeling process directly, but nor can we throw them away if we intent to back 
update the parents from the label. So the solution is to keep track of all the bundled edges in an EdgeGroupDivision
which also acts as a dividing line to partition space in the EdgeGroup.
*/
    class EdgeGroupDivision
    {
    protected:
        Label            label;

//    All ends that are part of this division. Note that they are all away edges
        EdgeNodeEndList E;

        bool            isEdgeAdded(const EdgeNodeEnd* e) const;
        void            updateLabel(const EdgeNodeEnd* e, int n);

//!\brief Forcibly updates the given EdgeNodeEnds in this Division with the information from the given labels
        void            updateParent(EdgeNodeEnd* e, Label left, Label right) const;

    public:
        EdgeGroupDivision(void);
        EdgeGroupDivision(const EdgeGroupDivision& other);
        ~EdgeGroupDivision(void);

        Location        getLocation(int n) const;
        void            setLocation(int n, Location l);
        Label            getLabel(void) const;
        void            setLabel(const Label& label);
    
/*!\brief addEdge
Tests EdgeGroupSegment e to see if its already added to this EdgeGroupDivision, if not it will
update the Label of this division with the information from e.
\param e EdgeGroupSegment to potentially add to this EdgeGroupDivision
*/
        void            addEdge(EdgeNodeEnd* e);

//!\brief Adds the first EdgeNodeEnd from this group into the list. This prevents adding duplicate EdgeNodeEnds to any list.
        void            getEdges(EdgeNodeEndList& list) const;

//!\brief Update all members of this EdgeGroupDivision with the information of this Division, as well as the given left and right Label.
        void            updateParents(const Label& left, const Label& right, const Label& center);

        void            updateIM(de9im* matrix) const;
        void            updateIM(de9im& matrix) const;
    };

}