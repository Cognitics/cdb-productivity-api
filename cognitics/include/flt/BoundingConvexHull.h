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
/*! \file flt/BoundingConvexHull.h
\headerfile flt/BoundingConvexHull.h
\brief Provides flt::BoundingConvexHull
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    struct BoundingConvexHullTriangle
    {
        ccl::BigEndian<double> vertex1X;
        ccl::BigEndian<double> vertex1Y;
        ccl::BigEndian<double> vertex1Z;
        ccl::BigEndian<double> vertex2X;
        ccl::BigEndian<double> vertex2Y;
        ccl::BigEndian<double> vertex2Z;
        ccl::BigEndian<double> vertex3X;
        ccl::BigEndian<double> vertex3Y;
        ccl::BigEndian<double> vertex3Z;
    };

    class BoundingConvexHull : public Record
    {
    public:
        std::vector<BoundingConvexHullTriangle> triangles;

        virtual ~BoundingConvexHull(void);
        BoundingConvexHull(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);
    };

}


