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
/*! \brief Provides sfa::PointNode
\author Josh Anghel <janghel@cognitics.net>
\date 4 November 2010
*/
#pragma once

#include "Label.h"
#include "Point.h"
#include "de9im.h"

namespace sfa {

    class PointNode;
    typedef std::vector<PointNode*> PointNodeList;

/*! \class sfa::PointNode PointNode.h PointNode.h
\brief PointNode

Provides a labeled point.
*/
    class PointNode
    {
    protected:
        int                arg;
        Point            point;
        Label            label;
        bool            isolated;

    public:
        PointNode(void) { }
        PointNode(const Point& p, Label label, int geo, bool isolated = false) : point(p), label(label), arg(geo), isolated(isolated) { }
        PointNode(const Point* p, Label label, int geo, bool isolated = false) : point(*p), label(label), arg(geo), isolated(isolated) { }
        PointNode(const PointNode* node);
        ~PointNode(void) { }

        int                getGeomArg(void) const;
        void            setGeomArg(int geo);
    
        const Point*    getPoint(void) const;
        void            setPoint(const Point& p);
        void            setPoint(const Point* p);

        Label            getLabel(void) const;
        void            setLabel(Label l);

        Location        getLocation(int n) const;
        void            setLocation(int n, Location l);

        bool            isIsolated(void) const;
        void            setIsolated(bool flag);

    //    Two Point Nodes are spacial equal if their two points are equal
        bool            equals(const PointNode* another) const;

        void            copyLocation(const PointNode* another);
        void            copyLocation(const PointNode* another, int n);
        void            copyLocation(const Label& another);
        void            copyLocation(const Label& another, int n);

        void            updateIM(de9im* matrix) const;
        void            updateIM(de9im& matrix) const;
    };

}