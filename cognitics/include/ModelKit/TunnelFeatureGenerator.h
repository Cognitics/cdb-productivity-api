/*************************************************************************
Copyright (c) 2020 Cognitics, Inc.

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
/*! \file ModelKit/TunnelFeatureGenerator.h
\headerfile ModelKit/TunnelFeatureGenerator.h
\brief Provides ModelKit::TunnelFeatureGenerator.
\author Kevin Bentley <kbentley@cognitics.net>
\date 16 September 2014
*/

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <ccl/ObjLog.h>
#include <ccl/mutex.h>

#include <elev/Elevation.h>
#include <elev/DataSourceManager.h>
#include <elev/Elevation_DSM.h>

#include <sfa/LineString.h>
#include <sfa/Projection2D.h>
#include <sfa/Feature.h>
#include <scenegraph/Scene.h>
#include "ModelKit/CrossSection.h"
#include "ModelKit/ModelKit.h"

#include "ModelKit/AvoidClipper.h"
#include "ModelKit/ModelGenerator.h"
#include <cad/AttributeDictionary.h>




namespace modelkit
{
	class TunnelFeatureGenerator
	{
		ccl::ObjLog log;
		std::string dem_path;
		elev::DataSourceManager *dsm;
		elev::Elevation_DSM *elev;
		cts::WGS84ToFlatEarthMathTransform *toFlatEarth;
		cts::WGS84FromFlatEarthMathTransform *fromFlatEarth;

		ModelGenerator mg;
	public:
		TunnelFeatureGenerator(const std::string &_dem_path, double origin_lat, double origin_lon);

		// Build entry faces and skirts for the specified model on the specifed line
		bool BuildFeaturesForLinearFromKIT(const std::string &filename, const sfa::LineString *line, const std::string &modelKitFilePath, std::vector<sfa::Feature *> &features, bool make_first,bool make_last, double conflict_s_first, double conflict_s_last);
	};

}