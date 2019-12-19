
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

    GDALAllRegister();

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    args.AddOption("bounds", 4, "<south> <west> <north> <east>", "bounds for area of interest");
    args.AddOption("lod", 1, "<lod>", "specify target LOD");
    args.AddOption("imagery", 1, "<filename/path>", "source imagery filename or path");
    args.AddOption("elevation", 1, "<filename/path>", "source elevation filename or path");
    args.AddOption("dry-run", 0, "", "perform dry run");
    args.AddOption("skip-overviews", 0, "", "skip LOD downsampling");
    args.AddArgument("CDB");

    if(args.Parse(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto cdb = args.Arguments().at(0);
    std::ofstream logfile;
    if(args.Option("logfile"))
    {
        auto logfn = args.Parameters("logfile").at(0);
        logfile.open(logfn.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }

    ccl::ObjLog log;
    log << ccl::LNOTICE << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();

    auto imagery_filenames = std::vector<std::string>();
    bool imagery_enabled = args.Option("imagery");
    for(auto imagery_param : args.Parameters("imagery"))
    {
        if(!std::filesystem::exists(imagery_param))
        {
            log << ccl::LWARNING << imagery_param << " not found." << log.endl;
            continue;
        }
        if(std::filesystem::is_directory(imagery_param))
        {
            auto tif_files = ccl::FileInfo::getAllFiles(imagery_param, "*.tif");
            std::transform(tif_files.begin(), tif_files.end(), std::back_inserter(imagery_filenames), [](const ccl::FileInfo& fi) { return fi.getFileName(); });
            auto jp2_files = ccl::FileInfo::getAllFiles(imagery_param, "*.jp2");
            std::transform(jp2_files.begin(), jp2_files.end(), std::back_inserter(imagery_filenames), [](const ccl::FileInfo& fi) { return fi.getFileName(); });
        }
        imagery_filenames.push_back(imagery_param);
    }

    auto elevation_filenames = std::vector<std::string>();
    bool elevation_enabled = args.Option("elevation");
    for(auto elevation_param : args.Parameters("elevation"))
    {
        if(!std::filesystem::exists(elevation_param))
        {
            log << ccl::LWARNING << elevation_param << " not found." << log.endl;
            continue;
        }
        if(std::filesystem::is_directory(elevation_param))
        {
            auto tif_files = ccl::FileInfo::getAllFiles(elevation_param, "*.tif");
            std::transform(tif_files.begin(), tif_files.end(), std::back_inserter(elevation_filenames), [](const ccl::FileInfo& fi) { return fi.getFileName(); });
        }
        elevation_filenames.push_back(elevation_param);
    }

    auto raster_info_by_filename = std::map<std::string, cognitics::cdb::RasterInfo>();

    auto imagery_tiles = std::vector<cognitics::cdb::Tile>();
    auto imagery_tileinfos = std::vector<cognitics::cdb::TileInfo>();
    if(imagery_enabled)
    {
        log << ccl::LINFO << "Gathering information on " << imagery_filenames.size() << " imagery file(s)..." << log.endl;
        for(auto filename : imagery_filenames)
        {
            auto raster_info = cognitics::cdb::ReadRasterInfo(filename);
            raster_info_by_filename[filename] = raster_info;
            auto pixel_size = std::min<double>(std::abs(raster_info.PixelSizeX), std::abs(raster_info.PixelSizeY));
            auto target_lod = cognitics::cdb::LodForPixelSize(pixel_size);
            if(args.Option("lod"))
                target_lod = std::stoi(args.Parameters("lod").at(0));
            if(args.Option("bounds"))
            {
                raster_info.South = std::max<double>(raster_info.South, std::stod(args.Parameters("bounds").at(0)));
                raster_info.West = std::max<double>(raster_info.West, std::stod(args.Parameters("bounds").at(1)));
                raster_info.North = std::min<double>(raster_info.North, std::stod(args.Parameters("bounds").at(2)));
                raster_info.East = std::min<double>(raster_info.East, std::stod(args.Parameters("bounds").at(3)));
            }
            auto coords = cognitics::cdb::CoordinatesRange(raster_info.West, raster_info.East, raster_info.South, raster_info.North);
            auto raster_tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)0), target_lod);
            imagery_tiles.insert(imagery_tiles.end(), raster_tiles.begin(), raster_tiles.end());
        }
        std::sort(imagery_tiles.begin(), imagery_tiles.end());
        imagery_tiles.erase(std::unique(imagery_tiles.begin(), imagery_tiles.end()), imagery_tiles.end());
        log << "Identified " << imagery_tiles.size() << " imagery tiles to generate." << log.endl;
        std::transform(imagery_tiles.begin(), imagery_tiles.end(), std::back_inserter(imagery_tileinfos), [](const cognitics::cdb::Tile& tile) { return cognitics::cdb::TileInfoForTile(tile); });
        std::for_each(imagery_tileinfos.begin(), imagery_tileinfos.end(), [](cognitics::cdb::TileInfo& ti) { ti.dataset = 4; });
    }

    auto elevation_tiles = std::vector<cognitics::cdb::Tile>();
    auto elevation_tileinfos = std::vector<cognitics::cdb::TileInfo>();
    if(elevation_enabled)
    {
        log << ccl::LINFO << "Gathering information on " << elevation_filenames.size() << " elevation file(s)..." << log.endl;
        for(auto filename : elevation_filenames)
        {
            auto raster_info = cognitics::cdb::ReadRasterInfo(filename);
            raster_info_by_filename[filename] = raster_info;
            auto pixel_size = std::min<double>(std::abs(raster_info.PixelSizeX), std::abs(raster_info.PixelSizeY));
            auto target_lod = cognitics::cdb::LodForPixelSize(pixel_size);
            if(args.Option("lod"))
                target_lod = std::stoi(args.Parameters("lod").at(0));
            if(args.Option("bounds"))
            {
                raster_info.South = std::max<double>(raster_info.South, std::stod(args.Parameters("bounds").at(0)));
                raster_info.West = std::max<double>(raster_info.West, std::stod(args.Parameters("bounds").at(1)));
                raster_info.North = std::min<double>(raster_info.North, std::stod(args.Parameters("bounds").at(2)));
                raster_info.East = std::min<double>(raster_info.East, std::stod(args.Parameters("bounds").at(3)));
            }
            auto coords = cognitics::cdb::CoordinatesRange(raster_info.West, raster_info.East, raster_info.South, raster_info.North);
            auto raster_tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)0), target_lod);
            elevation_tiles.insert(elevation_tiles.end(), raster_tiles.begin(), raster_tiles.end());
        }
        std::sort(elevation_tiles.begin(), elevation_tiles.end());
        elevation_tiles.erase(std::unique(elevation_tiles.begin(), elevation_tiles.end()), elevation_tiles.end());
        log << "Identified " << elevation_tiles.size() << " elevation tiles to generate." << log.endl;
        std::transform(elevation_tiles.begin(), elevation_tiles.end(), std::back_inserter(elevation_tileinfos), [](const cognitics::cdb::Tile& tile) { return cognitics::cdb::TileInfoForTile(tile); });
        std::for_each(elevation_tileinfos.begin(), elevation_tileinfos.end(), [](cognitics::cdb::TileInfo& ti) { ti.dataset = 1; });
    }

    if(args.Option("dry-run"))
    {
        // TODO: we need to differentiate between imagery and elevation
        auto tileinfos = std::vector<cognitics::cdb::TileInfo>();
        tileinfos.insert(tileinfos.end(), imagery_tileinfos.begin(), imagery_tileinfos.end());
        tileinfos.insert(tileinfos.end(), elevation_tileinfos.begin(), elevation_tileinfos.end());
        std::stringstream ss;
        ss << "{\n";
        for(size_t i = 0, c = tileinfos.size(); i < c; ++i)
        {
            auto& ti = tileinfos.at(i);
            double tile_north, tile_south, tile_east, tile_west;
            std::tie(tile_north, tile_south, tile_east, tile_west) = cognitics::cdb::NSEWBoundsForTileInfo(ti);
            if(i > 0)
                ss << ",\n";
            ss << "\"" << cognitics::cdb::FileNameForTileInfo(ti) << "\": { \"source_filenames\" = [";
            bool first = true;
            auto& source_filenames = (ti.dataset == 1) ? elevation_filenames : imagery_filenames;
            for(auto filename : source_filenames)
            {
                auto& raster_info = raster_info_by_filename[filename];
                bool match = !((tile_north <= raster_info.South) && (tile_south >= raster_info.North) && (tile_east <= raster_info.West) && (tile_west >= raster_info.East));
                if(!match)
                    continue;
                if(!first)
                    ss << ",";
                ss << "\"" << filename << "\"";
                first = false;
            }
            ss << "]}";
               
        }
        ss << "\n}\n";

        std::cout << ss.str();

        return EXIT_SUCCESS;
    }

    if(!cognitics::cdb::IsCDB(cdb))
        cognitics::cdb::MakeCDB(cdb);

    if(!imagery_tileinfos.empty())
    {
        GDALRasterSampler sampler;
        std::for_each(imagery_filenames.begin(), imagery_filenames.end(), [&](std::string& fn) { sampler.AddFile(fn); }); 

        std::vector<std::future<bool>> tasks;
        std::for_each(imagery_tileinfos.begin(), imagery_tileinfos.end(), [&](cognitics::cdb::TileInfo& ti) { 
            tasks.emplace_back(std::async(std::launch::async, cognitics::cdb::BuildImageryTileFromSampler, cdb, std::ref(sampler), ti)); } );
        for(size_t i = 0, c = tasks.size(); i < c; ++i)
        {
            tasks[i].get();
            log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(imagery_tileinfos[i]) << log.endl;
        }
    }

    if(!elevation_tileinfos.empty())
    {
        bool use_dsm = false;

        if(use_dsm)
        {
            elev::DataSourceManager dsm(50 * 1024 * 1024);
            std::for_each(elevation_filenames.begin(), elevation_filenames.end(), [&](std::string& fn) { dsm.AddFile_Raster_GDAL(fn); }); 
            dsm.generateBSP();
            elev::Elevation_DSM sampler(&dsm, elev::ELEVATION_BILINEAR);
            for(size_t i = 0, c = elevation_tileinfos.size(); i < c; ++i)
            {
                auto& ti = elevation_tileinfos.at(i);
                cognitics::cdb::BuildElevationTileFromSampler2(cdb, sampler, ti);
                log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(ti) << log.endl;
            }
        }
        else
        {
            GDALRasterSampler sampler;
            std::for_each(elevation_filenames.begin(), elevation_filenames.end(), [&](std::string& fn) { sampler.AddFile(fn); }); 

            /* testing
            for(size_t i = 0, c = elevation_tileinfos.size(); i < c; ++i)
            {
                auto& ti = elevation_tileinfos.at(i);
                cognitics::cdb::BuildElevationTileFromSampler(cdb, sampler, ti);
                log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(ti) << log.endl;
            }
            */

            std::vector<std::future<bool>> tasks;
            std::for_each(elevation_tileinfos.begin(), elevation_tileinfos.end(), [&](cognitics::cdb::TileInfo& ti) { 
                tasks.emplace_back(std::async(std::launch::async, cognitics::cdb::BuildElevationTileFromSampler, cdb, std::ref(sampler), ti)); } );
            for(size_t i = 0, c = tasks.size(); i < c; ++i)
            {
                tasks[i].get();
                log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(elevation_tileinfos[i]) << log.endl;
            }
        }
    }

    if(!args.Option("skip-overviews"))
    {
        if(!imagery_tileinfos.empty())
            cognitics::cdb::BuildImageryOverviews(cdb);
        if(!elevation_tileinfos.empty())
            cognitics::cdb::BuildElevationOverviews(cdb);
    }

    auto ts_stop = std::chrono::steady_clock::now();
    log << "ELAPSED: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return EXIT_SUCCESS;
}
