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
/*! \brief Provides sfa::EdgeGroupBuilder
\author Joshua Anghel <janghel@cognitics.net>
\date October 13, 2010
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once
#include "EdgeGroup.h"

namespace sfa {

/*!    \class sfa::EdgeGroupBuilder EdgeGroupBuilder.h EdgeGroupBuilder.h
\brief EdgeGroupBuilder

Takes a series of IntersectionNodes and creates EdgeGroupSegments from those IntersectionNodes. Then dynamically constructs
all the required EdgeGroups from those EdgeGroupSegments. The resulting EdgeGroups are stored internally by this class and accessed
only by the class.
*/
    class EdgeGroupBuilder
    {
    protected:
        EdgeNodeList    nodes;    //    Not owned by this
        EdgeNodeEndList ends;    //    Not owned by this
        EdgeGroupList    groups;    //    Owned by this

/*!\brief findGroup

Attempts to find the approapriate group by a simple scan of all currently constructed groups. If a group is found with an equivalent 
center point, then the the index location of that group will be retured. If no such group is found, a new group is constructed from that
center point and added to the end of the list. The index for that group is then returned.\

Currently only uses a simple scan method to search for the correct group. A quadinary tree structure can be developed in general for
optimized 2D geometric location searching and sorting and should be looked into more.
\param node PointNode to use in searching for the correct group.
\return Index of the group with the given center point.
*/
        int findGroup(PointNode* node);

//!\brief Adds an EdgeNodeEnd to the right group
        void addEdgeNodeEnd(EdgeNodeEnd* end);

//!\brief Adds an EdgeNode by creating two EdgeNodeEnds and inserting them
        void addEdgeNode(EdgeNode* edge);

    public:
        EdgeGroupBuilder(EdgeNodeList list): nodes(list) {}
        ~EdgeGroupBuilder(void) {}

        void constructGroups(void);
        void completeLabels(void);

//!\brief Updates the parent EdgeNodeEnds of each group with the labels derived from the group.
        void updateParents(void);

//!\brief Returns the EdgeNodeEnds created from the given EdgeNodes
        EdgeNodeEndList getEnds(void);

//!\brief Links the EdgeNodeEnds. This should only be used once the EdgeNodeEnds have been parsed to see if they should be included in the result.
        void linkEnds(void);

        void updateIM(de9im* matrix) const;
        void updateIM(de9im& matrix) const;
    };

}