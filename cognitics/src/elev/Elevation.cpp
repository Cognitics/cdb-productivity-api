/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/

//#pragma optimize("", off)

#include "elev/Elevation.h"

#include <vector>

#include <ccl/ccl.h>
#include <sfa/Point.h>
#include <elev/Line.h>

using namespace sfa;
using namespace ccl;
namespace elev
{
    void Elevation::Clear()
    {
        std::vector<Point *>::iterator iter = points.begin();
        while(iter!=points.end())
        {
            Point *p = *iter++;
            delete p;
        }
        points.clear();
    }

    bool Elevation::Get(double x, double y, double &z)
    {
        Point p(x, y);
        if(!Get(&p)) return false;
        z = p.Z();
        
        return true;
    }

    bool Elevation::Get(Point *sample_point)
    {
        if(points.empty())
            return false;

        // test for point on post
        for(size_t i = 0, c = points.size(); i < c; i++)
        {
            sfa::Point *point = points.at(i);
            if(*point == *sample_point)
            {
                Load(point);
                sample_point->setZ(point->Z());
                return true;
            }
        }

        cts::FlatEarthProjection *flat_earth_projection = NULL;
        {
            static const double blocksize = 0.1f;
            int offset_x = std::floor((sample_point->X() + (blocksize / 2)) / blocksize);
            int offset_y = std::floor((sample_point->Y() + (blocksize / 2)) / blocksize);
            sfa::Point origin(offset_x * blocksize, offset_y * blocksize);
            std::map<sfa::Point, cts::FlatEarthProjection>::iterator it = flat_earth_projection_by_origin.find(origin);
            if(it == flat_earth_projection_by_origin.end())
            {
                flat_earth_projection_by_origin[origin] = cts::FlatEarthProjection(origin.Y(), origin.X());
                it = flat_earth_projection_by_origin.find(origin);
            }
            flat_earth_projection = &it->second;
        }

        sfa::Point fe_sample_point = sample_point;
        fe_sample_point.setX(flat_earth_projection->convertGeoToLocalX(fe_sample_point.X()));
        fe_sample_point.setY(flat_earth_projection->convertGeoToLocalY(fe_sample_point.Y()));

        std::vector<sfa::Point> fe_points(points.size());
        for(size_t i = 0, c = fe_points.size(); i < c; i++)
        {
            sfa::Point *point = points.at(i);
            sfa::Point &fe_point = fe_points.at(i);
            fe_point.setX(flat_earth_projection->convertGeoToLocalX(point->X()));
            fe_point.setY(flat_earth_projection->convertGeoToLocalY(point->Y()));
        }

        std::vector<double> distances(points.size());
        for(size_t i = 0, c = fe_points.size(); i < c; i++)
            distances[i] = fe_sample_point.distance(&fe_points[i]);


        enum elevation_strategy strat = strategy;
        sfa::Point * nearest1 = NULL;
        sfa::Point * nearest2 = NULL;
        sfa::Point * n = NULL;
        sfa::Point * s = NULL;
        sfa::Point * e = NULL;
        sfa::Point * w = NULL;
        sfa::Point * nw = NULL;
        sfa::Point * ne = NULL;
        sfa::Point * se = NULL;
        sfa::Point * sw = NULL;

        for(size_t i = 0, c = points.size(); i < c; i++)
        {
            sfa::Point *point = points.at(i);
            sfa::Point &fe_point = fe_points.at(i);
            Load(point);
            fe_point.setZ(point->Z());

            if(nearest1)
            {
                if(fe_sample_point.distance(&fe_point) < fe_sample_point.distance(nearest1))
                {
                    nearest2 = nearest1;
                    nearest1 = &fe_point;
                }
            }
            else nearest1 = &fe_point;
            if(strat == ELEVATION_NEAREST)
                break;

            // post east of sample point?
            if(fe_point.X() > fe_sample_point.X())
            {
                // post north of sample point?
                if(fe_point.Y() > fe_sample_point.Y())
                    ne = fe_sample_point.nearest(ne, &fe_point);
                // post south of sample point?
                if(fe_point.Y() < fe_sample_point.Y())
                    se = fe_sample_point.nearest(se, &fe_point);
                // post on a horizontal axis?
                if(fe_point.Y() == fe_sample_point.Y())
                    e = fe_sample_point.nearest(e, &fe_point);
            }

            // post west of sample point?
            if(fe_point.X() < fe_sample_point.X())
            {
                // post north of sample point?
                if(fe_point.Y() > fe_sample_point.Y())
                    nw = fe_sample_point.nearest(nw, &fe_point);
                // post south of sample point?
                if(fe_point.Y() < fe_sample_point.Y())
                    sw = fe_sample_point.nearest(sw, &fe_point);
                // post on a horizontal axis?
                if(fe_point.Y() == fe_sample_point.Y())
                    w = fe_sample_point.nearest(w, &fe_point);
            }

            // point is on vertical axis
            if(fe_point.X() == fe_sample_point.X())
            {
                // post north of sample point?
                if(fe_point.Y() > fe_sample_point.Y())
                    n = fe_sample_point.nearest(n, &fe_point);
                // post south of sample point?
                if(fe_point.Y() < fe_sample_point.Y())
                    s = fe_sample_point.nearest(s, &fe_point);
            }
            
        }

        // update corners for cases where we are on an edge
        if(n && !nw)
            nw = n;
        if(n && !ne)
            ne = n;
        if(s && !nw)
            sw = s;
        if(s && !se)
            se = s;
        if(e && !ne)
            ne = e;
        if(e && !se)
            se = e;
        if(w && !nw)
            nw = w;
        if(w && !sw)
            sw = w;

        int validpoints = 0;
        if(nw)
            ++validpoints;
        if(ne)
            ++validpoints;
        if(se)
            ++validpoints;
        if(sw)
            ++validpoints;

        if((strat == ELEVATION_BILINEAR) && (validpoints < 4))
        {
            if(force)
                return false;
            strat = ELEVATION_TIN;
            strat = ELEVATION_NEAREST;
        }

        if((strat == ELEVATION_TIN) && (validpoints < 3))
        {
            if(force)
                return false;
            strat = ELEVATION_LINEAR;
        }

        if((strat == ELEVATION_PLANAR) && (validpoints < 3))
        {
            if(force)
                return false;
            strat = ELEVATION_LINEAR;
        }

        if((strat == ELEVATION_LINEAR) && (validpoints < 2))
        {
            if(force)
                return false;
            strat = ELEVATION_NEAREST;
        }

        if(strat == ELEVATION_LINEAR)
        {
            // attempt to use an axis that crosses the point in preference to just the next closest
            sfa::Point * pick = NULL;
            if(nearest1 != nw)
                pick = fe_sample_point.nearest(pick, nw);
            if(nearest1 != ne)
                pick = fe_sample_point.nearest(pick, ne);
            if(nearest1 != se)
                pick = fe_sample_point.nearest(pick, se);
            if(nearest1 != sw)
                pick = fe_sample_point.nearest(pick, sw);
            if(pick)
                nearest2 = pick;
            nw = ne = se = sw = NULL;    // prevent loading later
        }

        if(strat == ELEVATION_NEAREST)
        {
            nearest2 = nw = ne = se = sw = NULL;    // prevent loading later
        }

        // for subclasses
        /*
        if(nearest1)
            Load(nearest1);
        if(nearest2)
            Load(nearest2);
        if(nw && !Load(nw))
           return false;
        if(ne && !Load(ne))
            return false;
        if(se && !Load(se))
            return false;
        if(sw && !Load(sw))
            return false;
            */

        if(strat == ELEVATION_NEAREST)
        {
            sample_point->setZ(nearest1->Z());
            return true;
        }

        if(strat == ELEVATION_LINEAR)
        {
            Line line(nearest1, nearest2);
            line.InterpolateZfromY(&fe_sample_point);
            sample_point->setZ(fe_sample_point.Z());
            return true;
        }

        if(strat == ELEVATION_BILINEAR)
        {

            //http://www.geovista.psu.edu/sites/geocomp99/Gc99/082/gc_082.htm

            double nx = (fe_sample_point.X() - sw->X()) / (se->X() - sw->X());
            double ny = (fe_sample_point.Y() - sw->Y()) / (nw->Y() - sw->Y());
            double a00 = sw->Z();
            double a10 = se->Z() - sw->Z();
            double a01 = nw->Z() - sw->Z();
            double a11 = sw->Z() - se->Z() - nw->Z() + ne->Z();
            double z = a00 + (a10 * nx) + (a01 * ny) + (a11 * nx * ny);


            /*
            double nf = (fe_sample_point.X() - nw->X()) / (ne->X() - nw->X());
            double ny = nw->Y() + ((ne->Y() - nw->Y()) * nf);
            double nz = nw->Z() + ((ne->Z() - nw->Z()) * nf);

            double sf = (fe_sample_point.X() - sw->X()) / (se->X() - sw->X());
            double sy = sw->Y() + ((se->Y() - sw->Y()) * sf);
            double sz = sw->Z() + ((se->Z() - sw->Z()) * sf);

            double yf = (fe_sample_point.Y() - sy) / (ny - sy);
            double z = sz + ((nz - sz) * yf);
            */

            sample_point->setZ(z);

            return true;
        }

        return false;
    }

}

