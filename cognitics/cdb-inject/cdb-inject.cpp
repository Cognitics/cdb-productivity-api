
#include <cdb_util/cdb_inject.h>

#include <cdb_util/cdb_util.h>
#include <cdb_tile/Tile.h>
#include <ccl/FileInfo.h>
#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/ArgumentParser.h>
#include <ccl/gdal.h>

#include <iostream>
#include <fstream>
#include <chrono>

#if _WIN32
#include <filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#elif __GNUC__ && (__GNUC__ < 8)
#include <experimental/filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#else
#include <filesystem>
#endif


int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);
    cognitics::gdal::init(argv[0]);

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    args.AddOption("bounds", 4, "<south> <west> <north> <east>", "bounds for area of interest");
    args.AddOption("lod", 1, "<lod>", "specify target LOD");
    args.AddOption("workers", 1, "<N>", "specify the number of worker threads");
    args.AddOption("imagery", 1, "<filename/path>", "source imagery filename or path");
    args.AddOption("elevation", 1, "<filename/path>", "source elevation filename or path");
    args.AddOption("dry-run", 0, "", "perform dry run");
    args.AddOption("count-tiles", 0, "", "perform a dry run, and only report the number of tiles");
    args.AddOption("build-overviews", 0, "", "perform LOD downsampling");
    args.AddArgument("CDB");

    if(args.Parse(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto params = cognitics::cdb::cdb_inject_parameters();
    params.cdb = args.Arguments().at(0);
    std::ofstream logfile;
    if(args.Option("logfile"))
    {
        auto logfn = args.Parameters("logfile").at(0);
        logfile.open(logfn.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }
    if(args.Option("workers"))
        params.workers = std::stoi(args.Parameters("workers").at(0));
    params.build_overviews = args.Option("build-overviews");
    params.count_tiles = args.Option("count-tiles");
    params.dry_run = args.Option("dry-run");
    params.imagery = args.Parameters("imagery");
    params.elevation = args.Parameters("elevation");
    if(args.Option("lod"))
        params.lod = std::stoi(args.Parameters("lod").at(0));
    if(args.Option("bounds"))
    {
        params.south = std::stod(args.Parameters("bounds").at(0));
        params.west = std::stod(args.Parameters("bounds").at(1));
        params.north = std::stod(args.Parameters("bounds").at(2));
        params.east = std::stod(args.Parameters("bounds").at(3));
    }

    ccl::ObjLog log;
    log << ccl::LNOTICE << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();
    cognitics::cdb::cdb_inject(params);
    auto ts_stop = std::chrono::steady_clock::now();
    log << "ELAPSED: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return EXIT_SUCCESS;
}
