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
//#pragma optimize("", off) 

#include <boost/math/constants/constants.hpp>
#include <boost/foreach.hpp>
#include <boost/random.hpp>

#include "ModelKit/ModelGenerator.h"

#include <iostream>
#include <fstream>
#include <string>
#include <map>

#include <sfa/LineString.h>
#include <scenegraph/Scene.h>
#include <ModelKit/SceneData.h>

#include <flt/OpenFlight.h>
#include <flt/VertexWithColor.h>
#include <flt/VertexWithColorNormal.h>
#include <flt/VertexWithColorNormalUV.h>
#include <flt/VertexWithColorUV.h>

#include <sfa/PatchUnion.h>

#include <scenegraph/TransformVisitor.h>
#include <scenegraph/CoordinateTransformVisitor.h>
#include <scenegraph/ExtentsVisitor.h>
#include <ogr/File.h>
#include <scenegraph/IntegrateExternalsVisitor.h>

using namespace sfa;
using namespace ccl;
using namespace flt;

namespace modelkit
{

	modelKitMapType ModelGenerator::modelKitMap;
	ccl::mutex ModelGenerator::modelKitMapMutex;
	crossSectionKitMapType ModelGenerator::crossSectionMap;

	modelkit::ModelKitSP ModelGenerator::GetCachedModelKit(std::string filename)
	{
		modelKitMapMutex.lock();
		modelkit::ModelKitSP modelKit;
		if(modelKitMap.find(filename)!=modelKitMap.end())
		{
			modelKit = modelKitMap[filename];
		}
		else
		{
			modelKit = modelkit::ModelKitSP(new modelkit::ModelKit(filename));
			modelKitMap[filename] = modelKit;
		}
		modelKitMapMutex.unlock();
		return modelKit;
	}

	modelkit::CrossSectionSP ModelGenerator::GetCachedCrossSection(std::string filename)
	{
		modelKitMapMutex.lock();
		modelkit::CrossSectionSP crossSection;
		if(crossSectionMap.find(filename)!=crossSectionMap.end())
		{
			crossSection = crossSectionMap[filename];
		}
		else
		{
			crossSection = modelkit::CrossSectionSP(new modelkit::CrossSection(filename));
			crossSectionMap[filename] = crossSection;
		}

		modelKitMapMutex.unlock();
		return crossSection;
	}
	bool ModelGenerator::BuildModelForLinearFromCSM(const sfa::LineString *line,const std::string crossSectionFilePath, scenegraph::Scene &scene, ModelGeneratorParameters parms)
	{
		modelkit::SceneData *roadSceneData = dynamic_cast<modelkit::SceneData *>(scene.userData);
		modelkit::CrossSectionSP crossSection = GetCachedCrossSection(crossSectionFilePath);
		if(!crossSection)
			return false;

		if(roadSceneData)
			roadSceneData->crossSection = crossSection;
		bool ret = BuildModelForLinear(line,crossSection,scene,parms);

		return ret;
	}

	bool ModelGenerator::BuildModelForLinearFromKIT(const sfa::LineString *line,const std::string modelKitFilePath, scenegraph::Scene &scene, ModelGeneratorParameters parms)
	{
		modelkit::SceneData *roadSceneData = dynamic_cast<modelkit::SceneData *>(scene.userData);
		modelkit::ModelKitSP modelKit = GetCachedModelKit(modelKitFilePath);
		if(!modelKit)
			return false;

		if(roadSceneData)
			roadSceneData->modelKit = modelKit;
		bool ret = BuildModelForLinear(line,modelKit,scene,parms);

		return ret;
	}

	bool ModelGenerator::BuildModelForLinearFromKITS(const sfa::LineString *line,const std::string modelKitFilePath, scenegraph::Scene &scene, ModelGeneratorParameters parms)
	{
		if(ccl::FileInfo(modelKitFilePath).getSuffix() != "kits")
			return false;

		modelkit::ModelKitReferenceList kits;
		if(!read_kits(kits, modelKitFilePath))
			return false;

		// workingLine is the line we send to the model generator in endsOnly mode
		sfa::LineString workingLine(*line);
		double length = workingLine.getLength();
		double start_s = parms.endsDistance;
		double end_s = length - parms.endsDistance;
		if(end_s > start_s)
		{
			sfa::Point start_point;
			int start_index;
			workingLine.getPointAtS(start_s, start_point, start_index);
			++start_index;
			workingLine.insertPoint(start_index, start_point);

			sfa::Point end_point;
			int end_index;
			workingLine.getPointAtS(end_s, end_point, end_index);
			++end_index;
			workingLine.insertPoint(end_index, end_point);

			// get rid of points between the two
			sfa::LineString copyLine;
			for(int i = 0, c = workingLine.getNumPoints(); i < c; ++i)
			{
				if(i <= start_index)
					copyLine.addPoint(*workingLine.getPointN(i));
				if(i >= end_index)
					copyLine.addPoint(*workingLine.getPointN(i));
			}
			workingLine = sfa::LineString(copyLine);
			parms.endsSegment = start_index;
		}


		for(modelkit::ModelKitReferenceList::iterator it = kits.begin(), end = kits.end(); it != end; ++it)
		{
			ModelKitReference &mkref = *it;
			scenegraph::LOD *lod = new scenegraph::LOD;
			lod->name = mkref.filename;
			parms.endsOnly = !mkref.underground;
			const sfa::LineString *modelLine = parms.endsOnly ? &workingLine : line;
			scenegraph::Scene *group = lod->AddChild(mkref.min_dist, mkref.max_dist);
			// group now has the LOD range
			BuildModelForLinearFromKIT(modelLine, ccl::joinPaths(ccl::FileInfo(modelKitFilePath).getDirName(), mkref.filename), *group, parms);
			// group has the model kit scene faces, etc
			scene.children.push_back(lod);
			// the scene's children contains lod
			scene.matrix = group->matrix;
			// the scene's matrix is set to the group's matrix
			group->matrix.clear();
			// what happened to group? it was never added as a child to anything
		}

		return true;
	}

	bool ModelGenerator::BuildVBS2ModelForLinear(const sfa::LineString *line, const std::string modelKitFilePath, scenegraph::Scene &group, ModelGeneratorParameters parms)
	{
		if(parms.is_bridge)
			return BuildVBS2ModelForLinearBridge(line, modelKitFilePath, group, parms);

		// start by generating our regular visual model; if it's a regular kit, convert the result into a LOD
		if(ccl::FileInfo(modelKitFilePath).getSuffix() == "kits")
		{
			BuildModelForLinearFromKITS(line, modelKitFilePath, group, parms);
		}
		else
		{
			scenegraph::LOD *lod = new scenegraph::LOD;
			lod->name = "vbs2lod";
			scenegraph::Scene *scene = lod->AddChild(0, DBL_MAX);
			group.children.push_back(lod);
			BuildModelForLinearFromKIT(line, modelKitFilePath, *scene, parms);
			for(size_t i=0,ic=scene->children.size();i<ic;i++)
			{
				group.addChild(scene->children.at(i));
			}
			//group.children.insert(group.children.end(), scene->children.begin(), scene->children.end());
			// This is bad code because scene might have othe pointers (now or in the future.)
			// Even if it doesn't now, not deleting scene and just stealing child pointers is
			// a bad practice
			scene->children.clear();
			
		}

		// find the highest lod scene; we're going to use it to create our groups
		// it may be that we need these geometries for every scenegraph LOD; since the LOD node children are groups, this wouldn't be difficult to change
			// in the working examples we have, the groups are peers to the LOD, so we will do it at the top-level for now
		scenegraph::Scene *visualScene = NULL;
		scenegraph::LOD *visualLOD = NULL;
		for(size_t i = 0, ic = group.children.size(); i < ic; ++i)
		{
			scenegraph::Scene *scene = group.children.at(i);
			scenegraph::LOD *lod = dynamic_cast<scenegraph::LOD *>(scene);
			if(!lod)
				continue;
			double minrange = DBL_MAX;
			for(size_t j = 0, jc = lod->ranges.size(); j < jc; ++j)
			{
				scenegraph::LODRange range = lod->ranges.at(j);
				if(range.min < minrange)
				{
					minrange = range.min;
					visualScene = lod->children.at(j);
					visualLOD = lod;
				}
				scenegraph::Scene *scene = group.children.at(i);
			}
		}
		if(!visualScene)
			return false;

		group.matrix = visualScene->matrix;
		visualScene->matrix.clear();

		return true;
	}

	bool ModelGenerator::BuildVBS2ModelForLinearBridge(const sfa::LineString *line, const std::string modelKitFilePath, scenegraph::Scene &group, ModelGeneratorParameters parms)
	{
		// start by generating our regular visual model; if it's a regular kit, convert the result into a LOD
		if(ccl::FileInfo(modelKitFilePath).getSuffix() == "kits")
		{
			BuildModelForLinearFromKITS(line, modelKitFilePath, group, parms);
		}
		else
		{
			BuildModelForLinearFromKIT(line, modelKitFilePath, group, parms);
			scenegraph::Scene *model_group = new scenegraph::Scene;
			model_group->name = "model";
			scenegraph::Scene *mission_group = new scenegraph::Scene;
			mission_group->name = "MissionFunction";
			mission_group->attributes.setAttribute("#missionfunctiononly", true);
			scenegraph::Scene *visual_group = NULL;
			for(size_t i = 0, ic = group.children.size(); i < ic; ++i)
			{
				if(group.children.at(i)->name == "visual")
					visual_group = group.children.at(i);
				else
					mission_group->addChild(group.children.at(i));
			}
			group.children.clear();
			model_group->addChild(mission_group);
			if(visual_group)
				group.addChild(visual_group);
			group.addChild(model_group);
		}

		return true;
	}

	ReferenceModelInfo ModelGenerator::GetModelInfo(std::string filename)
	{
		ReferenceModelInfo ret;
		if(modelInfoMap.find(filename)!=modelInfoMap.end())
		{
			ret = modelInfoMap[filename];
		}
		else
		{
			if(!ReferenceModelInfo::GetModelInfo(filename,ret))
			{
				ret.zMin  = 0;
				ret.zMax = 1.0f;// default if we can't open the file.
				ret.radius = 1.0f;
			}
			modelInfoMap[filename] = ret;
		}

		return ret;
	}

	bool ModelGenerator::PlaceVertexModelInstances(scenegraph::Scene &scene, SceneData *scenedata,modelkit::ModelInstance *inst, scenegraph::LineString &ls,const ReferenceModelInfo &rmi)
	{
		size_t numPoints = ls.lineString.getNumPoints();
		sfa::Point *previousPoint = ls.lineString.getPointN(0);
		for(size_t i = 0; i < (numPoints-1); i++)
		{
			sfa::Point firstPt(ls.lineString.getPointN((int)i));
			sfa::Point secondPt(ls.lineString.getPointN((int)i+1));
			sfa::Point source_i = *scenedata->projections[i].getBasisVector(0);
			sfa::Point dest_i = *scenedata->projections[i+1].getBasisVector(0);
			double source_theta = asin( source_i.Z() / source_i.length() );
			double dest_theta = asin( dest_i.Z() / dest_i.length() );

			scenegraph::ExternalReference ref;

			ref.zMin = rmi.zMin;
			ref.zMax = rmi.zMax;
			ref.radius = rmi.radius;
			ref.position = firstPt;
			ref.scale = sfa::Point(1.0,1.0,1.0);
			//Calculate the heading for this line
			sfa::Quat roll;
			sfa::Quat align;
			align.makeRotation(sfa::Point(0,1,0),secondPt-firstPt);
			roll.makeRotation( source_theta, 0.0 , 1.0 , 0.0 );	//	Rotate about the y axis
			if (!roll.zero())
				ref.attitude = roll*align;
			else
				ref.attitude = align;
			ref.filename = rmi.filename;
			scene.externalReferences.push_back(ref);
		}
		PlaceEndInstances(scene,scenedata,inst,ls,rmi);

		return true;
	}

	bool ModelGenerator::PlaceStartInstances(scenegraph::Scene &scene, SceneData *scenedata,modelkit::ModelInstance *inst, scenegraph::LineString &ls,const ReferenceModelInfo &rmi)
	{
		sfa::Point source_i = *scenedata->projections[0].getBasisVector(0);
		sfa::Point dest_i = *scenedata->projections[1].getBasisVector(0);
		double source_theta = asin( source_i.Z() / source_i.length() );
		double dest_theta = asin( dest_i.Z() / dest_i.length() );

		sfa::Point firstPt = ls.lineString.getPointN(0);
		sfa::Point secondPt = ls.lineString.getPointN(1);
		scenegraph::ExternalReference ref;

		ref.zMin = rmi.zMin;
		ref.zMax = rmi.zMax;
		ref.radius = rmi.radius;
		ref.position = firstPt;
		ref.scale = sfa::Point(1.0,1.0,1.0);
		//Calculate the heading for this line
		sfa::Quat roll;
		sfa::Quat align;
		align.makeRotation(sfa::Point(0,1,0),secondPt-firstPt);
		roll.makeRotation( source_theta, 0.0 , 1.0 , 0.0 );	//	Rotate about the y axis
		if (!roll.zero())
			ref.attitude = roll*align;
		else
			ref.attitude = align;
		ref.filename = rmi.filename;
		scene.externalReferences.push_back(ref);
		return true;
	}

	bool ModelGenerator::PlaceEndInstances(scenegraph::Scene &scene, SceneData *scenedata,modelkit::ModelInstance *inst, scenegraph::LineString &ls,const ReferenceModelInfo &rmi)
	{
		int numPoints = ls.lineString.getNumPoints();
		sfa::Point source_i = *scenedata->projections[numPoints-2].getBasisVector(0);
		sfa::Point dest_i = *scenedata->projections[numPoints-1].getBasisVector(0);
		double source_theta = asin( source_i.Z() / source_i.length() );
		double dest_theta = asin( dest_i.Z() / dest_i.length() );

		sfa::Point firstPt = ls.lineString.getPointN(numPoints-2);
		sfa::Point secondPt = ls.lineString.getPointN(numPoints-1);
		scenegraph::ExternalReference ref;

		ref.zMin = rmi.zMin;
		ref.zMax = rmi.zMax;
		ref.radius = rmi.radius;
		ref.position = secondPt;
		ref.scale = sfa::Point(1.0,1.0,1.0);
		//Calculate the heading for this line
		sfa::Quat roll;
		sfa::Quat align;
		align.makeRotation(sfa::Point(0,1,0),secondPt-firstPt);
		roll.makeRotation( source_theta, 0.0 , 1.0 , 0.0 );	//	Rotate about the y axis
		if (!roll.zero())
			ref.attitude = roll*align;
		else
			ref.attitude = align;
		ref.filename = rmi.filename;
		scene.externalReferences.push_back(ref);
		return true;
	}

	bool ModelGenerator::PlaceIntervalModelInstances(scenegraph::Scene &scene, SceneData *scenedata,modelkit::ModelInstance *inst, scenegraph::LineString &ls,const ReferenceModelInfo &rmi)
	{
		double uniform_min = inst->getSpacingMean() - (inst->getSpacingStandardDeviation());
		double uniform_max = inst->getSpacingMean() + (inst->getSpacingStandardDeviation());
		if(uniform_min==uniform_max)
		{
			uniform_min = 0;// do this to prevent an assert in boost if they are the same
			uniform_max = 1;
		}
		// Spacing RNG
		boost::mt19937 generator;
		boost::uniform_real<double> uniformDistribution(uniform_min,uniform_max);
		boost::normal_distribution<double> normalDistribution(inst->getSpacingMean(),inst->getSpacingStandardDeviation());
		boost::variate_generator<boost::mt19937&, boost::uniform_real<double> > uniformGen(generator,uniformDistribution);
		boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > normalGen(generator,normalDistribution);


		double uniform_min2 = inst->getOffsetMean() - (inst->getOffsetStandardDeviation());
		double uniform_max2 = inst->getOffsetMean() + (inst->getOffsetStandardDeviation());
		if(uniform_min2==uniform_max2)
		{
			uniform_min2 = 0;// do this to prevent an assert in boost if they are the same
			uniform_max2 = 1;
		}
		// These are for the offset
		boost::mt19937 generator2;
		boost::uniform_real<double> uniformDistribution2(uniform_min2,uniform_max2);
		boost::normal_distribution<double> normalDistribution2(inst->getSpacingMean(),inst->getSpacingStandardDeviation());
		boost::variate_generator<boost::mt19937&, boost::uniform_real<double> > uniformGen2(generator2,uniformDistribution2);
		boost::variate_generator<boost::mt19937&, boost::normal_distribution<double> > normalGen2(generator2,normalDistribution2);

		double spacing = inst->getSpacingMean();
		if(inst->getDistributionType()==modelkit::DISTRIBUTION_NORMAL)
		{
			spacing = normalGen();
		}
		else if(inst->getDistributionType()==modelkit::DISTRIBUTION_UNIFORM)
		{
			if(inst->getOffsetStandardDeviation()!=0)
			{
				spacing = uniformGen();
			}							
		}
		// For each segment of the linestring, 
		size_t numPoints = ls.lineString.getNumPoints();
		if(numPoints>1)
		{
			double lastPlacePosition = 0; // The length along the line of the last instance we placed
			double position = 0;// The length of the line as of previousPoint
			sfa::Point *previousPoint = ls.lineString.getPointN(0);
			for(size_t i = 1; i < numPoints; i++)
			{
				sfa::Point *pt = ls.lineString.getPointN((int)i);
				sfa::Point vec(*pt - *previousPoint);

				//	Create initial alignment quat
				sfa::Quat align;
				sfa::Point vec_copy = vec;
#if 1
				vec_copy.clearZ();
#else
				if ( !corrent_z )
					vec_copy.clearZ();
#endif
				align.makeRotation(sfa::Point(0,1,0),vec_copy);

				//	Find roll angles
				sfa::Point source_i = *scenedata->projections[i-1].getBasisVector(0);
				sfa::Point dest_i = *scenedata->projections[i].getBasisVector(0);
				double source_theta = asin( source_i.Z() / source_i.length() );
				double dest_theta = asin( dest_i.Z() / dest_i.length() );

				double dist = vec.length();
				while((lastPlacePosition+spacing)<(position+dist))
				{
					double segPos = (spacing + lastPlacePosition) - position;
					double pct = segPos / dist;									
					//Multiply by the distance along the line we want
					sfa::Point placePt = (vec*pct) + *previousPoint;// Add the origin of the segment

					// Calculate an offset

					double offset_mean = inst->getOffsetMean();
					double offset = offset_mean;
					double offset_dev = inst->getOffsetStandardDeviation();
					if(offset_mean!=0 || offset_dev!=0)
					{
						if(inst->getDistributionType()==modelkit::DISTRIBUTION_NORMAL)
						{
							if(offset_dev!=0)
								offset = normalGen2();
						}
						else if(inst->getDistributionType()==modelkit::DISTRIBUTION_UNIFORM)
						{
							if(offset_dev!=0)
								offset = uniformGen2();
						}

						sfa::Point line2d(previousPoint->X() - pt->X(),previousPoint->Y() - pt->Y());
						sfa::Point perp2d(-1 * line2d.Y(),line2d.X());
						perp2d.normalize();
						placePt.setX(placePt.X()+(perp2d.X() * offset));
						placePt.setY(placePt.Y()+(perp2d.Y() * offset));
					}

					scenegraph::ExternalReference ref;

					ref.zMin = rmi.zMin;
					ref.zMax = rmi.zMax;
					ref.radius = rmi.radius;
					ref.position = placePt;
					ref.scale = sfa::Point(1.0,1.0,1.0);
					//Calculate the heading for this line
					sfa::Quat roll;
					roll.makeRotation( source_theta*(1 - pct) + dest_theta*(pct) , 0.0 , 1.0 , 0.0 );	//	Rotate about the y axis
					if (!roll.zero())
						ref.attitude = roll*align;
					else
						ref.attitude = align;
					ref.filename = rmi.filename;
					scene.externalReferences.push_back(ref);
					lastPlacePosition += spacing;

					if(inst->getDistributionType()==modelkit::DISTRIBUTION_NORMAL)
					{
						spacing = normalGen();
					}
					else if(inst->getDistributionType()==modelkit::DISTRIBUTION_UNIFORM)
					{
						if(inst->getOffsetStandardDeviation()!=0)
						{
							spacing = uniformGen();
						}
					}
				}
				position += dist;
				previousPoint = pt;
			}							
		}

		return true;
	}

	bool ModelGenerator::BuildModelForLinear(const sfa::LineString  *line,const modelkit::ModelKitSP modelKit, scenegraph::Scene &scene,ModelGeneratorParameters parms)
	{				
		modelkit::CrossSectionSP cs(modelKit->consolidate());
		SceneData *scenedata = NULL;
        if(!scene.userData)
		{
			scenedata = new SceneData();
			scene.userData = scenedata;
		}
		else
		{
			scenedata = dynamic_cast<SceneData *>(scene.userData);
		}
		scenedata->crossSection = cs;

		if(cs.get() && BuildModelForLinear(line,cs,scene,parms))
		{
			std::vector<modelkit::ModelInstance *> instances = modelKit->getModelInstances(); 
			BOOST_FOREACH(modelkit::ModelInstance *inst, instances)
			{
				std::string csname = inst->getCrossSectionReference()->getAlias();
				std::string attach = inst->getAttachmentPointName();
				std::string attach_name = csname + ":" + attach;

				std::string modelFilename = modelKit->getReferenceSource(inst);
				//set the model instance source filename. First check the working directory for the file;
                //if it is not found, then check the model kit 
                std::ifstream fname(inst->getSource().c_str(), std::ios::in);
                if (fname.fail())
                {
                    std::ifstream fname2(modelKit->getReferenceSource(inst).c_str(), std::ios::in);
                    if (fname2.fail())
                        log << ccl::LERR << "Cannot find model instance source file: " << log.endl;
                    else
                        modelFilename = modelKit->getReferenceSource(inst);
                }
                else
                    modelFilename = inst->getSource();

				ReferenceModelInfo rmi = GetModelInfo(modelFilename);
				BOOST_FOREACH(scenegraph::LineString &ls, scene.lineStrings)
				{					
					if((attach_name==ls.name)||(attach==ls.name))
					{
						ModelInstancePosition positionMode = inst->getPosition();
						
						if(!inst->getAutoPlaced())
							continue;// Nothing to see here!
						if(positionMode==ModelInstancePosition_Start)
						{
							PlaceStartInstances(scene,scenedata,inst,ls,rmi);
						}
						else if(positionMode==ModelInstancePosition_End)
						{
							PlaceEndInstances(scene,scenedata,inst,ls,rmi);
						}
						else if(positionMode==ModelInstancePosition_StartAndEnd)
						{
							PlaceStartInstances(scene,scenedata,inst,ls,rmi);
							PlaceEndInstances(scene,scenedata,inst,ls,rmi);
						}
						else if(positionMode==ModelInstancePosition_Vertex)
						{
							PlaceVertexModelInstances(scene,scenedata,inst,ls,rmi);
						}
						if(inst->getSpacingMean()!=0)
							PlaceIntervalModelInstances(scene,scenedata,inst,ls,rmi);
					}
				}
			}
			if (parms.consolidateInstancedModels)
			{
				//Use a visitor to transform all the faces from each model and replace the external references with the contents
				scenegraph::IntegrateExternalsVisitor integrator;
				integrator.integrate(&scene);					
			}		
            return true;
		}
		
        return false;
	}


	double compareDirection(const sfa::LineString *a, const sfa::LineString *b)
	{
		sfa::Point *A0 = a->getPointN(0);
		sfa::Point *A1 = a->getPointN(1);
		sfa::Point *B0 = b->getPointN(b->getNumPoints() - 2);
		sfa::Point *B1 = b->getPointN(b->getNumPoints() - 1);
		sfa::Point pointA(A1->X() - A0->X(), A1->Y() - A0->Y());
		pointA.normalize();
		sfa::Point normalB(0 - (B1->Y() - B0->Y()), B1->X() - B0->X());
		normalB.normalize();
		return pointA.dot(normalB);
	}

	bool ModelGenerator::BuildProjections(std::vector<sfa::Projection2D> &projections, 
			std::vector<sfa::Point> &normals,
			std::vector<sfa::Point> &lines,
			std::vector<double> &line_lengths,
			const sfa::LineString *line, 
			const modelkit::CrossSectionSP crossSection, 
			const ModelGeneratorParameters &parms,
			const sfa::Point line_origin)
	{
		double prevAngle = dictionary.getPreviousAngle(parms.featureAttributes);
		double nextAngle = dictionary.getNextAngle(parms.featureAttributes);
		
		int num_points = line->getNumPoints();
		if(num_points<2)
			return false;
		
		//log << "Generating model for linear with " << num_points << " points." << log.endl;

		// determine next and previous widths
		double thisWidth = crossSection->getWidth();
		double nextWidth = thisWidth;		// width of next connected feature (if scaling)
		double previousWidth = thisWidth;	// width of previous connected feature (if scaling)
		double nextOffsetX = 0.0f;
		double previousOffsetX = 0.0f;
		if(crossSection)
		{
			if(parms.linkinfo)
			{
				int nextCount = int(parms.linkinfo->successors.size());
				int previousCount = int(parms.linkinfo->predecessors.size());
				/*
				if(linkinfo->nextLine.isValid())
					nextWidth = linkinfo->nextLine.width;
				
				if(linkinfo->previousLine.isValid())
					previousWidth = linkinfo->previousLine.width;
				*/
				nextWidth = 0;
				for(int i = 0; i < nextCount; ++i)
					 nextWidth += parms.linkinfo->successors[i].width;
				
				previousWidth = 0;
				for(int i = 0; i < previousCount; ++i)
					 previousWidth += parms.linkinfo->predecessors[i].width;
			    

//XXXX TODO: Fix this
#if 0				
				if(scenedata->feature && (scenedata->feature->attributes.find("CIfixed") != scenedata->feature->attributes.end()))
				{
					nextWidth = thisWidth;
					previousWidth = thisWidth;
				}
#endif
				bool previousDone = false;
				bool nextDone = false;
				//if(previousCount == 1)
				if(parms.linkinfo->previousLine.isValid() && parms.linkinfo->beginSiblings.size()>0)
				{					
					previousWidth = thisWidth;	// we're moving, not resizing
					double targetWidth = thisWidth;
					for(size_t sibling_i=0,sibling_ic=parms.linkinfo->beginSiblings.size();sibling_i<sibling_ic;sibling_i++)
					{
						targetWidth+=parms.linkinfo->beginSiblings[sibling_i].width;
					}
					
					double offsetValue = (targetWidth - thisWidth) / 2;
					// this only handles two, which should suffice					
					// Compare the direction of the sibling node to the previous line
					double siblingDirection = compareDirection(&parms.linkinfo->beginSiblings[0].segment, &parms.linkinfo->previousLine.segment);
					// Compare the direction of this line to the previous line
					double thisDirection = compareDirection(const_cast<sfa::LineString *>(line), &parms.linkinfo->previousLine.segment);
					previousOffsetX = (siblingDirection > thisDirection) ? offsetValue : 0 - offsetValue;
					previousDone = true;					
				}
				if(parms.linkinfo->nextLine.isValid() && parms.linkinfo->endSiblings.size()>0)
				{
					nextWidth = thisWidth;	// we're moving, not resizing
					double targetWidth = thisWidth;
					for(size_t sibling_i=0,sibling_ic=parms.linkinfo->endSiblings.size();sibling_i<sibling_ic;sibling_i++)
					{
						targetWidth+=parms.linkinfo->endSiblings[sibling_i].width;
					}
					double offsetValue = (targetWidth - thisWidth) / 2;
					// this only handles two, which should suffice
					double siblingDirection = compareDirection(&parms.linkinfo->nextLine.segment, &parms.linkinfo->endSiblings[0].segment);
					double thisDirection = compareDirection(&parms.linkinfo->nextLine.segment, const_cast<sfa::LineString *>(line));
					nextOffsetX = (siblingDirection > thisDirection) ? offsetValue : 0 - offsetValue;
					nextDone = true;					

				}
				if(parms.linkinfo->endsAt.isValid() && !parms.linkinfo->nextLine.isValid() && !nextDone)
				{
					nextWidth = thisWidth;	// we're moving, not resizing
					double thisDirection = compareDirection(&parms.linkinfo->endsAt.segment, const_cast<sfa::LineString *>(line));
					double targetWidth = parms.linkinfo->endsAt.width;
					double offsetValue = (targetWidth - thisWidth) / 2;
					nextOffsetX = (thisDirection >= 0.0f) ? 0 - offsetValue : offsetValue;
				}
				if(parms.linkinfo->beginsAt.isValid() && !parms.linkinfo->previousLine.isValid() && !previousDone)
				{
					previousWidth = thisWidth;	// we're moving, not resizing
					double thisDirection = compareDirection(const_cast<sfa::LineString *>(line), &parms.linkinfo->beginsAt.segment);
					double targetWidth = parms.linkinfo->beginsAt.width;
					double offsetValue = (targetWidth - thisWidth) / 2;
					previousOffsetX = (thisDirection >= 0.0f) ? 0 - offsetValue : offsetValue;
				}
			}

		}

		// calculate the distances
		sfa::Point *line_endpoint = line->getPointN(line->getNumPoints() - 1);
		std::vector<double> distances0(num_points);		// each point's travel distance from the origin
		std::vector<double> distancesN(num_points);		// each point's travel distance from the endpoint
		double dist0 = 0.0f;
		double distN = 0.0f;
		distances0[0] = 0.0f;
		distancesN[num_points - 1] = 0.0f;
		for(int i = 1; i < num_points; ++i)
		{
			dist0 += GetLength(line->getPointN(i - 1), line->getPointN(i));
			distN += GetLength(line->getPointN(num_points - i - 1), line->getPointN(num_points - i));
			distances0[i] = dist0;
			distancesN[num_points - i - 1] = distN;
		}

		double current_length=0;
		for(int i=0;i<num_points;i++)
		{
			// widen the cross section at the connecting point if it is less width than the connecting feature
			double distMultiplier = 5.0f;	// this is arbitrary
			double scaleX = 1.0f;	// amount to scale (we only scale up)
			double scaleY = 1.0f;	// amount to scale Y
			double offsetX = 0.0f;	// amount to offset
			if(previousWidth > thisWidth + 0.0000001)
			{
				double maxScale = previousWidth / thisWidth;		// maximum scaling = width factor
				double scaleDiff = maxScale - 1;					// minimum scaling is 1.0
				double maxdist = previousWidth * distMultiplier;	// maximum distance for scaling
				double dist = distances0[i];						// distance from endpoint to this point
				double factor = dist / maxdist;						// difference factor (we want 0.0 for zero distance (max scaling), >= 1.0 for no scaling)
				scaleX = maxScale - (factor * scaleDiff); 
			}
			if(nextWidth > thisWidth + 0.0000001)
			{
				double maxScale = nextWidth / thisWidth;			// maximum scaling = width factor
				double scaleDiff = maxScale - 1;					// minimum scaling is 1.0
				double maxdist = nextWidth * distMultiplier;		// maximum distance for scaling
				double dist = distancesN[i];						// distance from endpoint to this point
				double factor = dist / maxdist;						// difference factor (we want 0.0 for zero distance (max scaling), >= 1.0 for no scaling)
				scaleX = maxScale - (factor * scaleDiff); 
			}
			if((previousOffsetX != 0.0f) && (i < num_points - 1))
			{
				double maxdist = previousWidth * distMultiplier * 5;
				double dist = distances0[i];
				double factor = dist / maxdist;
				if(factor < 1.0f)
					offsetX = previousOffsetX - (factor * previousOffsetX);
			}
			if((nextOffsetX != 0.0f) && (i > 0))
			{
				double maxdist = nextWidth * distMultiplier * 5;
				double dist = distancesN[i];
				double factor = dist / maxdist;
				if(factor < 1.0f)
					offsetX = nextOffsetX - (factor * nextOffsetX);
			}

			Projection2D projection;
			sfa::Point midpoint = *line->getPointN(i)- line_origin;
			sfa::Point point2;
			sfa::Point point3;
			//The plane origin is always midpoint
			
			bool hasNext = false;
			bool hasPrev = false;

			//Search for previous point
			for (int j=i-1; j>=0; j--)
			{
				point2 = *line->getPointN(j) - line_origin;
				if ((point2 - midpoint).length2() >= SFA_EPSILON)
				{
					hasPrev = true;
					break;
				}
			}
			//Search for next point
			for (int j=i+1; j<num_points; j++)
			{
				point3 = *line->getPointN(j) - line_origin;
				if ((point3 - midpoint).length2() >= SFA_EPSILON)
				{
					hasNext = true;
					break;
				}
			}

			if (!hasPrev && !hasNext)
			{
			//	This happens when all the points of a line are compressed into one point.
				return false;
			}
			else if(!hasPrev && hasNext)
			{
				if(parms.linkinfo && parms.linkinfo->previousLine.isValid())
				{
					if ( line->getStartPoint()->equals3D(parms.linkinfo->previousLine.segment.getEndPoint()) )
						point2 = *parms.linkinfo->previousLine.segment.getStartPoint() - line_origin;
					else if ( line->getStartPoint()->equals3D(parms.linkinfo->previousLine.segment.getStartPoint()) )
						point2 = *parms.linkinfo->previousLine.segment.getEndPoint() - line_origin;
					else
						point2 = midpoint;
				}
#if 1
				else if (prevAngle)
				{
					point2 = midpoint - point3;
					double cos_a = cos(2*prevAngle);
					double sin_a = sin(2*prevAngle);
					point2 = sfa::Point(cos_a*point2.X() - sin_a*point2.Y(), sin_a*point2.X() + cos_a*point2.Y());
					point2 = midpoint + point2;
				}
#endif
				else
					point2 = midpoint;

				sfa::Point n = GetBisectionalPlaneNormal(midpoint,point2,point3,true);
				projection.createBasis(&midpoint, &n);
				n.normalize();
				normals.push_back(n);
				
				//sfa::Point l = midpoint - point3;
				sfa::Point l = point3 - midpoint;
				l.normalize();
				lines.push_back(l);

			}
			else if(hasPrev && !hasNext)
			{
				if(parms.linkinfo && parms.linkinfo->nextLine.isValid())
				{
					if ( line->getEndPoint()->equals3D(parms.linkinfo->nextLine.segment.getEndPoint()) )
						point3 = *parms.linkinfo->nextLine.segment.getStartPoint()  - line_origin;
					else if ( line->getEndPoint()->equals3D(parms.linkinfo->nextLine.segment.getStartPoint()) )
						point3 = *parms.linkinfo->nextLine.segment.getEndPoint() - line_origin;
					else
						point3 = midpoint;
				}
#if 1
				else if (nextAngle)
				{
					point3 = midpoint - point2;
					double cos_a = cos(-2*nextAngle);
					double sin_a = sin(-2*nextAngle);
					point3 = sfa::Point(cos_a*point3.X() - sin_a*point3.Y(), sin_a*point3.X() + cos_a*point3.Y());
					point3 = midpoint + point3;
				}
#endif
				else
					point3 = midpoint;

				sfa::Point n = GetBisectionalPlaneNormal(midpoint,point2,point3);
				projection.createBasis(&midpoint, &n);
				n.normalize();
				normals.push_back(n);
				
				sfa::Point l = midpoint - point2;
				l.normalize();
				lines.push_back(l);

				current_length += GetLength(&midpoint,&point2);
			}
			else
			{
				sfa::Point pa = midpoint;
				sfa::Point pb = point2;
				sfa::Point pc = point3;
				//pa.setZ(pa.Z() + (crossSection->getHeight() / 2));
				//pb.setZ(pb.Z() + (crossSection->getHeight() / 2));
				//pc.setZ(pc.Z() + (crossSection->getHeight() / 2));
				sfa::Point n = GetBisectionalPlaneNormal(pa, pb, pc);
				projection.createBasis( &pa, &n);
				normals.push_back(n);

				sfa::Point l = point3 - midpoint;
				l.normalize();

				sfa::Point last_l = lines.back();
				double ldot = l.dot(last_l);
				if((i>1) && (ldot < 0.25f))
				{
					log << ccl::LWARNING << "Warning: severe angle in projections at vert#" << i << "." << log.endl;
					log << ccl::LINFO;
				}
				lines.push_back(l);

				/* 20151109/abrinton - commented this out to replace with manual cross section construction; if we don't allow compound angles, this might be a better solution
				// 20141010/abrinton - this scales the width and height of the cross section at the connecting point based on the angle
				// without scaling, the joint is pinched since the cross section may be rotated at the connection point
				sfa::Point a = midpoint - point2;
				a.clearZ();
				a.normalize();
				sfa::Point b = point3 - midpoint;
				b.clearZ();
				b.normalize();
				double dot = a.dot(b);
				// Dot can misbehave with floating point errors that cause acos to give us bad values 
				// even though a and b are normalized. This makes sure acos doesn't blow up in those cases
				dot = std::min<double>(1,dot);
				dot = std::max<double>(-1,dot);
				double angle = acos(dot) * 0.5;
				if(angle != 0.0f)
					scaleX = 1 / cos(angle);

				a.setX(midpoint.distance(&point2));
				a.setY(midpoint.Z() - point2.Z());
				a.clearZ();
				a.normalize();
				b.setX(midpoint.distance(&point3));
				b.setY(point3.Z() - midpoint.Z());
				b.clearZ();
				b.normalize();
				dot = a.dot(b);
				// Dot can misbehave with floating point errors that cause acos to give us bad values 
				// even though a and b are normalized. This makes sure acos doesn't blow up in those cases
				dot = std::min<double>(1,dot);
				dot = std::max<double>(-1,dot);
				angle = acos(dot) * 0.5;
				if(angle != 0.0f)
					scaleY = 1 / cos(angle);
				*/

				current_length += GetLength(&midpoint,&point2);

			}
			line_lengths.push_back(current_length);

			// abrinton - TODO: scaling/projecting the cross section potentially displaces intersections resolved in the pipeline
			//		these changes eventually need to take place in the linestring rather than here
			if(scaleX < 1.0f)		// only scale up
				scaleX = 1.0f;
			if(scaleY < 1.0f)		// only scale up
				scaleY = 1.0f;
			projection.scale2D(scaleX, scaleY);
			if(offsetX != 0.0f)
			{
				projection.trans2D(offsetX, 0.0f);
				//projection.scale2D(2, 1.0f);	// for testing, shows the offset geometry
			}
			projections.push_back(projection);

			
		}

		return true;
	}


	bool ModelGenerator::BuildModelForLinear(const sfa::LineString *origline,const modelkit::CrossSectionSP crossSection, scenegraph::Scene &scene, ModelGeneratorParameters parms)
	{
		if(origline->isEmpty())
			return false;
		if(parms.is_bridge)
			return BuildModelForLinearBridge(origline, crossSection, scene, parms);

		sfa::PolygonList footprints;
		this->crossSection = crossSection;	// need this for texture paths

		scenegraph::Scene *geometryGroup = new scenegraph::Scene();
		scene.addChild(geometryGroup);
		scenegraph::Scene *collisionGroup = NULL;
		scenegraph::Scene *shadowVolumeGroup = NULL;
		scenegraph::Scene *undergroundGroup = NULL;
		scenegraph::Scene *roadwayGroup = NULL;
		scenegraph::Scene *pathsGroup = NULL;
		if(parms.vbs2Mode)
		{
			collisionGroup = new scenegraph::Scene;
			collisionGroup->name = "collision";
			collisionGroup->attributes.setAttribute("collision", true);
			shadowVolumeGroup = new scenegraph::Scene;
			shadowVolumeGroup->name = "vbs2shadow";
			shadowVolumeGroup->attributes.setAttribute("vbs2shadow", true);
			undergroundGroup = new scenegraph::Scene;
			undergroundGroup->name = "vbs2underground";
			undergroundGroup->attributes.setAttribute("vbs2underground", true);
			roadwayGroup = new scenegraph::Scene;
			roadwayGroup->name = "vbs2roadway";
			roadwayGroup->attributes.setAttribute("vbs2roadway", true);
			pathsGroup = new scenegraph::Scene;
			pathsGroup->name = "vbs2ai";
			pathsGroup->attributes.setAttribute("vbs2ai", true);
		}

		attach_string_t attachmentStrings;
		std::vector<sfa::Projection2D> projections;
		std::vector<modelkit::CrossSectionSP> crossSections;
		std::vector<sfa::Point> normals;
		std::vector<sfa::Point> lines;
		std::vector<double> line_lengths;
		double prevOffset = dictionary.getPreviousOffset(parms.featureAttributes);
		double nextOffset = dictionary.getNextOffset(parms.featureAttributes);
		double prevAngle = dictionary.getPreviousAngle(parms.featureAttributes);
		double nextAngle = dictionary.getNextAngle(parms.featureAttributes);
		// Make a copy of the line to trim
		sfa::LineString *line = new sfa::LineString(*origline); 
		// Only trim road for junctions
		if (prevAngle > 0) prevOffset = 0;
		if (nextAngle > 0) nextOffset = 0;

		// Get the origin before trimming the line
		sfa::Point line_origin = *line->getPointN(0);

		if (prevOffset > 0 || nextOffset > 0)
		{
			if(!line->trimEnds(prevOffset,nextOffset))
			{
				// We can't trim it, so return an error
				log << "Model generator unable to trim next/previous offsets for linear. Possibly due to occluded road." << log.endl;
				return false;
			}
		}
		SceneData *scenedata = NULL;
		if(!scene.userData)
		{
			scenedata = new SceneData();
			scene.userData = scenedata;
		}
		else
		{
			scenedata = dynamic_cast<SceneData *>(scene.userData);
		}
		scenedata->crossSection = crossSection;
		std::stringstream ss;

		if(!BuildProjections(projections, normals, lines, line_lengths, line,crossSection, parms, line_origin))
		{
			delete line;
			return false;
		}

		// if vbs2 mode, we need to create some alternate projections
		// underground projections contain overlap points, so they are doubled up
		std::vector<modelkit::CrossSectionSP> undergroundCrossSections;
		std::vector<sfa::Projection2D> undergroundProjections;
		std::vector<sfa::Point> undergroundNormals;
		std::vector<sfa::Point> undergroundLines;
		std::vector<double> undergroundLineLengths1;
		std::vector<double> undergroundLineLengths2;
		modelkit::CrossSectionSP undergroundCrossSection;
		std::vector<modelkit::CrossSectionSP> collisionCrossSections;
		std::vector<sfa::Projection2D> collisionProjections;
		std::vector<sfa::Point> collisionNormals;
		std::vector<sfa::Point> collisionLines;
		std::vector<double> collisionLineLengths1;
		std::vector<double> collisionLineLengths2;
		modelkit::CrossSectionSP collisionCrossSection;
		if(parms.vbs2Mode)
		{
			collisionCrossSection = modelkit::CrossSectionSP(new modelkit::CrossSection(*crossSection));

			// make a copy of the cross section so we can expand it away from the linear
			undergroundCrossSection = modelkit::CrossSectionSP(new modelkit::CrossSection(*crossSection));
			undergroundCrossSection->translate(0.0f, -0.01f);	// get the origin off the surface so we can expand the surface downward
			undergroundCrossSection->expand(0.5f, 0.5f);	// expand moves all points x/y meters from origin

			// for underground volumes, we need to extend each segment by a short distance to introduce overlap
			for(int i = 0, c = line->getNumPoints(); i < c - 1; ++i)
			{
				sfa::Point offset = normals[i];		// normal is opposite the segment direction
				offset *= undergroundCrossSection->getWidth() / 2;		// we need enough overlap to ensure we cover the tunnel interior in a turn; half the cross section width will allow a 45 degree turn
				sfa::Point pointA = *line->getPointN(i);
				if(i > 0)
					pointA = sfa::Point(pointA.X() + offset.X(), pointA.Y() + offset.Y(), pointA.Z() + offset.Z());
				sfa::Point pointB = *line->getPointN(i + 1);
				if(i < c - 2)
					pointB = sfa::Point(pointB.X() - offset.X(), pointB.Y() - offset.Y(), pointB.Z() - offset.Z());
				sfa::LineString segment;
				segment.addPoint(pointA);
				segment.addPoint(pointB);
				{
					std::vector<sfa::Projection2D> segmentProjections;
					std::vector<sfa::Point> segmentNormals;
					std::vector<sfa::Point> segmentLines;
					std::vector<double> segmentLineLengths;
					if(BuildProjections(segmentProjections, segmentNormals, segmentLines, segmentLineLengths, &segment, undergroundCrossSection, parms, line_origin))
					{
						undergroundProjections.push_back(segmentProjections[0]);
						undergroundProjections.push_back(segmentProjections[1]);
						undergroundNormals.push_back(segmentNormals[0]);
						undergroundNormals.push_back(segmentNormals[1]);
						undergroundLines.push_back(segmentLines[0]);
						undergroundLines.push_back(segmentLines[1]);
						undergroundLineLengths1.push_back(segmentLineLengths[0]);
						undergroundLineLengths1.push_back(segmentLineLengths[1]);
						undergroundLineLengths2.push_back(segmentLineLengths[0]);
						undergroundLineLengths2.push_back(segmentLineLengths[1]);
					}
				}
				{
					if(i == 0)
						*segment.getPointN(0) = *line->getPointN(i);
					if(i == c - 2)
						*segment.getPointN(1) = *line->getPointN(i + 1);

					std::vector<sfa::Projection2D> segmentProjections;
					std::vector<sfa::Point> segmentNormals;
					std::vector<sfa::Point> segmentLines;
					std::vector<double> segmentLineLengths;
					if(BuildProjections(segmentProjections, segmentNormals, segmentLines, segmentLineLengths, &segment, collisionCrossSection, parms, line_origin))
					{
						collisionProjections.push_back(segmentProjections[0]);
						collisionProjections.push_back(segmentProjections[1]);
						collisionNormals.push_back(segmentNormals[0]);
						collisionNormals.push_back(segmentNormals[1]);
						collisionLines.push_back(segmentLines[0]);
						collisionLines.push_back(segmentLines[1]);
						collisionLineLengths1.push_back(segmentLineLengths[0]);
						collisionLineLengths1.push_back(segmentLineLengths[1]);
						collisionLineLengths2.push_back(segmentLineLengths[0]);
						collisionLineLengths2.push_back(segmentLineLengths[1]);
					}
				}
			}
			for(size_t i = 0, c = collisionProjections.size(); i < c; ++i)
			{
				modelkit::CrossSectionSP cs = CloneAndProjectCrossSection(collisionCrossSection, collisionProjections[i]);
				collisionCrossSections.push_back(cs);
			}
			for(size_t i = 0, c = undergroundProjections.size(); i < c; ++i)
			{
				modelkit::CrossSectionSP cs = CloneAndProjectCrossSection(undergroundCrossSection, undergroundProjections[i]);
				undergroundCrossSections.push_back(cs);
			}
		}

		scene.matrix.PushTranslate(line_origin);

		sfa::Matrix invertMatrix = scene.matrix;
		invertMatrix.invert();

		sfa::LineString localLine = *line;
		for(int i = 0, c = localLine.getNumPoints(); i < c; ++i)
		{
			sfa::Point *point = localLine.getPointN(i);
			point->transform(invertMatrix);
		}
		
		// construct a linestring for each attachment point projected into the world.
		BOOST_FOREACH(modelkit::AttachmentPoint *attach, crossSection->getAttachmentPoints())
		{
			sfa::LineString* line2 = new sfa::LineString();
			attachmentStrings[attach->getName()] = line2;
		}

		for(size_t i = 0, c = projections.size(); i < c; ++i)
		{
			const sfa::Projection2D projection = projections.at(i);
			bool extrude = (i > 0);
			modelkit::CrossSectionSP prevCrossSection = extrude ? crossSections.back() : modelkit::CrossSectionSP();
			sfa::Point origin = extrude ? projections[i - 1].getOrigin() : sfa::Point();
			sfa::Point normal = extrude ? *localLine.getPointN((int)i) - *localLine.getPointN((int)i - 1) : sfa::Point();
			normal.normalize();
			modelkit::CrossSectionSP cs_instance = CloneAndProjectCrossSection(crossSection, projection, prevCrossSection, origin, normal);
			scenedata->projections.push_back(projection);
			crossSections.push_back(cs_instance);
			// Add the 3d attachment point to the attachment strings
			BOOST_FOREACH(modelkit::AttachmentPoint *attach, cs_instance->getAttachmentPoints())
			{
				if(attachmentStrings.find(attach->getName())!=attachmentStrings.end())
				{
					sfa::LineString *line2 = attachmentStrings[attach->getName()];
					line2->addPoint(sfa::Point(attach->getPoint()));
				}
			}
		}		

		// At this point we have all the 3d cross-sections in their correct location.
		// Now just build end faces for the first and last cross-section and all the
		// edges in between

		size_t num_crosssections = crossSections.size();
		if(num_crosssections==0)
		{
			delete line;
			return false;
		}

		if(roadwayGroup)
			roadwayGroup->sectionFaceIndices.resize(num_crosssections-1);

		int collision_index = 0;
		geometryGroup->sectionFaceIndices.resize(num_crosssections-1);
		size_t num_edges = crossSection->getEdges().size();
		std::vector<double> edge_lengths1;
		std::vector<double> edge_lengths2;
		{
			for(size_t edge_no=0;edge_no<num_edges;edge_no++)
			{
				edge_lengths1.push_back(0);
				edge_lengths2.push_back(0);
			}
		}
		for(size_t i=0;i<(num_crosssections-1);i++)
		{
			if(parms.endsOnly && (parms.endsSegment == i))
				continue;

			//_snprintf_s(groupname,1024,_TRUNCATE,"section_%d",i);
			modelkit::CrossSectionSP cs1 = crossSections.at(i);
			modelkit::CrossSectionSP cs2 = crossSections.at(i+1);

			scenegraph::Scene *collisionComponent = NULL;
			scenegraph::Scene *shadowVolumeComponent = NULL;
			if(collisionGroup)
			{
				// the collision group is constructed by connecting the faces rather than the edges
				// for each face, we extract the face triangle segments and connect them
				// then we attach the faces on each end to create our closed component
				std::vector<modelkit::Face *> faces1 = crossSections.at(i)->getFaces();
				std::vector<modelkit::Face *> faces2 = crossSections.at(i + 1)->getFaces();
				if((i == 0) || (i == num_crosssections - 2))
				{
					faces1 = cs1->getFaces();
					faces2 = cs2->getFaces();
				}
				for(int j = 0; j < faces1.size(); ++j)
				{
					modelkit::Face *f1 = faces1.at(j);
					modelkit::Face *f2 = faces2.at(j);

					std::stringstream ss;
					ss << "c" << collision_index;
					++collision_index;
					collisionComponent = new scenegraph::Scene;
					collisionComponent->fltobject = "Object";
					collisionComponent->name = ss.str();
					collisionComponent->attributes.setAttribute("collision", true);
					collisionComponent->sharpEdges = true;
					SceneData *collisionData = new SceneData;
					collisionComponent->userData = collisionData;
					collisionData->crossSection = collisionCrossSection;
					collisionData->projections.push_back(collisionProjections.at((int)i * 2));
					collisionData->projections.push_back(collisionProjections.at(((int)i * 2) + 1));

					shadowVolumeComponent = new scenegraph::Scene;
					shadowVolumeComponent->fltobject = "Object";
					shadowVolumeComponent->name = ss.str();
					shadowVolumeComponent->attributes.setAttribute("vbs2shadow", true);
					shadowVolumeComponent->sharpEdges = true;

					// add our end faces
					{
						scenegraph::Face face1;
						scenegraph::Face face2;
						face1.smc = f1->getSMC();
						face1.featureID = f1->getFID();
						face2.smc = f2->getSMC();
						face2.featureID = f2->getFID();
						face1.getAttributes().setAttribute(f1->getName(), true);
						face2.getAttributes().setAttribute(f2->getName(), true);
						for(int k = 0, kc = f1->getNumPoints(); k < kc; ++k)
						{
							face1.addVert(f1->getPoints()[k]);
							face2.addVert(f2->getPoints()[kc - k - 1]);
						}

						collisionComponent->faces.push_back(face1);
						collisionComponent->faces.push_back(face2);

						std::reverse(face1.verts.begin(), face1.verts.end());
						std::reverse(face2.verts.begin(), face2.verts.end());
						shadowVolumeComponent->faces.push_back(face1);
						shadowVolumeComponent->faces.push_back(face2);
					}

					// add connecting faces
					for(int k = 0, kc = f1->getNumPoints(); k < kc; ++k)
					{
						int l = (k < kc - 1) ? k + 1 : 0;
						sfa::Point a1 = f1->getPoints()[k];
						sfa::Point a2 = f1->getPoints()[l];
						sfa::Point b1 = f2->getPoints()[k];
						sfa::Point b2 = f2->getPoints()[l];

						scenegraph::Face face1;
						scenegraph::Face face2;
						face1.smc = f1->getSMC();
						face1.featureID = f1->getFID();
						face2.smc = f2->getSMC();
						face2.featureID = f2->getFID();
						face1.getAttributes().setAttribute(f1->getName(), true);
						face2.getAttributes().setAttribute(f2->getName(), true);
						face1.addVert(b2);
						face1.addVert(a2);
						face1.addVert(a1);
						face2.addVert(a1);
						face2.addVert(b1);
						face2.addVert(b2);

						collisionComponent->faces.push_back(face1);
						collisionComponent->faces.push_back(face2);

						std::reverse(face1.verts.begin(), face1.verts.end());
						std::reverse(face2.verts.begin(), face2.verts.end());
						shadowVolumeComponent->faces.push_back(face1);
						shadowVolumeComponent->faces.push_back(face2);
					}

					collisionGroup->children.push_back(collisionComponent);
					shadowVolumeGroup->children.push_back(shadowVolumeComponent);
				}
			}

			scenegraph::Scene *undergroundComponent = NULL;
			if(undergroundGroup)
			{
				std::stringstream ss;
				ss << "c" << i;
				undergroundComponent = new scenegraph::Scene;
				undergroundComponent->fltobject = "Object";
				undergroundComponent->name = ss.str();
				undergroundComponent->sectionFaceIndices.resize(num_crosssections-1);
				undergroundComponent->attributes.setAttribute("vbs2underground", true);
			}

			size_t num_edges = crossSection->getEdges().size();
			for(size_t edge_no=0;edge_no<num_edges;edge_no++)
			{
				modelkit::Edge *e1 = cs1->getEdges()[edge_no];
				modelkit::Edge *e2 = cs2->getEdges()[edge_no];
				scenegraph::Face f1;
				scenegraph::Face f2;

				//Propagate edge attributes to the faces
				f1.setAttributes(e1->getAttributes());
				f2.setAttributes(e1->getAttributes());
				f1.getAttributes().insertAttributes(e1->getFlags());
				f2.getAttributes().insertAttributes(e1->getFlags());

				f1.smc = e1->getSMC();
				f1.featureID = e1->getFID();
				f2.smc = e1->getSMC();
				f2.featureID = e1->getFID();

				f1.getAttributes().setAttribute(e1->getName(),true);
				f2.getAttributes().setAttribute(e2->getName(),true);
				if(e1->getFlags().getAttributeAsBool("bothsides"))
				{
					f1.drawBothSides = true;
				}
				if(e1->getFlags().getAttributeAsBool("bothsides"))
				{
					f2.drawBothSides = true;
				}

				scenegraph::Color primaryColor = e1->getPrimaryColor();
				if(primaryColor.isInitialized())
				{
					f1.primaryColor = primaryColor;
					f2.primaryColor = primaryColor;
				}

				sfa::Point normal = normals.at(i);
				sfa::Point line2 = lines.at(i);
				double len = line_lengths.at(i);
				SetupFacesFromEdges(f1,f2,e1,e2,edge_lengths1[edge_no],edge_lengths2[edge_no],normal,line2);
				geometryGroup->sectionFaceIndices[i].push_back((int)geometryGroup->faces.size());
				geometryGroup->faces.push_back(f1);
				geometryGroup->sectionFaceIndices[i].push_back((int)geometryGroup->faces.size());
				geometryGroup->faces.push_back(f2);

				// our faces in vbs2 should not have textures, so make copies and clear them
				scenegraph::Face vbs_f1 = f1;
				vbs_f1.textures.clear();
				vbs_f1.materials.clear();
				scenegraph::Face vbs_f2 = f2;
				vbs_f2.textures.clear();
				vbs_f2.materials.clear();

				if(undergroundComponent)
				{
					// if we are a tunnel interior, we are building an underground block
					// it needs to face outwards, so we reverse the faces prior to insertion
					modelkit::Edge *underground_e1 = undergroundCrossSections[i * 2]->getEdges()[edge_no];
					scenegraph::Face underground_f1 = vbs_f1;
					underground_f1.verts.clear();

					modelkit::Edge *underground_e2 = undergroundCrossSections[(i * 2) + 1]->getEdges()[edge_no];
					scenegraph::Face underground_f2 = vbs_f2;
					underground_f2.verts.clear();

					SetupFacesFromEdges(underground_f1, underground_f2, underground_e1, underground_e2, undergroundLineLengths1[i * 2], undergroundLineLengths2[i * 2], undergroundNormals[i * 2], undergroundLines[i * 2]);
					underground_f1.textures.clear();
					underground_f2.textures.clear();

					if(underground_f1.getAttributes().hasAttribute("tunnelInterior") && (underground_f1.getAttributes().getAttributeAsBool("tunnelInterior")))
					{
						std::reverse(underground_f1.verts.begin(), underground_f1.verts.end());
						std::reverse(underground_f1.vertexNormals.begin(), underground_f1.vertexNormals.end());
						undergroundComponent->sectionFaceIndices[i].push_back((int)undergroundComponent->faces.size());
						undergroundComponent->faces.push_back(underground_f1);
					}
					if(underground_f2.getAttributes().hasAttribute("tunnelInterior") && (underground_f2.getAttributes().getAttributeAsBool("tunnelInterior")))
					{
						std::reverse(underground_f2.verts.begin(), underground_f2.verts.end());
						std::reverse(underground_f2.vertexNormals.begin(), underground_f2.vertexNormals.end());
						undergroundComponent->sectionFaceIndices[i].push_back((int)undergroundComponent->faces.size());
						undergroundComponent->faces.push_back(underground_f2);
					}
				}

				if(roadwayGroup)
				{
					// trafficable surfaces are the roadway group; nothing special needs to be done
					if(vbs_f1.getAttributes().hasAttribute("trafficable") && (vbs_f1.getAttributes().getAttributeAsBool("trafficable")))
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(vbs_f1);
					}
					if(vbs_f2.getAttributes().hasAttribute("trafficable") && (vbs_f2.getAttributes().getAttributeAsBool("trafficable")))
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(vbs_f2);
					}
				}

				if(e1->getFlags().getAttributeAsBool("footprint"))
				{
					if(parms.buildSFAFootprintFeatures)
					{
						sfa::LineString line3;
						sfa::Polygon *poly = new sfa::Polygon();
						line3.addPoint(e1->getPoint(0));
						line3.addPoint(e1->getPoint(1));
						line3.addPoint(e2->getPoint(1));
						line3.addPoint(e2->getPoint(0));
						line3.addPoint(e1->getPoint(0));
						poly->addRing(line3);
						footprints.push_back(poly);
						//footprints.push_back(new sfa::Polygon(f1.getPolygon()));
						//footprints.push_back(new sfa::Polygon(f2.getPolygon()));
					}
				}

			}

			if(undergroundComponent)
			{
				// the underground component must be closed, so we need to cap off the ends

				// gather the edges that make up the interior and create a hull
				std::vector<sfa::Point> cs_points;
				std::set<sfa::Point> cs_set;
				std::vector<Edge *> edges = undergroundCrossSection->getEdges();
				BOOST_FOREACH(Edge *edge, edges)
				{
					if(edge->getFlags().getAttributeAsBool("tunnelInterior", true, false))
					{
						cs_set.insert(edge->getPoint(0));
						cs_set.insert(edge->getPoint(1));
					}
				}
				BOOST_FOREACH(const sfa::Point &pt, cs_set)
				{
					cs_points.push_back(pt);
				}
				sfa::GrahamHull hull(cs_points);
				sfa::Geometry *hullgeom = hull.getHullGeometry();
				sfa::Polygon *hullpoly = dynamic_cast<sfa::Polygon *>(hullgeom);

				if(hullpoly)
				{
					std::vector<sfa::Point> entry_points;
					sfa::LineString *hullring = hullpoly->getExteriorRing();		
					if(hullring)
					{
						int num_entry_points = hullring->getNumPoints();
						for(int i = 0; i < num_entry_points;i++)
							entry_points.push_back(*hullring->getPointN(i));
					}
					sfa::LineString entry0 = *hullring;		// entry face
					sfa::LineString entry1 = *hullring;		// exit face
					entry1.reverse();	// we have to reverse the exit face in order to have it point outward

					sfa::LineString *result0 = undergroundProjections[i * 2].transformLineTo3D(&entry0);
					sfa::LineString *result1 = undergroundProjections[(i * 2) + 1].transformLineTo3D(&entry1);

					scenegraph::Face face0;
					for(int i = 0, c = result0->getNumPoints(); i < c; ++i)
					{
						face0.verts.push_back(*result0->getPointN(i));
						face0.vertexNormals.push_back(*projections[i].getNormal());
					}
					scenegraph::Face face1;
					for(int i = 0, c = result1->getNumPoints(); i < c; ++i)
					{
						face1.verts.push_back(*result1->getPointN(i));
						face1.vertexNormals.push_back(*projections[i + 1].getNormal());
					}

					delete result0;
					delete result1;

					undergroundComponent->faces.push_back(face0);
					undergroundComponent->faces.push_back(face1);

					delete hullgeom;
				}

				undergroundGroup->children.push_back(undergroundComponent);
			}

		}

		int start = (int)geometryGroup->faces.size();
		AddEndFaces(crossSections.at(0),false,geometryGroup->faces);
		for (int stop = (int)geometryGroup->faces.size(); start < stop; start++)
			geometryGroup->sectionFaceIndices.front().push_back(start);
		AddEndFaces(crossSections.at(num_crosssections-1),true,geometryGroup->faces);
		for (int stop = (int)geometryGroup->faces.size(); start < stop; start++)
			geometryGroup->sectionFaceIndices.back().push_back(start);


		BOOST_FOREACH(attach_string_t::value_type &attach, attachmentStrings)
		{
			// The name: attach.first (std::string)
			// The value: attach.second (sfa::LineStringSP)
			scenegraph::LineString ls;
			ls.name = attach.first;
			ls.lineString = sfa::LineString(*attach.second);
			scene.lineStrings.push_back(ls);
			delete attach.second;
		}
		//start portal
		if(parms.buildSFAPortalFeatures)
		{
			sfa::Projection2D entryPortal(projections.at(0));
			sfa::Point xvector = *entryPortal.getBasisVector(0);
			double radians = boost::math::constants::pi<double>() * -parms.portalAngle/180.0;
			sfa::Quat rot(radians,xvector);
			entryPortal.applyRotation(rot);
			entryPortal.scale2D(1.0f,1/cos(radians));

			std::vector<modelkit::AvoidArea *> avoids = crossSection->getAvoidAreas();
			BOOST_FOREACH(modelkit::AvoidArea *avoid,avoids)
			{
				sfa::Polygon portal;
				sfa::LineString ring;
				int numPoints = avoid->getNumPoints();
				for(int i=0;i<numPoints;i++)
				{
					sfa::Point pt;
					avoid->getPoint(i,pt);
					ring.addPoint(*entryPortal.transformPointTo3D(&pt));
				}
				portal.addRing(ring);
				scene.terrainHoles.push_back(portal);
			}
		}
		//end portal
		if(parms.buildSFAPortalFeatures)
		{
			sfa::Projection2D entryPortal(projections.at(projections.size()-1));
			sfa::Point xvector = *entryPortal.getBasisVector(0);
			double radians = boost::math::constants::pi<double>() * parms.portalAngle/180.0;
			sfa::Quat rot(radians,xvector);
			entryPortal.applyRotation(rot);
			entryPortal.scale2D(1.0f,1/cos(radians));

			std::vector<modelkit::AvoidArea *> avoids = crossSection->getAvoidAreas();
			BOOST_FOREACH(modelkit::AvoidArea *avoid,avoids)
			{
				sfa::Polygon portal;
				sfa::LineString ring;
				int numPoints = avoid->getNumPoints();
				for(int i=0;i<numPoints;i++)
				{
					sfa::Point pt;
					avoid->getPoint(i,pt);
					ring.addPoint(*entryPortal.transformPointTo3D(&pt));
				}
				portal.addRing(ring);
				scene.terrainHoles.push_back(portal);
			}
		}
		if(parms.buildSFAFootprintFeatures)
		{
			// consolidate the faces where possible.
			if(footprints.size())
			{
				sfa::Geometry *outer = new sfa::Polygon(*footprints.at(footprints.size()-1));
				footprints.pop_back();
				bool grouped = true;
				while(footprints.size() && grouped)
				{
					grouped = false;
					std::vector<sfa::Polygon *>::iterator iter = footprints.begin();
					while(iter!=footprints.end())
					{
						sfa::Geometry *poly = new sfa::Polygon(*iter);
						if(outer->intersects(poly))
						{
							sfa::Geometry *geom = outer->Union(poly);
							if(dynamic_cast<sfa::Polygon*>(geom))
							{
								outer = geom;
								grouped = true;
								footprints.erase(iter);
								break;
							}						
						}
						iter++;
					}
				}
				sfa::Polygon *poly = dynamic_cast<sfa::Polygon*>(outer);
				if(poly)
				{
					scene.footprints.push_back(*poly);
				}				
			}
			
			BOOST_FOREACH(sfa::Polygon *poly,footprints)
			{
				scene.footprints.push_back(*poly);
			}
		}

		// add our vbs groups if they exist
		if(collisionGroup)
		{
			scene.children.push_back(collisionGroup);
		}
		if(shadowVolumeGroup)
			scene.children.push_back(shadowVolumeGroup);
		if(undergroundGroup)
		{
			if(!undergroundGroup->children.empty())
				scene.children.push_back(undergroundGroup);
			else
				delete undergroundGroup;
		}
		if(roadwayGroup)
		{
			if(!roadwayGroup->faces.empty())
				scene.children.push_back(roadwayGroup);
			else
				delete roadwayGroup;
		}

		// the AI group needs to contain both waypoints as OpenFlight LightPoint records as well as faces; if it has no faces, the Paths LOD isn't created by SMACT
		if(pathsGroup)
		{
			scene.children.push_back(pathsGroup);

			sfa::Point *prevPoint = NULL;
			for(int i = 0, c = localLine.getNumPoints(); i < c; ++i)
			{
				sfa::Point *point = localLine.getPointN(i);

				// add the waypoints; #in for entry points, #pos for others
				scenegraph::LightPoint lightPoint;
				lightPoint.point = *point;
				lightPoint.point.setZ(lightPoint.point.Z() + 0.01f);
				if((i == 0) || (i == c - 1))
					lightPoint.attributes.setAttribute("in", true);
				else
					lightPoint.attributes.setAttribute("pos", true);
				pathsGroup->lightPoints.push_back(lightPoint);

				// add the faces; it appears from the examples that the faces are vertical
				if(prevPoint)
				{
					scenegraph::Face newFace;

					// we only use one vertex for the first point (prevPoint)
					if(i == 1)
					{
						newFace.verts.push_back(sfa::Point(prevPoint->X(), prevPoint->Y(), prevPoint->Z() + 0.02f));
					}
					else
					{
						newFace.verts.push_back(sfa::Point(prevPoint->X(), prevPoint->Y(), prevPoint->Z() + 0.02f));
						newFace.verts.push_back(sfa::Point(prevPoint->X(), prevPoint->Y(), prevPoint->Z() + 0.01f));
					}
					
					// we only use one vertex for the last point (this point) unless we only have two points
					if((i > 1) && (i <= c - 1))
					{
						newFace.verts.push_back(sfa::Point(point->X(), point->Y(), point->Z() + 0.02f));
					}
					else
					{
						newFace.verts.push_back(sfa::Point(point->X(), point->Y(), point->Z() + 0.01f));
						newFace.verts.push_back(sfa::Point(point->X(), point->Y(), point->Z() + 0.02f));
					}

					pathsGroup->faces.push_back(newFace);
				}

				prevPoint = point;
			}
		}

		log << "Placed " << crossSections.size() << " cross-sections." << log.endl;
		log << "Generated " << geometryGroup->faces.size() << " faces." << log.endl;
		log << "Generated " << attachmentStrings.size() << " attachment strings.\n" << log.endl;
		delete line;// We made a copy
		return true;
	}

	scenegraph::Face buildQuadFaceFromTriangleFaces(const scenegraph::Face &f1, const scenegraph::Face &f2)
	{
		if((f1.verts.size() != 3) || (f2.verts.size() != 3))
			return scenegraph::Face();

		scenegraph::Face quad_face;
		quad_face.attributes = f1.attributes;
		quad_face.smc = f1.smc;
		quad_face.featureID = f1.featureID;

		// combine all unmatched vertices into a single vector
		for(int i1 = 0; i1 < 3; ++i1)
			quad_face.verts.push_back(f1.verts.at(i1));
		for(size_t i2 = 0; i2 < 3; ++i2)
		{
			bool match = false;
			for(int i1 = 0; i1 < 3; ++i1)
			{
				if(f1.verts[i1] == f2.verts[i2])
					match = true;
			}
			if(!match)
				quad_face.verts.push_back(f2.verts.at(i2));
		}

		// if we don't have 4 verts, these triangles don't share an edge
		if(quad_face.verts.size() != 4)
			return scenegraph::Face();

		// we use segment intersections to determine if we have an ordering problem
		// start by setting up our segments
		sfa::LineString segment01;
		segment01.addPoint(quad_face.verts[0]);
		segment01.addPoint(quad_face.verts[1]);
		sfa::LineString segment12;
		segment12.addPoint(quad_face.verts[1]);
		segment12.addPoint(quad_face.verts[2]);
		sfa::LineString segment23;
		segment23.addPoint(quad_face.verts[2]);
		segment23.addPoint(quad_face.verts[3]);
		sfa::LineString segment30;
		segment30.addPoint(quad_face.verts[3]);
		segment30.addPoint(quad_face.verts[0]);

		bool fixed = false;		// optimization to prevent testing intersections if the first case already fixed it

		// test for intersection between 0-1 and 2-3 (0-1 is connected edge)
		if(segment01.intersects(&segment23))
		{
			// reorder to 0-1-3-2 (intermediate)
			std::swap(quad_face.verts[2], quad_face.verts[3]);
			// reorder to 0-3-1-2 (goal)
			std::swap(quad_face.verts[1], quad_face.verts[2]);
			fixed = true;
		}

		// test for intersection between 1-2 and 3-0 (1-2 is connected edge)
		if(!fixed && segment12.intersects(&segment30))
		{
			// reorder to 0-1-3-2
			std::swap(quad_face.verts[2], quad_face.verts[3]);
		}

		// otherwise, connected edge is 2-0 and 3 is in the right place

		return quad_face;
	}

	bool ModelGenerator::BuildModelForLinearBridge(const sfa::LineString *origline,const modelkit::CrossSectionSP crossSection, scenegraph::Scene &scene, ModelGeneratorParameters parms)
	{
		sfa::PolygonList footprints;
		this->crossSection = crossSection;	// need this for texture paths

		scenegraph::Scene *geometryGroup = new scenegraph::Scene();
		geometryGroup->name = "visual";
		scene.addChild(geometryGroup);
		scenegraph::Scene *collisionGroup = NULL;
		scenegraph::Scene *shadowVolumeGroup = NULL;
		scenegraph::Scene *undergroundGroup = NULL;
		scenegraph::Scene *roadwayGroup = NULL;
		if(parms.vbs2Mode)
		{
			collisionGroup = new scenegraph::Scene;
			collisionGroup->name = "Collision";
			collisionGroup->attributes.setAttribute("#collision", true);
			shadowVolumeGroup = new scenegraph::Scene;
			shadowVolumeGroup->name = "Shadow";
			shadowVolumeGroup->attributes.setAttribute("#vbs2shadow", true);
			/*
			undergroundGroup = new scenegraph::Scene;
			undergroundGroup->name = "vbs2underground";
			undergroundGroup->attributes.setAttribute("#vbs2underground", true);
			*/
			roadwayGroup = new scenegraph::Scene;
			roadwayGroup->name = "Roadway";
			roadwayGroup->attributes.setAttribute("#vbs2roadway", true);
		}

		attach_string_t attachmentStrings;
		std::vector<sfa::Projection2D> projections;
		std::vector<modelkit::CrossSectionSP> crossSections;
		std::vector<sfa::Point> normals;
		std::vector<sfa::Point> lines;
		std::vector<double> line_lengths;
		double prevOffset = dictionary.getPreviousOffset(parms.featureAttributes);
		double nextOffset = dictionary.getNextOffset(parms.featureAttributes);
		double prevAngle = dictionary.getPreviousAngle(parms.featureAttributes);
		double nextAngle = dictionary.getNextAngle(parms.featureAttributes);
		// Make a copy of the line to trim
		sfa::LineString *line = new sfa::LineString(*origline); 

		// Only trim road for junctions
		if (prevAngle > 0) prevOffset = 0;
		if (nextAngle > 0) nextOffset = 0;

		// Get the origin before trimming the line
		sfa::Point line_origin = *line->getPointN(0);

		if (prevOffset > 0 || nextOffset > 0)
		{
			if(!line->trimEnds(prevOffset,nextOffset))
			{
				// We can't trim it, so return an error
				log << "Model generator unable to trim next/previous offsets for linear. Possibly due to occluded road." << log.endl;
				return false;
			}
		}
		SceneData *scenedata = NULL;
		if(!scene.userData)
		{
			scenedata = new SceneData();
			scene.userData = scenedata;
		}
		else
		{
			scenedata = dynamic_cast<SceneData *>(scene.userData);
		}
		scenedata->crossSection = crossSection;
		std::stringstream ss;

		if(!BuildProjections(projections, normals, lines, line_lengths, line,crossSection, parms, line_origin))
		{
			delete line;
			return false;
		}

		// if vbs2 mode, we need to create some alternate projections
		// underground projections contain overlap points, so they are doubled up
		std::vector<modelkit::CrossSectionSP> undergroundCrossSections;
		std::vector<sfa::Projection2D> undergroundProjections;
		std::vector<sfa::Point> undergroundNormals;
		std::vector<sfa::Point> undergroundLines;
		std::vector<double> undergroundLineLengths1;
		std::vector<double> undergroundLineLengths2;
		modelkit::CrossSectionSP undergroundCrossSection;
		std::vector<modelkit::CrossSectionSP> collisionCrossSections;
		std::vector<sfa::Projection2D> collisionProjections;
		std::vector<sfa::Point> collisionNormals;
		std::vector<sfa::Point> collisionLines;
		std::vector<double> collisionLineLengths1;
		std::vector<double> collisionLineLengths2;
		modelkit::CrossSectionSP collisionCrossSection;
		if(parms.vbs2Mode)
		{
			collisionCrossSection = modelkit::CrossSectionSP(new modelkit::CrossSection(*crossSection));

			// make a copy of the cross section so we can expand it away from the linear
			undergroundCrossSection = modelkit::CrossSectionSP(new modelkit::CrossSection(*crossSection));
			undergroundCrossSection->translate(0.0f, -0.01f);	// get the origin off the surface so we can expand the surface downward
			undergroundCrossSection->expand(0.5f, 0.5f);	// expand moves all points x/y meters from origin

			// for underground volumes, we need to extend each segment by a short distance to introduce overlap
			for(int i = 0, c = line->getNumPoints(); i < c - 1; ++i)
			{
				sfa::Point offset = normals[i];		// normal is opposite the segment direction
				offset *= undergroundCrossSection->getWidth() / 2;		// we need enough overlap to ensure we cover the tunnel interior in a turn; half the cross section width will allow a 45 degree turn
				sfa::Point pointA = *line->getPointN(i);
				if(i > 0)
					pointA = sfa::Point(pointA.X() + offset.X(), pointA.Y() + offset.Y(), pointA.Z() + offset.Z());
				sfa::Point pointB = *line->getPointN(i + 1);
				if(i < c - 2)
					pointB = sfa::Point(pointB.X() - offset.X(), pointB.Y() - offset.Y(), pointB.Z() - offset.Z());
				sfa::LineString segment;
				segment.addPoint(pointA);
				segment.addPoint(pointB);
				{
					std::vector<sfa::Projection2D> segmentProjections;
					std::vector<sfa::Point> segmentNormals;
					std::vector<sfa::Point> segmentLines;
					std::vector<double> segmentLineLengths;
					if(BuildProjections(segmentProjections, segmentNormals, segmentLines, segmentLineLengths, &segment, undergroundCrossSection, parms, line_origin))
					{
						undergroundProjections.push_back(segmentProjections[0]);
						undergroundProjections.push_back(segmentProjections[1]);
						undergroundNormals.push_back(segmentNormals[0]);
						undergroundNormals.push_back(segmentNormals[1]);
						undergroundLines.push_back(segmentLines[0]);
						undergroundLines.push_back(segmentLines[1]);
						undergroundLineLengths1.push_back(segmentLineLengths[0]);
						undergroundLineLengths1.push_back(segmentLineLengths[1]);
						undergroundLineLengths2.push_back(segmentLineLengths[0]);
						undergroundLineLengths2.push_back(segmentLineLengths[1]);
					}
				}
				{
					if(i == 0)
						*segment.getPointN(0) = *line->getPointN(i);
					if(i == c - 2)
						*segment.getPointN(1) = *line->getPointN(i + 1);

					std::vector<sfa::Projection2D> segmentProjections;
					std::vector<sfa::Point> segmentNormals;
					std::vector<sfa::Point> segmentLines;
					std::vector<double> segmentLineLengths;
					if(BuildProjections(segmentProjections, segmentNormals, segmentLines, segmentLineLengths, &segment, collisionCrossSection, parms, line_origin))
					{
						collisionProjections.push_back(segmentProjections[0]);
						collisionProjections.push_back(segmentProjections[1]);
						collisionNormals.push_back(segmentNormals[0]);
						collisionNormals.push_back(segmentNormals[1]);
						collisionLines.push_back(segmentLines[0]);
						collisionLines.push_back(segmentLines[1]);
						collisionLineLengths1.push_back(segmentLineLengths[0]);
						collisionLineLengths1.push_back(segmentLineLengths[1]);
						collisionLineLengths2.push_back(segmentLineLengths[0]);
						collisionLineLengths2.push_back(segmentLineLengths[1]);
					}
				}
			}
			for(size_t i = 0, c = collisionProjections.size(); i < c; ++i)
			{
				modelkit::CrossSectionSP cs = CloneAndProjectCrossSection(collisionCrossSection, collisionProjections[i]);
				collisionCrossSections.push_back(cs);
			}
			for(size_t i = 0, c = undergroundProjections.size(); i < c; ++i)
			{
				modelkit::CrossSectionSP cs = CloneAndProjectCrossSection(undergroundCrossSection, undergroundProjections[i]);
				undergroundCrossSections.push_back(cs);
			}
		}

		scene.matrix.PushTranslate(line_origin);

		sfa::Matrix invertMatrix = scene.matrix;
		invertMatrix.invert();

		sfa::LineString localLine = *line;
		for(int i = 0, c = localLine.getNumPoints(); i < c; ++i)
		{
			sfa::Point *point = localLine.getPointN(i);
			point->transform(invertMatrix);
		}
		
		// construct a linestring for each attachment point projected into the world.
		BOOST_FOREACH(modelkit::AttachmentPoint *attach, crossSection->getAttachmentPoints())
		{
			sfa::LineString* line2 = new sfa::LineString();
			attachmentStrings[attach->getName()] = line2;
		}

		for(size_t i = 0, c = projections.size(); i < c; ++i)
		{
			const sfa::Projection2D projection = projections.at(i);
			bool extrude = (i > 0);
			modelkit::CrossSectionSP prevCrossSection = extrude ? crossSections.back() : modelkit::CrossSectionSP();
			sfa::Point origin = extrude ? projections[i - 1].getOrigin() : sfa::Point();
			sfa::Point normal = extrude ? *localLine.getPointN((int)i) - *localLine.getPointN((int)i - 1) : sfa::Point();
			normal.normalize();
			modelkit::CrossSectionSP cs_instance = CloneAndProjectCrossSection(crossSection, projection, prevCrossSection, origin, normal);
			scenedata->projections.push_back(projection);
			crossSections.push_back(cs_instance);
			// Add the 3d attachment point to the attachment strings
			BOOST_FOREACH(modelkit::AttachmentPoint *attach, cs_instance->getAttachmentPoints())
			{
				if(attachmentStrings.find(attach->getName())!=attachmentStrings.end())
				{
					sfa::LineString *line2 = attachmentStrings[attach->getName()];
					line2->addPoint(sfa::Point(attach->getPoint()));
				}
			}
		}		

		// At this point we have all the 3d cross-sections in their correct location.
		// Now just build end faces for the first and last cross-section and all the
		// edges in between

		size_t num_crosssections = crossSections.size();
		if(num_crosssections==0)
		{
			delete line;
			return false;
		}

		if(roadwayGroup)
			roadwayGroup->sectionFaceIndices.resize(num_crosssections-1);

		int collision_index = 0;
		geometryGroup->sectionFaceIndices.resize(num_crosssections-1);
		size_t num_edges = crossSection->getEdges().size();
		std::vector<double> edge_lengths1;
		std::vector<double> edge_lengths2;
		{
			for(size_t edge_no=0;edge_no<num_edges;edge_no++)
			{
				edge_lengths1.push_back(0);
				edge_lengths2.push_back(0);
			}
		}
		for(size_t i=0;i<(num_crosssections-1);i++)
		{
			if(parms.endsOnly && (parms.endsSegment == i))
				continue;

			scenegraph::LineString geometryLineString;
			geometryLineString.lineString.addPoint(*line->getPointN((int)i));
			geometryLineString.lineString.addPoint(*line->getPointN((int)i + 1));
			geometryGroup->lineStrings.push_back(geometryLineString);

			//_snprintf_s(groupname,1024,_TRUNCATE,"section_%d",i);
			modelkit::CrossSectionSP cs1 = crossSections.at(i);
			modelkit::CrossSectionSP cs2 = crossSections.at(i+1);

			scenegraph::Scene *collisionComponent = NULL;
			scenegraph::Scene *shadowVolumeComponent = NULL;
			if(collisionGroup)
			{
				// the collision group is constructed by connecting the faces rather than the edges
				// for each face, we extract the face triangle segments and connect them
				// then we attach the faces on each end to create our closed component
				std::vector<modelkit::Face *> faces1 = crossSections.at(i)->getFaces();
				std::vector<modelkit::Face *> faces2 = crossSections.at(i + 1)->getFaces();
				if((i == 0) || (i == num_crosssections - 2))
				{
					faces1 = cs1->getFaces();
					faces2 = cs2->getFaces();
				}

				// convert to scenegraph faces
				std::vector<scenegraph::Face> scene_faces1(faces1.size());
				std::vector<scenegraph::Face> scene_faces2(faces2.size());
				for(size_t findex = 0, fcount = faces1.size(); findex < fcount; ++findex)
				{
					modelkit::Face *face1 = faces1.at(findex);
					scene_faces1[findex].verts = face1->getPoints();
					scene_faces1[findex].smc = face1->getSMC();
					scene_faces1[findex].featureID = face1->getFID();
					scene_faces1[findex].getAttributes().setAttribute(face1->getName(), true);

					modelkit::Face *face2 = faces2.at(findex);
					scene_faces2[findex].verts = face2->getPoints();
					scene_faces2[findex].smc = face2->getSMC();
					scene_faces2[findex].featureID = face2->getFID();
					scene_faces2[findex].getAttributes().setAttribute(face2->getName(), true);
				}


				/*
				These groups need to be in quads rather than triangles.
				To do this, we need to combine adjacent faces and then combine the two triangles for each edge.
				In this first part, we identify what triangles in the faces to connect into quads.
				*/

				// build dataset of face interconnections
				// note that we are only working with faces1... we will match faces2 appropriately
				std::map<size_t, std::set<int> > face_connections;		// map of face index => (list of adjacent face indices)
				for(size_t findex1 = 0, fcount = scene_faces1.size(); findex1 < fcount; ++findex1)
				{
					scenegraph::Face &face1 = scene_faces1.at(findex1);
					for(size_t findex2 = findex1 + 1; findex2 < fcount; ++findex2)
					{
						scenegraph::Face &face2 = scene_faces1.at(findex2);

						// we add the points to a set and the count tells us how many were duplicate
						std::set<sfa::Point> points;
						points.insert(face1.verts.begin(), face1.verts.end());
						points.insert(face2.verts.begin(), face2.verts.end());

						// we are only interested in connections with two vertices
						if(points.size() != 4)
							continue;

						// add the entries for both sides
						// the number of missing points is the number of duplicates
						face_connections[findex1].insert((int)findex2);
						face_connections[findex2].insert((int)findex1);
					}
				}

				// face_indices stores a list of faces that need to be processed
				std::set<int> face_indices;
				for(int a = 0; a < faces1.size(); ++a)
					face_indices.insert(a);

				// gather faces for processing
				std::vector<scenegraph::Face> combined_faces1;
				std::vector<scenegraph::Face> combined_faces2;
				// face_indices will be empty when we are done processing the faces
				size_t prev_size = face_indices.size();
				while(!face_indices.empty())
				{
					// look for the first face that can be processed
					for(std::set<int>::iterator findex_it = face_indices.begin(), findex_end = face_indices.end(); findex_it != findex_end; ++findex_it)
					{
						int findex = *findex_it;

						// if no connections, we keep this face as it is
						if(face_connections[findex].empty())
						{
							// copy the existing faces
							combined_faces1.push_back(scene_faces1.at(findex));
							combined_faces2.push_back(scene_faces2.at(findex));

							// remove from processing list and connections
							face_indices.erase(findex_it);
							face_connections.erase(findex);

							// restart loop
							break;
						}

						// if one connection, we attach it to the adjacent
						if(face_connections[findex].size() == 1)
						{
							int cindex = *face_connections[findex].begin();

							// create combined faces
							combined_faces1.push_back(buildQuadFaceFromTriangleFaces(scene_faces1.at(findex), scene_faces1.at(cindex)));
							combined_faces2.push_back(buildQuadFaceFromTriangleFaces(scene_faces2.at(findex), scene_faces2.at(cindex)));

							// remove from processing list and connections
							face_indices.erase(findex_it);
							face_connections.erase(findex);
							face_indices.erase(face_indices.find(cindex));
							face_connections[cindex].erase(findex);
							if(!face_connections[cindex].empty())
								face_connections[*face_connections[cindex].begin()].erase(cindex);
							face_connections.erase(cindex);

							// restart loop
							break;
						}

						// if we have multiple connections, we'll leave it to and end connection to handle
					}

					// in case we have a closed fan, break into it by removing a connection
					if(face_indices.size() == prev_size)
					{
						int findex = *face_indices.begin();
						int cindex = *face_connections[findex].begin();
						face_connections[findex].erase(cindex);
						face_connections[cindex].erase(findex);
					}
				}

				for(int j = 0; j < combined_faces1.size(); ++j)
				{
					scenegraph::Face &f1 = combined_faces1.at(j);
					scenegraph::Face &f2 = combined_faces2.at(j);

					std::stringstream ss;
					ss << "c" << collision_index;
					++collision_index;
					collisionComponent = new scenegraph::Scene;
					collisionComponent->fltobject = "Object";
					collisionComponent->name = ss.str();
					collisionComponent->attributes.setAttribute("collision", true);
					collisionComponent->sharpEdges = true;
					scenegraph::LineString componentLineString;
					componentLineString.lineString.addPoint(*line->getPointN((int)i));
					componentLineString.lineString.addPoint(*line->getPointN((int)i + 1));
					collisionComponent->lineStrings.push_back(componentLineString);
					SceneData *collisionData = new SceneData;
					collisionComponent->userData = collisionData;
					collisionData->crossSection = collisionCrossSection;
					collisionData->projections.push_back(collisionProjections.at((int)i * 2));
					collisionData->projections.push_back(collisionProjections.at(((int)i * 2) + 1));

					shadowVolumeComponent = new scenegraph::Scene;
					shadowVolumeComponent->fltobject = "Object";
					shadowVolumeComponent->name = ss.str();
					shadowVolumeComponent->attributes.setAttribute("vbs2shadow", true);
					shadowVolumeComponent->sharpEdges = true;

					// add our end faces
					{
						scenegraph::Face face1;
						scenegraph::Face face2;
						face1.smc = f1.smc;
						face2.smc = f2.smc;
						face1.featureID = f1.featureID;
						face2.featureID = f2.featureID;
						face1.attributes = f1.attributes;
						face2.attributes = f2.attributes;
						for(size_t k = 0, kc = f1.verts.size(); k < kc; ++k)
						{
							face1.addVert(f1.verts[k]);
							face2.addVert(f2.verts[kc - k - 1]);
						}

						collisionComponent->faces.push_back(face1);
						collisionComponent->faces.push_back(face2);

						std::reverse(face1.verts.begin(), face1.verts.end());
						std::reverse(face2.verts.begin(), face2.verts.end());
						shadowVolumeComponent->faces.push_back(face1);
						shadowVolumeComponent->faces.push_back(face2);
					}

					// add connecting faces
					for (int k = 0, kc = (int)f1.verts.size(); k < kc; ++k)
					{
						int l = (k < kc - 1) ? k + 1 : 0;
						sfa::Point a1 = f1.verts[k];
						sfa::Point a2 = f1.verts[l];
						sfa::Point b1 = f2.verts[k];
						sfa::Point b2 = f2.verts[l];

						scenegraph::Face vbs_quad;
						vbs_quad.smc = f1.smc;
						vbs_quad.featureID = f1.featureID;
						vbs_quad.attributes = f1.attributes;
						vbs_quad.addVert(a1);
						vbs_quad.addVert(b1);
						vbs_quad.addVert(b2);
						vbs_quad.addVert(a2);
						collisionComponent->faces.push_back(vbs_quad);
						std::reverse(vbs_quad.verts.begin(), vbs_quad.verts.end());
						shadowVolumeComponent->faces.push_back(vbs_quad);
					}

					collisionGroup->children.push_back(collisionComponent);
					shadowVolumeGroup->children.push_back(shadowVolumeComponent);
				}
			}

			scenegraph::Scene *undergroundComponent = NULL;
			if(undergroundGroup)
			{
				std::stringstream ss;
				ss << "c" << i;
				undergroundComponent = new scenegraph::Scene;
				undergroundComponent->fltobject = "Object";
				undergroundComponent->name = ss.str();
				undergroundComponent->sectionFaceIndices.resize(num_crosssections-1);
				undergroundComponent->attributes.setAttribute("vbs2underground", true);
			}

			size_t num_edges = crossSection->getEdges().size();
			for(size_t edge_no=0;edge_no<num_edges;edge_no++)
			{
				modelkit::Edge *e1 = cs1->getEdges()[edge_no];
				modelkit::Edge *e2 = cs2->getEdges()[edge_no];
				scenegraph::Face f1;
				scenegraph::Face f2;

				//Propagate edge attributes to the faces
				f1.setAttributes(e1->getAttributes());
				f2.setAttributes(e1->getAttributes());
				f1.getAttributes().insertAttributes(e1->getFlags());
				f2.getAttributes().insertAttributes(e1->getFlags());

				// Leidos requested no footprint attribute on bridges
				f1.getAttributes().removeAttribute("footprint");
				f2.getAttributes().removeAttribute("footprint");

				f1.smc = e1->getSMC();
				f1.featureID = e1->getFID();
				f2.smc = e1->getSMC();
				f2.featureID = e1->getFID();

				f1.getAttributes().setAttribute(e1->getName(),true);
				f2.getAttributes().setAttribute(e2->getName(),true);
				if(e1->getFlags().getAttributeAsBool("bothsides"))
				{
					f1.drawBothSides = true;
				}
				if(e1->getFlags().getAttributeAsBool("bothsides"))
				{
					f2.drawBothSides = true;
				}

				scenegraph::Color primaryColor = e1->getPrimaryColor();
				if(primaryColor.isInitialized())
				{
					f1.primaryColor = primaryColor;
					f2.primaryColor = primaryColor;
				}

				sfa::Point normal = normals.at(i);
				sfa::Point line2 = lines.at(i);
				double len = line_lengths.at(i);
				SetupFacesFromEdges(f1,f2,e1,e2,edge_lengths1[edge_no],edge_lengths2[edge_no],normal,line2);
				geometryGroup->sectionFaceIndices[i].push_back((int)geometryGroup->faces.size());
				geometryGroup->faces.push_back(f1);
				geometryGroup->sectionFaceIndices[i].push_back((int)geometryGroup->faces.size());
				geometryGroup->faces.push_back(f2);

				// our faces in vbs2 should not have textures, so make copies and clear them
				scenegraph::Face vbs_f1 = f1;
				vbs_f1.textures.clear();
				vbs_f1.materials.clear();
				scenegraph::Face vbs_f2 = f2;
				vbs_f2.textures.clear();
				vbs_f2.materials.clear();

				// set up a quad equivalent for roadbase (Leidos wants the roadbase as a quad)
				scenegraph::Face vbs_quad = buildQuadFaceFromTriangleFaces(vbs_f1, vbs_f2);

				// add roadbed extensions
				// ... if we have a roadbed extension distance... and are on one of the ends... and the face is a roadbed
				// we want to create a vector pointed away from the bridge based on the segment normal (from above)
				// if we're on the first segment, it matches our desired normal
				// if we're on the last segment, we need to reverse it
				// since we are doing a distance, we make it 2D and scale by the distance parameter
				// the height parameter will be applied during face creation
				//
				// from above (texture-side):
				//
				// start (i == 0):
				//
				//         1 <- e1 -> 0
				//         B +------+ A
				//           |      |
				//      [1]  |      |  [0]
				//           |      |
				//         C +------+ D
				//             ext
				//
				// end (i != 0):
				//
				//              ext
				//         B +------+ A
				//           |      |
				//      [1]  |      |  [0]
				//           |      |
				//         C +------+ D
				//         1 <- e2 -> 0
				//
				if((i == 0) && (parms.roadbed_extension_0_distance > 0.0f) && f1.getAttributes().hasAttribute("bridge:roadbed") && f1.getAttributes().getAttributeAsBool("bridge:roadbed"))
				{
					sfa::Point ext_vector = sfa::Point(normal.X(), normal.Y());
					ext_vector.normalize();
					ext_vector *= parms.roadbed_extension_0_distance;

					sfa::Point A = e1->getPoints()[0];
					sfa::Point B = e1->getPoints()[1];
					sfa::Point C = B + ext_vector;
					C.setZ(B.Z() - parms.roadbed_extension_0_height);
					sfa::Point D = A + ext_vector;
					D.setZ(A.Z() - parms.roadbed_extension_0_height);

					// we're going to build our own face, but we can start with the vbs_quad since we already have it
					scenegraph::Face ext_quad = vbs_quad;
					ext_quad.verts.clear();
					ext_quad.verts.resize(4);
					ext_quad.verts[0] = A;
					ext_quad.verts[1] = B;
					ext_quad.verts[2] = C;
					ext_quad.verts[3] = D;

					// add the top quad to the roadway
					if(roadwayGroup)
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(ext_quad);
					}

					// build a box for the geometry and collision groups
					double height = 0.5f;
					sfa::Point Az = sfa::Point(A.X(), A.Y(), A.Z() - height);
					sfa::Point Bz = sfa::Point(B.X(), B.Y(), B.Z() - height);
					sfa::Point Cz = sfa::Point(C.X(), C.Y(), C.Z() - height);
					sfa::Point Dz = sfa::Point(D.X(), D.Y(), D.Z() - height);

					std::vector<scenegraph::Face> quads;

					// top matches the ext_quad
					quads.push_back(ext_quad);

					// bottom
					ext_quad.verts[0] = Dz;
					ext_quad.verts[1] = Cz;
					ext_quad.verts[2] = Bz;
					ext_quad.verts[3] = Az;
					quads.push_back(ext_quad);

					// left
					ext_quad.verts[0] = Az;
					ext_quad.verts[1] = A;
					ext_quad.verts[2] = D;
					ext_quad.verts[3] = Dz;
					quads.push_back(ext_quad);

					// right
					ext_quad.verts[0] = B;
					ext_quad.verts[1] = Bz;
					ext_quad.verts[2] = Cz;
					ext_quad.verts[3] = C;
					quads.push_back(ext_quad);

					// near
					ext_quad.verts[0] = Az;
					ext_quad.verts[1] = Bz;
					ext_quad.verts[2] = B;
					ext_quad.verts[3] = A;
					quads.push_back(ext_quad);

					// far
					ext_quad.verts[0] = D;
					ext_quad.verts[1] = C;
					ext_quad.verts[2] = Cz;
					ext_quad.verts[3] = Dz;
					quads.push_back(ext_quad);

					for(size_t qi = 0, qc = quads.size(); qi < qc; ++qi)
					{
						scenegraph::Face &quad = quads[qi];

						// add it to the collision component
						if(collisionComponent)
							collisionComponent->faces.push_back(quad);

						// set the textures/materials and add it to the geometry group
						quad.textures = f2.textures;
						quad.materials = f2.materials;
						geometryGroup->sectionFaceIndices[i].push_back((int)geometryGroup->faces.size());
						geometryGroup->faces.push_back(quad);
					}
				}
				if((i == num_crosssections - 2) && (parms.roadbed_extension_N_distance > 0.0f) && f1.getAttributes().hasAttribute("bridge:roadbed") && f1.getAttributes().getAttributeAsBool("bridge:roadbed"))
				{
					sfa::Point ext_vector = sfa::Point(normal.X(), normal.Y());
					ext_vector.normalize();
					ext_vector *= -1;
					ext_vector *= parms.roadbed_extension_N_distance;

					sfa::Point C = e2->getPoints()[1];
					sfa::Point D = e2->getPoints()[0];
					sfa::Point A = D + ext_vector;
					A.setZ(D.Z() - parms.roadbed_extension_N_height);
					sfa::Point B = C + ext_vector;
					B.setZ(C.Z() - parms.roadbed_extension_N_height);

					// we're going to build our own face, but we can start with the vbs_quad since we already have it
					scenegraph::Face ext_quad = vbs_quad;
					ext_quad.verts.clear();
					ext_quad.verts.resize(4);
					ext_quad.verts[0] = A;
					ext_quad.verts[1] = B;
					ext_quad.verts[2] = C;
					ext_quad.verts[3] = D;

					// add the top quad to the roadway
					if(roadwayGroup)
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(ext_quad);
					}

					// build a box for the geometry and collision groups
					double height = 0.5f;
					sfa::Point Az = sfa::Point(A.X(), A.Y(), A.Z() - height);
					sfa::Point Bz = sfa::Point(B.X(), B.Y(), B.Z() - height);
					sfa::Point Cz = sfa::Point(C.X(), C.Y(), C.Z() - height);
					sfa::Point Dz = sfa::Point(D.X(), D.Y(), D.Z() - height);

					std::vector<scenegraph::Face> quads;

					// top matches the ext_quad
					quads.push_back(ext_quad);

					// bottom
					ext_quad.verts[0] = Dz;
					ext_quad.verts[1] = Cz;
					ext_quad.verts[2] = Bz;
					ext_quad.verts[3] = Az;
					quads.push_back(ext_quad);

					// left
					ext_quad.verts[0] = Az;
					ext_quad.verts[1] = A;
					ext_quad.verts[2] = D;
					ext_quad.verts[3] = Dz;
					quads.push_back(ext_quad);

					// right
					ext_quad.verts[0] = B;
					ext_quad.verts[1] = Bz;
					ext_quad.verts[2] = Cz;
					ext_quad.verts[3] = C;
					quads.push_back(ext_quad);

					// near
					ext_quad.verts[0] = Az;
					ext_quad.verts[1] = Bz;
					ext_quad.verts[2] = B;
					ext_quad.verts[3] = A;
					quads.push_back(ext_quad);

					// far
					ext_quad.verts[0] = D;
					ext_quad.verts[1] = C;
					ext_quad.verts[2] = Cz;
					ext_quad.verts[3] = Dz;
					quads.push_back(ext_quad);

					for(size_t qi = 0, qc = quads.size(); qi < qc; ++qi)
					{
						scenegraph::Face &quad = quads[qi];

						// add it to the collision component
						if(collisionComponent)
							collisionComponent->faces.push_back(quad);

						// set the textures/materials and add it to the geometry group
						quad.textures = f2.textures;
						quad.materials = f2.materials;
						geometryGroup->sectionFaceIndices[i].push_back((int)geometryGroup->faces.size());
						geometryGroup->faces.push_back(quad);
					}
				}

				if(undergroundComponent)
				{
					// if we are a tunnel interior, we are building an underground block
					// it needs to face outwards, so we reverse the faces prior to insertion
					modelkit::Edge *underground_e1 = undergroundCrossSections[i * 2]->getEdges()[edge_no];
					scenegraph::Face underground_f1 = vbs_f1;
					underground_f1.verts.clear();

					modelkit::Edge *underground_e2 = undergroundCrossSections[(i * 2) + 1]->getEdges()[edge_no];
					scenegraph::Face underground_f2 = vbs_f2;
					underground_f2.verts.clear();

					SetupFacesFromEdges(underground_f1, underground_f2, underground_e1, underground_e2, undergroundLineLengths1[i * 2], undergroundLineLengths2[i * 2], undergroundNormals[i * 2], undergroundLines[i * 2]);
					underground_f1.textures.clear();
					underground_f2.textures.clear();

					if(underground_f1.getAttributes().hasAttribute("tunnelInterior") && (underground_f1.getAttributes().getAttributeAsBool("tunnelInterior")))
					{
						std::reverse(underground_f1.verts.begin(), underground_f1.verts.end());
						std::reverse(underground_f1.vertexNormals.begin(), underground_f1.vertexNormals.end());
						undergroundComponent->sectionFaceIndices[i].push_back((int)undergroundComponent->faces.size());
						undergroundComponent->faces.push_back(underground_f1);
					}
					if(underground_f2.getAttributes().hasAttribute("tunnelInterior") && (underground_f2.getAttributes().getAttributeAsBool("tunnelInterior")))
					{
						std::reverse(underground_f2.verts.begin(), underground_f2.verts.end());
						std::reverse(underground_f2.vertexNormals.begin(), underground_f2.vertexNormals.end());
						undergroundComponent->sectionFaceIndices[i].push_back((int)undergroundComponent->faces.size());
						undergroundComponent->faces.push_back(underground_f2);
					}
				}

				if(roadwayGroup)
				{
					// trafficable (or bridge:roadbed) surfaces are the roadway group; nothing special needs to be done
					if((vbs_quad.getAttributes().hasAttribute("trafficable") && vbs_quad.getAttributes().getAttributeAsBool("trafficable"))
					 || (vbs_quad.getAttributes().hasAttribute("bridge:roadbed") && vbs_quad.getAttributes().getAttributeAsBool("bridge:roadbed")))
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(vbs_quad);
					}
					/*
					if((vbs_f1.getAttributes().hasAttribute("trafficable") && vbs_f1.getAttributes().getAttributeAsBool("trafficable"))
					 || (vbs_f1.getAttributes().hasAttribute("bridge:roadbed") && vbs_f1.getAttributes().getAttributeAsBool("bridge:roadbed")))
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(vbs_f1);
					}
					if((vbs_f2.getAttributes().hasAttribute("trafficable") && vbs_f2.getAttributes().getAttributeAsBool("trafficable"))
					 || (vbs_f2.getAttributes().hasAttribute("bridge:roadbed") && vbs_f2.getAttributes().getAttributeAsBool("bridge:roadbed")))
					{
						roadwayGroup->sectionFaceIndices[i].push_back((int)roadwayGroup->faces.size());
						roadwayGroup->faces.push_back(vbs_f2);
					}
					*/
				}

				if(e1->getFlags().getAttributeAsBool("footprint"))
				{
					if(parms.buildSFAFootprintFeatures)
					{
						sfa::LineString line3;
						sfa::Polygon *poly = new sfa::Polygon();
						line3.addPoint(e1->getPoint(0));
						line3.addPoint(e1->getPoint(1));
						line3.addPoint(e2->getPoint(1));
						line3.addPoint(e2->getPoint(0));
						line3.addPoint(e1->getPoint(0));
						poly->addRing(line3);
						footprints.push_back(poly);
						//footprints.push_back(new sfa::Polygon(f1.getPolygon()));
						//footprints.push_back(new sfa::Polygon(f2.getPolygon()));
					}
				}

			}

			if(undergroundComponent)
			{
				// the underground component must be closed, so we need to cap off the ends

				// gather the edges that make up the interior and create a hull
				std::vector<sfa::Point> cs_points;
				std::set<sfa::Point> cs_set;
				std::vector<Edge *> edges = undergroundCrossSection->getEdges();
				BOOST_FOREACH(Edge *edge, edges)
				{
					if(edge->getFlags().getAttributeAsBool("tunnelInterior", true, false))
					{
						cs_set.insert(edge->getPoint(0));
						cs_set.insert(edge->getPoint(1));
					}
				}
				BOOST_FOREACH(const sfa::Point &pt, cs_set)
				{
					cs_points.push_back(pt);
				}
				sfa::GrahamHull hull(cs_points);
				sfa::Geometry *hullgeom = hull.getHullGeometry();
				sfa::Polygon *hullpoly = dynamic_cast<sfa::Polygon *>(hullgeom);

				if(hullpoly)
				{
					std::vector<sfa::Point> entry_points;
					sfa::LineString *hullring = hullpoly->getExteriorRing();		
					if(hullring)
					{
						int num_entry_points = hullring->getNumPoints();
						for(int i = 0; i < num_entry_points;i++)
							entry_points.push_back(*hullring->getPointN(i));
					}
					sfa::LineString entry0 = *hullring;		// entry face
					sfa::LineString entry1 = *hullring;		// exit face
					entry1.reverse();	// we have to reverse the exit face in order to have it point outward

					sfa::LineString *result0 = undergroundProjections[i * 2].transformLineTo3D(&entry0);
					sfa::LineString *result1 = undergroundProjections[(i * 2) + 1].transformLineTo3D(&entry1);

					scenegraph::Face face0;
					for(int i = 0, c = result0->getNumPoints(); i < c; ++i)
					{
						face0.verts.push_back(*result0->getPointN(i));
						face0.vertexNormals.push_back(*projections[i].getNormal());
					}
					scenegraph::Face face1;
					for(int i = 0, c = result1->getNumPoints(); i < c; ++i)
					{
						face1.verts.push_back(*result1->getPointN(i));
						face1.vertexNormals.push_back(*projections[i + 1].getNormal());
					}

					delete result0;
					delete result1;

					undergroundComponent->faces.push_back(face0);
					undergroundComponent->faces.push_back(face1);

					delete hullgeom;
				}

				undergroundGroup->children.push_back(undergroundComponent);
			}

		}

		int start = (int)geometryGroup->faces.size();
		AddEndFaces(crossSections.at(0),false,geometryGroup->faces);
		for (int stop = (int)geometryGroup->faces.size(); start < stop; start++)
			geometryGroup->sectionFaceIndices.front().push_back(start);
		AddEndFaces(crossSections.at(num_crosssections-1),true,geometryGroup->faces);
		for (int stop = (int)geometryGroup->faces.size(); start < stop; start++)
			geometryGroup->sectionFaceIndices.back().push_back(start);


		BOOST_FOREACH(attach_string_t::value_type &attach, attachmentStrings)
		{
			// The name: attach.first (std::string)
			// The value: attach.second (sfa::LineStringSP)
			scenegraph::LineString ls;
			ls.name = attach.first;
			ls.lineString = sfa::LineString(*attach.second);
			scene.lineStrings.push_back(ls);
			delete attach.second;
		}
		//start portal
		if(parms.buildSFAPortalFeatures)
		{
			sfa::Projection2D entryPortal(projections.at(0));
			sfa::Point xvector = *entryPortal.getBasisVector(0);
			double radians = boost::math::constants::pi<double>() * -parms.portalAngle/180.0;
			sfa::Quat rot(radians,xvector);
			entryPortal.applyRotation(rot);
			entryPortal.scale2D(1.0f,1/cos(radians));

			std::vector<modelkit::AvoidArea *> avoids = crossSection->getAvoidAreas();
			BOOST_FOREACH(modelkit::AvoidArea *avoid,avoids)
			{
				sfa::Polygon portal;
				sfa::LineString ring;
				int numPoints = avoid->getNumPoints();
				for(int i=0;i<numPoints;i++)
				{
					sfa::Point pt;
					avoid->getPoint(i,pt);
					ring.addPoint(*entryPortal.transformPointTo3D(&pt));
				}
				portal.addRing(ring);
				scene.terrainHoles.push_back(portal);
			}
		}
		//end portal
		if(parms.buildSFAPortalFeatures)
		{
			sfa::Projection2D entryPortal(projections.at(projections.size()-1));
			sfa::Point xvector = *entryPortal.getBasisVector(0);
			double radians = boost::math::constants::pi<double>() * parms.portalAngle/180.0;
			sfa::Quat rot(radians,xvector);
			entryPortal.applyRotation(rot);
			entryPortal.scale2D(1.0f,1/cos(radians));

			std::vector<modelkit::AvoidArea *> avoids = crossSection->getAvoidAreas();
			BOOST_FOREACH(modelkit::AvoidArea *avoid,avoids)
			{
				sfa::Polygon portal;
				sfa::LineString ring;
				int numPoints = avoid->getNumPoints();
				for(int i=0;i<numPoints;i++)
				{
					sfa::Point pt;
					avoid->getPoint(i,pt);
					ring.addPoint(*entryPortal.transformPointTo3D(&pt));
				}
				portal.addRing(ring);
				scene.terrainHoles.push_back(portal);
			}
		}
		if(parms.buildSFAFootprintFeatures)
		{
			// consolidate the faces where possible.
			if(footprints.size())
			{
				sfa::Geometry *outer = new sfa::Polygon(*footprints.at(footprints.size()-1));
				footprints.pop_back();
				bool grouped = true;
				while(footprints.size() && grouped)
				{
					grouped = false;
					std::vector<sfa::Polygon *>::iterator iter = footprints.begin();
					while(iter!=footprints.end())
					{
						sfa::Geometry *poly = new sfa::Polygon(*iter);
						if(outer->intersects(poly))
						{
							sfa::Geometry *geom = outer->Union(poly);
							if(dynamic_cast<sfa::Polygon*>(geom))
							{
								outer = geom;
								grouped = true;
								footprints.erase(iter);
								break;
							}						
						}
						iter++;
					}
				}
				sfa::Polygon *poly = dynamic_cast<sfa::Polygon*>(outer);
				if(poly)
				{
					scene.footprints.push_back(*poly);
				}				
			}
			
			BOOST_FOREACH(sfa::Polygon *poly,footprints)
			{
				scene.footprints.push_back(*poly);
			}
		}

		// add our vbs groups if they exist
		if(collisionGroup)
		{
			scene.children.push_back(collisionGroup);
		}
		if(shadowVolumeGroup)
			scene.children.push_back(shadowVolumeGroup);
		if(undergroundGroup)
		{
			if(!undergroundGroup->children.empty())
				scene.children.push_back(undergroundGroup);
			else
				delete undergroundGroup;
		}
		if(roadwayGroup)
		{
			if(!roadwayGroup->faces.empty())
				scene.children.push_back(roadwayGroup);
			else
				delete roadwayGroup;
		}

		log << "Placed " << crossSections.size() << " cross-sections." << log.endl;
		log << "Generated " << geometryGroup->faces.size() << " faces." << log.endl;
		log << "Generated " << attachmentStrings.size() << " attachment strings.\n" << log.endl;
		delete line;// We made a copy
		return true;
	}

	bool intersectRayPlane(const sfa::Point &segment_point_a, const sfa::Point &segment_point_b, const sfa::Point &plane_origin, const sfa::Point &plane_normal, sfa::Point &intersect_point)
	{
		sfa::Point ab = segment_point_b - segment_point_a; 
		sfa::Point ao = plane_origin - segment_point_a;
		double n_dot_ab = plane_normal.dot(ab);
		double n_dot_ao = plane_normal.dot(ao);
		if(abs(n_dot_ab) < 1e-6)
			return false;
		double t = n_dot_ao / n_dot_ab;
		intersect_point = segment_point_a + (ab * t);
		return true;
	}

	modelkit::CrossSectionSP ModelGenerator::CloneAndProjectCrossSection(const modelkit::CrossSectionSP crossSection, const sfa::Projection2D &proj, const modelkit::CrossSectionSP prevCrossSection, const sfa::Point &origin, const sfa::Point &normal)
	{
		modelkit::CrossSectionSP clone(new modelkit::CrossSection(*crossSection));

		std::vector<modelkit::Edge *> edges = clone->getEdges();
		for(size_t i = 0, c = edges.size(); i < c; ++i)
		{
			modelkit::Edge *edge = edges[i];
			modelkit::PointList points = edge->getPoints();
			if(prevCrossSection)
			{
				modelkit::Edge *prevEdge = prevCrossSection->getEdges().at(i);
				intersectRayPlane(prevEdge->getPoint(0), prevEdge->getPoint(0) + normal, *proj.getOrigin(), *proj.getNormal(), edge->getPoint(0));
				intersectRayPlane(prevEdge->getPoint(1), prevEdge->getPoint(1) + normal, *proj.getOrigin(), *proj.getNormal(), edge->getPoint(1));
				edge->getPoint(2) = edge->getPoint(0) + (normal * edge->getProjectedZ());
			}
			else
			{
				edge->clearPoints();
				modelkit::PointList sortedPoints = points;
				// The third point is always the same as the first, with the Z value projected outwards
				sortedPoints[2] = sfa::Point(sortedPoints[0].X(), sortedPoints[0].Y(), edge->getProjectedZ());
				for(modelkit::PointList::iterator pit = sortedPoints.begin(), pend = sortedPoints.end(); pit != pend; ++pit)
					edge->addPoint(proj.transformPointTo3D(*pit));
				edge->setNormal(proj.transformPointTo3D(edge->getNormal()));
			}
		}

		std::vector<modelkit::Face *> faces = clone->getFaces();
		for(size_t i = 0, c = faces.size(); i < c; ++i)
		{
			modelkit::Face *face = faces[i];
			modelkit::PointList points = face->getPoints();
			if(prevCrossSection)
			{
				modelkit::Face *prevFace = prevCrossSection->getFaces().at(i);
				for(size_t j = 0, jc = prevFace->getNumPoints(); j < jc; ++j)
					intersectRayPlane(prevFace->getPoint((int)j), prevFace->getPoint((int)j) + normal, *proj.getOrigin(), *proj.getNormal(), face->getPoint((int)j));
			}
			else
			{
				face->clearPoints();
				for(modelkit::PointList::iterator pit = points.begin(), pend = points.end(); pit != pend; ++pit)
					face->addPoint(proj.transformPointTo3D(*pit));
			}
		}

		std::vector<modelkit::AttachmentPoint *> attachmentPoints = clone->getAttachmentPoints();
		for(size_t i = 0, c = attachmentPoints.size(); i < c; ++i)
		{
			modelkit::AttachmentPoint *attachmentPoint = attachmentPoints[i];
			if(prevCrossSection)
			{
				modelkit::AttachmentPoint *prevAttachmentPoint = prevCrossSection->getAttachmentPoints().at(i);
				sfa::Point tempPoint;
				intersectRayPlane(prevAttachmentPoint->getPoint(), prevAttachmentPoint->getPoint() + normal, *proj.getOrigin(), *proj.getNormal(), tempPoint);
				attachmentPoint->setPoint(tempPoint);
			}
			else
			{
				attachmentPoint->setPoint(proj.transformPointTo3D(attachmentPoint->getPoint()));
			}
		}

		return clone;
	}

	scenegraph::MappedTexture ModelGenerator::ConvertToMappedTexture(modelkit::TextureReference *tr, bool reverseOrder)
	{
		scenegraph::MappedTexture ret;

		ret.SetTextureName(crossSection->getReferenceSource(tr->getTexture()));
		if(!reverseOrder)
		{
			BOOST_FOREACH(sfa::Point uv, tr->getUVs())
			{
				ret.uvs.push_back(uv);
			}
		}
		else
		{
			BOOST_REVERSE_FOREACH(sfa::Point uv, tr->getUVs())
			{
				ret.uvs.push_back(uv);
			}
		}

		return ret;
	}

    scenegraph::Material ModelGenerator::ConvertToScenegraphMaterial(modelkit::Material *m)
    {
        scenegraph::Material sceneMat;

        if (m)
        {
            sceneMat.ambient = scenegraph::Color( m->getAmbient().getR(), m->getAmbient().getG(), m->getAmbient().getB(), m->getAmbient().getA() );
            sceneMat.diffuse = scenegraph::Color( m->getDiffuse().getR(), m->getDiffuse().getG(), m->getDiffuse().getB(), m->getDiffuse().getA() );
            sceneMat.specular = scenegraph::Color( m->getSpecular().getR(), m->getSpecular().getG(), m->getSpecular().getB(), m->getSpecular().getA() );        
            sceneMat.emission = scenegraph::Color( m->getEmission().getR(), m->getEmission().getG(), m->getEmission().getB(), m->getEmission().getA() );               
            sceneMat.shine = m->getShine();
            sceneMat.transparency = m->getTransparency();
        }

        return sceneMat;
    }

	bool ModelGenerator::SetupTexturesFromCrossSection(scenegraph::Face &face, modelkit::Edge *fromEdge, bool reverseOrder)
	{
		BOOST_FOREACH(modelkit::TextureReference *tr, fromEdge->getTextureReferences())
		{
			scenegraph::MappedTexture mt = ConvertToMappedTexture(tr, reverseOrder);
			face.textures.push_back(mt);
		}

		return true;
	}

    bool ModelGenerator::SetupMaterialsFromCrossSection(scenegraph::Face &face, modelkit::Edge *fromEdge)
    {
	    modelkit::Material *m = fromEdge->getMaterialRef();
	    if (m)
        {
           scenegraph::Material sceneMat = ConvertToScenegraphMaterial(m);
           face.materials.push_back(sceneMat);
		    
           return true;
        }

        return false;
    }

	bool ModelGenerator::RemapUVsForFace(const scenegraph::Face &uvFace, scenegraph::Face &remapFace, bool remap1, bool remap2, bool remap3)
	{
		size_t numTexRefs = remapFace.textures.size();

		for(size_t i=0;i<numTexRefs;i++)
		{
			const scenegraph::MappedTexture &mt = uvFace.textures.at(i);
			if(remap1)
			{
				if(mt.uvs.size()>0)
					remapFace.textures[i].uvs[0] = uvFace.getUVForVertex(remapFace.getVertN(0),(int)i);
			}
			if(remap2)
			{
				if(mt.uvs.size()>1)
					remapFace.textures[i].uvs[1] = uvFace.getUVForVertex(remapFace.getVertN(1),(int)i);
			}
			if(remap3)
			{
				if(mt.uvs.size()>2)
					remapFace.textures[i].uvs[2] = uvFace.getUVForVertex(remapFace.getVertN(2),(int)i);
			}
		}
		return true;
	}

	bool ModelGenerator::SetupFacesFromEdges(scenegraph::Face &face1, scenegraph::Face &face2, modelkit::Edge *e1, modelkit::Edge *e2, double &line_length1, double &line_length2, sfa::Point normal, sfa::Point line)
	{

		/*
		e1[0]      e2[0]         
		+---line2--+
		|        / |
		e1    /    e2
		|  /       |
		+---line1--+
		e1[1]      e1[2] also e2[1]  

		*/

		// This is our 'unit face'
		scenegraph::Face uvFace;
		
		uvFace.addVert(e1->getPoints()[0]);
		uvFace.addVert(e1->getPoints()[1]);
		uvFace.addVert(e1->getPoints()[2]);

		// The actual edge length, which due to the orientation of the edge might be longer than we expect.
		sfa::Point edge_line1 = (e2->getPoints()[1] - e1->getPoints()[1]);
		sfa::Point edge_line2 = (e2->getPoints()[0] - e1->getPoints()[0]);
		double edge_len1 = edge_line1.length();
		double edge_len2 = edge_line2.length();

		SetupTexturesFromCrossSection(uvFace, e1, false);
        SetupMaterialsFromCrossSection(uvFace, e1);

		// if line==normal, use the original 3 points.
		if(line!=normal)
		{			
			sfa::Point vq1 = uvFace.getVertN(1) - uvFace.getVertN(0);
			sfa::Point vq2 = uvFace.getVertN(2) - uvFace.getVertN(0);
		    
			sfa::Point vt2 = line;// Line is already normalized
			sfa::Point vt1 = vt2.cross( vq1.cross(vt2) );
			vt1.normalize();
		    
			sfa::Point t1 = uvFace.getVertN(0) + vt1*(vq1.length());
			sfa::Point t2 = uvFace.getVertN(0) + vt2*(vq2.length());
		    
			uvFace.setVertN(1,t1);
			uvFace.setVertN(2,t2);
		}
		// Copy the face for the other edge texture map
		scenegraph::Face uvFace2 = uvFace;
		
		// Adjust the UV for length.
		size_t numTexRefs = uvFace.textures.size();
		for(size_t i=0;i<numTexRefs;i++)
		{
			/*
			scenegraph::MappedTexture &mt = uvFace.textures.at(i);
			double unit_y = (mt.uvs[2].Y() - mt.uvs[0].Y())/e1->getProjectedZ();
			double unit_x = (mt.uvs[2].X() - mt.uvs[0].X())/e1->getProjectedZ();

			{
				sfa::Point &uv = mt.uvs.at(0);
				double uvx = uv.X()+(unit_x*line_length2);
				uv.setX(uvx);				
				double uvy = uv.Y()+(unit_y*line_length2);
				uv.setY(uvy);
			}

			{
				sfa::Point &uv = mt.uvs.at(1);
				double uvx = uv.X()+(unit_x*line_length1);
				uv.setX(uvx);				
				double uvy = uv.Y()+(unit_y*line_length1);
				uv.setY(uvy);
			}
			{
				sfa::Point &uv = mt.uvs.at(2);
				double uvx = uv.X()+(unit_x*line_length1);
				uv.setX(uvx);				
				double uvy = uv.Y()+(unit_y*line_length1);
				uv.setY(uvy);
			}
			*/
		}
		
		face1.addVert(e2->getPoints()[0]);// needs a UV calculated
		face1.addVert(e1->getPoints()[1]);
		face1.addVert(e1->getPoints()[0]);
		
		
		if(e1->getNormal().length2()==0)
			face1.SetFaceNormal(face1.computeNormal());
		else
			face1.SetFaceNormal(e1->getNormal());

		sfa::Point computedNormal = face1.computeNormal();
		SetupTexturesFromCrossSection(face1, e1, false);
		RemapUVsForFace(uvFace,face1,true,true,true);

        SetupMaterialsFromCrossSection(face1, e1);
		face2.addVert(e2->getPoints()[0]);// needs a UV calculated
		face2.addVert(e2->getPoints()[1]);// needs a UV calculated
		face2.addVert(e1->getPoints()[1]);

		if(e1->getNormal().length2()==0)
			face2.SetFaceNormal(face1.computeNormal());
		else
			face2.SetFaceNormal(e1->getNormal());

		face2.SetFaceNormal(e1->getNormal());
		SetupTexturesFromCrossSection(face2,e1, false);
				
		RemapUVsForFace(uvFace,face2,true,true,true);

        SetupMaterialsFromCrossSection(face2, e1);

		return true;
	}


	bool ModelGenerator::SetupTexturesFromCrossSection(scenegraph::Face &face, modelkit::Face *fromFace, bool reverseOrder)
	{
		BOOST_FOREACH(modelkit::TextureReference *tr, fromFace->getTextureReferences())
		{
			scenegraph::MappedTexture mt = ConvertToMappedTexture(tr,reverseOrder);
			face.textures.push_back(mt);
		}

		return true;
	}
    
    bool ModelGenerator::SetupMaterialsFromCrossSection(scenegraph::Face &face, modelkit::Face *fromFace)
    {
        if (!fromFace || !fromFace->getMaterialRef())
            return false;

        scenegraph::Material sceneMat = ConvertToScenegraphMaterial(fromFace->getMaterialRef());
        face.materials.push_back(sceneMat);

        return true;
    }
    
	bool ModelGenerator::AddEndFaces(modelkit::CrossSectionSP crossSection, bool flipNormals, std::vector<scenegraph::Face> &faces)
	{
		BOOST_FOREACH(modelkit::Face *csFace, crossSection->getFaces())
		{
			scenegraph::Face sgFace;
			if(flipNormals)
			{
				sgFace.addVert(csFace->getPoints()[2]);
				sgFace.addVert(csFace->getPoints()[1]);
				sgFace.addVert(csFace->getPoints()[0]);
			}
			else
			{
				sgFace.addVert(csFace->getPoints()[0]);
				sgFace.addVert(csFace->getPoints()[1]);
				sgFace.addVert(csFace->getPoints()[2]);
			}
			sgFace.SetFaceNormal(sgFace.computeNormal());

			SetupTexturesFromCrossSection(sgFace,csFace, flipNormals);
			
            SetupMaterialsFromCrossSection(sgFace, csFace); 
			scenegraph::Color primaryColor = csFace->getPrimaryColor();
			if(primaryColor.isInitialized())
			{
				sgFace.primaryColor = primaryColor;
			}
			sgFace.getAttributes().insertAttributes(csFace->getFlags());
			sgFace.smc = csFace->getSMC();
			sgFace.featureID = csFace->getFID();
			if(sgFace.getAttributes().getAttributeAsBool("bothsides"))
			{
				sgFace.drawBothSides = true;
			}
            faces.push_back(sgFace);
		}

		return true;
	}


	bool ModelGenerator::BuildAvoidZoneForLinearFromCSM(const sfa::LineString *line, const std::string crossSectionFilePath, modelkit::AvoidZone &avoid, ModelGeneratorParameters parms)
	{
		modelkit::CrossSectionSP crossSection = GetCachedCrossSection(crossSectionFilePath);
		if(!crossSection)
			return false;
		return BuildAvoidZoneForLinear(line,crossSection,avoid,parms);
	}

	bool ModelGenerator::BuildAvoidZoneForLinearFromKIT(const sfa::LineString *line, const std::string modelKitFilePath, modelkit::AvoidZone &avoid, ModelGeneratorParameters parms)
	{

		modelkit::ModelKitSP modelKit = GetCachedModelKit(modelKitFilePath);
		if(!modelKit)
			return false;
		modelkit::CrossSectionSP cs(modelKit->consolidate());
		return BuildAvoidZoneForLinear(line,cs,avoid);
	}

	bool ModelGenerator::BuildAvoidZoneForLinear(const sfa::LineString *origline, const modelkit::CrossSectionSP crossSection, modelkit::AvoidZone &avoid, ModelGeneratorParameters parms)
	{

		attach_string_t attachmentStrings;

		std::vector<modelkit::CrossSectionSP> crossSections;
		std::vector<sfa::Point> normals;
		std::vector<sfa::Point> lines;
		std::vector<double> line_lengths;

		if(origline->getNumPoints()<2)
			return false;

		avoid.crossSection = crossSection;
		// Get the origin before trimming
		avoid.origin = *origline->getPointN(0);

		double prevOffset = dictionary.getPreviousOffset(parms.featureAttributes);
		double nextOffset = dictionary.getNextOffset(parms.featureAttributes);
		double prevAngle = dictionary.getPreviousAngle(parms.featureAttributes);
		double nextAngle = dictionary.getNextAngle(parms.featureAttributes);
		// Make a copy of the line to trim
		sfa::LineString *line = new sfa::LineString(*origline); 
		// Only trim road for junctions
		if (prevAngle > 0) prevOffset = 0;
		if (nextAngle > 0) nextOffset = 0;

		// Get the origin before trimming the line
		sfa::Point line_origin = *line->getPointN(0);

		if (prevOffset > 0 || nextOffset > 0)
		{
			if(!line->trimEnds(prevOffset,nextOffset))
			{
				// We can't trim it, so return an error
				log << "Model generator unable to trim next/previous offsets for linear. Possibly due to occluded road." << log.endl;
				return false;
			}
		}

		if(!BuildProjections(avoid.projections, normals, lines, line_lengths, line,crossSection,parms,avoid.origin))
			return false;
		
		

		return true;
	}

	bool ReferenceModelInfo::GetModelInfo(std::string filename, ReferenceModelInfo &refinfo)
	{
		double xmin = 0;
		double xmax = 0;
		double ymin = 0;
		double ymax = 0;
		double zmin = 0;
		double zmax = 0;
		double radius = 0;

		if(flt::OpenFlight::getBoundingBox(filename,xmin,xmax,ymin,ymax,zmin,zmax))
		{
			refinfo.zMin = zmin;
			refinfo.zMax = zmax;
			double xy1 = std::max<double>(fabs(xmin),fabs(ymin));
			double xy2 = std::max<double>(fabs(xmax),fabs(ymax));

			refinfo.radius = std::max<double>(xy1,xy2);
			refinfo.filename = filename;

			return true;
		}
		return false;
	}



}