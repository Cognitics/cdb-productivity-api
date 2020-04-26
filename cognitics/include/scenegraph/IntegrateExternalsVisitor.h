/****************************************************************************
Copyright 2014 Cognitics, Inc.
****************************************************************************/

#pragma once
#include <map>
#include <scenegraph/Scene.h>
#include "scenegraph/Visitor.h"

namespace scenegraph
{
	// Read external files and integrate their geometry into the scene, replacing
	// an external reference with a group node (Scene)
	class IntegrateExternalsVisitor : public Visitor
	{
	private:
		std::map<std::string,Scene *> sceneCache;
		Scene *newScene;
	public:
		virtual ~IntegrateExternalsVisitor();
		IntegrateExternalsVisitor();

		virtual void visiting(Scene *scene);
		// The scene is modified in place, so the return scene pointer will
		// be the same pointer as the passed scene
		Scene *integrate(Scene *scene);

	};



}


