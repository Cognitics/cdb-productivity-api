#pragma once

#include <scenegraph/Scene.h>

namespace scenegraph
{
	bool buildGltfFromScene(std::string &filename, Scene *scene,
		double north, double south, double east, double west, double minElev = 0.0, double maxElev = 1.0, int id = 0, double angle = 0.0);
	bool buildTilesetFromScene(const std::string &filename, Scene *scene, double north, double south, double east, double west);
}