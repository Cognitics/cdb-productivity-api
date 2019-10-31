#include <tg/FbxTerrainGenerator.h>
#include <scenegraphfbx/scenegraphfbx.h>

#include <fstream>
#include <vector>
#include <iomanip>
#include <cctype>
#include "gdal_utils.h"
#include <string>
#include <thread>
#include <mutex>

using namespace cognitics;

void FbxTerrainGenerator::BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight)
{
    //scenegraph::buildFbxFromScene(outputName, scene, "", false);
    scenegraph::buildFbxWithFeatures(outputName, scene, buildings, this, north, south, east, west, "", false);
}

void FbxTerrainGenerator::generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string & outputTmpPath, const std::string & outputPath, const std::string & outputFormat, int lodDepth, int textureHeight, int textureWidth)
{
	double deltaX = east - west;
	double deltaY = north - south;
	double delta = std::max(deltaX, deltaY) / 2;
	double centerLat = (north - south) / 2 + south;
	double centerLon = (east - west) / 2 + west;

	// make the bounds square for LOD divisions
	east = centerLon + delta;
	west = centerLon - delta;
	north = centerLat + delta;
	south = centerLat - delta;
	// reset the center to match the new bounds. 
	centerLat = (north - south) / 2 + south;
	centerLon = (east - west) / 2 + west;

	// set up tile information 
	std::vector<TileInfo> infos;
	TileInfo info;
	info.extents.east = east;
	info.extents.north = north;
	info.extents.south = south;
	info.extents.west = west;
	info.quadKey = std::to_string(0);
	GenerateLODBounds(infos, info, lodDepth);
	GenerateFileNames(infos, outputTmpPath, format);
	ComputeCenterPosition(infos, centerLat, centerLon);

	GetData(geoServerURL, 0, infos.size(), &infos);

	std::cout << "DONE" << std::endl;

	elev::DataSourceManager dsm(1000000);

	for (auto& info : infos)
	{
		dsm.AddFile_Raster_GDAL(info.elevationFileName);
	}

	elev::Elevation_DSM edsm(&dsm, elev::elevation_strategy::ELEVATION_BILINEAR);

	std::string dataPath = ccl::joinPaths(outputTmpPath, "data");
	buildings.ProcessBuildingData(ccl::joinPaths(dataPath, "building_models.xml"));

	int oldNorth = north;
	int oldSouth = south;
	int oldEast = east;
	int oldWest = west;
	for (auto& info : infos)
	{
		setBounds(info.extents.north, info.extents.south, info.extents.east, info.extents.west);
		generateFixedGrid(info.imageFileName, outputPath, info.quadKey, outputFormat, edsm, info.extents.north, info.extents.south, info.extents.east, info.extents.west);
	}

	//createFeatures(edsm);

	WriteLODfile(infos, outputPath + "/lodFile.txt", lodDepth);
	setBounds(oldNorth, oldSouth, oldEast, oldWest);
	CreateMasterFile();
}

