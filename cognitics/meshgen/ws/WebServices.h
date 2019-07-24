#pragma once
#include <tg/TerrainGenerator.h>
#include "gdal_utils.h"

namespace ws
{	
	void GetDataWithGDAL(cognitics::TerrainGenerator& tg, std::set<std::string>& elevationFiles, const std::string &outputPath, double north, double south, double east, double west, int& terrainWidth, int& terrainHeight, double originLat, double originLon, std::string format);
	void GetImagery(double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath, const std::string elevationPath, double originLat, double originLon, std::string format);
	std::string SetName(double originLon, double originLat, double textureHeight, double textureWidth, const std::string &outputPath, std::string format, std::string filetype);
}