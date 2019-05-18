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
#include "sfa/PointNode.h"

namespace sfa {

    PointNode::PointNode(const PointNode* node)
    {
        this->arg = node->arg;
        this->point = node->point;
        this->label = node->label;
        this->isolated = node->isolated;
    }
    
    int PointNode::getGeomArg(void) const
    {
        return arg;
    }

    void PointNode::setGeomArg(int geo)
    {
        arg = geo;
    }

    const Point* PointNode::getPoint(void) const
    {
        return &point;
    }

    void PointNode::setPoint(const Point& p)
    {
        point = p;
    }

    void PointNode::setPoint(const Point* p)
    {
        point = *p;
    }

    Label PointNode::getLabel(void) const
    {
        return label;
    }

    void PointNode::setLabel(Label l)
    {
        if (l.loc[0] != UNKNOWN) label.loc[0] = l.loc[0];
        if (l.loc[1] != UNKNOWN) label.loc[1] = l.loc[1];
    }

    Location PointNode::getLocation(int a) const
    {
        return label.loc[a];
    }

    void PointNode::setLocation(int n, Location l)
    {
        label.loc[n] = l;
    }

    bool PointNode::isIsolated(void) const
    {
        return isolated;
    }

    void PointNode::setIsolated(bool flag)
    {
        isolated = flag;
    }

    bool PointNode::equals(const PointNode* another) const
    {
        return point.equals(&another->point);
    }

    void PointNode::copyLocation(const PointNode* another)
    {
        label.copyLabel(another->getLabel());
    }

    void PointNode::copyLocation(const PointNode* another, int n)
    {
        label.copyLabel(another->getLabel(),n);
    }

    void PointNode::copyLocation(const Label& another)
    {
        label.copyLabel(another);
    }
    
    void PointNode::copyLocation(const Label& another, int n)
    {
        label.copyLabel(another,n);
    }

    void PointNode::updateIM(de9im* matrix) const
    {
        matrix->setAtLeast(label);
    }

    void PointNode::updateIM(de9im& matrix) const
    {
        matrix.setAtLeast(label);
    }

}