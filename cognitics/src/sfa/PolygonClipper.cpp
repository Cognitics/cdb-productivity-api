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
#include "sfa/PolygonClipper.h"

namespace sfa {

    //Append points to a linestring while ensuring no duplicate points are added
    static void appendCropPoint(LineString& linestring, const sfa::Point& p)
    {
        if (linestring.isEmpty())
            linestring.addPoint(p);
        else if (!linestring.getEndPoint()->equals(&p) &&
                 !linestring.getStartPoint()->equals(&p))
            linestring.addPoint(p);
    }

    //Crop a single point using parametric clipping (Liang-Barsky)
    static bool cropTest
        (
        double num,
        double denom,
        double& tEnter,
        double& tLeave
        )
    {
        if (denom == 0)
            return num < 0;

        double t = num/denom;

        if (denom > 0)
        {
            if (t > tLeave)
                return false;
            if (t > tEnter)
                tEnter = t;
        }
        else
        {
            if (t < tEnter)
                return false;
            if (t < tLeave)
                tLeave = t;
        }

        return true;
    }

    //Clip a line using parametric clipping (Liang-Barsky)
    static void clipLine
        (
        const sfa::Point& p1, 
        const sfa::Point& p2, 
        double xmin,
        double xmax, 
        double ymin,
        double ymax,
        LineString& output
        )
    {
        double tEnter = 0;
        double tLeave = 1;
        sfa::Point d = p2 - p1;

        //Collapsed Line
        if (d.X() == 0 && d.Y() == 0)
            return;

        //Crop
        if (cropTest(xmin - p1.X(), d.X(), tEnter, tLeave) &&
            cropTest(p1.X() - xmax, -d.X(), tEnter, tLeave) &&
            cropTest(ymin - p1.Y(), d.Y(), tEnter, tLeave) &&
            cropTest(p1.Y() - ymax, -d.Y(), tEnter, tLeave))
        {
            appendCropPoint(output, p1 + d*tEnter);
            appendCropPoint(output, p1 + d*tLeave);
        }
    }

    Polygon clipPolygon(const Polygon& polygon, double xmin, double xmax, double ymin, double ymax)
    {
        Polygon result;
        if (polygon.getExteriorRing())
        {
            if (polygon.getExteriorRing()->getNumPoints() < 4)
                return result;

            LineString outline;
            for (size_t i=1; i<polygon.getExteriorRing()->getNumPoints(); i++)
            {
                sfa::Point p1 = *polygon.getExteriorRing()->getPointN(i-1);
                sfa::Point p2 = *polygon.getExteriorRing()->getPointN(i);
                clipLine(p1, p2, xmin, xmax, ymin, ymax, outline);
            }

            if (outline.getNumPoints() < 3)
                return result;

            outline.addPoint(*outline.getStartPoint());
            result.addRing(outline);
        }
        return result;
    }

}