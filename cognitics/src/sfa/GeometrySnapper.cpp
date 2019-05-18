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
#include "sfa/GeometrySnapper.h"
#include "sfa/PointExtractor.h"
#include <float.h>

namespace sfa {

    void GeometrySnapper::ComputeSnapDistance(void)
    {
    //    Method 1 - Normalize based on middle-value of points
        double minx = DBL_MAX;
        double maxx = -DBL_MAX;
        double miny = DBL_MAX;
        double maxy = -DBL_MAX;
        double minz = DBL_MAX;
        double maxz = -DBL_MAX;
        for (size_t i=0, n=points.size(); i<n; i++)
        {
            minx = std::min<double>(points[i]->X(), minx);
            maxx = std::max<double>(points[i]->X(), maxx);
            miny = std::min<double>(points[i]->Y(), miny);
            maxy = std::max<double>(points[i]->Y(), maxy);
            minz = std::min<double>(points[i]->Z(), minz);
            maxz = std::max<double>(points[i]->Z(), maxz);
        }
        sfa::Point p((maxx+minx)/2,(maxy+miny)/2,(maxz+minz)/2);
        snapDistance = p.length()*SFA_EPSILON; 
    //    Method 2 - Normalize based on smallest segment length
        //TODO
    //    Method 3 - Normalize based on largest segment length
        //TODO
    }

    void GeometrySnapper::SnapPoint(Point* p, Point* s)
    {
        if (p->X() != s->X())
            p->setX(s->X());
        if (p->Y() != s->Y())
            p->setY(s->Y());
        if (p->Z() != s->Z())
            p->setZ(s->Z());
    }

    void GeometrySnapper::Snap(void)
    {
        for (size_t i=0, ni=points.size(); i<ni; i++)
            for (size_t j=0, nj=snapPoints.size(); j<nj; j++)
            {
            //    Method 1 - Distance
                /*
                if (points[i]->distance3D(snapPoints[j]) < snapDistance)
                    SnapPoint(points[i], snapPoints[j]);
                */
            //    Method 2 - Length
                /*
                if ((*points[i] - *snapPoints[j]).length() < snapDistance)
                    SnapPoint(points[i], snapPoints[j]);
                */
            //    Method 3 - Length2
                /*
                if ((*points[i] - *snapPoints[j]).length2() < snapDistance)
                    SnapPoint(points[i], snapPoints[j]);
                */
            //    Method 4 - bounding box
                Point p = *points[i] - *snapPoints[j];
                if (abs(p.X()) < snapDistance && abs(p.Y()) < snapDistance && abs(p.Z()) < snapDistance)
                    SnapPoint(points[i], snapPoints[j]);
            }
    }

    void GeometrySnapper::apply(Geometry* a, Geometry* b)
    {
        GeometrySnapper snapper;
        PointExtractor::apply(a, snapper.points);
        PointExtractor::apply(b, snapper.snapPoints);
        snapper.ComputeSnapDistance();
        snapper.Snap();
    }

    void GeometrySnapper::apply(Geometry* a, Geometry* b, double snapDistance)
    {
        GeometrySnapper snapper;
        PointExtractor::apply(a, snapper.points);
        PointExtractor::apply(b, snapper.snapPoints);
        snapper.snapDistance = snapDistance;
        snapper.Snap();
    }

}