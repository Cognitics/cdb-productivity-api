#pragma once
#include <tg/TerrainGenerator.h>
#include "gdal_utils.h"
#include <string>

namespace ws
{
    //void GetDataWithGDAL(std::set<std::string>& elevationFiles, const std::string &outputPath, double north, double south, double east, double west, int& terrainWidth, int& terrainHeight, double originLat, double originLon, std::string format);
    void GetImagery(const std::string& geoServerIPAddress, double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath);
    void GetImageryJPG(const std::string& geoServerURL, double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath);
    void GetElevation(const std::string& geoServerIPAddress, double north, double south, double east, double west, int& textureWidth, int& textureHeight, const std::string& outputPath);
    void GetElevationSquare(const std::string& geoServerIPAddress, double north, double south, double east, double west, int& textureWidth, int& textureHeight, const std::string& outputPath);
    void GetFeatureData(const std::string& geoServerIPAddress, std::string tmpPath, double north, double south, double east, double west);
    //void GetFeatures(double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &featurePath);
    std::string GetName(double originLon, double originLat, double textureHeight, double textureWidth, std::string format, std::string filetype);
    void generateFixedGridWeb(double north, double south, double west, double east, std::string format, const std::string& geoServerIPAddress, const std::string& outputTmpPath, const std::string& outputPath, const std::string& featurePath, const std::string& outputFormat, int textureWidth, int textureHeight, cognitics::TerrainGenerator& terrainGenerator);
    void generateFixedGridWeb2(double north, double south, double west, double east, std::string format, const std::string& geoServerIPAddress, const std::string& outputTmpPath, const std::string& outputPath, const std::string& outputFormat, cognitics::TerrainGenerator* terrainGenerator, int lodDepth, int textureHeight, int textureWidth);
    void generateFixedGridSofprep(double north, double south, double west, double east, const std::string& geoServerIPAddress, const std::string& outputTmpPath, const std::string& outputPath, const std::string& outputFormat);
	void generateCesiumLods(double north, double south, double west, double east, 
		const std::string& geoServerURL, const std::string& outputTmpPath, const std::string& outputPath, 
		cognitics::TerrainGenerator& terrainGenerator, int lodDepth);
    void ParseJSON(const std::string &filename, const std::string &outputPath, scenegraph::Scene& scene);
    void GenerateSingleOBJ(const std::string& elevationFile, const std::string& geoserverIPAddress, const std::string& outputPath, const std::string& outputTmpPath);
}