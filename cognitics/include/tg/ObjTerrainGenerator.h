#pragma once

#include "tg\TerrainGenerator.h"

namespace cognitics
{
    class ObjTerrainGenerator : public TerrainGenerator
    {
        virtual void BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight) override;
        virtual void BuildFromTriangulation(const std::string& outputPath, int width, int height, double spacingX, double spacingY, double localWidth, double localHeight, const std::vector<double>& grid) override;
        virtual void ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon) override;
        virtual void ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon, bool xzy) override;
        virtual void ExportFeaturesMetaData() override;
        virtual void ExportTextureMetaData(const std::string& filepath) override;
		virtual void generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string & outputTmpPath, const std::string & outputPath, const std::string & outputFormat, int lodDepth, int textureHeight, int textureWidth);
    };
}
