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
#include "sfa/ConvexHull3D.h"
#include "sfa/PointMath.h"
#include "sfa/PolyhedralSurface.h"
#include "sfa/Projection2D.h"
#include "sfa/ConvexHull.h"

#include <algorithm>

namespace sfa {

    ConvexHull3D::HullPoint::HullPoint(void)
    {
        coneEdge = NULL;
    }

    ConvexHull3D::HullEdge::HullEdge(void)
    {
        sym = NULL;
        ends[0] = NULL;
        ends[1] = NULL;
        left = right = NULL;
        prev = next = NULL;
    }

    void ConvexHull3D::reset(void)
    {
        for (int i = 0; i < int(hull_points.size()); i++)
            delete hull_points[i];
        for (int i = 0; i < int(hull_edges.size()); i++)
            delete hull_edges[i];
        for (int i = 0; i < int(hull_faces.size()); i++)
            delete hull_faces[i];

        hull_points.clear();
        hull_edges.clear();
        hull_faces.clear();
    }

    void ConvexHull3D::setPoints(const MultiPoint* points)
    {
        reset();
        multiPoint = points;

        for (int i = 1; i <= points->getNumGeometries(); i++)
        {
            HullPoint* next = new HullPoint;
            next->point = *static_cast<Point*>(points->getGeometryN(i));
            next->processed = false;
            hull_points.push_back(next);
        }
    }

    ConvexHull3D::HullEdge* ConvexHull3D::makeEdge(ConvexHull3D::HullPoint* p0, ConvexHull3D::HullPoint* p1)
    {
        HullEdge* e0 = new HullEdge;
        HullEdge* e1 = new HullEdge;

    //    Symetrically link edges
        e0->sym = e1;
        e1->sym = e0;

    //    set end points
        e0->ends[0] = e1->ends[1] = p0;
        e0->ends[1] = e1->ends[0] = p1;
        e0->toDelete = e1->toDelete = false;

    //    Add to vector
        hull_edges.push_back(e0);
        hull_edges.push_back(e1);

    //    Return the first edge
        return e0;
    }

    int ConvexHull3D::volumeSign(ConvexHull3D::HullFace* f, ConvexHull3D::HullPoint* p)
    {
        double vol;
        double ax, ay, az, bx, by, bz, cx, cy, cz;

        ax = f->points[0].X() - p->point.X();
        ay = f->points[0].Y() - p->point.Y();
        az = f->points[0].Z() - p->point.Z();
        bx = f->points[1].X() - p->point.X();
        by = f->points[1].Y() - p->point.Y();
        bz = f->points[1].Z() - p->point.Z();
        cx = f->points[2].X() - p->point.X();
        cy = f->points[2].Y() - p->point.Y();
        cz = f->points[2].Z() - p->point.Z();

    //    Addaptive epsilon value
        double eps = SFA_EPSILON*(abs(ax)+abs(ay)+abs(az)+abs(bx)+abs(by)+abs(bz)+abs(cx)+abs(cy)+abs(cz));

        vol = ax*(by*cz - bz*cy) + ay*(bz*cx - bx*cz) + az*(bx*cy - by*cx);

        if (vol > eps) return 1;
        else if (vol < -eps) return -1;
        else return 0;
    }

    ConvexHull3D::HullFace* ConvexHull3D::LinkFaceEdge(ConvexHull3D::HullEdge* e, ConvexHull3D::HullPoint* p)
    {
    //    Create new face
        HullFace* face = new HullFace;
        face->visible = false;

    //    Create new edges if needed
        if (!e->ends[0]->coneEdge) 
            e->ends[0]->coneEdge = makeEdge(e->ends[0],p);
        if (!e->ends[1]->coneEdge)
            e->ends[1]->coneEdge = makeEdge(e->ends[1],p);

    //    Link all edges together
        e->next = e->ends[1]->coneEdge;
        e->next->prev = e;
        e->prev = e->ends[0]->coneEdge->sym;
        e->prev->next = e;
        e->next->next = e->prev;
        e->prev->prev = e->next;

    //    Link faces
        e->left = e->next->left = e->prev->left = face;

    //    Link right faces and symetric faces
        e->sym->right = face;
        e->next->right = e->next->sym->left;
        e->prev->right = e->prev->sym->left;
        e->next->sym->right = face;
        e->prev->sym->right = face;

    //    Set the points of the face
        face->points[0] = e->ends[0]->point;
        face->points[1] = e->ends[1]->point;
        face->points[2] = e->next->ends[1]->point;
    
        return face;
    }

    bool ConvexHull3D::addOne(ConvexHull3D::HullPoint* p)
    {
        bool vis = false;
        p->processed = true;

    //    Find all visible edges
        for (int i = 0; i < int(hull_faces.size()); i++)
        {
            if (volumeSign(hull_faces[i],p) < 0)
            {
                hull_faces[i]->visible = true;
                vis = true;
            }
        }

    //    No visible faces so the point is inside the hull
        if (!vis) return false;

    //    Loop through the edges to find the counter-clockwise boundary edges that have a visible left and not visible right
    //    while deleting all other edges that have a visible left and right
        size_t size = hull_edges.size();
        for (size_t i = 0; i < size; i++)
        {
            bool left_v = hull_edges[i]->left->visible;
            bool right_v = hull_edges[i]->right->visible;

            if (left_v)
                hull_edges[i]->left = NULL;
            if (right_v)
                hull_edges[i]->right = NULL;

            if (left_v && right_v)
            {
                hull_edges[i]->toDelete = true;
                hull_edges[i]->sym->toDelete = true;
            }
            else if (left_v && !right_v)
            {
                hull_faces.push_back(LinkFaceEdge(hull_edges[i],p));
            }
        }

        return true;
    }

    Geometry* ConvexHull3D::handleCollinear(void)
    {
        double low, high;
        int low_i, high_i;
        low = low_i = 0;
        high = high_i = 1;

        Point origin = hull_points[0]->point;
        double u[3] = { 
            hull_points[1]->point.X() - hull_points[0]->point.X(),
            hull_points[1]->point.Y() - hull_points[0]->point.Y(),
            hull_points[1]->point.Z() - hull_points[0]->point.Z()
        };

        for (int i = 2; i < int(hull_points.size()); i++)
        {
            double d[3] = {
                hull_points[i]->point.X() - origin.X(),
                hull_points[i]->point.Y() - origin.Y(),
                hull_points[i]->point.Z() - origin.Z()
            };

            double dist = d[0]*u[0] + d[1]*u[1] + d[2]*u[2];

            if (dist > high)
            {
                high = dist;
                high_i = i;
            }
            if (dist < low)
            {
                low = dist;
                low_i = i;
            }
        }

        LineString* result = new LineString;
        result->addPoint(hull_points[low_i]->point);
        result->addPoint(hull_points[high_i]->point);
        return result;
    }

    Geometry* ConvexHull3D::handleCoplanar(void)
    {
        Projection2D proj(&hull_points[0]->point,&hull_points[1]->point,&hull_points[2]->point);
        MultiPoint* coplanar = static_cast<MultiPoint*>(proj.transformGeometryTo2D(multiPoint));
        Geometry* hull = GrahamHull::apply(coplanar);
        delete coplanar;
        Geometry* result = proj.transformGeometryTo3D(hull);
        delete hull;
        return result;
    }

    int ConvexHull3D::initialize(void)
    {
        std::vector<HullPoint*> initialPoints;
        initialPoints.push_back(hull_points[0]);
        initialPoints.push_back(hull_points[1]);

    //    find a third non-colinear point
        for (int i = 2; i < int(hull_points.size()); i++)
        {
            if (!Collinear3D(hull_points[0]->point,hull_points[1]->point,hull_points[i]->point))
            {
                initialPoints.push_back(hull_points[i]);
                break;
            }
        }

        if (initialPoints.size() != 3) return 1;

    //    Create double-triangle
        HullEdge* e0 = makeEdge(initialPoints[0],initialPoints[1]);
        HullEdge* e1 = makeEdge(initialPoints[1],initialPoints[2]);
        HullEdge* e2 = makeEdge(initialPoints[2],initialPoints[0]);

    //    Link edges
        e0->next = e2->prev = e1;
        e1->next = e0->prev = e2;
        e2->next = e1->prev = e0;

    //    Link symetric edges
        e0->sym->next = e1->sym->prev = e2->sym;
        e1->sym->next = e2->sym->prev = e0->sym;
        e2->sym->next = e0->sym->prev = e1->sym;

    //    Create Faces
        HullFace* f0 = new HullFace;
        HullFace* f1 = new HullFace;
        f0->visible = false;
        f1->visible = false;

    //    Link faces
        f0->points[0] = f1->points[0] = initialPoints[0]->point;
        f0->points[1] = f1->points[2] = initialPoints[1]->point;
        f0->points[2] = f1->points[1] = initialPoints[2]->point;
        e0->left = e1->left = e2->left = f0;
        e0->right = e1->right = e2->right = f1;
        e0->sym->left = e1->sym->left = e2->sym->left = f1;
        e0->sym->right = e1->sym->right = e2->sym->right = f0;

    //    Add faces to the list
        hull_faces.push_back(f0);
        hull_faces.push_back(f1);

    //    find a fourth non-coplanar point
        int index = -1;
        for (int i = 3; i < int(hull_points.size()); i++)
        {
            int vol = volumeSign(f0,hull_points[i]);
            if (vol!=0)
            {
                index = i;
                break;
            }
        }

        if (index==-1) return 2;
        initialPoints[0]->processed = true;
        initialPoints[1]->processed = true;
        initialPoints[2]->processed = true;

    //    Part of random step
        addOne(hull_points[index]);
        clean();

        return 0;
    }

    Geometry* ConvexHull3D::constructHull(void)
    {
        size_t count = hull_points.size();
        if (count == 0) return NULL;
        if (count == 1) return new Point(hull_points[0]->point);
        if (count == 2) return handleCollinear();
        if (count == 3) return handleCoplanar();
    
        int initializationResult = initialize();
        if (initializationResult == 1) return handleCollinear();
        if (initializationResult == 2) return handleCoplanar();

        completed = 4;

    //    Randomized loop
        while (completed < int(hull_points.size()))
        {
            //choose random
            size_t rand_i = rand()%hull_points.size();

            if (hull_points[rand_i]->processed) continue;
            else
            {
                 addOne(hull_points[rand_i]);
                
                //render();

                clean();
                completed++;    
            }
        }

    //construct result from faces
        PolyhedralSurface* result = new PolyhedralSurface;
        for (int i = 0; i < int(hull_faces.size()); i++)
        {
            LineString* line = new LineString;
            line->addPoint(new Point(hull_faces[i]->points[0]));
            line->addPoint(new Point(hull_faces[i]->points[1]));
            line->addPoint(new Point(hull_faces[i]->points[2]));
            line->addPoint(new Point(hull_faces[i]->points[0]));
            Polygon* poly = new Polygon;
            poly->addRing(line);
            result->addPatch(poly);
        }

        return result;
    }

    void ConvexHull3D::cleanPoints(void)
    {
        for (int i = 0; i < int(hull_points.size()); i++) hull_points[i]->coneEdge = NULL;
    }

    void ConvexHull3D::cleanEdges(void)
    {
        for (std::vector<HullEdge*>::iterator it = hull_edges.begin(); it != hull_edges.end(); ++it)
        {
            if ((*it)->toDelete)
            {
                delete (*it);
                it = hull_edges.erase(it);
                it--;
            }
        }
    }

    void ConvexHull3D::cleanFaces(void)
    {
        for (std::vector<HullFace*>::iterator it = hull_faces.begin(); it != hull_faces.end(); ++it)
        {
            if ((*it)->visible)
            {
                delete (*it);
                it = hull_faces.erase(it);
                it--;
            }
        }
    }

    void ConvexHull3D::clean(void)
    {
        cleanPoints();
        cleanEdges();
        cleanFaces();
    }

    Geometry* ConvexHull3D::apply(const MultiPoint* points)
    {
        ConvexHull3D op;
        op.setPoints(points);
        return op.constructHull();
    }

}