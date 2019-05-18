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
/*! \brief Provides sfa::EdgeGroup
\author Joshua Anghel <janghel@cognitics.net>
\date October 13, 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "EdgeNodeEnd.h"
#include "EdgeGroupDivision.h"

namespace sfa {

/*!\class sfa::EdgeGroup EdgeGroup.h EdgeGrou.h
\brief EdgeGroup

An EdgeGroup contains all EdgeNodeEnds incident upon a single defined PointNode. EdgeGroups could be defined as either 
incoming or outgoing edge nodes, but since we created symetrically linked EdgeNodeEnds we can choose to only link 1 type.
For this implementation, an EdgeGroup shall contain a sorted list of all EdgeNodeEnds LEAVING this PointNode.

These edge groups can be used to force label the parent EdgeNodeEnds and/or link EdgeNodeEnds together in series. Note that
before calling the linker, the EdgeNodeEnds should be flagged for whether to include in the result or not. If they are not
in the result, the linker will skip over them when assigning the links. Linked EdgeNodeEnds will link to the next directed
EdgeNodeEdge that is either maximum or minimum "turn" away.

If the labels for all EdgeNodeEnds in this group are not complete before calling LinkEdges, then it could (and will very likely)
result in the links never being created. Even if the EdgeNodeEdges are labeled afterwards, the links are not reforged until
calling LinkEdges again and could result in rings that go on forever or dead-end.
*/
    class EdgeGroup
    {
    protected:
        EdgeNodeList        includedEdges;        //    Not owned by this
        EdgeNodeEndList        E;                    //    Not owned by this

        std::vector<Label>    Partitions;
        std::vector<EdgeGroupDivision> Divisions;

        PointNode*            centerPoint;        //    Owned by this

        Label                mergeLabels(Label a, Label b) const;

//! \brief Update the centerPoint with information from e, but checks to make sure that the center point has not already been updated by e first.
        void                updateCenter(const EdgeNodeEnd* e);

    public:
        EdgeGroup(PointNode* cp);
        EdgeGroup(const EdgeGroup& other);
        ~EdgeGroup(void);

        PointNode*            getPoint(void) const;
        void                insertEdge(EdgeNodeEnd* e);

/*! \brief Forges links between all EdgeNodeEnds in this EdgeGroup that have been marked as included in the result.
!!WARNING!! Should only be called after all EdgeNodeEnds have had their Labels completed.
*/
        void                LinkEdges(void);

//! \brief Completes the labels of any unknown EdgesGroupDivisions and Partitions in this EdgeGroup by scanning the closes complete Partitions or EdgeGroupDivisions.
        void                completeLabels(int n);

//! \brief Updates all EdgeNodeEnds in this Group with information found in the EdgeGroup.
        void                updateParents(void);

//! \brief Updates an intersection matrix with information from this EdgeGroup
        void                updateIM(de9im* matrix) const;
        void                updateIM(de9im& matrix) const;
    };

    typedef std::vector<EdgeGroup> EdgeGroupList;

}