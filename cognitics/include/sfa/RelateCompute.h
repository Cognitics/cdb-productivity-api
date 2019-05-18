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
/*! \brief Provides sfa::RelateCompute.
\author Josh Anghel <janghel@cognitics.net>
\date 8 November 2010
*/
#pragma once
#include "RelateOperation.h"
#include "GraphOperation.h"

namespace sfa {

/*!    \class sfa::RelateCompute RelateCompute.h RelateCompute.h
\brief RelateCompute

TODO:: add more documentation

    Work-horse of the relational operations in the sfa.
    Takes in two Geometies, turns them into GeometryGraphs, computes self intersections,
    computes proper and improper intersections, and then constructs the de9im from those graphs.
    Specially implemented routines for each possible pair of Geometries was formerly used; however,
    even though these implementations were very optimal, they could not handle any kind of collection
    of Geometries very well and were not robust enough to handle special cases outside of their
    onw single-single geometry reference frames. Those functions will still be used for specialized
    3D relations
*/
    class RelateCompute : public RelateOperation, public GraphOperation
    {
    protected:
        bool    disjoint;

        void    updateIM(void);
        de9im    computeDisjointIM(void);

    public:
        RelateCompute(void) {}
        RelateCompute(const Geometry* a, const Geometry* b);
        RelateCompute(const GraphOperation* other);
        virtual ~RelateCompute(void) {}

        virtual de9im computeIM(void);
    };

    typedef std::shared_ptr<RelateCompute> RelateComputeSP;

}