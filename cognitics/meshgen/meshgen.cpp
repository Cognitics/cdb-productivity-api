

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include <ccl/FileInfo.h>
#include <tg/TerrainGenerator.h>
#include "elev/SimpleDEMReader.h"
#include "ws/WebServices.h"
#include <fstream>


ccl::ObjLog logger;

void RecordStatsForTileSize(const std::string& sOutputPath, int nTileSize, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, std::string outputFormat);
void TestAllTileSizes(const std::string& sOutputPath, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, std::string outputFormat);
void DeleteDirectory(const std::string &dir);

int usage(const std::string &error = std::string())
{
    std::cout << "usage: [options] <infile.shp>" << std::endl;
    std::cout << "\t-origin <lat> <lon>\tspecifies the origin" << std::endl;
    std::cout << "\t-bounds <north> <south> <east> <west>\tspecifies the bounding box" << std::endl;
    std::cout << "\t-output <path>\tspecifies the path for OpenFlight output" << std::endl;
    std::cout << "\t-imagery <path>\tspecifies a path to imagery (multiple)" << std::endl;
    std::cout << "\t-elevation <filename>\tspecifies a DEM (multiple)" << std::endl;
    std::cout << "\t-texturesize <width> <height>\tspecifies the texture size (default: 1024x1024)" << std::endl;
    std::cout << "\t-texelsize <size>\tspecifies the texel size (default: 5)" << std::endl;
    std::cout << "\t-row <row>\tspecifies the row of tiles to generate" << std::endl;
    std::cout << "\t-col <col>\tspecifies the column of tiles to generate" << std::endl;
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
    std::string outputPath("output");
	  std::string outputTmpPath("tmp/");
    std::set<std::string> imageryPaths;
    std::set<std::string> oddbFiles;
    std::set<std::string> elevationFiles;
    int textureWidth = 512;
    int textureHeight = 512;
    double texelSize = 5.0f;
    bool originSet = false;
    std::string rulesFilename;
    int row = -1;
    int col = -1;
	std::string outputFormat = ".gltf";
	bool useWebServices = false;

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
			/*if (argv[argi] == "gltf")
			{
				outputGltf = true;
			}*/
			outputFormat = argv[argi];
			continue;
		}
		if (param == "-ws")
		{
			useWebServices = true;
			continue;
		}

        if (param == "-text")
            return usage("Invalid parameters");
    }

    if (!originSet)
    {
        originLat = (north + south) / 2;
        originLon = (west + east) / 2;
    }

    // TODO: projInfo.txt for origin
    // TODO: use files to determine bounds
	
    logger << ccl::LINFO;
    logger << "CDB Mesh Generator" << logger.endl;

    cognitics::TerrainGenerator terrainGenerator;
	if (useWebServices)
	{
		std::cout << "using Web Services..." << std::endl;

		ws::GetDataWithGDAL(terrainGenerator, elevationFiles, outputTmpPath, north, south, east, west, textureWidth, textureHeight, originLat, originLon, outputFormat);
		terrainGenerator.setOrigin(originLat, originLon);
		terrainGenerator.setBounds(north, south, west, east);
		terrainGenerator.setOutputPath(outputPath);

		terrainGenerator.setTextureSize(textureWidth, textureHeight);
		terrainGenerator.setTexelSize(texelSize);
	}
	else
	{
		terrainGenerator.setOrigin(originLat, originLon);
		terrainGenerator.setBounds(north, south, west, east);
		terrainGenerator.setOutputPath(outputPath);
		for (std::set<std::string>::iterator it = imageryPaths.begin(), end = imageryPaths.end(); it != end; ++it)
			terrainGenerator.addImageryPath(*it);
		for (std::set<std::string>::iterator it = elevationFiles.begin(), end = elevationFiles.end(); it != end; ++it)
			terrainGenerator.addElevationFile(*it);

		terrainGenerator.setTextureSize(textureWidth, textureHeight);
		terrainGenerator.setTexelSize(texelSize);
	}

    //terrainGenerator.generate(row, col);
    //terrainGenerator.generateFixedGrid(*elevationFiles.begin());
	//terrainGenerator.generateFixedGrid(*elevationFiles.begin(), 128);
	//terrainGenerator.generateFixedGrid(*elevationFiles.begin(), outputFormat, 0);        

    terrainGenerator.generateFixedGrid(*elevationFiles.begin(), outputFormat, 64);

    //TestAllTileSizes(outputPath, terrainGenerator, elevationFiles, outputFormat);

	//DeleteDirectory(outputTmpPath);

    logger << "EXECUTION: " << execTimer.getElapsedTime() << " seconds" << logger.endl;
    return 0;
}

void DeleteDirectory(const std::string &dir)
{
	std::string name = dir;
	//std::uintmax_t n = std::experimental::filesystem::remove_all(dir);	
    int n = 0;
	if (rmdir(dir.c_str()) == 0)
	{
		logger << "Did not delete all the files in " << name << logger.endl;
	}
	else
	{
		logger << "Deleted " << n << " files or directories in " << name << logger.endl;
	}
}

void TestAllTileSizes(const std::string& sOutputPath, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, std::string outputFormat)
{
    for (int nTileSize = 1024; nTileSize >= 16; nTileSize /= 2)
    {
        RecordStatsForTileSize(sOutputPath, nTileSize, terrainGenerator, elevationFiles, outputFormat);
    }
}

void RecordStatsForTileSize(const std::string& sOutputPath, int nTileSize, cognitics::TerrainGenerator& terrainGenerator, const std::set<std::string>& elevationFiles, std::string outputFormat)
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

    terrainGenerator.generateFixedGrid(*elevationFiles.begin(), outputFormat, nTileSize);

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

            nTotalFileSize += ccl::getFileSize(fi.getFileName());//std::experimental::filesystem::file_size(entry.path());
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