/****************************************************************************
Copyright 2017 Cognitics, Inc.
****************************************************************************/

#pragma once

#include "Elevation.h"
#include "DataSourceManager.h"
#include <ip/GDALRasterReader.h>

namespace elev
{
    class DataSourceManager;

    class Elevation_DSM : public Elevation
    {
    public:
        DataSourceManager *dsm;
        std::vector<sfa::Feature *> debug_features;

        Elevation_DSM(DataSourceManager *dsm, enum elevation_strategy strategy = ELEVATION_NEAREST, bool force = false)
                        : Elevation(strategy, force), dsm(dsm) { }

        virtual ~Elevation_DSM();

        virtual bool Load(sfa::Point *p);
        virtual bool Get(sfa::Point *p);

    };

}
