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
//#pragma optimize( "", off )

#include "ModelKit/TunnelFeatureGenerator.h"
#include <boost/math/constants/constants.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <boost/foreach.hpp>

#include <sfa/LineString.h>
#include <sfa/ConvexHull.h>
#include <sfa/PolygonClipper.h>

using namespace sfa;


namespace modelkit
{
	TunnelFeatureGenerator::TunnelFeatureGenerator(const std::string &_dem_path, double origin_lat, double origin_lon) : dem_path(_dem_path)
	{
		log.init("TunnelFeatureGenerator", this);
		dsm = new elev::DataSourceManager(100 * 1024 * 1024);
		dsm->AddDirectory_Raster_GDAL(dem_path);
		dsm->generateBSP();
		elev = new elev::Elevation_DSM(dsm, elev::ELEVATION_BILINEAR);

		toFlatEarth = new cts::WGS84ToFlatEarthMathTransform(origin_lat, origin_lon);
		fromFlatEarth = new cts::WGS84FromFlatEarthMathTransform(origin_lat, origin_lon);
	}

	
	bool CreateTunnelFeatures(sfa::Projection2D &projection, double dist, std::vector<sfa::Feature *> &features, std::vector<sfa::Point> &entry_points, std::vector<sfa::Point> &skirt_points)
	{
		sfa::LineString *entryls = new sfa::LineString();
		sfa::LineString *skirtls = new sfa::LineString();

		// Get the max height of the entry and skirt points
		// Linearly 'slant' the feature along the normal based on the z value.
		//  So at a y value of min_y the distance along the normal will be 0, at a y value of max_y the distance along the normal will be dist
		double entry_max_y = -DBL_MAX;
		double entry_min_y = DBL_MAX;
		BOOST_FOREACH(sfa::Point &pt, entry_points)
		{
			entry_max_y = std::max<double>(entry_max_y, pt.Y());
			entry_min_y = std::min<double>(entry_min_y, pt.Y());
		}
		double entry_y_span = entry_max_y - entry_min_y;

		double skirt_max_y = -DBL_MAX;
		double skirt_min_y = DBL_MAX;
		BOOST_FOREACH(sfa::Point &pt, skirt_points)
		{
			skirt_max_y = std::max<double>(skirt_max_y, pt.Y());
			skirt_min_y = std::min<double>(skirt_min_y, pt.Y());
		}
		double skirt_y_span = skirt_max_y - skirt_min_y;

		BOOST_FOREACH(sfa::Point &pt, entry_points)
		{
			double y_dist = pt.Y() - entry_min_y;
			//figure out how far back to set the vert based on how high it is
			double vert_setback = (y_dist / entry_y_span)*dist;
			sfa::Point proj_pt = projection.transformPointTo3D(pt);
			proj_pt = proj_pt + ((*projection.getNormal())*vert_setback);
			entryls->addPoint(proj_pt);
		}
		BOOST_FOREACH(sfa::Point &pt, skirt_points)
		{
			double y_dist = pt.Y() - entry_min_y;
			//figure out how far back to set the vert based on how high it is
			double vert_setback = (y_dist / entry_y_span)*dist;
			sfa::Point proj_pt = projection.transformPointTo3D(pt);
			proj_pt = proj_pt + ((*projection.getNormal())*vert_setback);
			skirtls->addPoint(proj_pt);
		}

		sfa::Polygon *entry = new sfa::Polygon();
		entry->addRing(entryls);
		entry->close();
		sfa::Polygon *skirt = new sfa::Polygon();
		skirt->addRing(skirtls);
		skirt->close();

		// Use the previous projection unless this is the first projection we've tried, then use it
		sfa::Feature *entryfeature = new sfa::Feature();
		entryfeature->geometry = entry;
		entryfeature->setAttribute("CItype", "tunnel_entry");

		sfa::Feature *entryskirtfeature = new sfa::Feature();
		entryskirtfeature->geometry = skirt;
		entryskirtfeature->setAttribute("CItype", "tunnel_skirt");

		// Project the entry and skirt and add them as features
		features.push_back(entryfeature);
		features.push_back(entryskirtfeature);
		return true;
	}


	bool TunnelFeatureGenerator::BuildFeaturesForLinearFromKIT(const std::string &filename, const sfa::LineString *line, const std::string &modelKitFilePath, std::vector<sfa::Feature *> &features, bool make_first, bool make_last, double conflict_s_first, double conflict_s_last)
	{
		const double ABSOLUTE_MAX_TUNNEL_SETBACK = 50;
		if (line->getNumPoints() < 2)
		{
			log << ccl::LERR << "Invalid linear specified for BuildFeaturesForLinearFromKIT (fewer than two points), cannot build tunnel features." << log.endl;
			return false;
		}
		// Load the kit
		modelkit::ModelKitSP kit = mg.GetCachedModelKit(modelKitFilePath);
		if (!kit.get() || !kit->isValid())
		{
			log << ccl::LERR << "Invalid model kit path '" << modelKitFilePath << "' cannot build tunnel features." << log.endl;
			return false;
		}
		sfa::Feature *instancePointFeature = new sfa::Feature();
		sfa::Point *originPoint = new sfa::Point(*line->getPointN(0));
		instancePointFeature->geometry = originPoint;
		instancePointFeature->setAttribute("CItype", "tunnel");

		instancePointFeature->setAttribute("CIfile", filename);
		instancePointFeature->setAttribute("Model", "Model Library;Default;" + filename);

		features.push_back(instancePointFeature);

		// Get the consolidated CSM
		modelkit::CrossSectionSP cs(kit->consolidate());
		if (!cs.get())
		{
			log << ccl::LERR << "Unable to retrieve cross section for '" << modelKitFilePath << "' cannot build tunnel features." << log.endl;
			return false;		
		}
		std::vector<sfa::Point> cs_points;
		std::set<sfa::Point> cs_set;
		// Find the entry shape by walking the outer edge
		// Make a convex 2d Hull from the cross section outline
		std::vector<Edge *> edges = cs->getEdges();
		double min_x = DBL_MAX;
		double max_x = -DBL_MAX;
		double max_y = -DBL_MAX;

		BOOST_FOREACH(Edge *edge, edges)
		{
			// Only use the interior tunnel hull
			if(edge->getFlags().getAttributeAsBool("tunnelInterior",true,false))
			{
				cs_set.insert(edge->getPoint(0));
				cs_set.insert(edge->getPoint(1));
			}
		}
		BOOST_FOREACH(const sfa::Point &pt, cs_set)
		{
			cs_points.push_back(pt);
			min_x = std::min<double>(pt.X(), min_x);
			max_x = std::max<double>(pt.X(), max_x);

			max_y = std::max<double>(pt.Y(), max_y);
		}
		const double TUNNEL_HEIGHT_CUTOFF = 0.10f;//The top 10% of the y values are tested against the terrain
		double terrain_cutoff_y = max_y - (max_y*TUNNEL_HEIGHT_CUTOFF);

		sfa::GrahamHull hull(cs_points);
		sfa::Geometry *hullgeom = hull.getHullGeometry();
		sfa::Polygon *hullpoly = dynamic_cast<sfa::Polygon*>(hullgeom);
		if (!hullpoly)
		{
			log << ccl::LERR << "Invalid cross section hull, cannot build tunnel features." << log.endl;
			if (hullgeom)
				delete hullgeom;
			return false;
		}

		// Create the skirt by buffering the entry shape
		sfa::Polygon *skirtpoly = dynamic_cast<sfa::Polygon*>(hullgeom->buffer(3.0, sfa::Buffer::ROUND_ENDS, sfa::Buffer::UNION_RESULTS));
		if (!skirtpoly)
		{
			log << ccl::LERR << "Invalid cross section skirt, cannot build tunnel features." << log.endl;
			delete hullpoly;
			return false;
		}
		double cswidth = cs->getWidth();
		double csheight = cs->getHeight();
		sfa::Polygon clippedpoly = clipPolygon(*skirtpoly, 0 - cswidth, cswidth, 0, csheight * 2);
		delete skirtpoly;
		skirtpoly = new sfa::Polygon(clippedpoly);

		sfa::Polygon clippedhull = clipPolygon(*hullpoly, 0 - cswidth, cswidth, 0, csheight * 2);
		delete hullpoly;
		hullpoly = new sfa::Polygon(clippedhull);

		std::vector<sfa::Point> entry_points;
		std::vector<sfa::Point> skirt_points;
		sfa::LineString *hullring = hullpoly->getExteriorRing();		
		if (hullring)
		{
			int num_entry_points = hullring->getNumPoints();
			for (int i = 0; i < num_entry_points;i++)
			{
				entry_points.push_back(*hullring->getPointN(i));
			}
		}

		sfa::LineString *skirtring = skirtpoly->getExteriorRing();
		if (skirtring)
		{
			int num_hull_points = skirtring->getNumPoints();
			for (int i = 0; i < num_hull_points; i++)
			{
				skirt_points.push_back(*skirtring->getPointN(i));
			}
		}

		// Determine the angle to project the entry shape on to
		// The goal is to slant the entry shapes to the most vertical angle that will result in the top of the
		// shape being under the terrain still.

		if (make_first)
		{
			// Start with the first segment
			sfa::Point *p0 = line->getPointN(0);
			sfa::Point *p1 = line->getPointN(1);
			sfa::Point delta = *p0 - *p1;// Line pointing from the entry to the next vert
			sfa::Point normal = delta;
			normal.normalize();
			double segdist = delta.length();

			if (line->getNumPoints() == 2)
			{
				segdist = segdist / 2;// Only go to the middle in this case since we have a face at each end
			}
			const double MAX_TUNNEL_ENTRY_PROJECTION_SCALAR = 0.75;// How much of the end segment we will use to create the slant
			double s = segdist * MAX_TUNNEL_ENTRY_PROJECTION_SCALAR;

			// ensure we are past any conflict
			if(conflict_s_first != DBL_MAX)
				s = std::min<double>(s, conflict_s_first - 1.0f);
			if(s < 1.0f)
				s = 1.0f;

			// Don't set back too far
			s = std::min<double>(s, ABSOLUTE_MAX_TUNNEL_SETBACK);
			bool searching = true;
			const double minimum_setback = 3.0;//The minimum distance along the s between the top and the bottom (i.e. the minimum slant)
			const double search_interval = 1.0;//Search 1 meter at a time
			Projection2D prev_projection;
			bool first_projection = true;

			// Move toward the entry looking for the most vertical angle that results in the tunnel top being under the terrain
			while (searching)
			{
				// Create a projection for these points and project them into 3D
				Projection2D projection;
				sfa::Point entryNormal = normal * -1.0;
				
				projection.createBasis(p0, &entryNormal);
				if (first_projection)
				{
					prev_projection = projection;
					first_projection = false;
				}
				std::vector<sfa::Point> projected_points;
				BOOST_FOREACH(sfa::Point &pt, cs_points)
				{
					// Only add the top points
					if (terrain_cutoff_y <= pt.Y())
					{
						sfa::Point proj_pt = projection.transformPointTo3D(pt);
						proj_pt = proj_pt + (entryNormal * s);
						
						projected_points.push_back(proj_pt);
					}
				}
				//If the top points are under the terrain, keep searching
				bool terrain_below = false;
				BOOST_FOREACH(sfa::Point &pt, projected_points)
				{
					sfa::Point queryPt(pt.X(), pt.Y());
					queryPt.setCoordinateSystem(NULL, fromFlatEarth);
					if (elev->Get(&queryPt))
					{
						if (queryPt.Z() < pt.Z())
						{
							terrain_below = true;
							break;
						}
					}
				}
				//else, stop here and use the last identified projection
				if (terrain_below)
				{
					CreateTunnelFeatures(prev_projection, s, features, entry_points, skirt_points);
					break;
				}

				// Save this projection in case it's the last one where we are under the terrain
				prev_projection = projection;
				// If we reach the entry point (i.e. s==0) and the terrain is still below the top of the tunnel
				// then use a projection with an s of 1.0 so we have non-vertical faces
				if ((s - search_interval) < minimum_setback)
				{
					searching = false;
					CreateTunnelFeatures(prev_projection, s, features, entry_points, skirt_points);
					break;
				}

				s -= search_interval;
			}
		}
		if (make_last)
		{
			// Start with the first segment
			sfa::Point *p0 = line->getPointN(line->getNumPoints() - 1);
			sfa::Point *p1 = line->getPointN(line->getNumPoints() - 2);
			sfa::Point delta = *p0 - *p1;// Line pointing from the entry to the next vert
			sfa::Point normal = delta;
			normal.normalize();
			double segdist = delta.length();

			if (line->getNumPoints() == 2)
			{
				segdist = segdist / 2;// Only go to the middle in this case since we have a face at each end
			}
			const double MAX_TUNNEL_ENTRY_PROJECTION_SCALAR = 0.75;// How much of the end segment we will use to create the slant
			double s = segdist * MAX_TUNNEL_ENTRY_PROJECTION_SCALAR;

			// ensure we are past any conflict
			double len = line->getLength2D();
			if(conflict_s_last != -DBL_MAX)
				s = std::min<double>(s, len - (conflict_s_last + 1.0f));
			if(s > len - 1.0f)
				s = len - 1.0f;

			// Don't set back too far
			s = std::min<double>(s, ABSOLUTE_MAX_TUNNEL_SETBACK);
			bool searching = true;
			const double minimum_setback = 3.0;//The minimum distance along the s between the top and the bottom (i.e. the minimum slant)
			const double search_interval = 1.0;//Search 1 meter at a time
			Projection2D prev_projection;
			bool first_projection = true;

			// Move toward the entry looking for the most vertical angle that results in the tunnel top being under the terrain
			while (searching)
			{
				// Create a projection for these points and project them into 3D
				Projection2D projection;
				sfa::Point entryNormal = normal * - 1.0;
				
				projection.createBasis(p0, &entryNormal);
				if (first_projection)
				{
					prev_projection = projection;
					first_projection = false;
				}
				std::vector<sfa::Point> projected_points;
				BOOST_FOREACH(sfa::Point &pt, cs_points)
				{
					// Only add the top points
					if (terrain_cutoff_y <= pt.Y())
					{
						sfa::Point proj_pt = projection.transformPointTo3D(pt);
						proj_pt = proj_pt + (entryNormal * s);
						projected_points.push_back(proj_pt);
					}
				}
				//If the top points are under the terrain, keep searching
				bool terrain_below = false;
				BOOST_FOREACH(sfa::Point &pt, projected_points)
				{
					sfa::Point queryPt(pt.X(), pt.Y());
					queryPt.setCoordinateSystem(NULL, fromFlatEarth);
					if (elev->Get(&queryPt))
					{
						if (queryPt.Z() < pt.Z())
						{
							terrain_below = true;
							break;
						}
					}
				}
				//else, stop here and use the last identified projection
				if (terrain_below)
				{
					CreateTunnelFeatures(prev_projection, s, features, entry_points, skirt_points);
					break;
				}

				// Save this projection in case it's the last one where we are under the terrain
				prev_projection = projection;
				// If we reach the entry point (i.e. s==0) and the terrain is still below the top of the tunnel
				// then use a projection with an s of 1.0 so we have non-vertical faces
				if ((s - search_interval) < minimum_setback)
				{
					searching = false;
					CreateTunnelFeatures(prev_projection, s, features, entry_points, skirt_points);
					break;
				}
				
				s -= search_interval;
			}
		}

		delete hullpoly;
		delete skirtpoly;
		return true;
	}
}