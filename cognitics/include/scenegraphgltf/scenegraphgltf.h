#pragma once

#include <scenegraph/Scene.h>

namespace scenegraph
{
	bool buildGltfFromScene(const std::string &filename, Scene *scene, double north, double south, double east, double west);
	bool buildTilesetFromScene(const std::string &filename, Scene *scene);
}