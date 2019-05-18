/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/
#pragma once

#include <vector>

#include <sfa/Point.h>
#include <ccl/ccl.h>
#include <cts/FlatEarthProjection.h>
#include <sfa/Feature.h>

namespace elev
{
    enum elevation_strategy
    {
        ELEVATION_NEAREST,        // nearest neighbor (1 point)
        ELEVATION_LINEAR,        // linear interpolation (2 points)
        ELEVATION_PLANAR,        // planar interpolation (3 points) [NOT YET IMPLEMENTED]
        ELEVATION_TIN,            // triangle irregular network interpolation (3 points) [NOT YET IMPLEMENTED]
        ELEVATION_BILINEAR        // bilinear interpolation (4 points)
    };

    class Elevation
    {
    protected:
        enum elevation_strategy strategy;
        bool force;        // force strategy (fails if invalid points)
        std::vector<sfa::Point *> points;
        std::map<sfa::Point, cts::FlatEarthProjection> flat_earth_projection_by_origin;


    public:
        Elevation(enum elevation_strategy strategy = ELEVATION_NEAREST, bool force = false) : strategy(strategy), force(force) { }
        virtual ~Elevation() { Clear(); }

        void SetStrategy(enum elevation_strategy strategy) { this->strategy = strategy; }
        enum elevation_strategy GetStrategy() { return this->strategy; }
        void SetForce(bool force) { this->force = force; }
        bool GetForce() { return this->force; }

        virtual void AddPoint(sfa::Point *p) { points.push_back(p); }
        virtual void AddPoint(double x = 0, double y = 0, double z = 0) { AddPoint(new sfa::Point(x, y, z)); }
        virtual void Clear();

        //! for subclasses to load a z-value
        virtual bool Load(sfa::Point *p) { return false; }

        virtual bool Get(double x, double y, double &z);
        virtual bool Get(sfa::Point *p);

    };

}
