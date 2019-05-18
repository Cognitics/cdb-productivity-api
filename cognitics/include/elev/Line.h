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
/*! \file Line.h
\headerfile Line.h elev/Line.h

\brief Provides elev::Line, a Line represented by two points.

\author Aaron Brinton <abrinton@cognitics.net>
\date 17 June 2009
*/
#pragma once

#include <sfa/Point.h>

namespace elev
{
/*! \class elev::Line Line.h elev/Line.h
\brief A line represented by two points.

\sa elev::Point
*/
    class Line
    {
    public:
        const sfa::Point *a;
        const sfa::Point *b;

        Line(const sfa::Point *a, const sfa::Point *b) : a(a), b(b) { }

        double Length()
        {
            return a->distance(b);
        }

        //! Interpolates axis value(s) using an existing axis point.
        /*! This is using the parametric line equation:

            x = x1 + t(x2-x1)
            y = y1 + t(y2-y1)
            z = z1 + t(z2-z1)

            these return false if b.from - a.from is zero
        */
        bool InterpolateXfromY(sfa::Point *p)
        {
            if(b->Y() - a->Y() == 0)
                return false;
            p->setX(a->X() + ((p->Y() - a->Y()) / (b->Y() - a->Y()) * (b->X() - a->X())));
            return true;
        }

        bool InterpolateXfromZ(sfa::Point *p)
        {
            if(b->Z() - a->Z() == 0)
                return false;
            p->setX(a->X() + ((p->Z() - a->Z()) / (b->Z() - a->Z()) * (b->X() - a->X())));
            return true;
        }

        bool InterpolateYfromX(sfa::Point *p)
        {
            if(b->X() - a->X() == 0)
                return false;
            p->setY(a->Y() + ((p->X() - a->X()) / (b->X() - a->X()) * (b->Y() - a->Y())));
            return true;
        }

        bool InterpolateYfromZ(sfa::Point *p)
        {
            if(b->Z() - a->Z() == 0)
                return false;
            p->setY(a->Y() + ((p->Z() - a->Z()) / (b->Z() - a->Z()) * (b->Y() - a->Y())));
            return true;
        }

        bool InterpolateZfromX(sfa::Point *p)
        {
            if(b->X() - a->X() == 0)
                return false;
            p->setZ(a->Z() + ((p->X() - a->X()) / (b->X() - a->X()) * (b->Z() - a->Z())));
            return true;
        }

        bool InterpolateZfromY(sfa::Point *p)
        {
            if(b->Y() - a->Y() == 0)
                return false;
            p->setZ(a->Z() + ((p->Y() - a->Y()) / (b->Y() - a->Y()) * (b->Z() - a->Z())));
            return true;
        }


    };

}