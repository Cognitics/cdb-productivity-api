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
#include "sfa/EnvelopeCheck.h"
#include "sfa/LineString.h"

namespace sfa {

    bool EnvelopeCheck::apply(const Geometry* a, const Geometry* b)
    {
        LineString* envA = dynamic_cast<LineString*>(a->getEnvelope());
        LineString* envB = dynamic_cast<LineString*>(b->getEnvelope());

        if (!envA && !envB)
            return false;
        else if (!envA) {
            delete envB;
            return false;
        }
        else if (!envB) {
            delete envA;
            return false;
        }

        Point* Amin = envA->getPointN(0);
        Point* Amax = envA->getPointN(1);
        Point* Bmin = envB->getPointN(0);
        Point* Bmax = envB->getPointN(1);

        bool result = true;

        if (Amin->X() > (Bmax->X() + SFA_EPSILON)) result = false;
        else if (Amin->Y() > (Bmax->Y() + SFA_EPSILON)) result =  false;
        else if (Amax->X() < (Bmin->X() - SFA_EPSILON)) result =  false;
        else if (Amax->Y() < (Bmin->Y() - SFA_EPSILON)) result =  false;

        delete envA;
        delete envB;
        return result;
    }

    bool EnvelopeCheck::apply3D(const Geometry* a, const Geometry* b)
    {
        LineString* envA = dynamic_cast<LineString*>(a->getEnvelope());
        LineString* envB = dynamic_cast<LineString*>(b->getEnvelope());

        if (!envA && !envB)
            return false;
        else if (!envA) {
            delete envB;
            return false;
        }
        else if (!envB) {
            delete envA;
            return false;
        }

        Point* Amin = envA->getPointN(0);
        Point* Amax = envA->getPointN(1);
        Point* Bmin = envB->getPointN(0);
        Point* Bmax = envB->getPointN(1);

        bool result = true;

        if (Amin->X() > (Bmax->X() + SFA_EPSILON)) result =  false;
        else if (Amin->Y() > (Bmax->Y() + SFA_EPSILON)) result =  false;
        else if (Amin->Z() > (Bmax->Z() + SFA_EPSILON)) result =  false;
        else if (Amax->X() < (Bmin->X() - SFA_EPSILON)) result =  false;
        else if (Amax->Y() < (Bmin->Y() - SFA_EPSILON)) result =  false;
        else if (Amax->Z() < (Bmin->Z() - SFA_EPSILON)) result =  false;

        delete envA;
        delete envB;
        return result;
    }

}