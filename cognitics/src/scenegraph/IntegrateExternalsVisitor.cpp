/****************************************************************************
Copyright 2014 Cognitics, Inc.
****************************************************************************/


#include "scenegraph/IntegrateExternalsVisitor.h"
#include <scenegraphflt/scenegraphflt.h>
#include <scenegraph/LOD.h>
namespace scenegraph
{
	IntegrateExternalsVisitor::~IntegrateExternalsVisitor()
	{
		//Clean up the sceneCache
		std::map<std::string,Scene *>::iterator iter = sceneCache.begin();
		while(iter!=sceneCache.end())
		{
			delete iter->second;
			iter++;
		}
	}

	IntegrateExternalsVisitor::IntegrateExternalsVisitor()
	{

	}

	void IntegrateExternalsVisitor::visiting(Scene *scene)
	{	
		scenegraph::Scene *ext_group = NULL;
		if(scene->externalReferences.size()>0)
		{
			ext_group = new scenegraph::Scene();
			ext_group->name = "externals";
			scene->addChild(ext_group);
		}
		for(size_t i = 0, c = scene->externalReferences.size(); i < c; ++i)
		{
			std::stringstream ss;
			ExternalReference externalReference = scene->externalReferences.at(i);		// copy assignment
			
			// consolidate transform
			sfa::Matrix m;
			m.PushRotate(externalReference.attitude);
			m.PushScale(externalReference.scale);
			m.PushTranslate(externalReference.position);
			if(sceneCache.find(externalReference.filename)==sceneCache.end())
			{
				sceneCache[externalReference.filename] = buildSceneFromOpenFlight(externalReference.filename);
			}

			Scene *ext_scene = sceneCache[externalReference.filename]->copy();
			if(ext_scene)
			{
				ss << "ext" << i;
				ext_scene->name = ss.str();
				//Position the scene. We multiply against the
				//existing scene in case it already has a matrix other than the identity matrix
				ext_scene->matrix = ext_scene->matrix * m;
				ext_group->addChild(ext_scene);
			}

		}
		//Remove all the previous external references
		scene->externalReferences.clear();
		traverse(scene);
	}

	
	// The scene is modified in place
	Scene *IntegrateExternalsVisitor::integrate(Scene *scene)
	{
		newScene = new Scene();
		visit(scene);
		return scene;
	}


}