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

/*! \file ogr/ogr.h
\headerfile ogr/ogr.h
\brief Cognitics OGR Library (OGR)
\author Aaron Brinton <abrinton@cognitics.net>
\date 21 September 2010

\page ogr_page Cognitics OGR Library (OGR)

\section Description

The Cognitics OGR Library is a wrapper for the GDAL OGR library for working with objects in the Cognitics Simple Feature Access (SFA) library.

\section Usage

Reading a shapefile:
\code
#include <ogr.h>

sfa::FeatureList features;	// for storing features
ogr::FileSP file(ogr::File::open("test.shp"));
ogr::LayerList layers(file->getLayers());
for(ogr::LayerList::iterator it = layers.begin(), end = layers.end(); it != end; ++it)
{
	ogr::LayerSP layer(*it);
	layer->resetReading();
	while(sfa::FeatureSP feature = layer->getNextFeature())
	{
		// store for use later
		features.push_back(feature);

		// or just do something with the feature
	}
}
\endcode

Writing a shapefile:
\code
#include <ogr.h>

// create a list of features (with just one feature in this example)
sfa::FeatureList features;
sfa::FeatureSP myFeature(new sfa::Feature());
myFeature->geometry = sfa::PointSP(new sfa::Point(1.0, 2.0, 3.0));
myFeature->attributes["id"] = 1234;
features.push_back(myFeature);

ogr::FileSP file(ogr::File::create("", "output.shp"));	// driverName parameter defaults to "ESRI Shapefile"

// creating layers manually:
LayerSP layer = file->addLayer("My Layer", sfa::wkbPoint);
for(sfa::FeatureList::iterator it = features.begin(), end = features.end(); it != end; ++it)
{
	layer->addFeature(*it);
}

// or creating layers dynamically:
for(sfa::FeatureList::iterator it = features.begin(), end = features.end(); it != end; ++it)
{
	file->addFeature(*it);
}
\endcode

\section Notes

*/
#pragma once

#define COGNITICS_OGR_VERSION 1.0

#include "File.h"

void InitOGR();

//! \namespace ogr Cognitics OGR Library
namespace ogr { }

