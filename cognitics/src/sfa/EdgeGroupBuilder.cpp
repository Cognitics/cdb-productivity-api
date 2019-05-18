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
#include "sfa/EdgeGroupBuilder.h"

namespace sfa {

    int EdgeGroupBuilder::findGroup(PointNode* node)
    {
        for (int i = 0; i < int(groups.size()); i++)
        {
            if (groups[i].getPoint()->equals(node)) return i;
        }
        return -1;
    }

    void EdgeGroupBuilder::addEdgeNodeEnd(EdgeNodeEnd* end)
    {
        ends.push_back(end);
        size_t location = findGroup(end->getOrigin());

        if (location == -1)
        {
            groups.push_back(EdgeGroup(end->getOrigin()));
            location = groups.size() - 1;
        }

        groups[location].insertEdge(end);
    }

    void EdgeGroupBuilder::addEdgeNode(EdgeNode* edge)
    {
        addEdgeNodeEnd(edge->getEnd(0));
        addEdgeNodeEnd(edge->getEnd(1));
    }

    void EdgeGroupBuilder::constructGroups(void)
    {
        for (EdgeNodeList::iterator it = nodes.begin(); it != nodes.end(); ++it)
            addEdgeNode(*it);
    }

    void EdgeGroupBuilder::completeLabels(void)
    {
        for(EdgeGroupList::iterator it = groups.begin(); it != groups.end(); ++it)
        {
            it->completeLabels(0);
            it->completeLabels(1);
        }
    }

    void EdgeGroupBuilder::updateParents(void)
    {
        for (EdgeGroupList::iterator it = groups.begin(); it != groups.end(); ++it)
            it->updateParents();
    }

    EdgeNodeEndList EdgeGroupBuilder::getEnds(void)
    {
        return ends;
    }

    void EdgeGroupBuilder::linkEnds(void)
    {
        for (EdgeGroupList::iterator it = groups.begin(); it != groups.end(); ++it)
            it->LinkEdges();
    }

    void EdgeGroupBuilder::updateIM(de9im* matrix) const
    {
        for (EdgeGroupList::const_iterator it = groups.begin(), end = groups.end(); it != end; ++it)
            it->updateIM(matrix);
    }

}