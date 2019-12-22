
#include <cdb_tile/Tile.h>
#include <cdb_util/cdb_util.h>

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/ArgumentParser.h>
#include <ccl/JobManager.h>

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

void initializeGDAL(int argc, char **argv)
{
#ifndef WIN32
    char *gdal_data_var = getenv("GDAL_DATA");
    if (gdal_data_var == NULL)
    {
        putenv("GDAL_DATA=/usr/local/share/gdal");
    }
    char *gdal_plugins_var = getenv("GDAL_DRIVER_PATH");
    if (gdal_plugins_var == NULL)
    {
        putenv("GDAL_DRIVER_PATH=/usr/local/bin/gdalplugins");
    }
#else
    size_t requiredSize;
    getenv_s(&requiredSize, NULL, 0, "GDAL_DATA");
    if (requiredSize == 0)
    {
        ccl::FileInfo fi(argv[0]);
        int bufSize = 1024;
        char *envBuffer = new char[bufSize];
        std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");
        //std::cout << "GDAL_DATA=" << dataDir << "\n";
        //std::cout << "argv[0]=" << argv[0] << "\n";
        sprintf_s(envBuffer, bufSize, "GDAL_DATA=%s", dataDir.c_str());
        _putenv(envBuffer);
        std::string driverDir = ccl::joinPaths(fi.getDirName(), "gdalplugins");
        char *pluginsEnvBuffer = new char[bufSize];
        sprintf_s(pluginsEnvBuffer, bufSize, "GDAL_DRIVER_PATH=%s", driverDir.c_str());
        _putenv(pluginsEnvBuffer);


    }
#endif
    GDALAllRegister();
}


class JobProgressReporter
{
    ccl::ObjLog log;
    int totalJobs;
    int completedJobs;
    std::mutex m;
public:
    JobProgressReporter(int totalJobs=0) : totalJobs(totalJobs),completedJobs(0)
    {
        
    }
    void setTotalJobCount(int val)
    {
        totalJobs = val;
    }
    void reportCompletedJob(const std::string &msg)
    {
        m.lock();
        completedJobs++;
        if (totalJobs)
        {
            std::stringstream ss;
            ss.precision(3);
            float pct = float(completedJobs) / float(totalJobs) * 100.0;
            ss << pct << "%";
            if(msg.length()>0)
                log << ccl::LINFO << msg << log.endl;
            log << ccl::LINFO << ss.str() << log.endl;
        }
        else
        {
            if (msg.length() > 0)
                log << ccl::LINFO << msg << log.endl;
            log << ccl::LINFO << "0%" << log.endl;
        }

        m.unlock();
    }
    
};

class CDBTileJobThreadDataManager : public ccl::ThreadDataManager
{
public:
    virtual void onThreadFinished(void) {}
};

class CDBTileJob : public ccl::Job
{
    GDALRasterSampler& sampler;
    std::string cdb;
    cognitics::cdb::TileInfo tileinfo;
    bool isElevation;
    static int createdJobCount;
    static int processedJobCount;
    static std::mutex countMutex;
    JobProgressReporter& reporter;
protected:
    virtual int onChildFinished(ccl::Job *job, int result)
    {
        return 0;
    }

public:
    virtual ~CDBTileJob() {}

    CDBTileJob(ccl::JobManager *manager, 
        const std::string& cdb, 
        GDALRasterSampler& sampler, 
        const cognitics::cdb::TileInfo& tileinfo, JobProgressReporter &reporter, bool isElevation=false) :
        ccl::Job(manager, NULL), cdb(cdb),sampler(sampler), tileinfo(tileinfo), reporter(reporter), isElevation(isElevation)
    {
        countMutex.lock();
        createdJobCount++;
        countMutex.unlock();
    }

    int execute(void)
    {
        ccl::ObjLog log;
        log <<  "Processing " << cognitics::cdb::FileNameForTileInfo(tileinfo) << log.endl;
        if(isElevation)
        {
            cognitics::cdb::BuildElevationTileFromSampler(cdb, sampler, tileinfo);
        }
        else
        {
            cognitics::cdb::BuildImageryTileFromSampler(cdb, sampler, tileinfo);
        }
        reporter.reportCompletedJob("");

        //log << "Finished " << cognitics::cdb::FileNameForTileInfo(tileinfo) << log.endl;
        
        return 0;
    }
};

int CDBTileJob::createdJobCount = 0;
int CDBTileJob::processedJobCount = 0;
std::mutex CDBTileJob::countMutex;

int main(int argc, char** argv)
{
    std::ios_base::sync_with_stdio(false);

    initializeGDAL(argc, argv);

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
    int workerCount = 8;
    if (args.Option("workers"))
    {
        auto workerCountStr = args.Parameters("workers").at(0);
        workerCount = atoi(workerCountStr.c_str());
        if(workerCount==0)
        {
            log << ccl::LERR << workerCountStr << " is not a valid number of worker threads. Defaulting to 8." << log.endl;
            workerCount = 8;
        }
    }
    auto imagery_filenames = std::vector<std::string>();
    bool imagery_enabled = args.Option("imagery");
    for(auto imagery_param : args.Parameters("imagery"))
    {
        std::string newFilename = imagery_param;

        if(!std::filesystem::exists(imagery_param))
        {
            //Is this a pseudo filename ending with the table name?
            std::string tableName;
            std::string strippedFilename = imagery_param;
            ccl::GetFilenameAndTable(imagery_param, strippedFilename, tableName);
            if (!tableName.empty())
            {
                //This is a geopackage file with the tablename included
                if (!std::filesystem::exists(strippedFilename))
                {
                    log << ccl::LWARNING << strippedFilename << " not found." << log.endl;
                    continue;
                }
                // Does the table exist inside the geopackage?
                //todo: test the tablename inside the gpkg
            }
            else
            {
                log << ccl::LWARNING << imagery_param << " not found." << log.endl;
                continue;
            }
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

    bool countTiles = args.Option("count-tiles");
    if(args.Option("dry-run") || countTiles)
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
        if (!countTiles)
        {
            std::cout << ss.str();
        }
        else
        {
            std::cout << tileinfos.size();
        }

        return EXIT_SUCCESS;
    }

    if(!cognitics::cdb::IsCDB(cdb))
        cognitics::cdb::MakeCDB(cdb);
    JobProgressReporter jobReporter;
    CDBTileJobThreadDataManager cdbTileJobThreadDataManager;
    ccl::JobManager jobManager(workerCount, NULL, &cdbTileJobThreadDataManager);

    if(!imagery_tileinfos.empty())
    {
        GDALRasterSampler sampler;
        std::for_each(imagery_filenames.begin(), imagery_filenames.end(), [&](std::string& fn) { sampler.AddFile(fn); });
        for(auto&& ti : imagery_tileinfos)
        {
            auto cdbTileJob = new CDBTileJob(&jobManager, cdb, std::ref(sampler), ti, jobReporter,false);
            jobManager.submitJob(cdbTileJob);
        }
        jobReporter.setTotalJobCount(imagery_tileinfos.size());
        jobManager.waitForCompletion();
        
#if 0
        std::vector<std::future<bool>> tasks;
        std::for_each(imagery_tileinfos.begin(), imagery_tileinfos.end(), [&](cognitics::cdb::TileInfo& ti) { 
            tasks.emplace_back(std::async(std::launch::async, cognitics::cdb::BuildImageryTileFromSampler, cdb, std::ref(sampler), ti)); } );
        for(size_t i = 0, c = tasks.size(); i < c; ++i)
        {
            tasks[i].get();
            log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(imagery_tileinfos[i]) << log.endl;
        }
#endif
    }

    if(!elevation_tileinfos.empty())
    {
        bool use_dsm = true;

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
            for (auto&& ti : elevation_tileinfos)
            {
                auto cdbTileJob = new CDBTileJob(&jobManager, cdb, std::ref(sampler), ti, jobReporter, true);
                jobManager.submitJob(cdbTileJob);
            }
            jobReporter.setTotalJobCount(elevation_tileinfos.size());
            jobManager.waitForCompletion();
            /*
            for(size_t i = 0, c = tasks.size(); i < c; ++i)
            {
                tasks[i].get();
                log << "[" << (i + 1) << "/" << c << "] " << cognitics::cdb::FileNameForTileInfo(elevation_tileinfos[i]) << log.endl;
            }
            */
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
