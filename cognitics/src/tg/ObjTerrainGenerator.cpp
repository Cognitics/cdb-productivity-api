#include <tg/ObjTerrainGenerator.h>
#include <scenegraphobj/scenegraphobj.h>
#include <fstream>

#include <vector>
#include <iomanip>
#include <cctype>
#include "gdal_utils.h"
#include <string>
#include <thread>
#include <mutex>

using namespace cognitics;
//using namespace ws;

void ObjTerrainGenerator::BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight)
{
    scenegraph::buildObjFromScene(outputName, localWest, localNorth, localWidth, localHeight, scene);
}

void ObjTerrainGenerator::BuildFromTriangulation(const std::string& outputPath, int width, int height, double spacingX, double spacingY, double localWidth, double localHeight, const std::vector<double>& grid)
{
    ctl::PointList workingPoints;
    int delaunayResizeIncrement = 100;
    {
        for (int row = 0; row < height; ++row)
        {
            printf("");
            for (int col = 0; col < width; ++col)
            {
                // Go from pixel space to geo
                double lat = (row * spacingY) + north;
                double lon = (col * spacingX) + west;
                // Go from geo to local
                double localPostX = flatEarth.convertGeoToLocalX(lon);
                double localPostY = flatEarth.convertGeoToLocalY(lat);
                workingPoints.push_back(ctl::Point(localPostX, localPostY, grid[(row*width) + col]));
            }
        }

        delaunayResizeIncrement = (height * width) / 8;
    }
    scenegraph::buildObjFromTriangulation(outputPath, localWest, localNorth, localWidth, localHeight, width, height, workingPoints);
}

void ObjTerrainGenerator::ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon)
{
    std::string featurefilename = outputPath + fi.getBaseName(true) + ".obj";
    scenegraph::buildObjFromScene(featurefilename, 0, 0, 0, 0, scene);
    featureInfo.outputModelPath = featurefilename;
}

void ObjTerrainGenerator::ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon, bool xzy)
{
    std::string featurefilename = outputPath + fi.getBaseName(true) + ".obj";
    scenegraph::buildObjXZY(featurefilename, 0, 0, 0, 0, scene, xzy);
    featureInfo.outputModelPath = featurefilename;
}

void ObjTerrainGenerator::ExportFeaturesMetaData()
{
    std::string tileInfoName = ccl::joinPaths(outputPath, "tileInfo.txt");
    std::ofstream tileInfo;
    tileInfo.open(tileInfoName, std::ofstream::out | std::ofstream::app);
    tileInfo << "beginBuildingList\n";
    
    for (int i = 0; i < buildings.Count(); ++i)
    {
        float lat = buildings.GetBuilding(i).lat;
        float lon = buildings.GetBuilding(i).lon;
        if (lat < north && lat > south && lon < east && lon > west)
        {                        
            double y = flatEarth.convertGeoToLocalY(lat);
            double x = flatEarth.convertGeoToLocalX(lon);
            double z = buildings.GetBuilding(i).elev;

            double ao1 = buildings.GetBuilding(i).AO1;
            double scalex = buildings.GetBuilding(i).scaleX;
            double scaley = buildings.GetBuilding(i).scaleY;
            double scalez = buildings.GetBuilding(i).scaleZ;

            std::string modelPath = buildings.GetBuilding(i).modelpath;
            tileInfo << x << " " << y << " " << z << " " << ao1 << " " << modelPath << " " << scalex << " " << scaley << " " << scalez << "\n";
        }
    }

    tileInfo << "endBuildingList\n";
    tileInfo << "beginTreeList\n";

    for (int i = 0; i < trees.size(); ++i)
    {
        float lon = trees[i].treePoints[0].worldPosition.fLon;
        float lat = trees[i].treePoints[0].worldPosition.fLat;
        if (lat < north && lat > south && lon < east && lon > west)
        {   
            float elev = trees[i].treePoints[0].elev;

            double y = flatEarth.convertGeoToLocalY(lat);
            double x = flatEarth.convertGeoToLocalX(lon);
            // localX, localY, elevation, ao1 = 0, model string, scalex, scaley, scalez
            tileInfo << x << " " << y << " " << elev << " 0 " << trees[i].treePoints[0].sModel << " 1 1 1" << "\n";
        }
    }
    
    tileInfo << "endTreeList\n";
    flatEarth.setOrigin((terrainNorth + terrainSouth) / 2, (terrainEast + terrainWest) / 2);
    auto x = flatEarth.convertGeoToLocalX((east + west) / 2);
    auto y = flatEarth.convertGeoToLocalY((north + south) / 2);
    flatEarth.setOrigin((north + south) / 2, (east + west) / 2);
    tileInfo << x << " " << y << "\n";
    tileInfo.close();
}

void ObjTerrainGenerator::ExportTextureMetaData(const std::string& filepath)
{   
    scenegraph::CreateMetaForObj(filepath + ".jpg");
}

void ObjTerrainGenerator::generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string& outputTmpPath, const std::string& outputPath, const std::string&outputFormat, int lodDepth, int textureHeight, int textureWidth)
{
	double deltaX = east - west;
	double deltaY = north - south;

	double delta = std::max<double>(deltaX, deltaY) / 2;

	double centerLat = (north - south) / 2 + south;
	double centerLon = (east - west) / 2 + west;

	east = centerLon + delta;
	west = centerLon - delta;
	north = centerLat + delta;
	south = centerLat - delta;

	centerLat = (north - south) / 2 + south;
	centerLon = (east - west) / 2 + west;

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

	for (auto info : infos)
	{
		std::cout << info.elevationFileName << std::endl;
	}

	GetData(geoServerURL, 0, infos.size(), &infos);

	std::cout << "DONE" << std::endl;
	elev::DataSourceManager dsm(1000000);

	for (auto& info : infos)
	{
		dsm.AddFile_Raster_GDAL(info.elevationFileName);
	}

	elev::Elevation_DSM edsm(&dsm, elev::elevation_strategy::ELEVATION_BILINEAR);

	for (auto& info : infos)
	{
		generateFixedGrid(info.imageFileName, outputPath, info.quadKey, outputFormat, edsm, info.extents.north, info.extents.south, info.extents.east, info.extents.west);
	}

	createFeatures(edsm);
	WriteLODfile(infos, outputPath + "/lodFile.txt", lodDepth);
	setBounds(north, south, east, west);
	CreateMasterFile();
}

