#pragma once

#include "tg/TerrainGenerator.h"

namespace cognitics
{
    class FbxTerrainGenerator : public TerrainGenerator
    {
        virtual void BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight) override;
		virtual void generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string & outputTmpPath, const std::string & outputPath, const std::string & outputFormat, int lodDepth, int textureHeight, int textureWidth) override;
    };
}
