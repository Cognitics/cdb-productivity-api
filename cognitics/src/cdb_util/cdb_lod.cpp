
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

class TileJob : public ccl::Job
{
public:
    TileJob(ccl::JobManager* manager, ccl::Job *owner = NULL) : Job(manager, owner) { }

    ccl::ObjLog log;
    std::string cdb;
    std::string filename;
    std::vector<std::string> child_filenames;

    virtual int execute(void)
    {
        auto stem = std::filesystem::path(filename).stem().string();
        log << "    " << stem << log.endl;
        try
        {
            auto errcode = std::error_code();
            auto filetime = std::filesystem::last_write_time(filename, errcode);
            if(!errcode)
            {
                auto filtered_children = std::vector<std::string>();
                for (auto child_filename : child_filenames)
                {
                    auto child_filetime = std::filesystem::last_write_time(child_filename);
                    if(child_filetime > filetime)
                        filtered_children.push_back(child_filename);
                }
                child_filenames = filtered_children;
            }
            if(child_filenames.empty())
                return 0;

            auto tile_info = cognitics::cdb::TileInfoForFileName(stem);
            double tile_north, tile_south, tile_east, tile_west;
            std::tie(tile_north, tile_south, tile_east, tile_west) = cognitics::cdb::NSEWBoundsForTileInfo(tile_info);
            auto coords = cognitics::cdb::CoordinatesRange(tile_west, tile_east, tile_south, tile_north);
            auto tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)tile_info.dataset), tile_info.lod - 1);
            auto coverage_tiles = cognitics::cdb::CoverageTilesForTiles(cdb, tiles);
            auto coverage_filenames = std::vector<std::string>();
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
                if (std::find(coverage_filenames.begin(), coverage_filenames.end(), filename) == coverage_filenames.end())
                    coverage_filenames.push_back(filename);
            }


            GDALRasterSampler sampler;
            for (auto coverage_filename : coverage_filenames)
                sampler.AddFile(coverage_filename);
            for (auto child_filename : child_filenames)
                sampler.AddFile(child_filename);
            if(tile_info.dataset == 1)
                cognitics::cdb::BuildElevationTileFromSampler(cdb, sampler, tile_info);
            if(tile_info.dataset == 4)
                cognitics::cdb::BuildImageryTileFromSampler(cdb, sampler, tile_info);
            gdalsampler::CacheManager::getInstance()->Unload();
        }
        catch(std::exception& e)
        {
            log << "      EXCEPTION: " << e.what() << log.endl;
        }
        return 0;
    }

};


}


namespace cognitics {
namespace cdb {

bool cdb_lod(const std::string& cdb, int workers)
{
    bool result = true;
    if(!cdb_lod(cdb, 1, workers))
        result = false;
    if(!cdb_lod(cdb, 4, workers))
        result = false;
    return result;
}

bool cdb_lod(const std::string& cdb, int dataset, int workers)
{
    ccl::ObjLog log;

    auto geocells = GeocellsForCdb(cdb);
    for(auto geocell : geocells)
    {
        std::string geocell_path = cdb + "/Tiles/" + geocell.first + "/" + geocell.second;
        int lat = LatitudeFromSubdirectory(geocell.first);
        int lon = LongitudeFromSubdirectory(geocell.second);
        auto dataset_path = geocell_path + "/" + DatasetSubdirectory(dataset);
        auto max_lod = MaxLodForDatasetPath(dataset_path);
        for(int target_lod = max_lod; target_lod > -10; --target_lod)
        {
            ccl::JobManager job_manager(workers);

            auto lod_path = dataset_path + "/" + SubdirectoryForLOD(target_lod);
            auto parent_path = dataset_path + "/" + SubdirectoryForLOD(target_lod - 1);
            log << parent_path << log.endl;

            auto lod_files = std::vector<std::string>();
            {
                auto tmp_files = std::vector<std::string>();
                for(const auto& entry : std::filesystem::recursive_directory_iterator(lod_path))
                {
                    if(std::filesystem::is_regular_file(entry))
                        tmp_files.push_back(entry.path().string());
                }
                for(auto tmp_file : tmp_files)
                {
                    try
                    {
                        auto ti = TileInfoForFileName(std::filesystem::path(tmp_file).stem().string());
                        if((ti.latitude == lat) && (ti.longitude == lon) && (ti.lod == target_lod))
                            lod_files.push_back(tmp_file);
                    }
                    catch(std::exception &)
                    {
                    }
                }
            }

            auto parent_files = std::vector<std::string>();
            {
                for(const auto& entry : std::filesystem::recursive_directory_iterator(parent_path))
                {
                    if(std::filesystem::is_regular_file(entry))
                        parent_files.push_back(entry.path().string());
                }
            }

            auto lod_files_by_parent = std::map<std::string, std::vector<std::string>>();

            for(auto lod_file : lod_files)
            {
                auto tile_info = TileInfoForFileName(std::filesystem::path(lod_file).stem().string());
                auto parent_info = tile_info;
                parent_info.lod -= 1;
                parent_info.uref /= 2;
                parent_info.rref /= 2;
                auto parent_file = cdb + "/Tiles/" + FilePathForTileInfo(parent_info) + "/" + FileNameForTileInfo(parent_info);
                parent_file = std::filesystem::path(parent_file).string();
                if (tile_info.dataset == 1)
                    parent_file += ".tif";
                if (tile_info.dataset == 4)
                    parent_file += ".jp2";
                lod_files_by_parent[parent_file].push_back(lod_file);
            }

            auto jobs = std::vector<TileJob*>();

            for(const auto& entry : lod_files_by_parent)
            {
                auto parent_file = entry.first;
                auto child_files = entry.second;
                jobs.push_back(new TileJob(&job_manager));
                auto job = jobs.back();
                job->cdb = cdb;
                job->filename = parent_file;
                job->child_filenames = child_files;
                job_manager.submitJob(job);
            }

            job_manager.waitForCompletion();
            for(auto job : jobs)
                delete job;
        }
    }

    return true;
}

}
}
