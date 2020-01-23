
#include <cdb_util/cdb_util.h>

#include <ccl/LogStream.h>
#include <ccl/ObjLog.h>
#include <ccl/FileInfo.h>
#include <ccl/ArgumentParser.h>
#include <ccl/JobManager.h>

#include <cstdlib>
#include <fstream>
#include <chrono>
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
    GDALRasterSampler* sampler;
    
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

        for(auto filename : child_filenames)
            sampler->AddFile(filename);
        if(tile_info.dataset == 1)
            cognitics::cdb::BuildElevationTileFromSampler(cdb, *sampler, tile_info);
        if(tile_info.dataset == 4)
            cognitics::cdb::BuildImageryTileFromSampler(cdb, *sampler, tile_info);

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
            job->sampler = sampler;
            manager->submitJob(job);
        }
        return (int)children.size();
    }
};



int main(int argc, char** argv)
{
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    //args.AddOption("bounds", 4, "<south> <west> <north> <east>", "bounds for area of interest");
    //args.AddOption("force", 0, "", "force overwrite");
    args.AddOption("workers", 1, "<N>", "number of worker threads (default 8)");
    args.AddArgument("CDB");

    //if(args.Parse({ "cdbinfo.exe", "-logfile", "d:/cdbinfo.log", "-bounds", "12.8", "45.0", "13.0", "45.2", "D:/CDB/CDB_Yemen_4.0.0" }) == EXIT_FAILURE)
    //if(args.Parse({ "cdbinfo.exe", "-logfile", "d:/cdbinfo_yemen.log", "D:/CDB/CDB_Yemen_4.0.0" }) == EXIT_FAILURE)
    //if(args.Parse({ "cdbinfo.exe", "-gsfeatures", "-gtfeatures", "-logfile", "d:/cdbinfo_la.log", "-bounds", "34.0", "-118.0", "34.2", "-117.8", "D:/CDB/LosAngeles_CDB" }) == EXIT_FAILURE)
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

    double north = DBL_MAX;
    double south = -DBL_MAX;
    double east = DBL_MAX;
    double west = -DBL_MAX;
    if(args.Option("bounds"))
    {
        south = strtod(args.Parameters("bounds")[0].c_str(), nullptr);
        west = strtod(args.Parameters("bounds")[1].c_str(), nullptr);
        north = strtod(args.Parameters("bounds")[2].c_str(), nullptr);
        east = strtod(args.Parameters("bounds")[3].c_str(), nullptr);
    }

    int workers = 8;
    if(args.Option("workers"))
        workers = std::stoi(args.Parameters("workers").at(0));

    ccl::ObjLog log;
    log << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();

    ccl::JobManager job_manager(workers);
    auto jobs = std::vector<TileJob*>();


    GDALRasterSampler sampler;

    auto geocells = cognitics::cdb::GeocellsForCdb(cdb);
    for(auto geocell : geocells)
    {
        std::string geocell_path = cdb + "/Tiles/" + geocell.first + "/" + geocell.second;
        int lat = std::stoi(geocell.first.substr(1));
        if(geocell.first[0] == 'S')
            lat *= -1;
        int lon = std::stoi(geocell.second.substr(1));
        if(geocell.second[0] == 'W')
            lon *= -1;
        if(0)
        {
            auto maxlod_elevation = cognitics::cdb::MaxLodForDatasetPath(geocell_path + "/001_Elevation");
            if(maxlod_elevation >= 0)
            {
                jobs.push_back(new TileJob(&job_manager));
                auto job = jobs.back();
                job->cdb = cdb;
                job->max_lod = maxlod_elevation;
                memset(&job->tile_info, 0, sizeof(job->tile_info));
                job->tile_info.latitude = lat;
                job->tile_info.longitude = lon;
                job->tile_info.lod = -10;
                job->tile_info.dataset = 1;
                job->tile_info.selector1 = 1;
                job->tile_info.selector2 = 1;
                job->sampler = &sampler;
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
                job->cdb = cdb;
                job->max_lod = maxlod_imagery;
                memset(&job->tile_info, 0, sizeof(job->tile_info));
                job->tile_info.latitude = lat;
                job->tile_info.longitude = lon;
                job->tile_info.lod = -10;
                job->tile_info.dataset = 4;
                job->tile_info.selector1 = 1;
                job->tile_info.selector2 = 1;
                job->sampler = &sampler;
                job_manager.submitJob(job);
            }
        }
    }
    job_manager.waitForCompletion();
    for(auto job : jobs)
        delete job;

    log << log.endl;

    auto ts_stop = std::chrono::steady_clock::now();
    log << "cdb-lod runtime: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return EXIT_SUCCESS;
}


