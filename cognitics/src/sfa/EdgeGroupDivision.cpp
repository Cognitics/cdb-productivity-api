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
#include "sfa/EdgeGroupDivision.h"

namespace sfa {

    EdgeGroupDivision::EdgeGroupDivision(void)
    {
        label.dim = 1;
        label.loc[0] = UNKNOWN;
        label.loc[0] = UNKNOWN;
    }

    EdgeGroupDivision::EdgeGroupDivision(const EdgeGroupDivision& other)
    {
        this->label = other.label;
        this->E = other.E;
    }

    EdgeGroupDivision::~EdgeGroupDivision(void)
    {
    }

    bool EdgeGroupDivision::isEdgeAdded(const EdgeNodeEnd* e) const
    {
        //    Search for edge, if found return true;
        for (EdgeNodeEndList::const_iterator it = E.begin(); it != E.end(); ++it)
            if ((*it)==e) return true;

        return false;
    }

    void EdgeGroupDivision::updateLabel(const EdgeNodeEnd* e, int n)
    {
        label.copyLabel(e->getOn(),n);
    }

    void EdgeGroupDivision::updateParent(EdgeNodeEnd* e, Label left, Label right) const
    {
        e->setLabels(left,label,right);
    }

    Location EdgeGroupDivision::getLocation(int n) const
    {
        return label.loc[n];
    }

    void EdgeGroupDivision::setLocation(int n, Location l)
    {
        label.loc[n] = l;
    }

    Label EdgeGroupDivision::getLabel(void) const
    {
        return label;
    }

    void EdgeGroupDivision::setLabel(const Label& label)
    {
        this->label = label;
    }

    void EdgeGroupDivision::addEdge(EdgeNodeEnd* e)
    {
        if (!isEdgeAdded(e))
        {
            E.push_back(e);
            updateLabel(e,e->getRootParent()->getGeomArg());
        }
    }

    void EdgeGroupDivision::getEdges(EdgeNodeEndList& list) const
    {
        list.push_back(E.front());
    }

    void EdgeGroupDivision::updateParents(const Label& left, const Label& right, const Label& center)
    {
        for (int i = 0; i < int(E.size()); i++)
        {
            E[i]->setLabels(left,label,right);
            E[i]->getOrigin()->setLabel(center);
            E[i]->getSym()->setLabels(right,label,left);
        }
    }

    void EdgeGroupDivision::updateIM(de9im* matrix) const
    {
        matrix->setAtLeast(label);
    }

    void EdgeGroupDivision::updateIM(de9im& matrix) const
    {
        matrix.setAtLeast(label);
    }

}