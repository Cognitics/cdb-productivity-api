#pragma once
#include <string>
#include "scenegraph/Scene.h"

namespace gltf
{
	class Tileset
	{
		std::string name;
		scenegraph::Scene* scene;
	public:
		Tileset(std::string filename, scenegraph::Scene* sceneA) :
			name(filename), scene(sceneA)
		{}
		void GetRadianRectFromExtRef(scenegraph::ExternalReference& ref, GeoRect& out_rect);
		void write();

	};
}