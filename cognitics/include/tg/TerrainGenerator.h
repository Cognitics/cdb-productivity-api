/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/
#pragma once

#include <ccl/ObjLog.h>
#include <cts/FlatEarthProjection.h>
#include <ip/GDALRasterSampler.h>
#include <elev/Elevation.h>
#include <elev/DataSourceManager.h>
#include <elev/Elevation_DSM.h>
#include <scenegraph/Scene.h>
#include <dom/dom.h>
#include <features/GsBuildings.h>
#include "features/GMLParser.h"


namespace cognitics
{

	struct GeoExtents
	{
		double north;
		double east;
		double south;
		double west;
	};
	struct TileInfo
	{
		GeoExtents extents;
		float centerX;
		float centerY;
		std::string elevationFileName;
		std::string imageFileName;
		int width;
		int height;
		std::string quadKey;
		GsBuildings GSFeatures;
	};

    class TerrainGenerator
    {
    public:
        ~TerrainGenerator(void);
        TerrainGenerator(void);

        void setOrigin(double lat, double lon);
        void setBounds(double north, double south, double east, double west);
        void addImageryPath(const std::string &imageryPath);
        void addElevationFile(const std::string &elevationFile);
        void setOutputPath(const std::string &outputPath);
		void setOutputTmpPath(const std::string &outputTmpPath);
		void setOutputFormat(std::string format);
        void setTextureSize(int width, int height);
        void setTexelSize(double texelSize);
		void ComputeCenterPosition(std::vector<TileInfo>& infos, double originlat, double originLon);
        //void generate(int row = -1, int col = -1);
		void createFeatures(elev::Elevation_DSM& edsm);
		void generateFixedGrid(const std::string &elevFile, const std::string &featurePath, const std::string& modelCachePath, std::string format, int tileSize = 0);
		//void generateFixedGrid(const std::string &elevFile, int tileSize=0); 
        void generateFixedGrid(const std::string &elevFile, const std::string &outputName, const std::string &featurePath, int windowTop, int windowBottom, int windowRight, int windowLeft);
		virtual void generateFixedGrid(const std::string &imgFile, const std::string &outputPath, const std::string &outputName, std::string format, elev::Elevation_DSM& edsm, double north, double south, double east, double west);
		virtual void generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string & outputTmpPath, const std::string & outputPath, const std::string & outputFormat, int lodDepth, int textureHeight, int textureWidth) {};
		//void generateFeatures(const std::string &featureFile, const std::string &outputName, GsBuildings *features, const std::vector<double> &grid, int spacingX, int spacingY, int width);
		void ParseJSON(const std::string & filename, const std::string & outputPath, scenegraph::Scene & scene);
		//void ReadForFLT(const std::string &filename, std::string outputDir);
        void parseFeatures(const std::string &outputPath);
        //void CreateMetaForObj(std::string filename);
		static bool IsGltfTypeOutput(const std::string& outputFormat);
		static bool IsObjOutput(const std::string& outputFormat);
        static bool IsFbxOutput(const std::string& outputFormat);
		static bool IsOpenFlightOutput(const std::string& outputFormat);
		double ConvertGeoToLocalX(double lon);
		double ConvertGeoToLocalY(double lat);
        void transformSceneFromFlatEarthToUTM(scenegraph::Scene& scene, const std::string& utm_zone);
        
        virtual void AdjustJSON_UV(double& u, double& v) {}
        virtual void BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight) = 0;
        virtual void BuildFromTriangulation(const std::string& outputPath, int width, int height, double spacingX, double spacingY, double localWidth, double localHeight, const std::vector<double>& grid) {};
        virtual void CreateMasterFile() {}
        virtual void ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon) {};
        virtual void ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon, bool xzy) {};
        virtual void ExportFeaturesMetaData() {};
        virtual void ExportTree(int treeIndex, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, elev::Elevation_DSM* edsm, TreePoints_Feature& treePointsFeature) {};
        virtual void ExportTextureMetaData(const std::string& filepath) {};
		static void GetElevation(const std::string& geoServerURL, double north, double south, double east, double west, int& textureWidth, int& textureHeight, const std::string& outputPath);
		static void GetImagery(const std::string& geoServerURL, double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath);
		static void GetData(const std::string& geoServerURL, int beginIndex, int endIndex, std::vector<TileInfo>* infos);
		void GenerateLODBounds(std::vector<TileInfo>& infos, const TileInfo& info, int depth);
		void GenerateFileNames(std::vector<TileInfo>& infos, const std::string& outputTmpPath, std::string format);
		std::string GetName(double originLat, double originLon, std::string formatIn, std::string filetype);
		void WriteLODfile(std::vector<TileInfo>& infos, std::string outputFilename, int nLODs);
		void SetBuildingElevations(elev::Elevation_DSM& edsm);

    protected:
        ccl::ObjLog logger;
        cts::FlatEarthProjection flatEarth;
        GDALRasterSampler rasterSampler;
        std::set<std::string> rasterExtensions;
        std::set<std::string> oddbFilenames;
        elev::DataSourceManager elevationDSM;
        elev::Elevation_DSM elevationSampler;
        double north, south, east, west;
        double terrainNorth;
        double terrainSouth;
        double terrainEast;
        double terrainWest;
        double localNorth, localSouth, localEast, localWest;
        std::string modelKitPath;        // "modelkits/"
        std::string texturePath;        // "textures/"
        std::string outputPath;            // "output/"
        std::string outputTmpPath;		  // "tmp/"
        int textureHeight;                // 1024
        int textureWidth;                // 1024
        double texelSize;                // 5.0f
        scenegraph::Scene master;
        dom::DocumentSP cerDocument;
        GsBuildings buildings;
        std::vector<ManMadePoints_FootprintsFeature> footprints;
        std::vector<TreePoints_Feature> trees;
        std::string outputFormat;

        //void generateRow(int row, int col = -1);
        //void generateRowColumn(int row, int col);        
        double getZ(double x, double y);

    };

}
