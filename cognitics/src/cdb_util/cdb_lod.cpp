
#include <cdb_util/cdb_lod.h>

#include <cdb_util/cdb_util.h>

#include <ccl/FileInfo.h>
#include <ccl/JobManager.h>

#include <cstdlib>
#include <fstream>
#include <mutex>
#include <functional>

#if _WIN32
#include <filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#elif __GNUC__ && (__GNUC__ < 8)
#include <experimental/filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#else
#include <filesystem>
#endif

namespace
{

std::string FullFilenameForTileInfo(const cognitics::cdb::TileInfo& tile_info)
{
    auto child_path = cognitics::cdb::FilePathForTileInfo(tile_info);
    auto child_filename = cognitics::cdb::FileNameForTileInfo(tile_info);
    auto child_extension = ".tif";
    if(tile_info.dataset == 4)
        child_extension = ".jp2";
    auto child_fn = ccl::joinPaths(child_path, child_filename + child_extension);
    return child_fn;
}

class TileJob : public ccl::Job
{
public:
    std::string cdb;
    int max_lod { 0 };
    cognitics::cdb::TileInfo tile_info;
    
    std::vector<TileJob*> children;
    std::vector<std::string> child_filenames;
    std::vector<std::filesystem::file_time_type> child_filetimes;
    std::mutex children_mutex;

    TileJob(ccl::JobManager* manager, ccl::Job *owner = NULL) : Job(manager, owner) { }

    virtual int onChildFinished(Job *job, int result)
    {
        children_mutex.lock();
        auto child_it = std::find(children.begin(), children.end(), job);
        if(child_it != children.end())
            children.erase(child_it);
        children_mutex.unlock();
        {
            auto child = dynamic_cast<TileJob*>(job);
            auto child_fn = cdb + "/Tiles/" + FullFilenameForTileInfo(child->tile_info);
            if(ccl::fileExists(child_fn))
            {
                children_mutex.lock();
                child_filenames.push_back(child_fn);
                child_filetimes.push_back(std::filesystem::last_write_time(child_fn));
                children_mutex.unlock();
            }
        }
        if(children.size() > 0)
            return (int)children.size();

        if(child_filenames.empty())
            return 0;

        auto fn = cdb + "/Tiles/" + FullFilenameForTileInfo(tile_info);
        if(ccl::fileExists(fn))
        {
            auto file_ts = std::filesystem::last_write_time(fn);
            auto child_filenames_tmp = std::vector<std::string>();
            for(size_t i = 0, c = child_filenames.size(); i < c; ++i)
            {
                if(child_filetimes[i] > file_ts)
                    child_filenames_tmp.push_back(child_filenames[i]);
            }
            child_filenames = child_filenames_tmp;
        }

        if(child_filenames.empty())
            return 0;

        auto sampler_filenames = child_filenames;
        double tile_north, tile_south, tile_east, tile_west;
        std::tie(tile_north, tile_south, tile_east, tile_west) = cognitics::cdb::NSEWBoundsForTileInfo(tile_info);
        auto coords = cognitics::cdb::CoordinatesRange(tile_west, tile_east, tile_south, tile_north);
        auto tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)tile_info.dataset), tile_info.lod - 1);
        auto coverage_tiles = cognitics::cdb::CoverageTilesForTiles(cdb, tiles);
        for (auto ctile : coverage_tiles)
        {
            auto cdb = ctile.first;
            auto tile = ctile.second;
            auto tile_info = cognitics::cdb::TileInfoForTile(tile);
            auto tile_filepath = cognitics::cdb::FilePathForTileInfo(tile_info);
            auto tile_filename = cognitics::cdb::FileNameForTileInfo(tile_info);
            auto filename = cdb + "/Tiles/" + tile_filepath + "/" + tile_filename;
            if (tile_info.dataset == 1)
                filename += ".tif";
            if (tile_info.dataset == 4)
                filename += ".jp2";
            if (std::find(sampler_filenames.begin(), sampler_filenames.end(), filename) == sampler_filenames.end())
                sampler_filenames.push_back(filename);
        }
        GDALRasterSampler sampler;
        for(auto filename : sampler_filenames)
            sampler.AddFile(filename);
        if(tile_info.dataset == 1)
        {
            printf("%s\n", cognitics::cdb::FileNameForTileInfo(tile_info).c_str());
            cognitics::cdb::BuildElevationTileFromSampler(cdb, sampler, tile_info);
        }
        if(tile_info.dataset == 4)
        {
            printf("%s\n", cognitics::cdb::FileNameForTileInfo(tile_info).c_str());
            cognitics::cdb::BuildImageryTileFromSampler(cdb, sampler, tile_info);
        }
        gdalsampler::CacheManager::getInstance()->Unload();
        return 0;
    }

    virtual int execute(void)
    {
        if(tile_info.lod >= max_lod)
            return 0;

        //printf("[TileJob] %s\n", cognitics::cdb::FileNameForTileInfo(tile_info).c_str());

        auto raster_info = cognitics::cdb::RasterInfoFromTileInfo(tile_info);
        auto coords = cognitics::cdb::CoordinatesRange(raster_info.West, raster_info.East, raster_info.South, raster_info.North);
        auto tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset(tile_info.dataset), tile_info.lod + 1);
        for(auto tile : tiles)
        {
            children_mutex.lock();
            children.push_back(new TileJob(manager, this));
            auto job = children.back();
            children_mutex.unlock();
            job->cdb = cdb;
            job->max_lod = max_lod;
            job->tile_info = cognitics::cdb::TileInfoForTile(tile);
            manager->submitJob(job);
        }
        return (int)children.size();
    }
};


}


namespace cognitics {
namespace cdb {

bool cdb_lod(cdb_lod_parameters& params)
{
    auto ts_start = std::chrono::steady_clock::now();

    ccl::JobManager job_manager(params.workers);
    auto jobs = std::vector<TileJob*>();


    auto geocells = cognitics::cdb::GeocellsForCdb(params.cdb);
    for(auto geocell : geocells)
    {
        std::string geocell_path = params.cdb + "/Tiles/" + geocell.first + "/" + geocell.second;
        int lat = std::stoi(geocell.first.substr(1));
        if(geocell.first[0] == 'S')
            lat *= -1;
        int lon = std::stoi(geocell.second.substr(1));
        if(geocell.second[0] == 'W')
            lon *= -1;
        if(1)
        {
            auto maxlod_elevation = cognitics::cdb::MaxLodForDatasetPath(geocell_path + "/001_Elevation");
            if(maxlod_elevation >= 0)
            {
                jobs.push_back(new TileJob(&job_manager));
                auto job = jobs.back();
                job->cdb = params.cdb;
                job->max_lod = maxlod_elevation;
                memset(&job->tile_info, 0, sizeof(job->tile_info));
                job->tile_info.latitude = lat;
                job->tile_info.longitude = lon;
                job->tile_info.lod = -10;
                job->tile_info.dataset = 1;
                job->tile_info.selector1 = 1;
                job->tile_info.selector2 = 1;
                job_manager.submitJob(job);
            }
        }
        if(1)
        {
            auto maxlod_imagery = cognitics::cdb::MaxLodForDatasetPath(geocell_path + "/004_Imagery");
            if(maxlod_imagery >= 0)
            {
                jobs.push_back(new TileJob(&job_manager));
                auto job = jobs.back();
                job->cdb = params.cdb;
                job->max_lod = maxlod_imagery;
                memset(&job->tile_info, 0, sizeof(job->tile_info));
                job->tile_info.latitude = lat;
                job->tile_info.longitude = lon;
                job->tile_info.lod = -10;
                job->tile_info.dataset = 4;
                job->tile_info.selector1 = 1;
                job->tile_info.selector2 = 1;
                job_manager.submitJob(job);
            }
        }
    }
    job_manager.waitForCompletion();
    for(auto job : jobs)
        delete job;

    return true;
}

}
}
