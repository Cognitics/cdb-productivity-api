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
/*! \file ModelKit/ModelGenerator.h
\headerfile ModelKit/ModelGenerator.h
\brief Provides ModelKit::ModelGenerator.
\author Kevin Bentley <kbentley@cognitics.net>
\date 03 May 2011
*/

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <sstream>

#include <ccl/ObjLog.h>
#include <ccl/mutex.h>

#include <sfa/LineString.h>
#include <sfa/Projection2D.h>
#include <sfa/Feature.h>
#include <scenegraph/Scene.h>
#include <scenegraph/LOD.h>
#include "ModelKit/CrossSection.h"
#include "ModelKit/ModelKit.h"

#include "ModelKit/AvoidClipper.h"
#include <cad/AttributeDictionary.h>

namespace modelkit
{
	class FeatureLink
	{
	public:
		sfa::LineString segment;
		double width;
		std::string guid;//If empty, this isn't a valid link

		bool isValid() const { return guid.size()>0; }
	};

	class FeatureLinkInfo
	{
	public:
		std::vector<FeatureLink> predecessors;// a predecessor ends where we start
		std::vector<FeatureLink> successors;//a successor starts where we end
		std::vector<FeatureLink> beginSiblings;// a sibling that starts the same place we start
		std::vector<FeatureLink> endSiblings;// a sibling that ends the same place we end
		FeatureLink endsAt;// endsAt is the next segment of the line we end at
		FeatureLink beginsAt;//beginsAt is the segment of the line where we start
		FeatureLink previousLine;// the case where there is a 1-1 relationship between begins and ends
		FeatureLink nextLine;// the case where there is a 1-1 relationship between begins and ends
	};

	typedef std::map<std::string,FeatureLinkInfo> FeatureLinkMapType;


	class ReferenceModelInfo
	{

		static std::map<std::string,ReferenceModelInfo> modelInfoMap;
	public:
		double zMin;
		double zMax;
		double radius;
		std::string filename;

		static bool GetModelInfo(std::string filename, ReferenceModelInfo &refinfo);

	};

	class ModelGeneratorParameters
	{
	public:
		ModelGeneratorParameters() : buildSFAFootprintFeatures(false),
			buildSFAPortalFeatures(false),
			portalAngle(45.0f),
			linkinfo(NULL),
			endsOnly(false),
			endsDistance(50.0f),
			endsSegment(-1),
			vbs2Mode(false),
			origin_lat(0.0f),
			origin_lon(0.0f),
			consolidateInstancedModels(true),
			is_bridge(false),
			roadbed_extension_0_distance(0.0f),
			roadbed_extension_0_height(0.0f),
			roadbed_extension_N_distance(0.0f),
			roadbed_extension_N_height(0.0f)
		{

		}
		bool buildSFAFootprintFeatures;
		bool buildSFAPortalFeatures;
		double portalAngle;
		bool endsOnly;
		bool vbs2Mode;
		bool is_bridge;
		double roadbed_extension_0_distance;
		double roadbed_extension_0_height;
		double roadbed_extension_N_distance;
		double roadbed_extension_N_height;
		double endsDistance;
		double origin_lat;
		double origin_lon;
		int endsSegment;
		//If true all instanced models will be integrated into the scene, otherwise external references are used.
		bool consolidateInstancedModels;
		const FeatureLinkInfo *linkinfo;
		ccl::AttributeContainer featureAttributes;
	};

	typedef std::map<std::string,modelkit::ModelKitSP> modelKitMapType;
	typedef std::map<std::string,modelkit::CrossSectionSP> crossSectionKitMapType;
	typedef std::map<std::string,sfa::LineString*> attach_string_t;

	class SceneData;
	//! Model Generator Class
	/*!
	\brief Model Generator Class that can assemble cross sections folowing an sfa::LineString
	\description The Model Generator is a utility class that takes as input a modelkit::CrossSection shared pointer, and an sfa::LineString shared
	pointer, as well as a reference to a collection of faces. The modelkit::ModelGenerator::BuildModelForLinear method will project the
	cross section into 3D space and populate the collection of faces. To create an OpenScenegraph scene from these faces, you can use the
	modelkit::ModelGeneratorOSG::WriteOSGFile method.

	\ref modelkitosg_page
	\section Usage
	\code
		// This example will create a spiral model with the specified cross section.
		modelkit::CrossSectionSP crossSection(new modelkit::CrossSection("..\\..\\data\\modelkit_editor\\rescaled_imported.csm"));	

		LineStringSP linear(new LineString());

		double z = 0;
		int num_steps = 60;
		double radius = 150;
		for(int l=0;l<5;l++)
		{
			for(int i=0;i<num_steps;i++)
			{			
				double angle = i*(360/num_steps);
				double radians = M_PI * angle/180;
				double x = radius * sin(radians);
				double y = radius * cos(radians);
				z += (60/num_steps);
				linear->addPoint(sfa::PointSP(new sfa::Point(x,y,z)));
			}
		}

		modelkit::ModelGenerator mg;
		modelkit::ModelGeneratorOSG mgosg;
		scenegraph::Scene scene;
		mg.BuildModelForLinear(linear,crossSection, scene);
		osg::Vec3d origin;// You can place the model under any location by initializing origin here.
		buildOSGFileFromScene("spiral.osg",origin,&scene);
	\endcode
	\section Notes
	*/
	class ModelGenerator
	{
		static modelKitMapType modelKitMap;
		static ccl::mutex modelKitMapMutex;
		static crossSectionKitMapType crossSectionMap;
		std::map<std::string,ReferenceModelInfo> modelInfoMap;
		cad::AttributeDictionary dictionary;
		ccl::ObjLog log;
		CrossSectionSP crossSection;

		double GetLength(const sfa::Point *a,const sfa::Point *b) const
		{
			double delta_x = a->X() - b->X();
			double delta_y = a->Y() - b->Y();
			double delta_z = a->Z() - b->Z();
			return sqrt((delta_x*delta_x)+(delta_y*delta_y)+(delta_z*delta_z));
		}

/*
	Resulting normal will be represent the bisectional plane as looked at from the previous point.
*/
		inline sfa::Point GetBisectionalPlaneNormal(const sfa::Point &point, const sfa::Point &prev, const sfa::Point &next, bool start = false)
		{
			sfa::Point v1 = next - point;
			sfa::Point v2 = prev - point;
			v1.normalize();
			v2.normalize();
			sfa::Point sum = v1 + v2;
			sfa::Point n;

			if (v1.length2() < sfa::SFA_EPSILON)
				n = ( start ? v2*-1 : v2 );
			else if (v2.length2() < sfa::SFA_EPSILON)
				n = ( start ? v1*-1 : v1 );
			else if(sum.dot(sum) < sfa::SFA_EPSILON)
				n = v2;
			else
				n = (v2*(v1.dot(sum)) - v1*(v2.dot(sum)));

			n.normalize();
			return n;
		}

		ReferenceModelInfo GetModelInfo(std::string filename);

		modelkit::CrossSectionSP CloneAndProjectCrossSection(const modelkit::CrossSectionSP crossSection, const sfa::Projection2D &proj, const modelkit::CrossSectionSP prevCrossSection = modelkit::CrossSectionSP(), const sfa::Point &origin = sfa::Point(), const sfa::Point &normal = sfa::Point());
		scenegraph::MappedTexture ConvertToMappedTexture(modelkit::TextureReference *tr, bool reverseOrder);
        scenegraph::Material ConvertToScenegraphMaterial(modelkit::Material *m);
		bool SetupTexturesFromCrossSection(scenegraph::Face &face, modelkit::Edge *fromEdge, bool reverseOrder);
        bool SetupMaterialsFromCrossSection(scenegraph::Face &face, modelkit::Edge *fromEdge);
		bool RemapUVsForFace(const scenegraph::Face &uvFace, scenegraph::Face &remapFace, bool remap1, bool remap2, bool remap3);
		bool SetupFacesFromEdges(scenegraph::Face &face1, scenegraph::Face &face2, modelkit::Edge *e1, modelkit::Edge *e2, double &line_length1, double &line_length2, sfa::Point normal, sfa::Point line);
		bool SetupTexturesFromCrossSection(scenegraph::Face &face, modelkit::Face *fromFace, bool reverseOrder);
		bool SetupMaterialsFromCrossSection(scenegraph::Face &face, modelkit::Face *fromFace);
		bool AddEndFaces(modelkit::CrossSectionSP crossSection, bool flipNormals, std::vector<scenegraph::Face> &faces);

		bool PlaceIntervalModelInstances(scenegraph::Scene &scene,
			SceneData *scenedata,
			modelkit::ModelInstance *inst, 
			scenegraph::LineString &ls,
			const ReferenceModelInfo &rmi);


		bool PlaceVertexModelInstances(scenegraph::Scene &scene,
			SceneData *scenedata,
			modelkit::ModelInstance *inst, 
			scenegraph::LineString &ls,
			const ReferenceModelInfo &rmi);

		bool PlaceStartInstances(scenegraph::Scene &scene,
			SceneData *scenedata,
			modelkit::ModelInstance *inst, 
			scenegraph::LineString &ls,
			const ReferenceModelInfo &rmi);

		bool PlaceEndInstances(scenegraph::Scene &scene,
			SceneData *scenedata,
			modelkit::ModelInstance *inst, 
			scenegraph::LineString &ls,
			const ReferenceModelInfo &rmi);
	public:

		//! Default constructor
		ModelGenerator()
		{
			log.init("ModelGenerator");
			log << ccl::LINFO;
		}

		//! Build a model using the cross section that follows the specified linestring
		/*!
			\param line The sfa::LineString to build the model for.
				line is a 3D sfa::LineString that is used to build the model. The first
				point in the model will be used as the origin for the faces generated.
			\param crossSection The cross section used to build the model.
			\param scene 
				(output) The scene generated by this method.
		*/
		
		bool BuildModelForLinear(const sfa::LineString *line, const modelkit::CrossSectionSP crossSection, scenegraph::Scene &scene, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildModelForLinearBridge(const sfa::LineString *line, const modelkit::CrossSectionSP crossSection, scenegraph::Scene &scene, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildModelForLinear(const sfa::LineString *line, const modelkit::ModelKitSP modelKit, scenegraph::Scene &scene, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildModelForLinearFromCSM(const sfa::LineString *line, const std::string crossSectionFilePath, scenegraph::Scene &scene, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildModelForLinearFromKIT(const sfa::LineString *line, const std::string modelKitFilePath, scenegraph::Scene &scene, ModelGeneratorParameters parms = ModelGeneratorParameters());

		bool BuildModelForLinearFromKITS(const sfa::LineString *line, const std::string modelKitFilePath, scenegraph::Scene &scene, ModelGeneratorParameters parms = ModelGeneratorParameters());

		bool BuildVBS2ModelForLinear(const sfa::LineString *line, const std::string modelKitFilePath, scenegraph::Scene &group, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildVBS2ModelForLinearBridge(const sfa::LineString *line, const std::string modelKitFilePath, scenegraph::Scene &group, ModelGeneratorParameters parms = ModelGeneratorParameters());

		bool BuildAvoidZoneForLinearFromCSM(const sfa::LineString *line, const std::string crossSectionFilePath, modelkit::AvoidZone &avoid, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildAvoidZoneForLinearFromKIT(const sfa::LineString *line, const std::string modelKitFilePath, modelkit::AvoidZone &avoid, ModelGeneratorParameters parms = ModelGeneratorParameters());
		bool BuildAvoidZoneForLinear(const sfa::LineString *line, const modelkit::CrossSectionSP crossSection, modelkit::AvoidZone &avoid, ModelGeneratorParameters parms = ModelGeneratorParameters());



		bool BuildProjections(std::vector<sfa::Projection2D> &projections, 
			std::vector<sfa::Point> &normals,
			std::vector<sfa::Point> &lines,
			std::vector<double> &line_lengths,
			const sfa::LineString *line, 
			const modelkit::CrossSectionSP crossSection, 
			const ModelGeneratorParameters &parms,
			const sfa::Point line_origin);

		static modelkit::ModelKitSP GetCachedModelKit(std::string filename);
		static modelkit::CrossSectionSP GetCachedCrossSection(std::string filename);
	};


}