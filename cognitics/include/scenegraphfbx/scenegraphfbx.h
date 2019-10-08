#pragma once

#include <scenegraph/Scene.h>
#include <ctl/ctl.h>
#include <features/GsBuildings.h>
#include <tg/TerrainGenerator.h>

namespace scenegraph
{
	bool buildFbxWithFeatures(const std::string &outputName, scenegraph::Scene* scene, GsBuildings buildings, cognitics::TerrainGenerator *tg, double north, double south, double east, double west, std::string imageFileName, bool setTextureFilename);
}