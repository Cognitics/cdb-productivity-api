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
/*! \page sfa_page Simple Feature Access (SFA)

\section Description

This is an implementation of the Open Geospatial Consortium (OGC) Simple Feature Access (SFA) interface specification.

It has been extended in the following ways:
<ul>
<li>Relationship operations have three-dimensional implementations in addition to normal two-dimensional implementations.
<li>A sfa::Feature container that includes a sfa::Geometry and attribution.
<li>Abstract sfa::File / sfa::Layer definition for reading and writing features in a data source.
</ul>

sfa::FileRegistry is a singleton registry for sfa::File factories implementing the sfa::IFileFactory interface.

\section Usage

In this example, we are searching a file for geometries that intersect a point.
\code
#include <sfa_file_factory/sfa_file_factory.h>
#include <sfa/Point.h>

sfa::Point intersectPoint(1.0, 2.0);
std::vector<sfa::Feature *> matchingFeatures;

sfa::File *file = sfa::FileRegistry::instance()->getFile("myfile.shp");
std::vector<sfa::Layer *> layers = file->getLayers();
for(size_t i = 0; i < layers.size(); ++i)
{
    sfa::Layer *layer = layers.at(i);
    while(sfa::Feature *feature = layer->getNextFeature())
    {
        if(feature && feature->geometry)
        {
            if(intersectPoint.intersects(feature->geometry))
            {
                matchingFeatures.push_back(feature);
                if(feature->attributes.find("name") != feature->attributes.end())
                    std::cout << feature->attributes["name"] << std::endl;
            }
        }
    }
}
\endcode

\section Notes

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)

*/
#pragma once

#define COGNITICS_SFA_VERSION 1.0

#include "Factory.h"
#include "Feature.h"
#include "Layer.h"
#include "Geometry.h"
#include "GeometryCollection.h"
#include "Point.h"
#include "MultiPoint.h"
#include "Curve.h"
#include "LineString.h"
#include "MultiCurve.h"
#include "MultiLineString.h"
#include "Surface.h"
#include "Polygon.h"
#include "PolyhedralSurface.h"
#include "MultiSurface.h"
#include "MultiPolygon.h"
#include "Buffer.h"
#include "Buffer3D.h"
#include "ConvexHull.h"
#include "ConvexHull3D.h"
#include "Distance.h"
#include "Distance3D.h"
#include "Overlay.h"
#include "Overlay3D.h"
#include "Relate.h"
#include "Intersector.h"
#include "Intersector3D.h"
#include "Relate.h"
#include "PointMath.h"
#include "RingMath.h"


//! \namespace sfa Cognitics Simple Feature Access Library
namespace sfa { }