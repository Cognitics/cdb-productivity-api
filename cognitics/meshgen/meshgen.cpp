

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include <tg/TerrainGenerator.h>

ccl::ObjLog logger;

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
    std::set<std::string> imageryPaths;
    std::set<std::string> oddbFiles;
    std::set<std::string> elevationFiles;
    int textureWidth = 1024;
    int textureHeight = 1024;
    double texelSize = 5.0f;
    bool originSet = false;
    std::string rulesFilename;
    int row = -1;
    int col = -1;

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
    terrainGenerator.setOrigin(originLat, originLon);
    terrainGenerator.setBounds(north, south, west, east);
    terrainGenerator.setOutputPath(outputPath);
    for (std::set<std::string>::iterator it = imageryPaths.begin(), end = imageryPaths.end(); it != end; ++it)
        terrainGenerator.addImageryPath(*it);
    for (std::set<std::string>::iterator it = elevationFiles.begin(), end = elevationFiles.end(); it != end; ++it)
        terrainGenerator.addElevationFile(*it);

    terrainGenerator.setTextureSize(textureWidth, textureHeight);
    terrainGenerator.setTexelSize(texelSize);

    terrainGenerator.generate(row, col);

    logger << "EXECUTION: " << execTimer.getElapsedTime() << " seconds" << logger.endl;
    return 0;
}

