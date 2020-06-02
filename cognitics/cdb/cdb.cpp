
#include <cdb_util/cdb_util.h>
#include <cdb_util/cdb_inject.h>
#include <cdb_util/cdb_lod.h>
#include <cdb_util/cdb_sample.h>

#include <ccl/gdal.h>
#include <ccl/LogStream.h>
#include <ccl/ObjLog.h>

#include <fstream>
#include <cstdlib>
#include <chrono>

namespace
{
    int to_int(const std::string& str, int default_value)
    {
        try { return std::stoi(str); }
        catch(...) { return default_value; }
    }

    double to_double(const std::string& str, double default_value)
    {
        try { return std::stod(str); }
        catch(...) { return default_value; }
    }
}


std::vector<std::string> args;
std::string cdb;

void cout_global_options()
{
    std::cout << "    Options:\n";
    std::cout << "        -logfile <filename>    logfile for debug output\n";
}

int usage(const std::string& error = "")
{
    if(!error.empty())
        std::cerr << "\nERROR: " << error << "\n\n";
    std::cout << "Usage: " << args[0] << " [options] <cdbpath> <command> [command_options] [command_parameters]\n";
    cout_global_options();
    std::cout << "    Commands:\n";
    std::cout << "        INJECT                 inject data into a dataset\n";
    std::cout << "        BUILD                  build dataset\n";
    std::cout << "        LOD                    generate LODs for dataset(s)\n";
    std::cout << "        SAMPLE                 sample a dataset\n";
    std::cout << "        VALIDATE               validate a dataset\n";
    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int usage_inject(const std::string& error = "")
{
    if(!error.empty())
        std::cerr << "\nERROR: " << error << "\n\n";
    std::cout << "Usage: " << args[0] << " [options] <cdbpath> INJECT [command_options] <dataset> <cs1> <cs2> <source> [source] [source] [...]\n";
    cout_global_options();
    std::cout << "    Command Options:\n";
    std::cout << "        -bounds <n> <s> <e> <w>  bounds for area of interest\n";
    std::cout << "        -lod <lod>               forced level of detail\n";
    std::cout << "        -models <path>           path to models\n";
    std::cout << "        -textures <path>         path to textures\n";
    std::cout << "        -workers <#>             number of worker threads (default: 8)\n";
    std::cout << "        -replace                 replace target tile data\n";
    std::cout << "    Supported Components (dataset cs1 cs2):\n";
    std::cout << "        Imagery 001 001\n";
    std::cout << "        Imagery 003 001-012\n";
    std::cout << "        Imagery 004 001-004\n";
    std::cout << "        Elevation 001 001\n";
    //std::cout << "        GSFeature 001 001\n";
    //std::cout << "        GSFeature 001 003\n";
    //std::cout << "        GSFeature 001 005\n";
    //std::cout << "        GSFeature 002 001\n";
    //std::cout << "        GSFeature 002 003\n";
    //std::cout << "        GSFeature 002 005\n";
    //std::cout << "        GSFeature 003 001\n";
    //std::cout << "        GSFeature 004 001\n";
    //std::cout << "        GSFeature 004 003\n";
    //std::cout << "        GSFeature 004 005\n";
    //std::cout << "        GSFeature 005 001\n";
    std::cout << "        GTFeature 001 001\n";
    std::cout << "        GTFeature 001 003\n";
    std::cout << "        GTFeature 001 005\n";
    std::cout << "        GTFeature 002 001\n";
    std::cout << "        GTFeature 002 003\n";
    std::cout << "        GTFeature 002 005\n";
    std::cout << "        GTFeature 003 001\n";
    std::cout << "        GeoPolitical 001 001\n";
    std::cout << "        GeoPolitical 001 003\n";
    std::cout << "        GeoPolitical 001 005\n";
    std::cout << "        GeoPolitical 002 001\n";
    std::cout << "        GeoPolitical 002 003\n";
    std::cout << "        GeoPolitical 002 005\n";
    std::cout << "        GeoPolitical 003 001\n";
    std::cout << "        GeoPolitical 003 003\n";
    std::cout << "        GeoPolitical 003 005\n";
    std::cout << "        RoadNetwork 002 003\n";
    std::cout << "        RoadNetwork 002 007\n";
    std::cout << "        RoadNetwork 003 003\n";
    std::cout << "        RoadNetwork 003 005\n";
    std::cout << "        RailRoadNetwork 002 003\n";
    std::cout << "        RailRoadNetwork 002 007\n";
    std::cout << "        PowerLineNetwork 002 003\n";
    std::cout << "        PowerLineNetwork 002 007\n";
    std::cout << "        HydrographyNetwork 002 003\n";
    std::cout << "        HydrographyNetwork 002 005\n";
    std::cout << "        HydrographyNetwork 002 007\n";
    std::cout << "        HydrographyNetwork 002 009\n";
    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int usage_build(const std::string& error = "")
{
    if(!error.empty())
        std::cerr << "\nERROR: " << error << "\n\n";
    std::cout << "Usage: " << args[0] << " [options] <cdbpath> BUILD [command_options] <dataset> <cs1> <cs2>\n";
    cout_global_options();
    std::cout << "    Supported Components (dataset cs1 cs2):\n";
    std::cout << "        MinMaxElevation 001\n";
    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int usage_lod(const std::string& error = "")
{
    if(!error.empty())
        std::cerr << "\nERROR: " << error << "\n\n";
    std::cout << "Usage: " << args[0] << " [options] <cdbpath> LOD [command_options] <dataset> <cs1> <cs2>\n";
    cout_global_options();
    std::cout << "    Command Options:\n";
    std::cout << "        -workers <#>           number of worker threads (default: 8)\n";
    std::cout << "    Supported Components:\n";
    std::cout << "        Imagery 001 001\n";
    std::cout << "        Elevation 001 001\n";
    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int usage_sample(const std::string& error = "")
{
    if(!error.empty())
        std::cerr << "\nERROR: " << error << "\n\n";
    std::cout << "Usage: " << args[0] << " [options] <cdbpath> SAMPLE [command_options] <dataset> <cs1> <cs2> <outfile>\n";
    cout_global_options();
    std::cout << "    Command Options:\n";
    std::cout << "        -bounds <n> <s> <e> <w>  bounds for area of interest\n";
    std::cout << "        -width <#>               width (x) dimension\n";
    std::cout << "        -height <#>              height (y) dimension\n";
    std::cout << "        -lod <lod>               forced level of detail\n";
    std::cout << "    Supported Components:\n";
    std::cout << "        Imagery 001 001\n";
    std::cout << "        Elevation 001 001\n";
    std::cout << "        GSFeature 001 001\n";
    std::cout << "        GSFeature 001 003\n";
    std::cout << "        GSFeature 001 005\n";
    std::cout << "        GSFeature 002 001\n";
    std::cout << "        GSFeature 002 003\n";
    std::cout << "        GSFeature 002 005\n";
    std::cout << "        GSFeature 003 001\n";
    std::cout << "        GSFeature 004 001\n";
    std::cout << "        GSFeature 004 003\n";
    std::cout << "        GSFeature 004 005\n";
    std::cout << "        GSFeature 005 001\n";
    std::cout << "        GTFeature 001 001\n";
    std::cout << "        GTFeature 001 003\n";
    std::cout << "        GTFeature 001 005\n";
    std::cout << "        GTFeature 002 001\n";
    std::cout << "        GTFeature 002 003\n";
    std::cout << "        GTFeature 002 005\n";
    std::cout << "        GTFeature 003 001\n";
    std::cout << "        GeoPolitical 001 001\n";
    std::cout << "        GeoPolitical 001 003\n";
    std::cout << "        GeoPolitical 001 005\n";
    std::cout << "        GeoPolitical 002 001\n";
    std::cout << "        GeoPolitical 002 003\n";
    std::cout << "        GeoPolitical 002 005\n";
    std::cout << "        GeoPolitical 003 001\n";
    std::cout << "        GeoPolitical 003 003\n";
    std::cout << "        GeoPolitical 003 005\n";
    std::cout << "        RoadNetwork 002 003\n";
    std::cout << "        RoadNetwork 002 007\n";
    std::cout << "        RoadNetwork 003 003\n";
    std::cout << "        RoadNetwork 003 005\n";
    std::cout << "        RailRoadNetwork 002 003\n";
    std::cout << "        RailRoadNetwork 002 007\n";
    std::cout << "        PowerLineNetwork 002 003\n";
    std::cout << "        PowerLineNetwork 002 007\n";
    std::cout << "        HydrographyNetwork 002 003\n";
    std::cout << "        HydrographyNetwork 002 005\n";
    std::cout << "        HydrographyNetwork 002 007\n";
    std::cout << "        HydrographyNetwork 002 009\n";
    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int usage_validate(const std::string& error = "")
{
    if(!error.empty())
        std::cerr << "\nERROR: " << error << "\n\n";
    std::cout << "Usage: " << args[0] << " [options] <cdbpath> VALIDATE [command_options] <dataset> <cs1> <cs2>\n";
    cout_global_options();
    std::cout << "    Command Options:\n";
    std::cout << "        -bounds <n> <s> <e> <w>  bounds for area of interest\n";
    std::cout << "    Supported Components (dataset cs1 cs2):\n";
    std::cout << "        GTFeature 001 001\n";
    std::cout << "        GTFeature 002 001\n";
    std::cout << "        GSFeature 001 001\n";
    return error.empty() ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main_inject(size_t arg_start)
{
    int lod { 24 };
    int workers { 8 };
    double north { DBL_MAX };
    double south { -DBL_MAX };
    double east { DBL_MAX };
    double west { -DBL_MAX };
    int dataset { 0 };
    int cs1 { 0 };
    int cs2 { 0 };
    bool replace = false;
    auto models = std::string();
    auto textures = std::string();
    auto sources = std::vector<std::string>();
    for(size_t argi = arg_start, argc = args.size(); argi < argc; ++argi)
    {
        if(args[argi] == "-lod")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing target LOD");
            lod = to_int(args[argi], 24);
            continue;
        }
        if(args[argi] == "-textures")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing source textures path");
            textures = args[argi];
            continue;
        }
        if(args[argi] == "-models")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing source models path");
            models = args[argi];
            continue;
        }
        if(args[argi] == "-workers")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing worker thread count");
            workers = to_int(args[argi], 8);
            continue;
        }
        if(args[argi] == "-replace")
        {
            replace = true;
            continue;
        }
        if(args[argi] == "-bounds")
        {
            if(argi > argc - 4)
                return usage_inject("Missing bounds");
            ++argi;
            north = to_double(args[argi], DBL_MAX);
            ++argi;
            south = to_double(args[argi], -DBL_MAX);
            ++argi;
            east = to_double(args[argi], DBL_MAX);
            ++argi;
            west = to_double(args[argi], -DBL_MAX);
            continue;
        }
        if(dataset == 0)
        {
            dataset = to_int(args[argi], 0);
            if(dataset == 0)
                dataset = cognitics::cdb::DatasetCode(args[argi]);
            if(dataset == 0)
                return usage_inject("Invalid Dataset: " + args[argi]);
            continue;
        }
        if(cs1 == 0)
        {
            cs1 = to_int(args[argi], 0);
            if(cs1 == 0)
                return usage_inject("Invalid Component Selector 1: " + args[argi]);
            continue;
        }
        if(cs2 == 0)
        {
            cs2 = to_int(args[argi], 0);
            if(cs2 == 0)
                return usage_inject("Invalid Component Selector 2: " + args[argi]);
            continue;
        }
        sources.push_back(args[argi]);
    }
    if(sources.empty())
        return usage_inject();
    auto params = cognitics::cdb::cdb_inject_parameters();
    params.cdb = cdb;
    if(lod != 24)
        params.lod = lod;
    if(workers != 8)
        params.workers = workers;
    params.north = north;
    params.south = south;
    params.east = east;
    params.west = west;
    params.cs1 = cs1;
    params.cs2 = cs2;
    if(lod == 24)
        lod = 0;

    if((dataset == 1) && (cs1 == 1) && (cs2 == 1))  // Elevation, PrimaryTerrainElevation
    {
        params.elevation = sources;
        return cognitics::cdb::cdb_inject(params) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if((dataset == 4) && (cs1 == 1) && (cs2 == 1))  // Imagery, YearlyVstiRepresentation
    {
        params.imagery = sources;
        return cognitics::cdb::cdb_inject(params) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if((dataset == 4) && (cs1 == 3) && (cs2 >= 1) && (cs2 <= 12))  // Imagery, MonthlyVstiRepresentation
    {
        params.imagery = sources;
        return cognitics::cdb::cdb_inject(params) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if((dataset == 4) && (cs1 == 4) && (cs2 >= 1) && (cs2 <= 4))  // Imagery, QuarterlyVstiRepresentation
    {
        params.imagery = sources;
        return cognitics::cdb::cdb_inject(params) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if((dataset == 100) && (cs1 == 1) && (cs2 == 1))    // GSFeature, Man-made, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 1) && (cs2 == 3))    // GSFeature, Man-made, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 1) && (cs2 == 5))    // GSFeature, Man-made, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 2) && (cs2 == 1))    // GSFeature, Natural, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 2) && (cs2 == 3))    // GSFeature, Natural, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 2) && (cs2 == 5))    // GSFeature, Natural, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 3) && (cs2 == 1))    // GSFeature, Trees, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 4) && (cs2 == 1))    // GSFeature, Airport light, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 4) && (cs2 == 3))    // GSFeature, Airport, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 4) && (cs2 == 5))    // GSFeature, Airport, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 5) && (cs2 == 1))    // GSFeature, Environmental light, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 1) && (cs2 == 1))    // GTFeature, Man-made, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 1) && (cs2 == 3))    // GTFeature, Man-made, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 1) && (cs2 == 5))    // GTFeature, Man-made, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 2) && (cs2 == 1))    // GTFeature, Tree, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 2) && (cs2 == 3))    // GTFeature, Tree, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 2) && (cs2 == 5))    // GTFeature, Tree, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 101) && (cs1 == 3) && (cs2 == 1))    // GTFeature, Moving Model location, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 1) && (cs2 == 1))    // GeoPoliticalGTFeature, Boundary, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 1) && (cs2 == 3))    // GeoPoliticalGTFeature, Boundary, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 1) && (cs2 == 5))    // GeoPoliticalGTFeature, Boundary, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 2) && (cs2 == 1))    // GeoPoliticalGTFeature, Location, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 2) && (cs2 == 3))    // GeoPoliticalGTFeature, Location, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 2) && (cs2 == 5))    // GeoPoliticalGTFeature, Location, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 3) && (cs2 == 1))    // GeoPoliticalGTFeature, Constraint, point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 3) && (cs2 == 3))    // GeoPoliticalGTFeature, Constraint, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 102) && (cs1 == 3) && (cs2 == 5))    // GeoPoliticalGTFeature, Constraint, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 201) && (cs1 == 2) && (cs2 == 3))    // RoadNetwork, Road Network, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 201) && (cs1 == 2) && (cs2 == 7))    // RoadNetwork, Road Network, lineal figure point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 201) && (cs1 == 3) && (cs2 == 3))    // RoadNetwork, Airport Network, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 201) && (cs1 == 3) && (cs2 == 5))    // RoadNetwork, Airport Network, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 202) && (cs1 == 2) && (cs2 == 3))    // RailRoadNetwork, RailRoad Network, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 202) && (cs1 == 2) && (cs2 == 7))    // RailRoadNetwork, RailRoad Network, lineal figure point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 203) && (cs1 == 2) && (cs2 == 3))    // PowerLineNetwork, PowerLine Network, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 203) && (cs1 == 2) && (cs2 == 7))    // PowerLineNetwork, PowerLine Network, lineal figure point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 3))    // HydrographyNetwork, Hydrography Network, lineal features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 5))    // HydrographyNetwork, Hydrography Network, polygon features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 7))    // HydrographyNetwork, Hydrography Network, lineal figure point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 9))    // HydrographyNetwork, Hydrography Network, polygon figure point features
        return cognitics::cdb::InjectFeatures(cdb, dataset, cs1, cs2, lod, sources, replace, models, textures) ? EXIT_SUCCESS : EXIT_FAILURE;
    else
        return usage_inject("Unsupported Component: " + cognitics::cdb::DatasetName(dataset) + " " + std::to_string(cs1) + " " + std::to_string(cs2));
    return EXIT_SUCCESS;
}

int main_build(size_t arg_start)
{
    double north { DBL_MAX };
    double south { -DBL_MAX };
    double east { DBL_MAX };
    double west { -DBL_MAX };
    int dataset { 0 };
    int cs1 { 0 };
    int cs2 { 0 };
    for(size_t argi = arg_start, argc = args.size(); argi < argc; ++argi)
    {
        if(dataset == 0)
        {
            dataset = to_int(args[argi], 0);
            if(dataset == 0)
                dataset = cognitics::cdb::DatasetCode(args[argi]);
            if(dataset == 0)
                return usage_build("Invalid Dataset: " + args[argi]);
            continue;
        }
        if(cs1 == 0)
        {
            cs1 = to_int(args[argi], 0);
            if(cs1 == 0)
                return usage_build("Invalid Component Selector 1: " + args[argi]);
            continue;
        }
        if(cs2 == 0)
        {
            cs2 = to_int(args[argi], 0);
            if(cs2 == 0)
                return usage_build("Invalid Component Selector 2: " + args[argi]);
            continue;
        }
    }
    if((dataset == 2) && (cs1 == 1))    // MinMaxElevation
    {
        cognitics::cdb::BuildMinMaxElevation(cdb, 2);
        return EXIT_SUCCESS;
    }
    else
    {
        return usage_build("Unsupported Component: " + cognitics::cdb::DatasetName(dataset) + " " + std::to_string(cs1) + " " + std::to_string(cs2));
    }
    return EXIT_SUCCESS;
}

int main_lod(size_t arg_start)
{
    int workers { 8 };
    int dataset { 0 };
    int cs1 { 0 };
    int cs2 { 0 };
    for(size_t argi = arg_start, argc = args.size(); argi < argc; ++argi)
    {
        if(args[argi] == "-workers")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_lod("Missing worker thread count");
            workers = to_int(args[argi], 8);
            continue;
        }
        if(dataset == 0)
        {
            dataset = to_int(args[argi], 0);
            if(dataset == 0)
                dataset = cognitics::cdb::DatasetCode(args[argi]);
            if(dataset == 0)
                return usage_lod("Invalid Dataset: " + args[argi]);
            continue;
        }
        if(cs1 == 0)
        {
            cs1 = to_int(args[argi], 0);
            if(cs1 == 0)
                return usage_lod("Invalid Component Selector 1: " + args[argi]);
            continue;
        }
        if(cs2 == 0)
        {
            cs2 = to_int(args[argi], 0);
            if(cs2 == 0)
                return usage_lod("Invalid Component Selector 2: " + args[argi]);
            continue;
        }
    }
    if((dataset == 1) && (cs1 == 1) && (cs2 == 1))  // Elevation, PrimaryTerrainElevation
    {
        return cognitics::cdb::cdb_lod(cdb, 1, workers) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if((dataset == 4) && (cs1 == 1) && (cs2 == 1))  // Imagery, YearlyVstiRepresentation
    {
        return cognitics::cdb::cdb_lod(cdb, 4, workers) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else
        return usage_lod("Unsupported Component: " + cognitics::cdb::DatasetName(dataset) + " " + std::to_string(cs1) + " " + std::to_string(cs2));
    return EXIT_SUCCESS;
}

int main_sample(size_t arg_start)
{
    int lod { 24 };
    double north { DBL_MAX };
    double south { -DBL_MAX };
    double east { DBL_MAX };
    double west { -DBL_MAX };
    double width { 1024 };
    double height { 1024 };
    int dataset { 0 };
    int cs1 { 0 };
    int cs2 { 0 };
    std::string outfile;
    for(size_t argi = arg_start, argc = args.size(); argi < argc; ++argi)
    {
        if(args[argi] == "-lod")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing target LOD");
            lod = to_int(args[argi], 24);
            continue;
        }
        if(args[argi] == "-bounds")
        {
            if(argi > argc - 4)
                return usage_sample("Missing bounds");
            ++argi;
            north = to_double(args[argi], DBL_MAX);
            ++argi;
            south = to_double(args[argi], -DBL_MAX);
            ++argi;
            east = to_double(args[argi], DBL_MAX);
            ++argi;
            west = to_double(args[argi], -DBL_MAX);
            continue;
        }
        if(args[argi] == "-width")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing width");
            width = to_int(args[argi], 1024);
            continue;
        }
        if(args[argi] == "-height")
        {
            ++argi;
            if(argi > argc - 1)
                return usage_inject("Missing width");
            width = to_int(args[argi], 1024);
            continue;
        }
        if(dataset == 0)
        {
            dataset = to_int(args[argi], 0);
            if(dataset == 0)
                dataset = cognitics::cdb::DatasetCode(args[argi]);
            if(dataset == 0)
                return usage_sample("Invalid Dataset: " + args[argi]);
            continue;
        }
        if(cs1 == 0)
        {
            cs1 = to_int(args[argi], 0);
            if(cs1 == 0)
                return usage_sample("Invalid Component Selector 1: " + args[argi]);
            continue;
        }
        if(cs2 == 0)
        {
            cs2 = to_int(args[argi], 0);
            if(cs2 == 0)
                return usage_sample("Invalid Component Selector 2: " + args[argi]);
            continue;
        }
        outfile = args[argi];
    }
    if(outfile.empty())
        return usage_sample("Missing outfile parameter");
    if(north == DBL_MAX)
        return usage_sample("-bounds option is required");
    auto params = cognitics::cdb::cdb_sample_parameters();
    params.cdb = cdb;
    params.dataset = dataset;
    params.north = north;
    params.south = south;
    params.east = east;
    params.west = west;
    params.width = width;
    params.height = height;
    params.lod = lod;
    params.outfile = outfile;
    if((dataset == 1) && (cs1 == 1) && (cs2 == 1))  // Elevation, PrimaryTerrainElevation
        return cognitics::cdb::cdb_sample(params) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 4) && (cs1 == 1) && (cs2 == 1))  // Imagery, YearlyVstiRepresentation
        return cognitics::cdb::cdb_sample(params) ? EXIT_SUCCESS : EXIT_FAILURE;
    else if((dataset == 100) && (cs1 == 1) && (cs2 == 1))    // GSFeature, Man-made, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 1) && (cs2 == 3))    // GSFeature, Man-made, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 1) && (cs2 == 5))    // GSFeature, Man-made, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 2) && (cs2 == 1))    // GSFeature, Natural, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 2) && (cs2 == 3))    // GSFeature, Natural, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 2) && (cs2 == 5))    // GSFeature, Natural, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 3) && (cs2 == 1))    // GSFeature, Trees, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 4) && (cs2 == 1))    // GSFeature, Airport light, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 4) && (cs2 == 3))    // GSFeature, Airport, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 4) && (cs2 == 5))    // GSFeature, Airport, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 100) && (cs1 == 5) && (cs2 == 1))    // GSFeature, Environmental light, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 1) && (cs2 == 1))    // GTFeature, Man-made, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 1) && (cs2 == 3))    // GTFeature, Man-made, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 1) && (cs2 == 5))    // GTFeature, Man-made, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 2) && (cs2 == 1))    // GTFeature, Tree, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 2) && (cs2 == 3))    // GTFeature, Tree, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 2) && (cs2 == 5))    // GTFeature, Tree, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 101) && (cs1 == 3) && (cs2 == 1))    // GTFeature, Moving Model, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 1) && (cs2 == 1))    // GeoPoliticalGTFeature, Boundary, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 1) && (cs2 == 3))    // GeoPoliticalGTFeature, Boundary, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 1) && (cs2 == 5))    // GeoPoliticalGTFeature, Boundary, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 2) && (cs2 == 1))    // GeoPoliticalGTFeature, Location, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 2) && (cs2 == 3))    // GeoPoliticalGTFeature, Location, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 2) && (cs2 == 5))    // GeoPoliticalGTFeature, Location, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 3) && (cs2 == 1))    // GeoPoliticalGTFeature, Constraint, point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 3) && (cs2 == 3))    // GeoPoliticalGTFeature, Constraint, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 102) && (cs1 == 3) && (cs2 == 5))    // GeoPoliticalGTFeature, Constraint, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 201) && (cs1 == 2) && (cs2 == 3))    // RoadNetwork, Road Network, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 201) && (cs1 == 2) && (cs2 == 7))    // RoadNetwork, Road Network, lineal figure point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 201) && (cs1 == 3) && (cs2 == 3))    // RoadNetwork, Airport Network, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 201) && (cs1 == 3) && (cs2 == 5))    // RoadNetwork, Airport Network, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 202) && (cs1 == 2) && (cs2 == 3))    // RailRoadNetwork, RailRoad Network, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 202) && (cs1 == 2) && (cs2 == 7))    // RailRoadNetwork, RailRoad Network, lineal figure point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 203) && (cs1 == 2) && (cs2 == 3))    // PowerLineNetwork, PowerLine Network, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 203) && (cs1 == 2) && (cs2 == 7))    // PowerLineNetwork, PowerLine Network, lineal figure point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 3))    // HydrographyNetwork, Hydrography Network, lineal features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 5))    // HydrographyNetwork, Hydrography Network, polygon features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 7))    // HydrographyNetwork, Hydrography Network, lineal figure point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else if((dataset == 204) && (cs1 == 2) && (cs2 == 9))    // HydrographyNetwork, Hydrography Network, polygon figure point features
    {
        auto features = cognitics::cdb::Features(cdb, dataset, cs1, cs2, lod, std::make_tuple(north, south, east, west));
        cognitics::cdb::WriteFeaturesToOGRFile(outfile, features);
        return EXIT_SUCCESS;
    }
    else
        return usage_sample("Unsupported Component: " + cognitics::cdb::DatasetName(dataset) + " " + std::to_string(cs1) + " " + std::to_string(cs2));
    return EXIT_SUCCESS;
}

int main_validate(size_t arg_start)
{
    double north { DBL_MAX };
    double south { -DBL_MAX };
    double east { DBL_MAX };
    double west { -DBL_MAX };
    int dataset { 0 };
    int cs1 { 0 };
    int cs2 { 0 };
    for(size_t argi = arg_start, argc = args.size(); argi < argc; ++argi)
    {
        if(args[argi] == "-bounds")
        {
            if(argi > argc - 4)
                return usage_validate("Missing bounds");
            ++argi;
            north = to_double(args[argi], DBL_MAX);
            ++argi;
            south = to_double(args[argi], -DBL_MAX);
            ++argi;
            east = to_double(args[argi], DBL_MAX);
            ++argi;
            west = to_double(args[argi], -DBL_MAX);
            continue;
        }
        if(dataset == 0)
        {
            dataset = to_int(args[argi], 0);
            if(dataset == 0)
                dataset = cognitics::cdb::DatasetCode(args[argi]);
            if(dataset == 0)
                return usage_validate("Invalid Dataset: " + args[argi]);
            continue;
        }
        if(cs1 == 0)
        {
            cs1 = to_int(args[argi], 0);
            if(cs1 == 0)
                return usage_validate("Invalid Component Selector 1: " + args[argi]);
            continue;
        }
        if(cs2 == 0)
        {
            cs2 = to_int(args[argi], 0);
            if(cs2 == 0)
                return usage_validate("Invalid Component Selector 2: " + args[argi]);
            continue;
        }
    }
    if((dataset == 100) && (cs1 == 1) && (cs2 == 1))    // GSFeature, Man-made, point features
    {
        cognitics::cdb::ReportMissingGSFeatureData(cdb, std::make_tuple(north, south, east, west));
        return EXIT_SUCCESS;
    }
    if((dataset == 101) && (cs1 == 1) && (cs2 == 1))    // GTFeature, Man-made, point features
    {
        cognitics::cdb::ReportMissingGTFeatureData(cdb, std::make_tuple(north, south, east, west));
        return EXIT_SUCCESS;
    }
    if((dataset == 101) && (cs1 == 2) && (cs2 == 1))    // GTFeature, Tree, point features
    {
        cognitics::cdb::ReportMissingGTFeatureData(cdb, std::make_tuple(north, south, east, west));
        return EXIT_SUCCESS;
    }
    else
    {
        return usage_validate("Unsupported Component: " + cognitics::cdb::DatasetName(dataset) + " " + std::to_string(cs1) + " " + std::to_string(cs2));
    }
    return EXIT_SUCCESS;
}

int main(int argc, char** argv)
{
    cognitics::gdal::init(argv[0]);

    std::ofstream logfile;
    int result { EXIT_FAILURE };
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    auto ts_start = std::chrono::steady_clock::now();
    for(int argi = 0; argi < argc; ++argi)
        args.emplace_back(argv[argi]);
    std::string command;
    size_t command_argi { 0 };
    for(size_t argi = 1; argi < argc; ++argi)
    {
        if(args[argi] == "-logfile")
        {
            if(argi > argc - 1)
                return usage_inject("Missing log filename");
            ++argi;
            logfile.open(args[argi].c_str(), std::ios::out);
            ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
            continue;
        }
        if(cdb.empty())
        {
            cdb = args[argi];
            continue;
        }
        command = args[argi];
        command_argi = argi + 1;
        break;
    }
    if(cdb.empty())
        return usage();
    if(command.empty())
        return usage();
    std::transform(command.begin(), command.end(), command.begin(), ::tolower);

    if(command == "inject")
        result = main_inject(command_argi);
    else if(command == "build")
        result = main_build(command_argi);
    else if(command == "lod")
        result = main_lod(command_argi);
    else if(command == "sample")
        result = main_sample(command_argi);
    else if(command == "validate")
        result = main_validate(command_argi);
    else
        return usage("Invalid command: " + command);

    auto ts_stop = std::chrono::steady_clock::now();
    ccl::Log::instance()->write(ccl::LINFO, "");
    ccl::Log::instance()->write(ccl::LINFO, "runtime: " + std::to_string(std::chrono::duration<double>(ts_stop - ts_start).count()) +  "s");
    return result;
}


