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

//#pragma optimize("", off)

#pragma warning (disable : 4996)

#include "tg/TerrainGenerator.h"
#include <ccl/FileInfo.h>
#include <ip/pngwrapper.h>
#include <ip/jpgwrapper.h>
#include <limits>
//#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <ctl/ctl.h>

#include <scenegraph/SceneCropper.h>

#include <scenegraph/FlattenVisitor.h>
#include <scenegraph/TransformVisitor.h>
#include "elev/SimpleDEMReader.h"
#include <scenegraph/CoordinateTransformVisitor.h>

#undef max
#undef min
#define NOMINMAX
#include "rapidjson/document.h"
#include "scenegraph/Face.h"

#include <fstream>
//#include <filesystem>
#include <iomanip>
#include "features/GsBuildings.h"
#include "features/GMLParser.h"
#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <cctype>

#include "scenegraphgltf/scenegraphgltf.h"

namespace cognitics
{   

    TerrainGenerator::~TerrainGenerator(void)
    {
    }


    TerrainGenerator::TerrainGenerator(void) :
        north(DBL_MAX), south(-DBL_MAX), east(DBL_MAX), west(-DBL_MAX),
        localNorth(DBL_MAX), localSouth(-DBL_MAX), localEast(DBL_MAX), localWest(-DBL_MAX),
        outputPath("output/"), 
        textureWidth(1024), textureHeight(1024), texelSize(5.0f),
        elevationDSM(100 * 1024 * 1024), elevationSampler(&elevationDSM, elev::ELEVATION_BILINEAR)
    {
        logger.init("TerrainGenerator");
        rasterExtensions.insert("jp2");
        rasterExtensions.insert("tif");
        rasterExtensions.insert("sid");
        rasterExtensions.insert("ecw");
    }

    void TerrainGenerator::setOrigin(double lat, double lon)
    {
        double oldLat = flatEarth.getOriginLatitude();
        double oldLon = flatEarth.getOriginLongitude();
        flatEarth.setOrigin(lat, lon);
        if(((oldLat != lat) || (oldLon != lon)) && ((oldLat != 0.0f) || (oldLon != 0.0f)))
            logger << ccl::LWARNING << "WARNING: origin change (OLD: " << oldLat << ", " << oldLon << ")" << logger.endl;
        logger << ccl::LINFO << "setOrigin(" << lat << ", " << lon << ")" << logger.endl;
    }

    void TerrainGenerator::setBounds(double north, double south, double east, double west)
    {
        this->north = north;
        this->south = south;
        this->west = west;
        this->east = east;
        localNorth = flatEarth.convertGeoToLocalY(north);
        localSouth = flatEarth.convertGeoToLocalY(south);
        localWest = flatEarth.convertGeoToLocalX(west);
        localEast = flatEarth.convertGeoToLocalX(east);
        logger << ccl::LINFO << "setBounds(): N:" << north << "(" << localNorth << ") S:" << south << "(" << localSouth << ") W:" << west << "(" << localWest << ") E:" << east << "(" << localEast << ")" << logger.endl;
    }

    void TerrainGenerator::addImageryPath(const std::string &imageryPath)
    {
        rasterSampler.AddDirectory(imageryPath, rasterExtensions);
        logger << ccl::LINFO << "addImageryPath(" << imageryPath << ")" << logger.endl;
    }

    void TerrainGenerator::addElevationFile(const std::string &elevationFile)
    {
        elevationDSM.AddFile_Raster_GDAL(elevationFile);
        logger << ccl::LINFO << "addElevationFile(" << elevationFile << ")" << logger.endl;
    }
    

    void TerrainGenerator::setOutputPath(const std::string &outputPath)
    {
        this->outputPath = outputPath;
        logger << ccl::LINFO << "setOutputPath(" << outputPath << ")" << logger.endl;
    }

	void TerrainGenerator::setOutputTmpPath(const std::string &outputTmpPath)
	{
		this->outputTmpPath = outputTmpPath;
		logger << ccl::LINFO << "setOutputTmpPath(" << outputTmpPath << ")" << logger.endl;
	}

	void TerrainGenerator::setOutputFormat(std::string format)
	{
		outputFormat = format;
	}

    void TerrainGenerator::setTextureSize(int width, int height)
    {
        this->textureWidth = width;
        this->textureHeight = height;
        logger << ccl::LINFO << "setTextureSize(" << width << ", " << height << ")" << logger.endl;
    }

    void TerrainGenerator::setTexelSize(double texelSize)
    {
        this->texelSize = texelSize;
        logger << ccl::LINFO << "setTexelSize(" << texelSize << ")" << logger.endl;
    }

    /*void TerrainGenerator::generate(int row, int col)
    {
        master.externalReferences.clear();
        int rows = ceil((localNorth - localSouth) / (textureHeight * texelSize));
        for(int r = 0; r < rows; ++r)
        {
            if((row == -1) || (row == r))
                generateRow(r, col);
        }
        scenegraph::buildOpenFlightFromScene(ccl::joinPaths(outputPath, "master.flt"), &master);
        master.externalReferences.clear();
    }*/

	void TerrainGenerator::createFeatures(elev::Elevation_DSM& edsm)
	{
		std::string dataPath = ccl::joinPaths(outputTmpPath, "data");
		buildings.ProcessBuildingData(ccl::joinPaths(dataPath, "building_models.xml"));
		ParseTreePoints(ccl::joinPaths(dataPath, "tree_points.xml"), trees);

		//modeled features
		for (int i = 0; i < buildings.Count(); ++i)
		{
			ccl::FileInfo fi(buildings.GetBuilding(i).modelpath);

			std::string sModelPath = buildings.GetBuilding(i).modelpath;

			if (!fi.fileExists(sModelPath))
			{
				continue;
			}

			scenegraph::Scene scene;
			ParseJSON(sModelPath, outputPath, scene);

			float lat = buildings.GetBuilding(i).lat;
			float lon = buildings.GetBuilding(i).lon;

			sfa::Point loc(lon, lat);
			edsm.Get(&loc);
			buildings.GetBuilding(i).elev = loc.Z();

            ExportBuilding(buildings.GetBuilding(i), fi, outputPath, &scene, lat, lon);
		}

		//tree features
		{
			int missingTrees = 0;
			std::map<std::string, int> treeModels;
			for (int i = 0; i < trees.size(); ++i)
			{
				auto model = treeModels.find(trees[i].treePoints[0].sModel);
				if (model == treeModels.end())
				{
					for (int j = 0; j < buildings.Count(); ++j)
					{
						if (std::string::npos != buildings.GetBuilding(j).modelpath.find(trees[i].treePoints[0].sModel))
						{
							treeModels.insert(std::pair<std::string, int>(trees[i].treePoints[0].sModel, j));
							model = treeModels.find(trees[i].treePoints[0].sModel);
							break;
						}
					}
				}

				if (model != treeModels.end())
				{
					std::string fullTreePath = buildings.GetBuilding(model->second).modelpath;

					ccl::FileInfo fi(fullTreePath);
					if (!fi.fileExists(fullTreePath))
					{
						++missingTrees;
						continue;
					}

                    ExportTree(i, fi, fullTreePath, nullptr, &edsm, trees[i]);
				}
				else
				{
					++missingTrees;
				}
			}
            if(missingTrees > 0)
			    std::cout << "Missing " << missingTrees << " tree models" << std::endl;
		}
	}

    /*void TerrainGenerator::generateRow(int row, int col)
    {
        int cols = ceil((localEast - localWest) / (textureWidth * texelSize));
        for(int c = 0; c < cols; ++c)
        {
            if((col == -1) || (col == c))
                generateRowColumn(row, c);
        }
    }*/
	
	void TerrainGenerator::generateFixedGrid(const std::string &elevFile, const std::string &featurePath, const std::string& modelCachePath, std::string format, int tileSize)
    {
		outputFormat = format;
        terrainNorth = north;
        terrainSouth = south;
        terrainEast = east;
        terrainWest = west;

        std::string dataPath = ccl::joinPaths(outputTmpPath, "data");
#ifdef CAE_MESH
        buildings.ProcessBuildingData(ccl::joinPaths(dataPath, "building_models.xml"));
		ParseTreePoints(ccl::joinPaths(dataPath, "tree_points.xml"), trees);
#endif
        ccl::FileInfo fi(elevFile);
        std::string tileName = fi.getBaseName(true);
        std::string filename = ccl::joinPaths(outputPath, tileName + format);
		std::string tileInfoName = ccl::joinPaths(outputPath, "tileInfo.txt");
		std::ofstream tileInfo(tileInfoName);
		tileInfo << tileName << "\n";
        int width = 0;
        int height = 0;
        {
            OGRSpatialReference oSRS;
            oSRS.SetWellKnownGeogCS("WGS84");
            elev::SimpleDEMReader demReader(elevFile, oSRS);
            demReader.Open();
            width = demReader.getWidth();
            height = demReader.getHeight();
        }
        int windowTop = 0; 
        int windowBottom = 0;
        int windowRight = 0;
        int windowLeft = 0;
        if (tileSize == 0)
        {
			tileInfo << "1 1\n";
			tileInfo.close();
			std::stringstream stringstream;
			stringstream << tileName << "_0_0" << format;
			filename = ccl::joinPaths(outputPath, stringstream.str());
            generateFixedGrid(elevFile, filename, featurePath, 0, height, width, 0);
        }
        else
        {
            int rows = ceil(height / tileSize);
            int cols = ceil(width / tileSize);
			tileInfo << rows << " " << cols << "\n";
			tileInfo.close();

            for (int row = 0; row < rows; row++)
            {
                for (int col = 0; col < cols; col++)
                {
                    std::stringstream ss;
                    ss << tileName  << "_" << row << "_" << col << format;
                    filename = ccl::joinPaths(outputPath, ss.str());

                    std::cout << filename << std::endl;

                    int windowBottom = (row + 1) * (tileSize);
                    int windowRight = (col + 1) * tileSize;
                    if (row != rows - 1)
                    {
                        windowBottom += 1;
                    }
                    if (col != cols - 1)
                    {
                        windowRight += 1;
                    }
                    generateFixedGrid(elevFile, filename, featurePath,
                        row * tileSize,
                        windowBottom,
                        windowRight,
                        col * tileSize);
                }
            }
        }

#ifdef CAE_MESH
		//modeled features
		int featureCount = buildings.Count();
		int featuresNotFound = 0;
		for (int i = 0; i < buildings.Count(); ++i)
		{
			ccl::FileInfo fi(buildings.GetBuilding(i).modelpath);
            std::string sModelPath = buildings.GetBuilding(i).modelpath;

            if (!modelCachePath.empty())
            {
                std::string s = buildings.GetBuilding(i).modelpath;
                size_t n = s.find("ModelCache");
                n += strlen("ModelCache/");
                sModelPath = ccl::joinPaths(modelCachePath, s.substr(n));
            }
            
            if (!fi.fileExists(sModelPath))
            {
				logger << ccl::LINFO << "Missing model at " << sModelPath << logger.endl;
				featuresNotFound++;
                continue;
            }

            //std::cout << "Opening " << buildings.GetBuilding(i).modelpath << std::endl;

			scenegraph::Scene scene;
			ParseJSON(sModelPath, outputPath, scene);

			float lat = buildings.GetBuilding(i).lat;
			float lon = buildings.GetBuilding(i).lon;

            bool xzy = true;
            ExportBuilding(buildings.GetBuilding(i), fi, outputPath, &scene, lat, lon, xzy);
        }
        if(featuresNotFound > 0)
		    logger << ccl::LINFO << "Couldn't find " << featuresNotFound << " models of " << featureCount << ". Please check your model cache path" << logger.endl; 
		
		//tree features
		{
			int missingTrees = 0;
			std::map<std::string, int> treeModels;
			for (int i = 0; i < trees.size(); ++i)
			{
				auto model = treeModels.find(trees[i].treePoints[0].sModel);
				if (model == treeModels.end())
				{
					for (int j = 0; j < buildings.Count(); ++j)
					{
						if (std::string::npos != buildings.GetBuilding(j).modelpath.find(trees[i].treePoints[0].sModel))
						{
							treeModels.insert(std::pair<std::string, int>(trees[i].treePoints[0].sModel, j));
							model = treeModels.find(trees[i].treePoints[0].sModel);
							break;
						}
					}
				}

				if (model != treeModels.end())
				{
					std::string fullTreePath = buildings.GetBuilding(model->second).modelpath;

					ccl::FileInfo fi(fullTreePath);
					if (!fi.fileExists(fullTreePath))
					{
						++missingTrees;
						continue;
					}

                    ExportTree(i, fi, outputPath, nullptr, nullptr, trees[i]);
				}
				else
				{
					++missingTrees;
				}
			}
            if(missingTrees > 0)
			    std::cout << "Missing " << missingTrees << " tree models" << std::endl;
		}
#endif

		CreateMasterFile();
		      
        master.externalReferences.clear();
    }
	/*
	void TerrainGenerator::generateFixedGrid(const std::string &elevFile, int tileSize)
	{
		ccl::FileInfo fi(elevFile);
		std::string tileName = fi.getBaseName(true);
		std::string fltFilename = ccl::joinPaths(outputPath, tileName + ".flt");
		int width = 0;
		int height = 0;
		{
			OGRSpatialReference oSRS;
			oSRS.SetWellKnownGeogCS("WGS84");
			elev::SimpleDEMReader demReader(elevFile, oSRS);
			demReader.Open();
			width = demReader.getWidth();
			height = demReader.getHeight();
		}
		int windowTop = 0;
		int windowBottom = 0;
		int windowRight = 0;
		int windowLeft = 0;
		if (tileSize == 0)
		{
			generateFixedGrid(elevFile, fltFilename, 0, height, width, 0);
		}
		else
		{
			int rows = ceil(height / tileSize);
			int cols = ceil(width / tileSize);
			for (int row = 0; row < rows; row++)
			{
				for (int col = 0; col < cols; col++)
				{
					std::stringstream ss;
					ss << tileName << "_" << row << "_" << col << ".flt";
					fltFilename = ccl::joinPaths(outputPath, ss.str());

					generateFixedGrid(elevFile, fltFilename,
						row * tileSize,
						(row + 1) * (tileSize),
						(col + 1) * tileSize,
						col * tileSize);
				}
			}
		}
		scenegraph::buildOpenFlightFromScene(ccl::joinPaths(outputPath, "master.flt"), &master);
		master.externalReferences.clear();
	}*/

	void TerrainGenerator::ParseJSON(const std::string &filename, const std::string &outputPath, scenegraph::Scene& scene)
	{		
		ccl::FileInfo fi(filename);
		FILE *fp = fopen(filename.c_str(), "r");
		char* source;

		if (fp != nullptr)
		{
			if (fseek(fp, 0L, SEEK_END) == 0)
			{
				long bufSize = ftell(fp);
				if (bufSize == -1) 
				{
					;
				}

				source = new char[bufSize + 1];

				if (fseek(fp, 0L, SEEK_SET) != 0)
				{
					;
				}

				size_t newLength = fread(source, sizeof(char), bufSize, fp);

				if (ferror(fp) != 0)
				{
					std::cout << "error" << std::endl;
				}
				else
				{
					source[newLength++] = '\0';
				}

				rapidjson::Document document;
				document.Parse(source);		

				// Vertices
				auto vertices = document["vertices"].GetArray();
				std::vector<sfa::Point> verts;				

				for (rapidjson::Value::ConstValueIterator itr = vertices.Begin(); itr != vertices.End(); ++itr)
				{
					double x = itr->GetDouble();
					++itr;
					double y = itr->GetDouble();
					++itr;
					double z = itr->GetDouble();

					sfa::Point point(x,y,z);
					verts.push_back(point);
				}

				// Normals
				auto normals = document["normals"].GetArray();
				std::vector<sfa::Point> normal;

                for (rapidjson::Value::ConstValueIterator itr = normals.Begin(); itr != normals.End(); ++itr)
                {
                    double x = itr->GetDouble();
                    ++itr;
                    double y = itr->GetDouble();
                    ++itr;
                    double z = itr->GetDouble();

                    sfa::Point point(x, y, z);
                    normal.push_back(point);
                }

				// UVS
				auto uvs = document["uvs"].GetArray();
				auto uv = uvs[0].GetArray(); // update
				std::vector<sfa::Point> uvsPoint;

				for (rapidjson::Value::ConstValueIterator itr = uv.Begin(); itr != uv.End(); ++itr)
				{
					double x = itr->GetDouble();
					++itr;
					double y = itr->GetDouble();

                    AdjustJSON_UV(x, y);

					sfa::Point point(x, y);
					uvsPoint.push_back(point);
				}

				// Materials
				auto materials = document["materials"].GetArray();
				std::vector<scenegraph::Material> materialList;

				for (rapidjson::Value::ConstValueIterator itr = materials.Begin(); itr != materials.End(); ++itr)
				{      
                    scenegraph::Material mat;

                    auto ambient = (*itr)["colorAmbient"].GetArray();   // Ka
                    mat.ambient.r = ambient[0].GetFloat();
                    mat.ambient.g = ambient[1].GetFloat();
                    mat.ambient.b = ambient[2].GetFloat();

                    auto diffuse = (*itr)["colorDiffuse"].GetArray();   // Kd
                    mat.diffuse.r = diffuse[0].GetFloat();
                    mat.diffuse.g = diffuse[1].GetFloat();
                    mat.diffuse.b = diffuse[2].GetFloat();

                    auto specular = (*itr)["colorSpecular"].GetArray(); // Ks
                    mat.specular.r = specular[0].GetFloat();
                    mat.specular.g = specular[1].GetFloat();
                    mat.specular.b = specular[2].GetFloat();

                    mat.illumination = (*itr)["illumination"].GetInt(); // illum

                    std::string mapDiffuse = (*itr)["mapDiffuse"].GetString(); // material name  
                    ccl::FileInfo fi(mapDiffuse);                  
                    mat.mapDiffuse = fi.getBaseName();

                    if (!ccl::copyFile(ccl::joinPaths(fi.getDirName(), mapDiffuse), ccl::joinPaths(outputPath, mapDiffuse)))
                    {
                        std::cout << "copy failed" << std::endl;
                    }                    

                    mat.transparency = (*itr)["transparency"].GetDouble();
                    mat.transparent = (*itr)["transparent"].GetBool();      

                    materialList.push_back(mat);
				}

				// Faces
				auto faces = document["faces"].GetArray();
				std::vector<scenegraph::Face> face;
				int separator;
				int vertex_index1;
				int vertex_index2;
				int vertex_index3;
				int material_index;
				int vertex_uv1;
				int vertex_uv2;
				int vertex_uv3;
				int vertex_normal1;
				int vertex_normal2;
				int vertex_normal3;

				for (rapidjson::Value::ConstValueIterator itr = faces.Begin(); itr != faces.End(); ++itr)
				{
					separator = itr->GetInt();
					++itr;
					vertex_index1 = itr->GetInt();
					++itr;
					vertex_index2 = itr->GetInt(); 
					++itr;
					vertex_index3 = itr->GetInt();
					++itr;
					material_index = itr->GetInt();
					++itr;
					vertex_uv1 = itr->GetInt();
					++itr;
					vertex_uv2 = itr->GetInt();
					++itr;
					vertex_uv3 = itr->GetInt();
					++itr;
					vertex_normal1 = itr->GetInt();
					++itr;
					vertex_normal2 = itr->GetInt();
					++itr;
					vertex_normal3 = itr->GetInt();

					scenegraph::Face f;

					scenegraph::MappedTexture mt;
					mt.SetTextureName(materialList.at(material_index).mapDiffuse);
					mt.uvs.push_back(uvsPoint[vertex_uv1]);
					mt.uvs.push_back(uvsPoint[vertex_uv2]);
					mt.uvs.push_back(uvsPoint[vertex_uv3]);
					f.textures.push_back(mt);
					
					f.addVert(verts[vertex_index1]);
					f.addVert(verts[vertex_index2]);
					f.addVert(verts[vertex_index3]);

					f.setNormalN(0, normal[vertex_normal1]);
					f.setNormalN(1, normal[vertex_normal2]);
					f.setNormalN(2, normal[vertex_normal3]);

					face.push_back(f);
				}

				scene.faces = face;
                scene.faceMaterials = materialList;		
					
                size_t beg = filename.find_last_of("/");
                size_t end = filename.find_first_of(".");
                std::string name = filename.substr(beg+1, end - beg - 1);

				//scenegraph::buildOpenFlightFromScene(outputPath+"/"+name+".flt", &scene, 1640);				
			}
		}	        
		
		fclose(fp);
		//free(fp);
	}

	//void TerrainGenerator::ReadForFLT(const std::string &filename, std::string outputDir)
	//{
 //       ccl::FileInfo fi(filename);
 //       FILE *fp = fopen(filename.c_str(), "r");
 //       char* source;

 //       if (fp != nullptr)
 //       {
 //           if (fseek(fp, 0L, SEEK_END) == 0)
 //           {
 //               long bufSize = ftell(fp);
 //               if (bufSize == -1)
 //               {
 //                   ;
 //               }

 //               source = new char[bufSize + 1];

 //               if (fseek(fp, 0L, SEEK_SET) != 0)
 //               {
 //                   ;
 //               }

 //               size_t newLength = fread(source, sizeof(char), bufSize, fp);

 //               if (ferror(fp) != 0)
 //               {
 //                   std::cout << "error" << std::endl;
 //               }
 //               else
 //               {
 //                   source[newLength++] = '\0';
 //               }

 //               rapidjson::Document document;
 //               document.Parse(source);

 //               std::ofstream featurePositions;
 //               std::string outputName = outputDir + "featurePositions.txt";
 //               featurePositions.open(outputName, std::ofstream::out);
 //               featurePositions << std::setprecision(9);

 //               if (document.IsObject())
 //               {
 //                   std::cout << "woo hoo" << std::endl;
 //                   const char thing[] = "features";
 //                   if (document.HasMember(thing))
 //                   {
 //                       std::cout << "boo" << std::endl;
 //                   }
 //                   else
 //                   {
 //                       std::cout << "foo" << std::endl;
 //                   }
 //               }
 //               else
 //               {
 //                   std::cout << "Oh no" << std::endl;
 //               }

 //               // Features
 //               auto features = document["features"].GetArray();
 //               std::vector<scenegraph::Scene> scenes;

 //               for (rapidjson::Value::ConstValueIterator itr = features.Begin(); itr != features.End(); ++itr)
 //               {
 //                   scenegraph::Scene scene;

 //                   double lat = (*itr)["Lat"].GetDouble();
 //                   double lon = (*itr)["Lon"].GetDouble();

 //                   scene.attributes.setAttribute("Lat", lat);
 //                   scene.attributes.setAttribute("Lon", lon);

 //                   double y = flatEarth.convertGeoToLocalY(lat);
 //                   double x = flatEarth.convertGeoToLocalX(lon);
 //                   sfa::Point point(x, y);
 //                   featurePositions << x << " " << y << "\n";

 //                   std::string dType = (*itr)["dType"].GetString();
 //                   scene.attributes.setAttribute("dType", dType);

 //                   if (dType == "gt")
 //                   {
 //                       scene.attributes.setAttribute("SCALz", (*itr)["SCALz"].GetDouble());
 //                       scene.attributes.setAttribute("SCALy", (*itr)["SCALy"].GetDouble());
 //                       scene.attributes.setAttribute("SCALx", (*itr)["SCALx"].GetDouble());
 //                   }

 //                   scene.attributes.setAttribute("AO1", (*itr)["AO1"].GetDouble());
 //                   scene.attributes.setAttribute("MODL", (*itr)["MODL"].GetString());
 //                   scene.attributes.setAttribute("modelpath", (*itr)["modelpath"].GetString());

 //                   scenes.push_back(scene);

 //               }
 //               featurePositions.close();
 //           }
 //       }
		
        //	double lat = -1;
        //	double lon = -1;
        //	double SCALz = -1;
        //	double SCALy = -1;
        //	double SCALx = -1;
        //	double AO1 = -1;
        //	if ((*itr).HasMember("Lat"))
        //	{
        //		lat = (*itr)["Lat"].GetDouble();
        //	}
        //	if ((*itr).HasMember("Lon"))
        //	{
        //		lon = (*itr)["Lon"].GetDouble();
        //	}
        //	if (lat == -1 || lon == -1)
        //	{
        //		continue;
        //	}
        //	/*double lat = (*itr)["Lat"].GetDouble();
        //	double lon = (*itr)["Lon"].GetDouble();
        //	double SCALz = (*itr)["SCALz"].GetDouble();
        //	double SCALy = (*itr)["SCALy"].GetDouble();
        //	double SCALx = (*itr)["SCALx"].GetDouble();
        //	double AO1 = (*itr)["AO1"].GetDouble();*/
        //	//std::string modelPath = (*itr)["mapDiffuse"].GetString();
        //			
        //	int y = flatEarth.convertGeoToLocalY(lat);
        //	int x = flatEarth.convertGeoToLocalX(lon);
        //	sfa::Point point(x, y);
        //	featurePositions << x << " " << y << "\n";

        //	/*scenegraph::Scene child;
        //	ParseJSON(modelPath, "tmp", child);

        //	std::experimental::filesystem::path path = modelPath;
        //	path.replace_extension(".flt");
        //	auto filenameOfModel = path.filename();

        //	child.name = filenameOfModel.string();
        //	parent.addChild(&child);	

        //	feature.push_back(point);
        //	scenegraph::ExternalReference ref;
        //	parent.externalReferences.push_back(ref);*/

        //	// copy texture into outputDir
        //	/*std::experimental::filesystem::copy("", "");*/
	//}

    void TerrainGenerator::generateFixedGrid(const std::string &elevFile, const std::string &outputName, const std::string &featurePath, int windowTop, int windowBottom, int windowRight, int windowLeft)
    {
        ccl::FileInfo fi(outputName);
        std::string tileName = fi.getBaseName(true);
        std::string jpgFilename = ccl::joinPaths(outputPath, tileName + ".jpg");
        std::string attrFilename = jpgFilename + ".attr";
		std::string format = fi.getSuffix();

        OGRSpatialReference oSRS;
        oSRS.SetWellKnownGeogCS("WGS84");
        elev::SimpleDEMReader demReader(elevFile, oSRS, windowTop, windowBottom, windowRight, windowLeft, 0.5f);
        demReader.Open();
        int width = demReader.getScaledWidth();
        int height = demReader.getScaledHeight();
        std::vector<double> grid;
        demReader.getMBR(north, south, east, west);
        demReader.getGrid(grid);
        flatEarth.setOrigin((north + south) / 2, (east + west) / 2);

        double tileWorldNorth = north;
        double tileWorldSouth = south;

        double tileWorldWest = west;
        double tileWorldEast = east;

        localWest = flatEarth.convertGeoToLocalX(west);
        localEast = flatEarth.convertGeoToLocalX(east);
        localNorth = flatEarth.convertGeoToLocalY(north);
        localSouth = flatEarth.convertGeoToLocalY(south);
        double localWidth = localEast - localWest;
        double localHeight = localNorth - localSouth;
        logger << ccl::LINFO << "Using Elevation File MBR: N:" << north << "(" << localNorth << ") S:" << south << "(" << localSouth << ") W:" << west << "(" << localWest << ") E:" << east << "(" << localEast << ")" << logger.endl;
		
		double minElev = grid[0];
		double maxElev = grid[0];
		for (int i = 0; i < grid.size(); ++i)
		{
			minElev = std::min(grid[i], minElev);
			maxElev = std::max(grid[i], maxElev);
		}

        // create texture
        {
            gdalsampler::GeoExtents window;
            window.north = tileWorldNorth;
            window.south = tileWorldSouth;
            window.west = tileWorldWest;
            window.east = tileWorldEast;
            window.filename = tileName;
            window.width = textureWidth;
            window.height = textureHeight;
            int len = window.height * window.width * 3;
            u_char *buf = new u_char[len];
            memset(buf, 255, len);
            ip::ImageInfo info;
            info.width = window.width;
            info.height = window.height;
            info.depth = 3;
            info.interleaved = true;
            info.dataType = ip::ImageInfo::UBYTE;
            ccl::binary buffer;
            buffer.resize(len);
            rasterSampler.Sample(window, buf);
			int counter = 0;
			for (int i = 0; i < window.height; ++i)
			{
				int rowIndex = (window.height - i - 1) * (window.width * 3);
				for (int j = 0; j < window.width * 3; j++)
				{
					buffer[rowIndex++] = buf[counter++];
				}
			}
            ip::WriteJPG24(jpgFilename, info, buffer);

            ExportTextureMetaData(tileName);
            
            delete[] buf;
        }

        ctl::PointList gamingArea;
        {
            double z = 0;
            ctl::Point southwest(localWest, localSouth, grid[(width*(height - 2))]);
            ctl::Point southeast(localEast, localSouth, grid[(width*(height - 1)) - 1]);
            ctl::Point northeast(localEast, localNorth, grid[width-1]);
            ctl::Point northwest(localWest, localNorth, grid[0]);
            gamingArea.push_back(southwest);
            gamingArea.push_back(southeast);
            gamingArea.push_back(northeast);
            gamingArea.push_back(northwest);
        }
        double spacingX = demReader.getPostSpacingX();
        double spacingY = demReader.getPostSpacingY();

        //check for features present in this tile, to get their elevation
		//models
        for (int i = 0; i < buildings.Count(); ++i)
        {
            float lat = buildings.GetBuilding(i).lat;
            float lon = buildings.GetBuilding(i).lon;
            if (lat < north && lat > south && lon < east && lon > west)
            {
                int row = (lat - north) / spacingY;
                int col = (lon - west) / spacingX;
                float elev = grid[(row*width) + col];
                buildings.GetBuilding(i).elev = elev;
            }
        }

		//tree points
		for (int i = 0; i < trees.size(); ++i)
		{
			float lon = trees[i].treePoints[0].worldPosition.fLon;
			float lat = trees[i].treePoints[0].worldPosition.fLat;
			if (lat < north && lat > south && lon < east && lon > west)
			{
				int row = (lat - north) / spacingY;
				int col = (lon - west) / spacingX;
				float elev = grid[(row*width) + col];
				trees[i].treePoints[0].elev = elev;
			}
		}

        ExportFeaturesMetaData();

        ctl::PointList boundaryPoints;
        {
            sfa::LineString boundaryLineString;
            // Left boundary
            int col = 0;
            double lon = west;
            double localPostX = flatEarth.convertGeoToLocalX(lon);
            for (int row = 0; row < height; row++)
            {   // Go from pixel space to geo
                double lat = (row * spacingY) + north;
                // Go from geo to local                
                double localPostY = flatEarth.convertGeoToLocalY(lat);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, grid[(row*width)+col]));
            }
            // Right boundary
            col = width - 1;
            lon = east;
            localPostX = flatEarth.convertGeoToLocalX(lon);
            for (int row = 0; row < height; row++)
            {   // Go from pixel space to geo
                double lat = (row * spacingY) + north;
                // Go from geo to local
                double localPostY = flatEarth.convertGeoToLocalY(lat);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, grid[(row*width) + col]));
            }
            // Bottom boundary
            double lat = south;
            double localPostY = flatEarth.convertGeoToLocalY(lat);
            for (int col = 0; col < width; col++)
            {   // Go from pixel space to geo
                double lon = (col * spacingX) + west;
                // Go from geo to local
                double localPostX = flatEarth.convertGeoToLocalX(lon);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, grid[(width*(height-1)) + col]));
            }
            // Top boundary
            lat = north;
            localPostY = flatEarth.convertGeoToLocalY(lat);
            for (int col = 0; col < width; col++)
            {   // Go from pixel space to geo
                double lon = (col * spacingX) + west;
                // Go from geo to local
                double localPostX = flatEarth.convertGeoToLocalX(lon);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, grid[col]));
            }

//            boundaryLineString.removeColinearPoints(0, 0.5);
            for (int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
            {
                sfa::Point *p = boundaryLineString.getPointN(i);
                boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
            }
        }

		if (format == ".obj" || format == "obj")
		{
            BuildFromTriangulation(outputName, width, height, spacingX, spacingY, localWidth, localHeight, grid);
			return;
		}

        ctl::PointList workingPoints;

        int delaunayResizeIncrement = 100;
        {
            for (int row = 1; row < height-2; ++row)
            {
                printf("");
                for (int col = 1; col < width-2; ++col)
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

        // TODO: Allocate this based on a polygon budget
        ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(gamingArea, delaunayResizeIncrement);

        //Randomly the order of point insertions to avoid worst case performance of DelaunayTriangulation
        std::random_shuffle(boundaryPoints.begin(), boundaryPoints.end());
        std::random_shuffle(workingPoints.begin(), workingPoints.end());

        {
            //Alternate inserting boundary and working points to avoid worst case performance of DelaunayTriangulation
            size_t i = 0;
            size_t j = 0;
            while (i < boundaryPoints.size() || j < workingPoints.size())
            {
                if (i < boundaryPoints.size())
                    dt->InsertConstrainedPoint(boundaryPoints[i++]);
                if (j < workingPoints.size())
                    dt->InsertWorkingPoint(workingPoints[j++]);
            }
        }

        //dt->Simplify(1, float(0.05));    // simplify based on coplanar points
        //dt->Simplify(20000, 0.5);        // if we still have over 20k triangles, simplify using the triangle budget

        ctl::TIN *tin = new ctl::TIN(dt);
        scenegraph::Scene *scene = new scenegraph::Scene;
        scene->faces.reserve(tin->triangles.size() / 3);
        for (size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
        {
            // get the triangle points from the ctl tin
            ctl::Point pa = tin->verts[tin->triangles[i * 3 + 0]];
            ctl::Point pb = tin->verts[tin->triangles[i * 3 + 1]];
            ctl::Point pc = tin->verts[tin->triangles[i * 3 + 2]];
            sfa::Point sfaA = sfa::Point(pa.x, pa.y, pa.z);
            sfa::Point sfaB = sfa::Point(pb.x, pb.y, pb.z);
            sfa::Point sfaC = sfa::Point(pc.x, pc.y, pc.z);

            // get the normals from the ctl tin
            ctl::Vector na = tin->normals[tin->triangles[i * 3 + 0]];
            ctl::Vector nb = tin->normals[tin->triangles[i * 3 + 1]];
            ctl::Vector nc = tin->normals[tin->triangles[i * 3 + 2]];
            sfa::Point sfaAN = sfa::Point(na.x, na.y, na.z);
            sfa::Point sfaBN = sfa::Point(nb.x, nb.y, nb.z);
            sfa::Point sfaCN = sfa::Point(nc.x, nc.y, nc.z);

            // create the new face
            scenegraph::Face face;
            face.verts.push_back(sfaA);
            face.verts.push_back(sfaB);
            face.verts.push_back(sfaC);
            face.vertexNormals.push_back(sfaAN);
            face.vertexNormals.push_back(sfaBN);
            face.vertexNormals.push_back(sfaCN);

            face.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            face.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);

            // Add texturing
            scenegraph::MappedTexture mt;
            mt.SetTextureName(jpgFilename);
            // Each texture should map to the tile extents directly
            // So create a transform from the tile boundaries to local coordinates
            mt.uvs.push_back(sfa::Point((sfaA.X() - localWest) / localWidth, (localNorth - sfaA.Y()) / localHeight));
            mt.uvs.push_back(sfa::Point((sfaB.X() - localWest) / localWidth, (localNorth - sfaB.Y()) / localHeight));
            mt.uvs.push_back(sfa::Point((sfaC.X() - localWest) / localWidth, (localNorth - sfaC.Y()) / localHeight));
            face.textures.push_back(mt);

            scene->faces.push_back(face);
        }
		
        logger << "Writing " << outputName << "..." << logger.endl;

        BuildFromScene(outputName, scene, localWidth, localHeight);

        scenegraph::ExternalReference ext;
        ext.scale = sfa::Point(1.0, 1.0, 1.0);
        ext.filename = outputName;
		if (format == "b3dm")
		{
			ext.position.setX(west);
			ext.position.setY(south);
			ext.position.setZ(minElev);
			ext.scale.setX(east - west);
			ext.scale.setY(north - south);
			ext.scale.setZ(maxElev - minElev);
		}
        master.externalReferences.push_back(ext);

		delete tin;
        delete dt;
    }

    void TerrainGenerator::generateFixedGrid(const std::string &imgFile, const std::string &outputPath, const std::string &outputName, std::string format, elev::Elevation_DSM& edsm, double north, double south, double east, double west)
    {
        std::string outputFileName = ccl::joinPaths(outputPath, outputName + format);
        // open imgFile which is a tif.
        GDALDataset  *poDataset;
        GDALAllRegister();
        poDataset = (GDALDataset *)GDALOpen(imgFile.c_str(), GA_ReadOnly);
        if (poDataset == NULL)
        {
            return;
        }
        int rasterWidth = poDataset->GetRasterXSize();
        int rasterHeight = poDataset->GetRasterYSize();
        int len = rasterWidth * rasterHeight * 3;
        unsigned char* buf = new unsigned char[len];
        unsigned char* bufPtr = buf;

        for (int i = 0; i < 3; i++)
        {
            auto pBand = poDataset->GetRasterBand(i + 1);
            pBand->RasterIO(GF_Read, 0, 0, rasterWidth, rasterHeight, bufPtr + i, rasterWidth, rasterHeight, GDT_Byte, 3, 3 * rasterWidth);
        }

        std::string jpgFilename = ccl::joinPaths(outputPath, outputName + ".jpg");
        ExportTextureMetaData(jpgFilename);
        ip::ImageInfo info;
        info.width = rasterWidth;
        info.height = rasterHeight;
        info.depth = 3;
        info.interleaved = true;
        info.dataType = ip::ImageInfo::UBYTE;
        ccl::binary buffer;
        buffer.resize(len);
        int counter = 0;
        for (int i = 0; i < len; i++)
        {
            buffer[i] = buf[i];
        }
        ip::WriteJPG24(jpgFilename, info, buffer);
        ExportTextureMetaData(jpgFilename);
        delete[] buf;
		ccl::FileInfo fi(outputName);
		std::string tileName = fi.getBaseName(true);

		flatEarth.setOrigin((north + south) / 2, (east + west) / 2);

		double tileWorldNorth = north;
		double tileWorldSouth = south;
		double tileWorldWest = west;
		double tileWorldEast = east;

		localWest = flatEarth.convertGeoToLocalX(west);
		localEast = flatEarth.convertGeoToLocalX(east);
		localNorth = flatEarth.convertGeoToLocalY(north);
		localSouth = flatEarth.convertGeoToLocalY(south);
		double localWidth = localEast - localWest;
		double localHeight = localNorth - localSouth;
		logger << ccl::LINFO << "Using Elevation File MBR: N:" << north << "(" << localNorth << ") S:" << south << "(" << localSouth << ") W:" << west << "(" << localWest << ") E:" << east << "(" << localEast << ")" << logger.endl;

        int nSamples = 100;

        double spacingX = (north - south) / nSamples;
        double spacingY = -(east - west) / nSamples;
		
		ctl::PointList workingPoints;

        sfa::Point p;
        ctl::PointList gamingArea;
        {
            double z = 0;
            p.setX(west);
            p.setY(south);
            edsm.Get(&p);
            ctl::Point southwest(localWest, localSouth, p.Z());
            p.setX(east);
            p.setY(south);
            edsm.Get(&p);
            ctl::Point southeast(localEast, localSouth, p.Z());
            p.setX(east);
            p.setY(north);
            edsm.Get(&p);
            ctl::Point northeast(localEast, localNorth, p.Z());
            p.setX(west);
            p.setY(north);
            edsm.Get(&p);
            ctl::Point northwest(localWest, localNorth, p.Z());
            gamingArea.push_back(southwest);
            gamingArea.push_back(southeast);
            gamingArea.push_back(northeast);
            gamingArea.push_back(northwest);
        }

        ctl::PointList boundaryPoints;
        {
            sfa::LineString boundaryLineString;
            // Left boundary
            int col = 0;
            double lon = west;
            p.setX(lon);
            double localPostX = flatEarth.convertGeoToLocalX(lon);
            for (int row = 0; row < nSamples; row++)
            {   // Go from pixel space to geo
                double lat = (row * spacingY) + north;
                // Go from geo to local                
                double localPostY = flatEarth.convertGeoToLocalY(lat);
                p.setY(lat);
                edsm.Get(&p);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
            }
            // Right boundary
            col = nSamples - 1;
            lon = east;
            p.setX(lon);
            localPostX = flatEarth.convertGeoToLocalX(lon);
            for (int row = 0; row < nSamples; row++)
            {   // Go from pixel space to geo
                double lat = (row * spacingY) + north;
                // Go from geo to local
                p.setY(lat);
                edsm.Get(&p);
                double localPostY = flatEarth.convertGeoToLocalY(lat);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
            }
            // Bottom boundary
            double lat = south;
            p.setY(lat);
            double localPostY = flatEarth.convertGeoToLocalY(lat);
            for (int col = 0; col < nSamples; col++)
            {   // Go from pixel space to geo
                double lon = (col * spacingX) + west;
                // Go from geo to local
                p.setX(lon);
                edsm.Get(&p);
                double localPostX = flatEarth.convertGeoToLocalX(lon);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
            }
            // Top boundary
            lat = north;
            p.setY(lat);
            localPostY = flatEarth.convertGeoToLocalY(lat);
            for (int col = 0; col < nSamples; col++)
            {   // Go from pixel space to geo
                double lon = (col * spacingX) + west;
                // Go from geo to local
                p.setX(lon);
                edsm.Get(&p);
                double localPostX = flatEarth.convertGeoToLocalX(lon);
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
            }

            //            boundaryLineString.removeColinearPoints(0, 0.5);
            for (int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
            {
                sfa::Point *p = boundaryLineString.getPointN(i);
                boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
            }
        }


        int delaunayResizeIncrement = 100;
        {
            for (int row = 1; row < nSamples - 2; ++row)
            {
                printf("");
                for (int col = 1; col < nSamples - 2; ++col)
                {
                    // Go from pixel space to geo
                    double lat = (row * spacingY) + north;
                    double lon = (col * spacingX) + west;
                    // Go from geo to local
                    double localPostX = flatEarth.convertGeoToLocalX(lon);
                    double localPostY = flatEarth.convertGeoToLocalY(lat);
                    p.setX(lon);
                    p.setY(lat);
                    edsm.Get(&p);
                    workingPoints.push_back(ctl::Point(localPostX, localPostY, p.Z()));
                }
            }

            delaunayResizeIncrement = (nSamples * nSamples) / 8;
        }

        // TODO: Allocate this based on a polygon budget
        ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(gamingArea, delaunayResizeIncrement);

        //Randomly the order of point insertions to avoid worst case performance of DelaunayTriangulation
        std::random_shuffle(boundaryPoints.begin(), boundaryPoints.end());
        std::random_shuffle(workingPoints.begin(), workingPoints.end());

        {
            //Alternate inserting boundary and working points to avoid worst case performance of DelaunayTriangulation
            size_t i = 0;
            size_t j = 0;
            while (i < boundaryPoints.size() || j < workingPoints.size())
            {
                if (i < boundaryPoints.size())
                    dt->InsertConstrainedPoint(boundaryPoints[i++]);
                if (j < workingPoints.size())
                    dt->InsertWorkingPoint(workingPoints[j++]);
            }
        }

        dt->Simplify(1, float(0.05));    // simplify based on coplanar points
        //dt->Simplify(20000, 0.5);        // if we still have over 20k triangles, simplify using the triangle budget

        ctl::TIN *tin = new ctl::TIN(dt);
        scenegraph::Scene *scene = new scenegraph::Scene;
        scene->faces.reserve(tin->triangles.size() / 3);
        for (size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
        {
            // get the triangle points from the ctl tin
            ctl::Point pa = tin->verts[tin->triangles[i * 3 + 0]];
            ctl::Point pb = tin->verts[tin->triangles[i * 3 + 1]];
            ctl::Point pc = tin->verts[tin->triangles[i * 3 + 2]];
            sfa::Point sfaA = sfa::Point(pa.x, pa.y, pa.z);
            sfa::Point sfaB = sfa::Point(pb.x, pb.y, pb.z);
            sfa::Point sfaC = sfa::Point(pc.x, pc.y, pc.z);

            // get the normals from the ctl tin
            ctl::Vector na = tin->normals[tin->triangles[i * 3 + 0]];
            ctl::Vector nb = tin->normals[tin->triangles[i * 3 + 1]];
            ctl::Vector nc = tin->normals[tin->triangles[i * 3 + 2]];
            sfa::Point sfaAN = sfa::Point(na.x, na.y, na.z);
            sfa::Point sfaBN = sfa::Point(nb.x, nb.y, nb.z);
            sfa::Point sfaCN = sfa::Point(nc.x, nc.y, nc.z);

            // create the new face
            scenegraph::Face face;
            face.verts.push_back(sfaA);
            face.verts.push_back(sfaB);
            face.verts.push_back(sfaC);
            face.vertexNormals.push_back(sfaAN);
            face.vertexNormals.push_back(sfaBN);
            face.vertexNormals.push_back(sfaCN);

            face.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            face.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);

            // Add texturing
            scenegraph::MappedTexture mt;
            mt.SetTextureName(jpgFilename);
            // Each texture should map to the tile extents directly
            // So create a transform from the tile boundaries to local coordinates
            if (IsObjOutput(format))
            {
                mt.uvs.push_back(sfa::Point((sfaA.X() - localWest) / localWidth, (sfaA.Y() - localSouth) / localHeight));
                mt.uvs.push_back(sfa::Point((sfaB.X() - localWest) / localWidth, (sfaB.Y() - localSouth) / localHeight));
                mt.uvs.push_back(sfa::Point((sfaC.X() - localWest) / localWidth, (sfaC.Y() - localSouth) / localHeight));
            }
            else
            {
                mt.uvs.push_back(sfa::Point((sfaA.X() - localWest) / localWidth, (localNorth - sfaA.Y()) / localHeight));
                mt.uvs.push_back(sfa::Point((sfaB.X() - localWest) / localWidth, (localNorth - sfaB.Y()) / localHeight));
                mt.uvs.push_back(sfa::Point((sfaC.X() - localWest) / localWidth, (localNorth - sfaC.Y()) / localHeight));
            }
            face.textures.push_back(mt);

            scene->faces.push_back(face);
        }

        logger << "Writing " << outputName << "..." << logger.endl;

		std::string outputExportName = ccl::joinPaths(outputPath, outputName + format);

		SetBuildingElevations(edsm);

		if (!IsGltfTypeOutput(format))
		{
			BuildFromScene(outputExportName, scene, localWidth, localHeight);
		}

        scenegraph::ExternalReference ext;
        ext.scale = sfa::Point(1.0, 1.0, 1.0);
        ext.filename = outputExportName;
        if (IsGltfTypeOutput(format))
        {
            double minElev = DBL_MAX;
            double maxElev = DBL_MIN;
            for (int row = 0; row < nSamples; ++row)
            {
                for (int col = 0; col < nSamples; ++col)
                {
                    // Go from pixel space to geo
                    double lat = (row * spacingY) + north;
                    double lon = (col * spacingX) + west;
					sfa::Point p1;
					p1.setX(lon);
                    p1.setY(lat);
                    edsm.Get(&p1);
                    auto z = p1.Z();
                    minElev = std::min(z, minElev);
                    maxElev = std::max(z, maxElev);
                }
            }

			ext.groupID = 1;
            ext.position.setX(west);
            ext.position.setY(south);
            ext.position.setZ(minElev);
            ext.scale.setX(east - west);
            ext.scale.setY(north - south);
            ext.scale.setZ(maxElev - minElev);

			scenegraph::buildGltfFromScene(outputExportName, scene, north, south, east, west, 0 /*minElev*/, maxElev, 1);

        }
        master.externalReferences.push_back(ext);

        /*if (format != "obj" || format != "fbx")
        {
            delete scene;
        }*/
        delete tin;
        delete dt;
	}

	/*void TerrainGenerator::generateFeatures(const std::string &featureFile, const std::string &outputName, GsBuildings *features, const std::vector<double> &grid, int spacingX, int spacingY, int width)
	{
		features->ProcessBuildingData(featureFile);

		scenegraph::Scene scene;

		for (int i = 0; i < features->Count(); ++i)
		{
			float lat = features->GetBuilding(i).lat;
			float lon = features->GetBuilding(i).lon;

			int row = (lat - north) / spacingY;
			int col = (lon - west) / spacingX;
			//float elev = grid[(row*width) + col];
			//features->GetBuilding(i).elev = elev;

			double y = flatEarth.convertGeoToLocalY(lat);
			double x = flatEarth.convertGeoToLocalX(lon);
			double z = features->GetBuilding(i).elev; 
			double ao1 = features->GetBuilding(i).AO1;
			double scalex = features->GetBuilding(i).scaleX;
			double scaley = features->GetBuilding(i).scaleY;
			double scalez = features->GetBuilding(i).scaleZ;
			std::string modelPath = features->GetBuilding(i).modelpath;

			ParseJSON(modelPath, outputPath, scene);
			ccl::FileInfo fi(features->GetBuilding(i).modelpath);
			std::string featureFilename = outputPath + fi.getBaseName(true);
			featureFilename += ".obj";
			scenegraph::buildObjFromScene(featureFilename, 0, 0, 0, 0, &scene);
		}
	}*/

    /*void TerrainGenerator::generateRowColumn(int row, int col)
    {
        std::string tileName = "tile_" + boost::lexical_cast<std::string>(row) + "_" + boost::lexical_cast<std::string>(col);
        std::string jpgFilename = ccl::joinPaths(outputPath, tileName + ".jpg");
        std::string attrFilename = jpgFilename + ".attr";

        logger << ccl::LINFO << "Generating Tile (" << row << ", " << col << ")" << logger.endl;

        double tileLocalNorth = localNorth - (row * (textureHeight * texelSize));
        double tileLocalSouth = tileLocalNorth - (textureHeight * texelSize);
        double tileWorldNorth = flatEarth.convertLocalToGeoLat(tileLocalNorth);
        double tileWorldSouth = flatEarth.convertLocalToGeoLat(tileLocalSouth);
        double tileLocalHeight = tileLocalNorth - tileLocalSouth;

        double tileLocalWest = localWest + (col * (textureWidth * texelSize));
        double tileLocalEast = tileLocalWest + (textureWidth * texelSize);
        double tileWorldWest = flatEarth.convertLocalToGeoLon(tileLocalWest);
        double tileWorldEast = flatEarth.convertLocalToGeoLon(tileLocalEast);
        double tileLocalWidth = tileLocalEast - tileLocalWest;

        // TODO: Add the ability to introspect the elevation sampler to get the range of
        //       elevation post space.
        double postSpacingX = 30.0f;
        double postSpacingY = 30.0f;

        // create texture
        {
            gdalsampler::GeoExtents window;
            window.north = tileWorldNorth;
            window.south = tileWorldSouth;
            window.west = tileWorldWest;
            window.east = tileWorldEast;
            window.filename = tileName;
            window.width = textureWidth;
            window.height = textureHeight;
            int len = window.height * window.width * 3;
            u_char *buf = new u_char[len];
            memset(buf, 255, len);
            ip::ImageInfo info;
            info.width = window.width;
            info.height = window.height;
            info.depth = 3;
            info.interleaved = true;
            info.dataType = ip::ImageInfo::UBYTE;
            ccl::binary buffer;
            buffer.resize(len);
            rasterSampler.Sample(window,buf);
			int counter = 0;
			for (int i = 0; i < window.height; ++i)
			{
				int rowIndex = (window.height - i - 1) * (window.width * 3);
				for (int j = 0; j < window.width * 3; j++)
				{
					buffer[rowIndex++] = buf[counter++];
				}
			}
            ip::WriteJPG24(jpgFilename, info, buffer);

			
				ip::attrFile attr;
				attr.wrapMode = ip::attrFile::WRAP_CLAMP;
				attr.wrapMode_u = ip::attrFile::WRAP_CLAMP;
				attr.wrapMode_v = ip::attrFile::WRAP_CLAMP;
				attr.Write(attrFilename);
            
            delete[] buf;
        }

        ctl::PointList gamingArea;
        {
            double z = 0;
            ctl::Point southwest(tileLocalWest, tileLocalSouth, getZ(tileLocalWest, tileLocalSouth));
            ctl::Point southeast(tileLocalEast, tileLocalSouth, getZ(tileLocalEast, tileLocalSouth));
            ctl::Point northeast(tileLocalEast, tileLocalNorth, getZ(tileLocalEast, tileLocalNorth));
            ctl::Point northwest(tileLocalWest, tileLocalNorth, getZ(tileLocalWest, tileLocalNorth));
            gamingArea.push_back(southwest);
            gamingArea.push_back(southeast);
            gamingArea.push_back(northeast);
            gamingArea.push_back(northwest);
        }

        ctl::PointList boundaryPoints;
        {
            sfa::LineString boundaryLineString;
            for(double x = tileLocalWest + postSpacingX; x < tileLocalEast; x += postSpacingX)
                boundaryLineString.addPoint(sfa::Point(x, tileLocalNorth, getZ(x, tileLocalNorth)));
            for(double y = tileLocalSouth + postSpacingY; y < tileLocalNorth; y += postSpacingY)
                boundaryLineString.addPoint(sfa::Point(tileLocalWest, y, getZ(tileLocalWest, y)));
            for(double x = tileLocalWest + postSpacingX; x < tileLocalEast; x += postSpacingX)
                boundaryLineString.addPoint(sfa::Point(x, tileLocalSouth, getZ(x, tileLocalSouth)));
            for(double y = tileLocalSouth + postSpacingY; y < tileLocalNorth; y += postSpacingY)
                boundaryLineString.addPoint(sfa::Point(tileLocalEast, y, getZ(tileLocalEast, y)));
            boundaryLineString.removeColinearPoints(0, 0.5);
            for(int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
            {
                sfa::Point *p = boundaryLineString.getPointN(i);
                boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
            }
        }

        ctl::PointList workingPoints;
        int delaunayResizeIncrement = 100;
        {
            int postRows = ceil((tileLocalNorth - tileLocalSouth) / postSpacingY);
            int postCols = ceil((tileLocalEast - tileLocalWest) / postSpacingX);
            for(int row = 0; row < postRows; ++row)
            {
                double tileY = tileLocalSouth + (postSpacingY * row);
                if((tileY == tileLocalNorth) || (tileY == tileLocalSouth))
                    continue;    //The boundary constraints already handle this

                sfa::Point pt;
                double lat = flatEarth.convertLocalToGeoLat(tileY);
                pt.setY(lat);
                for(int col = 0; col < postCols; ++col)
                {
                    double tileX = tileLocalWest + (postSpacingX * col);
                    if((tileX == tileLocalWest) || (tileX == tileLocalEast))
                        continue;    //The boundary constraints already handle this

                    double lon = flatEarth.convertLocalToGeoLon(tileX);
                    pt.setX(lon);
                    if(elevationSampler.Get(&pt))
                        workingPoints.push_back(ctl::Point(tileX, tileY, pt.Z()));
                }
            }    

            delaunayResizeIncrement = (postRows * postCols) / 8;
        }

        // TODO: Allocate this based on a polygon budget
        ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(gamingArea, delaunayResizeIncrement);

        //Randomly the order of point insertions to avoid worst case performance of DelaunayTriangulation
        std::random_shuffle(boundaryPoints.begin(), boundaryPoints.end());
        std::random_shuffle(workingPoints.begin(), workingPoints.end());

        {
            //Alternate inserting boundary and working points to avoid worst case performance of DelaunayTriangulation
            size_t i = 0;
            size_t j = 0;
            while(i < boundaryPoints.size() || j < workingPoints.size())
            {
                if(i < boundaryPoints.size())
                    dt->InsertConstrainedPoint(boundaryPoints[i++]);
                if(j < workingPoints.size())
                    dt->InsertWorkingPoint(workingPoints[j++]);
            }
        }

        dt->Simplify(1, float(0.05));    // simplify based on coplanar points
        dt->Simplify(20000, 0.5);        // if we still have over 20k triangles, simplify using the triangle budget

        ctl::TIN *tin = new ctl::TIN(dt);
        scenegraph::Scene *scene = new scenegraph::Scene;
        scene->faces.reserve(tin->triangles.size() / 3);
        for(size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
        {
            // get the triangle points from the ctl tin
            ctl::Point pa = tin->verts[tin->triangles[i * 3 + 0]];
            ctl::Point pb = tin->verts[tin->triangles[i * 3 + 1]];
            ctl::Point pc = tin->verts[tin->triangles[i * 3 + 2]];
            sfa::Point sfaA = sfa::Point(pa.x, pa.y, pa.z);
            sfa::Point sfaB = sfa::Point(pb.x, pb.y, pb.z);
            sfa::Point sfaC = sfa::Point(pc.x, pc.y, pc.z);

            // get the normals from the ctl tin
            ctl::Vector na = tin->normals[tin->triangles[i * 3 + 0]];
            ctl::Vector nb = tin->normals[tin->triangles[i * 3 + 1]];
            ctl::Vector nc = tin->normals[tin->triangles[i * 3 + 2]];
            sfa::Point sfaAN = sfa::Point(na.x, na.y, na.z);
            sfa::Point sfaBN = sfa::Point(nb.x, nb.y, nb.z);
            sfa::Point sfaCN = sfa::Point(nc.x, nc.y, nc.z);

            // create the new face
            scenegraph::Face face;
            face.verts.push_back(sfaA);
            face.verts.push_back(sfaB);
            face.verts.push_back(sfaC);
            face.vertexNormals.push_back(sfaAN);
            face.vertexNormals.push_back(sfaBN);
            face.vertexNormals.push_back(sfaCN);

            face.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            face.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);

            // Add texturing
            scenegraph::MappedTexture mt;
            mt.SetTextureName(jpgFilename);
            // Each texture should map to the tile extents directly
            // So create a transform from the tile boundaries to local coordinates
            mt.uvs.push_back(sfa::Point((sfaA.X() - tileLocalWest) / tileLocalWidth, (tileLocalNorth - sfaA.Y()) / tileLocalHeight));
            mt.uvs.push_back(sfa::Point((sfaB.X() - tileLocalWest) / tileLocalWidth, (tileLocalNorth - sfaB.Y()) / tileLocalHeight));
            mt.uvs.push_back(sfa::Point((sfaC.X() - tileLocalWest) / tileLocalWidth, (tileLocalNorth - sfaC.Y()) / tileLocalHeight));
            face.textures.push_back(mt);

            scene->faces.push_back(face);
        }

        std::string fltfilename = ccl::joinPaths(outputPath, tileName + std::string(".flt"));
        logger << "Writing " << fltfilename << "..." << logger.endl;
        scenegraph::buildOpenFlightFromScene(fltfilename, scene);
        scenegraph::ExternalReference ext;
        ext.scale = sfa::Point(1.0, 1.0, 1.0);
        ext.filename = fltfilename;
        master.externalReferences.push_back(ext);

        delete scene;
        delete tin;
        delete dt;
    }*/

    double TerrainGenerator::getZ(double x, double y)
    {
        sfa::Point p(flatEarth.convertLocalToGeoLon(x), flatEarth.convertLocalToGeoLat(y));
        return elevationSampler.Get(&p) ? p.Z() : 0.0f;
    }

    void TerrainGenerator::parseFeatures(const std::string &outputPath)
    {
        //ParseCapabilities(featurePath + "capabilities.xml");
        //ParseCountryBoundaries(featurePath + "CountryBoundaries2.xml", north, south, east, west);
        std::string dataPath = ccl::joinPaths(outputPath, "data");
        buildings.ProcessBuildingData(ccl::joinPaths(dataPath, "building_models.xml"));
        ParseManMadePoints_Footprints(ccl::joinPaths(dataPath, "building_footprints.xml"), footprints);
        ParseTreePoints(ccl::joinPaths(dataPath, "tree_points.xml"));
    }

	bool TerrainGenerator::IsGltfTypeOutput(const std::string& outputFormat)
	{
		return (outputFormat == ".gltf" || outputFormat == ".glb" || outputFormat == ".b3dm" || outputFormat == ".tileset" ||
			outputFormat == "gltf" || outputFormat == "glb" || outputFormat == "b3dm" || outputFormat == "tileset");
	}

	bool TerrainGenerator::IsObjOutput(const std::string& outputFormat)
	{
		return (outputFormat == ".obj" || outputFormat == "obj");
	}

    bool TerrainGenerator::IsFbxOutput(const std::string& outputFormat)
    {
        return (outputFormat == ".fbx" || outputFormat == "fbx");
    }

	bool TerrainGenerator::IsOpenFlightOutput(const std::string& outputFormat)
	{
		return (outputFormat == ".flt" || outputFormat == "flt");
	}

	double TerrainGenerator::ConvertGeoToLocalX(double lon)
	{
		return flatEarth.convertGeoToLocalX(lon);
	}

	double TerrainGenerator::ConvertGeoToLocalY(double lat)
	{
		return flatEarth.convertGeoToLocalY(lat);
	}

    void TerrainGenerator::transformSceneFromFlatEarthToUTM(scenegraph::Scene& scene, const std::string& utm_zone)
    {
        bool zone_north = utm_zone[utm_zone.size() - 1] == 'N';
        int zone_id = atoi(utm_zone.substr(0, utm_zone.size() - 1).c_str());
        if (zone_id <= 0)
            throw std::runtime_error("invalid zone");

        cts::CS_CoordinateSystemFactory coordinateSystemFactory;
        cts::CT_MathTransformFactory mathTransformFactory;
        cts::CS_CoordinateSystem* wgs84 = coordinateSystemFactory.createFromWKT("WGS84");
        cts::WGS84FromFlatEarthMathTransform* fromFlatEarth = new cts::WGS84FromFlatEarthMathTransform(flatEarth.getOriginLatitude(), flatEarth.getOriginLongitude());

        OGRSpatialReference utm_sr;
        utm_sr.SetWellKnownGeogCS("WGS84");
        utm_sr.SetUTM(zone_id, zone_north);
        char *wktp;
        utm_sr.exportToWkt(&wktp);
        std::string wkt(wktp);
        cts::CS_CoordinateSystem *utm = coordinateSystemFactory.createFromWKT(wkt);

        // transform from flat earth to geographic
        scenegraph::CoordinateTransformVisitor transformVisitor1(fromFlatEarth);
        transformVisitor1.transform(&scene);

        // transform from geographic to UTM
        cts::CT_MathTransform *mt = mathTransformFactory.createFromOGR(wgs84, utm);
        scenegraph::CoordinateTransformVisitor transformVisitor2(mt);
        transformVisitor2.transform(&scene);

        delete mt;
        delete utm;
    }

	void TerrainGenerator::GetElevation(const std::string& geoServerURL, double north, double south, double east, double west, int& textureWidth, int& textureHeight, const std::string& outputPath)
	{
		GDALDataset* elevationDataset;
		char** papszDrivers = NULL;
		papszDrivers = CSLAddString(papszDrivers, "WCS");
		char** papszOptions = NULL;
		papszOptions = CSLAddString(papszOptions, "GridCRSOptional");

		std::stringstream parameters;

		parameters << std::fixed << std::setprecision(15)
			<< "SERVICE=WCS&amp;FORMAT=GeoTIFF&amp;BOUNDINGBOX="
			<< south
			<< "," << west
			<< "," << north
			<< "," << east
			<< ",urn:ogc:def:crs:EPSG::4326&amp;WIDTH="
			<< std::to_string(textureWidth)
			<< "&amp;HEIGHT="
			<< std::to_string(textureHeight);

		std::string xml = "";
		xml += "<WCS_GDAL>";
		xml += "  <ServiceURL>" + geoServerURL + "/wcs?SERVICE=WCS</ServiceURL>";
		xml += "  <Version>1.1.1</Version>";
		xml += "  <CoverageName>CDB Elevation_Terrain_Primary</CoverageName>";
		xml += "  <Parameters>" + parameters.str() + "</Parameters>";
		xml += "  <GridCRSOptional>TRUE</GridCRSOptional>";
		xml += "  <CoverageDescription>";
		xml += "    <Identifier>Base:CDB Elevation_Terrain_Primary</Identifier>";
		xml += "    <Domain>";
		xml += "      <SpatialDomain>";
		xml += "        <BoundingBox crs=\"urn:ogc:def:crs:OGC:1.3 : CRS84\" dimensions=\"2\">";
		xml += "          <LowerCorner>-180.0 -90.0</LowerCorner>";
		xml += "          <UpperCorner>180.0 90.0</UpperCorner>";
		xml += "        </BoundingBox>";
		xml += "        <BoundingBox crs=\"urn:ogc:def:crs:EPSG::4326\" dimensions=\"2\">";
		xml += "          <LowerCorner>-90.0 -180.0</LowerCorner>";
		xml += "          <UpperCorner>90.0 180.0</UpperCorner>";
		xml += "        </BoundingBox>";
		xml += "        <BoundingBox crs=\":imageCRS\" dimensions=\"2\">";
		xml += "          <LowerCorner>0 0</LowerCorner>";
		xml += "          <UpperCorner>356356356 3563456</UpperCorner>";
		xml += "        </BoundingBox>";
		xml += "        <GridCRS>";
		xml += "          <GridBaseCRS>urn:ogc:def:crs:EPSG::4326</GridBaseCRS>";
		xml += "          <GridType>urn:ogc:def:method:WCS:1.1:2dGridIn2dCrs</GridType>";
		xml += "          <GridOrigin>-179.82421875 89.91259765625</GridOrigin>";
		xml += "          <GridOffsets>0.3515625 0.0 0.0 -0.1748046875</GridOffsets>";
		xml += "          <GridCS>urn:ogc:def:cs:OGC:0.0:Grid2dSquareCS</GridCS>";
		xml += "        </GridCRS>";
		xml += "      </SpatialDomain>";
		xml += "    </Domain>";
		xml += "    <SupportedCRS>urn:ogc:def:crs:EPSG::4326</SupportedCRS>";
		xml += "    <SupportedCRS>EPSG:4326</SupportedCRS>";
		xml += "    <SupportedFormat>image/tiff</SupportedFormat>";
		xml += "  </CoverageDescription>";
		xml += "  <FieldName>contents</FieldName>";
		xml += "  <BandType>Float32</BandType>";
		xml += "  <PreferredFormat>image/tiff</PreferredFormat>";
		xml += "</WCS_GDAL>";

		elevationDataset = (GDALDataset*)GDALOpenEx(xml.c_str(), GDAL_OF_READONLY, papszDrivers, papszOptions, NULL);

		auto band = elevationDataset->GetRasterBand(1);

		// set these values to something random to get the correct values.
		textureWidth = 2452;
		textureHeight = 2456;

		float *data = new float[textureWidth * textureHeight + 1];

		// attempt to read with the random values to produce an error message with the correct width and height
		band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);
		// extract the width and height from the error message
		std::string err = CPLGetLastErrorMsg();

		auto result = err.find("Got ");

		result += 4;

		err.erase(0, result);

		result = err.find('x');

		std::string firstnum = err.substr(0, result);

		err.erase(0, result + 1);

		result = err.find(' ');

		std::string secondnum = err.substr(0, result);
		// set the texture width and height to the extracted values
		textureWidth = std::stoi(firstnum);
		textureHeight = std::stoi(secondnum);

		// read with the correct values in textureWidht and textureHeight
		band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);

		// set any null elevation value to 0 to prevent errors when reading.
		for (int z = 0; z < textureHeight * textureWidth; z++)
		{
			if (std::isnan(data[z]))
			{
				data[z] = 0.0f;
			}
		}

		float* rotate90 = new float[textureWidth * textureHeight];

		for (int i = 0; i < textureWidth; ++i)
		{
			for (int j = 0; j < textureHeight; ++j)
			{
				rotate90[i * textureHeight + j] = data[j * textureWidth + i];
			}
		}

		auto tmp = textureWidth;
		textureWidth = textureHeight; 
		textureHeight = tmp;

		float* flipped = new float[textureWidth * textureHeight];
		float* lastRowSrc = rotate90 + (textureWidth * (textureHeight - 1));
		float* firstRowDst = flipped;

		for (int i = 0; i < textureHeight; i++)
		{
			float* forward = firstRowDst;
			float* backward = lastRowSrc + textureWidth - 1;
			for (int j = 0; j < textureWidth; j++)
			{
				*forward++ = *backward--;
			}
			firstRowDst += textureWidth;
			lastRowSrc -= textureWidth;
		}

		GDALDriver* poDriver;
		poDriver = GetGDALDriverManager()->GetDriverByName("GTIFF");
		GDALDataset *poDstDS;
		papszOptions[0] = NULL;
		papszOptions[1] = NULL;

		std::string path = outputPath;
		poDstDS = poDriver->Create(path.c_str(), textureWidth, textureHeight, 1, GDT_Float32, papszOptions);

		double adfGeoTransform[6];
		adfGeoTransform[0] = west;
		adfGeoTransform[1] = (east - west) / textureWidth;
		adfGeoTransform[2] = 0;
		adfGeoTransform[3] = north;
		adfGeoTransform[4] = 0;
		adfGeoTransform[5] = ((north - south) / textureHeight) * -1;

		OGRSpatialReference oSRS;
		char *pszSRS_WKT = NULL;
		GDALRasterBand *poBand;

		poDstDS->SetGeoTransform(adfGeoTransform);

		oSRS.SetWellKnownGeogCS("WGS84");
		oSRS.exportToWkt(&pszSRS_WKT);
		poDstDS->SetProjection(pszSRS_WKT);
		CPLFree(pszSRS_WKT);
		poBand = poDstDS->GetRasterBand(1);

		poBand->RasterIO(GF_Write, 0, 0, textureWidth, textureHeight, flipped, textureWidth, textureHeight, GDT_Float32, 4, 0);
		GDALClose(elevationDataset);
		delete poDstDS;
		delete[] data;
		delete[] rotate90;
		delete[] flipped;
	}

	void TerrainGenerator::GetImagery(const std::string& geoServerURL, double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath)
	{
		bool gdalError = false;

		/// TIFF /// 
		std::ostringstream xmlTiff;
		xmlTiff.precision(10);
		xmlTiff << std::fixed
			<< "<GDAL_WMS>"
			<< "<Service name=\"WMS\">"
			<< "<ServerUrl>" + geoServerURL + "/wms?</ServerUrl>"
			<< "<Layers>Base%3ACDB%20Imagery_YearRound</Layers>"
			<< "<ImageFormat>image/geotiff</ImageFormat>"
			<< "</Service>"
			<< "<DataWindow>"
			<< "<UpperLeftX>"
			<< west
			<< "</UpperLeftX>"
			<< "<UpperLeftY>"
			<< north
			<< "</UpperLeftY>"
			<< "<LowerRightX>"
			<< east
			<< "</LowerRightX>"
			<< "<LowerRightY>"
			<< south
			<< "</LowerRightY>"
			<< "<SizeX>"
			<< textureWidth
			<< "</SizeX>"
			<< "<SizeY>"
			<< textureHeight
			<< "</SizeY>"
			<< "</DataWindow>"
			<< "</GDAL_WMS>";

		GDALDataset *datasetTiff = (GDALDataset*)GDALOpen(xmlTiff.str().c_str(), GA_ReadOnly);
		if (datasetTiff == NULL)
		{
			std::cout << "Not getting imagery from xmlTiff" << std::endl;
			gdalError = true;
		}

		const char* tiffFileFormat = "GTiff";
		GDALDriver* tiffDriver = GetGDALDriverManager()->GetDriverByName(tiffFileFormat);
		if (tiffDriver == NULL)
		{
			std::cout << "GTiff driver failed" << std::endl;
			gdalError = true;
		}

		std::string tiffOutputFile = outputPath;
		GDALDataset* ImageDatasetTiff = tiffDriver->CreateCopy(tiffOutputFile.c_str(), datasetTiff, FALSE, NULL, NULL, NULL);
		if (ImageDatasetTiff == NULL)
		{
			std::cout << "GTiff dataset failed" << std::endl;
			gdalError = true;
		}

		// cleanup GDAL 
		GDALClose((GDALDatasetH)datasetTiff);
		GDALClose((GDALDatasetH)ImageDatasetTiff);
	}

	void TerrainGenerator::GetData(const std::string& geoServerURL, int beginIndex, int endIndex, std::vector<TileInfo>* infos)
	{
		if (infos == NULL)
		{
			return;
		}
		for (int i = beginIndex; i < endIndex; i++)
		{
			if (i >= infos->size())
			{
				break;
			}
			TileInfo& info = (*infos)[i];
			GetElevation(geoServerURL, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.elevationFileName);
			GetImagery(geoServerURL, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.imageFileName);
		}
	}

	void TerrainGenerator::ComputeCenterPosition(std::vector<TileInfo>& infos, double originLat, double originLon)
	{
		for (TileInfo& info : infos)
		{
			double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
			double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;
			info.centerX = flatEarth.convertGeoToLocalX(centerLon);
			info.centerY = flatEarth.convertGeoToLocalY(centerLat);
		}
	}

	void TerrainGenerator::GenerateLODBounds(std::vector<TileInfo>& infos, const TileInfo& info, int depth)
	{
		infos.push_back(info);

		if (depth == 0)
		{
			return;
		}

		double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
		double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;

		TileInfo topLeft;
		topLeft.extents.north = info.extents.north;
		topLeft.extents.south = centerLat;
		topLeft.extents.east = centerLon;
		topLeft.extents.west = info.extents.west;
		topLeft.quadKey = info.quadKey + std::to_string(0);

		TileInfo topRight;
		topRight.extents.north = info.extents.north;
		topRight.extents.south = centerLat;
		topRight.extents.east = info.extents.east;
		topRight.extents.west = centerLon;
		topRight.quadKey = info.quadKey + std::to_string(1);

		TileInfo bottomLeft;
		bottomLeft.extents.north = centerLat;
		bottomLeft.extents.south = info.extents.south;
		bottomLeft.extents.east = centerLon;
		bottomLeft.extents.west = info.extents.west;
		bottomLeft.quadKey = info.quadKey + std::to_string(2);

		TileInfo bottomRight;
		bottomRight.extents.north = centerLat;
		bottomRight.extents.south = info.extents.south;
		bottomRight.extents.east = info.extents.east;
		bottomRight.extents.west = centerLon;
		bottomRight.quadKey = info.quadKey + std::to_string(3);

		GenerateLODBounds(infos, topLeft, depth - 1);
		GenerateLODBounds(infos, topRight, depth - 1);
		GenerateLODBounds(infos, bottomLeft, depth - 1);
		GenerateLODBounds(infos, bottomRight, depth - 1);
	}

	void TerrainGenerator::GenerateFileNames(std::vector<TileInfo>& infos, const std::string& outputTmpPath, std::string format)
	{
		for (TileInfo& info : infos)
		{
			double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
			double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;
			std::string name = GetName(centerLat, centerLon, format, ".tif");
			info.elevationFileName = outputTmpPath + "/" + name;
			info.imageFileName = outputTmpPath + "/img/" + name;
		}
	}

	std::string TerrainGenerator::GetName(double originLat, double originLon, std::string formatIn, std::string filetype)
	{
		std::string lat = "N";
		std::string lon = "E";
		std::string format = formatIn;
		double latValue = originLat;
		double lonValue = originLon;

		// change the characters if the lat/lon are south/west instead of north/east 
		if (latValue < 0)
		{
			lat = "S";
		}
		if (lonValue < 0)
		{
			lat = "W";
		}
		
		// remove potential leading '.' from the format
		std::string chars = ".";
		for (char c : chars)
		{
			format.erase(std::remove(format.begin(), format.end(), c), format.end());
		}

		// make sure the format is upper case
		for (std::string::size_type i = 0; i < format.length(); i++)
		{
			std::toupper(format[i]);
		}
		std::transform(format.begin(), format.end(), format.begin(), ::toupper);

		std::string name;
		name = lat + std::to_string(fabs(latValue)) + "_" + lon + std::to_string(fabs(lonValue)) + "_" + format + filetype;
		return name;
	}
	void TerrainGenerator::WriteLODfile(std::vector<TileInfo>& infos,  std::string outputFilename, int nLODs)
	{
		std::ofstream lodFile;
		lodFile.open(outputFilename, std::ofstream::out);
		lodFile << nLODs << std::endl;

		double originLat = flatEarth.getOriginLatitude();
		double originLon = flatEarth.getOriginLongitude();
		for (TileInfo& info : infos)
		{
			flatEarth.setOrigin(info.extents.south, info.extents.west);
			lodFile << "beginBuildingList\n";
			for (int i = 0; i < info.GSFeatures.Count(); ++i)
			{
				float lat = info.GSFeatures.GetBuilding(i).lat;
				float lon = info.GSFeatures.GetBuilding(i).lon;
				float elev = info.GSFeatures.GetBuilding(i).elev;
				double y = flatEarth.convertGeoToLocalY(lat);
				double x = flatEarth.convertGeoToLocalX(lon);
				double z = info.GSFeatures.GetBuilding(i).elev;
				double ao1 = info.GSFeatures.GetBuilding(i).AO1;
				double scalex = info.GSFeatures.GetBuilding(i).scaleX;
				double scaley = info.GSFeatures.GetBuilding(i).scaleY;
				double scalez = info.GSFeatures.GetBuilding(i).scaleZ;
				std::string modelpath = info.GSFeatures.GetBuilding(i).modelpath;
				lodFile << x << " " << y << " " << z << " " << ao1 << " " << modelpath << " " << scalex << " " << scaley << " " << scalez << "\n";
			}
			lodFile << "endBuildingList\n";
			lodFile << info.quadKey << " " << info.centerX << " " << info.centerY << std::endl;
		}
	}

	void TerrainGenerator::SetBuildingElevations(elev::Elevation_DSM& edsm)
	{
		int buildingCount = buildings.Count();
		for (int i = 0; i < buildingCount; i++)
		{
			FeatureInfo& building = buildings.GetBuilding(i);
			double lat = building.lat;
			double lon = building.lon;
			if (lat > north || lat < south || lon > east || lon < west)
			{
				continue;
			}
			sfa::Point loc(lon, lat);
			edsm.Get(&loc);
			building.elev = loc.Z();
		}
	}
}



