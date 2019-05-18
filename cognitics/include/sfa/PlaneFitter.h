/****************************************************************************
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
/* \brief Provides sfa::PlaneFitter.
\author Kevin Bentley <kbentley@cognitics.net>
\date 26 July 2015
\sa Trend Surface generator using the Levenberg-Marquardt algorithm to fit a (horizontalish) plane to a collection of 2D points
*/
#pragma once

#include "Point.h"

namespace sfa
{
    typedef std::pair<int, int> intpair_t;
    class PlaneFitter
    {
        ccl::ObjLog log;
        bool validFit;
        struct PlaneFitterData;
        PlaneFitterData *data;
        static void Callback(const double *par, int m_dat, const void *data, double *fvec, int *info);

    public:
        ~PlaneFitter();

        //If forceBelowPoints is true, the plane will be below the reference point, even if the error
        //is greater. It has the effect of making the error cubed when the plane is above a point instead
        //points is a vector of non-owned pointers
        PlaneFitter(sfa::PointList points, bool forceBelowPoints=false);
        // Do the fit. 
        bool Fit();
        bool getPlane(sfa::Point &p1, sfa::Point &p2, sfa::Point &p3);
        // This must be called before Fit is called
        void constrainPoint(size_t idx);
        void constrainPair(int a, int b);
        // set the basis to start with. only the Z can be modified by the fitter, so these must be independant points
        // Must call before calling Fit
        void setInitialBasis(sfa::Point pt, int idx);
    };


}