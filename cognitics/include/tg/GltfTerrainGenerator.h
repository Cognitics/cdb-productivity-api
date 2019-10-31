#pragma once

#include "tg\TerrainGenerator.h"

namespace cognitics
{
    class GltfTerrainGenerator : public TerrainGenerator
    {
        virtual void AdjustJSON_UV(double& u, double& v) override;
        virtual void BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight) override;
        virtual void CreateMasterFile() override;
        virtual void ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon) override;
        virtual void ExportTree(int treeIndex, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, elev::Elevation_DSM* edsm, TreePoints_Feature& treePointsFeature) override;
		//virtual void generateFixedGrid(const std::string &imgFile, const std::string &outputPath, const std::string &outputName, std::string format, elev::Elevation_DSM& edsm, double north, double south, double east, double west);
		virtual void generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string& outputTmpPath, const std::string& outputPath, const std::string&outputFormat, int lodDepth, int textureHeight, int textureWidth);

    };
}

