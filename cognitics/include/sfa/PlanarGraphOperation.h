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
#pragma once
#include "PlanarGraph.h"
#include "GraphOperation.h"
#include <vector>

namespace sfa {

    class PlanarGraphOperation
    {
    protected:

        std::vector<PlanarGraph*> planes;
        PointList isolatedPoints[2];        //DISJOINT POINTS

        std::vector<const Polygon*> polygons[2];
        std::vector<const LineString*> lines[2];
        std::vector<const Point*> points[2];

        PlanarGraphOperation(void) { }
        ~PlanarGraphOperation(void);

    public:

        virtual void initialize(const Geometry* a, const Geometry* b);

        virtual PlanarGraph* createPlanarGraph(Projection2D proj) = 0;

        virtual void queueGeometry(const Geometry* geom, int arg);
        virtual void queuePoint(const Geometry* point, int arg);
        virtual void queueLineString(const Geometry* line, int arg);
        virtual void queuePolygon(const Geometry* polygon, int arg);
        virtual void queueSurface(const Geometry* surface, int arg);
        virtual void queueCollection(const Geometry* collection, int arg);

        virtual void addPolygons(int arg);
        virtual void addLines(int arg);
        virtual void addPoints(int arg);

        virtual void addPolygon(const Geometry* polygon, int arg);
        virtual void addSegment(const Point* p1, const Point* p2, int arg);
        virtual void addLineString(const Geometry* linestring, int arg);
        virtual void addPoint(const Geometry* point, int arg);

        virtual bool doPlanesIntersect(int p1, int p2);
        virtual void getIntersectingLine(int p1, int p2, Point &p, Point &direction);
        virtual void getIntersectionEdgeFromLine(Point* point, Point* destination, EdgeNode* edge, int source, EdgeNodeList &resultingEdges, PointNodeList &resultingPoints);
        virtual void getIntersectionEdgeFromArea(Point* point, Point* destination, EdgeNode* edge, int source, EdgeNodeList &resultingEdges, PointNodeList &resultingPoints);
        virtual void combinePlanes(int p1, int p2);
        virtual void intersectPlanes(void);

    };

}