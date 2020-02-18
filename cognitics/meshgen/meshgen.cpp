

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include <ccl/FileInfo.h>
#include <tg/TerrainGenerator.h>
#include <tg/GltfTerrainGenerator.h>
#include <tg/ObjTerrainGenerator.h>
#include <tg/OpenFlightTerrainGenerator.h>
#include <tg/FbxTerrainGenerator.h>
#include "elev/SimpleDEMReader.h"
#include "ws/WebServices.h"
#include <fstream>


ccl::ObjLog logger;

void CopyBuildingFeatureFiles(const std::string& modelCache, const std::string& featurePath);
void CopyFeatureFiles(const std::string& sOutputPath, const std::string& sFeaturePath);
void CopyFeatureFile(const std::string& sOutputPath, const std::string& sFeaturePath, const std::string& sFile);
cognitics::TerrainGenerator* CreateTerrainGenerator(const std::string& format);
void RecordStatsForTileSize(const std::string& sOutputPath, int nTileSize, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, const std::string& featurePath, const std::string& modelCachePath, std::string outputFormat);
void TestAllTileSizes(const std::string& sOutputPath, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, const std::string& modelCachePath, std::string outputFormat);
void DeleteDirectory(const std::string &dir);
std::string CheckFormatString(std::string inFormat);

int usage(const std::string &error = std::string())
{
    std::cout << "usage: [options] <infile.shp>" << std::endl;
    std::cout << "\t-origin <lat> <lon>\tspecifies the origin" << std::endl;
    std::cout << "\t-bounds <north> <south> <east> <west>\tspecifies the bounding box" << std::endl;
    std::cout << "\t-output <path>\tspecifies the path for OpenFlight output" << std::endl;
	std::cout << "\t-outputTmpPath <path>\tspecifies the path for temporary files" << std::endl;
    std::cout << "\t-imagery <path>\tspecifies a path to imagery (multiple)" << std::endl;
    std::cout << "\t-elevation <filename>\tspecifies a DEM (multiple)" << std::endl;
	std::cout << "\t-features <path>\tspecifies a path to feature data" << std::endl;
    std::cout << "\t-texturesize <width> <height>\tspecifies the texture size (default: 1024x1024)" << std::endl;
    std::cout << "\t-texelsize <size>\tspecifies the texel size (default: 5)" << std::endl;
    std::cout << "\t-row <row>\tspecifies the row of tiles to generate" << std::endl;
    std::cout << "\t-col <col>\tspecifies the column of tiles to generate" << std::endl;
	std::cout << "\t-format <format>\tspecifies the format of the output. Choices are obj fbx b3dm and flt" << std::endl;
	std::cout << "\t-ws\tindicates that web services should be used to query a geoserver rather than using file input" << std::endl;
	std::cout << "\t-tileSize <size>\tspecifies the size of the tiles (default: 256)" << std::endl;
	std::cout << "\t-startLOD <lod>\tspecifies the lower LOD of the desired export (default: 8)" << std::endl;
	std::cout << "\t-endLOD <lod>\tspecifies the higher LOD of the desired export (default: 8)" << std::endl;
	std::cout << "\t-projection <projection>\tspecifies the projection of the extents" << std::endl;
	std::cout << "\t-combineMeshes \tindicates that the feature meshes should be combined with the terrain mesh" << std::endl;
    if (error.size())
        std::cerr << "ERROR: " << error << std::endl;
    return (error.size()) ? -1 : 0;
}

int main(int argc, char **argv)
{
    logger.init("main");
    logger << ccl::LINFO;

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    ccl::Timer execTimer;
    execTimer.startTimer();

    double north = 0.0f;
    double south = 0.0f;
    double west = 0.0f;
    double east = 0.0f;
    double originLat;
    double originLon;
    std::string modelKitPath("modelkits/");
    std::string texturePath("textures/");
    std::string outputPath("output/");
	std::string outputTmpPath("output/tmp/");
    std::set<std::string> imageryPaths;
    std::set<std::string> oddbFiles;
    std::set<std::string> elevationFiles;
    std::string featurePath = "data/";
    std::string modelCachePath = "";
    int textureWidth = 512;
    int textureHeight = 512;
    double texelSize = 5.0f;
    bool originSet = false;
    std::string rulesFilename;
    int row = -1;
    int col = -1;
	std::string outputFormat = ".gltf";
	bool useWebServices = false;
    std::string geoServerURL = "localhost:81/geoserver";
	int tileSize = 256;
	std::string projection = "";
	int startLOD = 8;
	int endLOD = 8;
	bool combineMeshes = false;

    if (argc <= 1)
        return usage();
    for (int argi = 1; argi < argc; ++argi)
    {
        std::string param(argv[argi]);
        if (param == "-origin")
        {
            ++argi;
            if ((argi + 1) >= argc)
                return usage("Missing latitude and longitude for origin");
            originLat = atof(argv[argi]);
            ++argi;
            originLon = atof(argv[argi]);
            originSet = true;
            continue;
        }
        if (param == "-bounds")
        {
            if ((argi + 1) < argc)
            {
                north = atof(argv[argi + 1]);
                argi++;
            }
            if ((argi + 1) < argc)
            {
                south = atof(argv[argi + 1]);
                argi++;
            }
            if ((argi + 1) < argc)
            {
                east = atof(argv[argi + 1]);
                argi++;
            }
            if ((argi + 1) < argc)
            {
                west = atof(argv[argi + 1]);
                argi++;
            }
            continue;
        }
        if (param == "-modelkits")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing model kit path");
            modelKitPath = argv[argi];
            continue;
        }
        if (param == "-textures")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing textures path");
            texturePath = argv[argi];
            continue;
        }
        if (param == "-output")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing output path");
            outputPath = argv[argi];
            outputPath += "/";
            continue;
        }
		if (param == "-outputTmpPath")
		{
			++argi;
			if (argi >= argc)
				return usage("Missing output temp path");
			outputTmpPath = argv[argi];
			outputTmpPath += "/";
            continue;
        }
        if (param == "-imagery")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing imagery path");
            imageryPaths.insert(argv[argi]);
            continue;
        }
        if (param == "-elevation")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing elevation file");
            elevationFiles.insert(argv[argi]);
            continue;
        }
        if (param == "-features")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing feature path");
            featurePath = argv[argi];
            featurePath += "/";
            continue;
        }
        if (param == "-texturesize")
        {
            ++argi;
            if ((argi + 1) >= argc)
                return usage("Missing width and height for texture size");
            textureWidth = atoi(argv[argi]);
            ++argi;
            textureHeight = atoi(argv[argi]);
            continue;
        }
        if (param == "-row")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing row");
            row = atof(argv[argi]);
            continue;
        }
        if (param == "-col")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing col");
            col = atof(argv[argi]);
            continue;
        }
        if (param == "-texelsize")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing texel size");
            texelSize = atof(argv[argi]);
            continue;
        }
		if (param == "-format")
		{
			++argi;
			if (argi >= argc)
				return usage("Missing format");
			outputFormat = argv[argi];
			continue;
		}
		if (param == "-ws")
		{
			useWebServices = true;
			continue;
		}
        if (param == "-geoserverURL")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing geoserver URL");
            geoServerURL = argv[argi];
        }
        if (param == "-modelCache")
        {
            ++argi;
            if (argi >= argc)
                return usage("Missing model cache path");
            modelCachePath = argv[argi];
        }
		if (param == "-tileSize")
		{
			++argi;
			if (argi >= argc)
				return usage("Missing tile size");
			tileSize = atoi(argv[argi]);
			if (tileSize < 0)
			{
				tileSize = 256;
				return usage("Invalid tile size.");
			}
		}
		if (param == "-projection")
		{
			++argi;
			if (argi >= argc)
				return usage("Missing projection");
			projection = argv[argi];
		}
		if (param == "-startLOD")
		{
			++argi;
			if (argi >= argc)
				return usage("missing start LOD");
			startLOD = atoi(argv[argi]);
		}
		if (param == "-endLOD")
		{
			++argi;
			if (argi >= argc)
				return usage("missing end LOD");
			endLOD = atoi(argv[argi]);
		}
		if (param == "-combineMeshes")
		{
			combineMeshes = true;
			continue;
		}

        if (param == "-text")
            return usage("Invalid parameters");
    }
	outputFormat = CheckFormatString(outputFormat);

    if (!originSet)
    {
        originLat = (north + south) / 2;
        originLon = (west + east) / 2;
    }

    // TODO: projInfo.txt for origin
    // TODO: use files to determine bounds
	
    logger << ccl::LINFO;
    logger << "CDB Mesh Generator" << logger.endl;

    cognitics::TerrainGenerator* terrainGenerator = CreateTerrainGenerator(outputFormat);
	if (useWebServices)
	{
		// Create necessary directories 
		if (!ccl::directoryExists(outputPath))
		{
            ccl::makeDirectory(outputPath);
		}
		if (!ccl::directoryExists(outputTmpPath))
		{
            ccl::makeDirectory(outputTmpPath);
		}
		if (!ccl::directoryExists(outputTmpPath + "/data"))
		{
            ccl::makeDirectory(outputTmpPath + "/data");
		}
		if (!ccl::directoryExists(outputTmpPath + "/img"))
		{
            ccl::makeDirectory(outputTmpPath + "/img");
		}
		std::cout << "using Web Services..." << std::endl;
		ws::GetFeatureData(geoServerURL, outputTmpPath, north, south, east, west);

    	GDALAllRegister();

    	auto filename = ws::GetName(originLon, originLat, textureHeight, textureWidth, outputFormat, ".tif");
    	auto elevationFilename = outputTmpPath + "/" + filename;
    	elevationFiles.insert(elevationFilename);
		ws::GetElevation(geoServerURL, north, south, east, west, textureWidth, textureHeight, elevationFilename);
		ws::GetImagery(geoServerURL, north, south, east, west, textureWidth, textureHeight, outputTmpPath + "/img/" + filename);
        //terrainGenerator.parseFeatures(outputTmpPath);

        terrainGenerator->addImageryPath(outputTmpPath + "/img");

		terrainGenerator->setOrigin(originLat, originLon);
		terrainGenerator->setBounds(north, south, west, east);
		terrainGenerator->setOutputPath(outputPath);
		terrainGenerator->setOutputTmpPath(outputTmpPath);
		terrainGenerator->setOutputFormat(outputFormat);

		//terrainGenerator.setTextureSize(textureWidth, textureHeight);
		//terrainGenerator.setTexelSize(texelSize);
	}
	else
	{
#ifdef CAE_MESH
        CopyBuildingFeatureFiles(modelCachePath, featurePath);
        CopyFeatureFiles(outputTmpPath, featurePath);
#endif
		terrainGenerator->setOrigin(originLat, originLon);
		terrainGenerator->setBounds(north, south, west, east);
		terrainGenerator->setOutputPath(outputPath);
		terrainGenerator->setOutputTmpPath(outputTmpPath);
        logger << "Adding imagery paths..." << logger.endl;
		for (std::set<std::string>::iterator it = imageryPaths.begin(), end = imageryPaths.end(); it != end; ++it)
			terrainGenerator->addImageryPath(*it);
        logger << "Adding elevation paths..." << logger.endl;
		for (std::set<std::string>::iterator it = elevationFiles.begin(), end = elevationFiles.end(); it != end; ++it)
			terrainGenerator->addElevationFile(*it);
#ifdef CAE_MESH
        terrainGenerator->parseFeatures(outputTmpPath);
#endif
		terrainGenerator->setTextureSize(textureWidth, textureHeight);
		terrainGenerator->setTexelSize(texelSize);
	}

    //ws::generateFixedGridSofprep(north, south, west, east, geoServerURL, outputTmpPath, outputPath, outputFormat);
    //return 0;

    //terrainGenerator.generate(row, col);
    //terrainGenerator.generateFixedGrid(*elevationFiles.begin());
    //terrainGenerator.generateFixedGrid(*elevationFiles.begin(), featurePath, outputPath, 128);
    //terrainGenerator.generateFixedGrid(*elevationFiles.begin(), featurePath, outputFormat, 0);
    //if(outputFormat == ".b3dm")
	//{
	//	ws::generateCesiumLods(north, south, west, east, geoServerURL, outputTmpPath, outputPath, *terrainGenerator, 2);
	//}
	//else
	{
        if (startLOD == endLOD)
        {
            if (tileSize == 0)
            {
                if (outputFormat == ".obj")
                {
                    auto secondElement = std::next(elevationFiles.begin(), 1);
                    ws::GenerateSingleOBJ(*secondElement, geoServerURL, outputPath, outputTmpPath);
                }
            }
            else
            {
                terrainGenerator->generateFixedGrid(*elevationFiles.begin(), featurePath, modelCachePath, outputFormat, tileSize);
            }
            //ws::generateFixedGridSofprep(north, south, west, east, geoServerURL, outputTmpPath, outputPath, outputFormat);
        }
        //terrainGenerator->generateFixedGrid(*elevationFiles.begin(), featurePath, modelCachePath, outputFormat, tileSize);
        //ws::generateFixedGridWeb(north, south, west, east, outputFormat, geoServerIPAddress, outputTmpPath, outputPath, featurePath, outputFormat, textureWidth, textureHeight, terrainGenerator);
        //ws::generateFixedGridWeb2(north, south, west, east, outputFormat, geoServerURL, outputTmpPath, outputPath, outputFormat, terrainGenerator, 5);
        if (startLOD < endLOD)
        {
            terrainGenerator->generateFixedGridWithLOD(geoServerURL, north, south, east, west, outputFormat, outputTmpPath, outputPath, outputFormat, (endLOD - startLOD), textureHeight, textureWidth);
        }
	}
    //terrainGenerator->generateFixedGrid(*elevationFiles.begin(), featurePath, modelCachePath, outputFormat, tileSize);
    //ws::generateFixedGridWeb(north, south, west, east, outputFormat, geoServerIPAddress, outputTmpPath, outputPath, featurePath, outputFormat, textureWidth, textureHeight, terrainGenerator);
    //ws::generateFixedGridWeb2(north, south, west, east, outputFormat, geoServerURL, outputTmpPath, outputPath, outputFormat, terrainGenerator, 5);

    //TestAllTileSizes(outputPath, terrainGenerator, elevationFiles, outputFormat);

	//DeleteDirectory(outputTmpPath);

    delete terrainGenerator;
    terrainGenerator = nullptr;

    logger << "EXECUTION: " << execTimer.getElapsedTime() << " seconds" << logger.endl;
    return 0;
}

void CopyBuildingFeatureFiles(const std::string& modelCache, const std::string& featurePath)
{
    std::string modelCacheFrom = ccl::joinPaths(featurePath, "ModelCache");

    logger << "Copying files from " << modelCacheFrom << " to " << modelCache << "..." << logger.endl;
    ccl::copyFilesRecursive(modelCacheFrom, modelCache);
}

void CopyFeatureFiles(const std::string& outputPath, const std::string& featurePath)
{
    CopyFeatureFile(outputPath, featurePath, "building_footprints.xml");
    CopyFeatureFile(outputPath, featurePath, "building_models.xml");
    CopyFeatureFile(outputPath, featurePath, "tree_points.xml");
}

void CopyFeatureFile(const std::string& outputPath, const std::string& featurePath, const std::string& sFile)
{
    std::string srcPath = ccl::joinPaths(featurePath, sFile);
    std::string dstPath = ccl::joinPaths(outputPath, "data\\"+sFile);

    std::string outputDataPath = ccl::joinPaths(outputPath, "data");
    
    if (!ccl::fileExists(outputDataPath))
    {
        
        ccl::makeDirectory(outputDataPath);
    }

    if (!ccl::fileExists(srcPath))
    {
        logger << "File does not exist " << srcPath << logger.endl;
        return;
    }

    ccl::copyFile(srcPath, dstPath);
}

void DeleteDirectory(const std::string &dir)
{
	std::string name = dir;
	//std::uintmax_t n = std::experimental::filesystem::remove_all(dir);	
    int n = 0;
#ifndef LINUX
	if (_rmdir(dir.c_str()) == 0)
#else 
    if (rmdir(dir.c_str()) == 0)
#endif // !LINUX
	{
		logger << "Did not delete all the files in " << name << logger.endl;
	}
	else
	{
		logger << "Deleted " << n << " files or directories in " << name << logger.endl;
	}
}

void TestAllTileSizes(const std::string& sOutputPath, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, const std::string& featurePath, const std::string& modelCachePath, std::string outputFormat)
{
    for (int nTileSize = 1024; nTileSize >= 16; nTileSize /= 2)
    {
        RecordStatsForTileSize(sOutputPath, nTileSize, terrainGenerator, elevationFiles, featurePath, modelCachePath, outputFormat);
    }
}

void RecordStatsForTileSize(const std::string& sOutputPath, int nTileSize, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, const std::string& featurePath, const std::string& modelCachePath, std::string outputFormat) 
{
    ccl::Timer timer;
    timer.startTimer();

    std::stringstream sstream;
    sstream << sOutputPath << "\\" << nTileSize;
    std::string sTileSizeOutput = sstream.str();
#if 0
    if (!std::experimental::filesystem::exists(sTileSizeOutput))
    {
        std::experimental::filesystem::create_directory(sTileSizeOutput);
    }
#endif
    if(!ccl::directoryExists(sTileSizeOutput))
    {
        ccl::makeDirectory(sTileSizeOutput,true);
    }
    terrainGenerator.setOutputPath(sTileSizeOutput);

    terrainGenerator.generateFixedGrid(*elevationFiles.begin(), featurePath, modelCachePath, outputFormat, nTileSize);

    std::ofstream fout(sOutputPath + "\\stats.csv", std::ofstream::out | std::ofstream::app);
    int nNumOBJs = 0;
    int nTotalFileSize = 0;

    std::string path = sTileSizeOutput;
    std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles(path,"*.*", true);
    for (auto&& fi : files)
    {
        //ext does NOT include the dot (.)
        std::string ext = fi.getSuffix();
        if (ext == "obj")
        {
            ++nNumOBJs;

            nTotalFileSize += ccl::getFileSize(fi.getFileName());
        }
        else if (ext == "jpg" /*|| ext == "attr"*/)
        {
            nTotalFileSize += ccl::getFileSize(fi.getFileName());
        }
    }
#if 0
    for (const auto& entry : std::experimental::filesystem::directory_iterator(path))
    {
        //std::cout << entry.path() << std::endl;
        if (entry.path().extension() == ".obj")
        {
            ++nNumOBJs;

            nTotalFileSize += std::experimental::filesystem::file_size(entry.path());
        }
        else if (entry.path().extension() == ".jpg" /*|| entry.path().extension() == ".attr"*/)
        {
            nTotalFileSize += std::experimental::filesystem::file_size(entry.path());
        }
    }
#endif
    nTotalFileSize /= static_cast<float>(1024 * 1024);//get total file size in MB

    fout << nTileSize << "\t" << nNumOBJs << "\t" << nTotalFileSize / static_cast<float>(nNumOBJs) << "\t" << timer.getElapsedTime() / static_cast<float>(nNumOBJs) << "\t" << nTotalFileSize << "\t" << timer.getElapsedTime() << std::endl;

    fout.close();
}

std::string CheckFormatString(std::string inFormat)
{
	std::string outFormat;
	if (inFormat.size() == 0)
	{
		// default to OBJ output
		return ".obj";
	}
	if (inFormat.at(0) != '.')
	{
		outFormat = ".";
		outFormat += inFormat;
	}
	else
	{
		outFormat = inFormat;
	}

	return outFormat;
}

cognitics::TerrainGenerator* CreateTerrainGenerator(const std::string& format)
{
    if (cognitics::TerrainGenerator::IsGltfTypeOutput(format))
    {
        return new cognitics::GltfTerrainGenerator();
    }
    else if (cognitics::TerrainGenerator::IsObjOutput(format))
    {
        return new cognitics::ObjTerrainGenerator();
    }
    else if (cognitics::TerrainGenerator::IsFbxOutput(format))
    {
        return new cognitics::FbxTerrainGenerator();
    }
    else if (cognitics::TerrainGenerator::IsOpenFlightOutput(format))
    {
        return new cognitics::OpenFlightTerrainGenerator();
    }

    return nullptr;
}