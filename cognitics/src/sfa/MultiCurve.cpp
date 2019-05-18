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

#include "sfa/MultiCurve.h"
#include "sfa/Curve.h"
#include "sfa/MultiPoint.h"

#include <sstream>

namespace sfa
{
    MultiCurve::~MultiCurve(void)
    {
    }

    MultiCurve::MultiCurve(void)
    {
    }

    MultiCurve::MultiCurve(const MultiCurve& curves) : GeometryCollection(curves)
    {
    }

    MultiCurve::MultiCurve(const MultiCurve* curves) : GeometryCollection(curves)
    {
    }

    MultiCurve& MultiCurve::operator=(const MultiCurve& rhs)
    {
        clearGeometries();
        for(GeometryList::const_iterator it = rhs.geometries.begin(), end = rhs.geometries.end(); it != end; ++it)
            addGeometry((*it)->copy());
        return *this;
    }

    int MultiCurve::getDimension(void)
    {
        return 1;
    }

    Geometry* MultiCurve::getBoundary(void)
    {
        PointList points;
        std::vector<int> count;
        for (int i = 0; i < int(geometries.size()); i++)
        {
            MultiPoint* next = dynamic_cast<MultiPoint*>(geometries[i]->getBoundary());
            if (next)
            {
                for (int j = 1; j < next->getNumGeometries()+1; j++)
                {
                    Point* nextPoint = static_cast<Point*>(next->getGeometryN(j));

                    bool found = false;
                    for (int k = 0; k < int(points.size()); ++k)
                    {
                        if (points[k]->equals(nextPoint))
                        {
                            found = true;
                            count[k]++;
                        }
                    }
                    if (!found)
                    {
                        points.push_back(nextPoint);
                        count.push_back(1);
                    }
                }

                delete next;
            }
        }
        //add any points with an odd index
        MultiPoint* result = new MultiPoint;
        for (int i = 0; i < int(count.size()); i++)
        {
            if (count[i]%2 == 1) result->addGeometry(new Point(points[i]));
        }
        return result;
    }

    bool MultiCurve::isClosed(void)
    {
        for (GeometryList::iterator it = geometries.begin(); it != geometries.end(); ++it)
        {
            if (!dynamic_cast<Curve*>(*it)->isClosed()) return false;
        }
        return true;
    }

    double MultiCurve::getLength(void)
    {
        double length = 0;
        //sum up the length of all the curves
        for (GeometryList::iterator it = geometries.begin(); it != geometries.end(); ++it)
        {
            length += dynamic_cast<Curve*>(*it)->getLength();
        }
        return length;
    }

}