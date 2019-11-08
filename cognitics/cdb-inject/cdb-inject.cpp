
#include <cdb_tile/Tile.h>
#include <cdb_util/cdb_util.h>

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/ArgumentParser.h>

#include <iostream>
#include <fstream>
#include <chrono>
#include <future>
#include <deque>


int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    GDALAllRegister();

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    args.AddOption("dry-run", 0, "", "perform dry run");
    args.AddOption("skip-overviews", 0, "", "skip LOD downsampling");
    args.AddArgument("Image/Path");
    args.AddArgument("CDB");

    if(args.Parse(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto cdb = args.Arguments().at(1);
    std::ofstream logfile;
    if(args.Option("logfile"))
    {
        auto logfn = args.Parameters("logfile").at(0);
        logfile.open(logfn.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }

    ccl::ObjLog log;
    log << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();

    auto raster_param = args.Arguments().at(0);
    bool raster_param_is_directory = ccl::directoryExists(raster_param);

    auto raster_filenames = std::vector<std::string>();
    raster_filenames.push_back(raster_param);
    if(raster_param_is_directory)
    {
        raster_filenames.clear();
        auto tif_files = ccl::FileInfo::getAllFiles(raster_param, "*.tif");
        std::transform(tif_files.begin(), tif_files.end(), std::back_inserter(raster_filenames), [](const ccl::FileInfo& fi) { return fi.getFileName(); });
        auto jp2_files = ccl::FileInfo::getAllFiles(raster_param, "*.jp2");
        std::transform(jp2_files.begin(), jp2_files.end(), std::back_inserter(raster_filenames), [](const ccl::FileInfo& fi) { return fi.getFileName(); });
    }

    log << "Gathering information on " << raster_filenames.size() << " source file(s)..." << log.endl;
    auto tiles = std::vector<cognitics::cdb::Tile>();
    for(auto raster_filename : raster_filenames)
    {
        auto raster_info = cognitics::cdb::ReadRasterInfo(raster_filename);
        auto pixel_size = std::min<double>(std::abs(raster_info.PixelSizeX), std::abs(raster_info.PixelSizeY));
        auto target_lod = cognitics::cdb::LodForPixelSize(pixel_size);      // or specify manually
        auto coords = cognitics::cdb::CoordinatesRange(raster_info.West, raster_info.East, raster_info.South, raster_info.North);
        auto raster_tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)0), target_lod);
        /*
        // only build tiles that are fully covered by source
        raster_tiles.erase(std::remove_if(raster_tiles.begin(), raster_tiles.end(), [=](const cognitics::cdb::Tile& tile)
            {
                double tile_north, tile_south, tile_east, tile_west;
                std::tie(tile_north, tile_south, tile_east, tile_west) = cognitics::cdb::NSEWBoundsForTileInfo(cognitics::cdb::TileInfoForTile(tile));
                return !((tile_north <= raster_info.North) && (tile_south >= raster_info.South) && (tile_east <= raster_info.East) && (tile_west >= raster_info.West));
            }
        ), raster_tiles.end());
        */
        tiles.insert(tiles.end(), raster_tiles.begin(), raster_tiles.end());
    }
    std::sort(tiles.begin(), tiles.end());
    tiles.erase(std::unique(tiles.begin(), tiles.end()), tiles.end());

    log << "Identified " << tiles.size() << " tiles to generate." << log.endl;

    GDALRasterSampler sampler;
    if(raster_param_is_directory)
        sampler.AddDirectory(raster_param, { "tif", "jp2" });
    else
        sampler.AddFile(raster_param);

    // TODO: if we want to build partially covered tiles, we need to pull in the existing CDB imagery

    auto tileinfos = std::vector<cognitics::cdb::TileInfo>();
    std::transform(tiles.begin(), tiles.end(), std::back_inserter(tileinfos), [](const cognitics::cdb::Tile& tile) { return cognitics::cdb::TileInfoForTile(tile); });
    std::for_each(tileinfos.begin(), tileinfos.end(), [](cognitics::cdb::TileInfo& ti) { ti.dataset = 4; });


    if(args.Option("dry-run"))
    {
        // TODO: generate json job descriptions

    }

    if(!cognitics::cdb::IsCDB(cdb))
        cognitics::cdb::MakeCDB(cdb);

    std::vector<std::future<bool>> tasks;
    std::for_each(tileinfos.begin(), tileinfos.end(), [&](cognitics::cdb::TileInfo& ti) { 
        tasks.emplace_back(std::async(std::launch::async, cognitics::cdb::BuildImageryTileFromSampler, cdb, std::ref(sampler), ti)); } );
    for(size_t i = 0, c = tasks.size(); i < c; ++i)
    {
        tasks[i].get();
        log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(tileinfos[i]) << log.endl;
    }

    if(!args.Option("skip-overviews"))
        cognitics::cdb::BuildImageryOverviews(cdb);

    auto ts_stop = std::chrono::steady_clock::now();
    log << "ELAPSED: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return EXIT_SUCCESS;
}
