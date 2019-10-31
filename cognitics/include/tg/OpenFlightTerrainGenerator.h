#pragma once

#include "tg/TerrainGenerator.h"

namespace cognitics
{
    class OpenFlightTerrainGenerator : public TerrainGenerator
    {
        virtual void BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight) override;
        virtual void CreateMasterFile() override;
        virtual void ExportTextureMetaData(const std::string& filepath) override;
    };
}

