
#include <cdb_util/cdb_util.h>

#include <cdb_util/FeatureDataDictionary.h>

#include <ccl/ObjLog.h>
#include <ccl/FileInfo.h>

#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <chrono>

void ReportMissingGSFeatureData(const std::string& cdb, double north = DBL_MAX, double south = -DBL_MAX, double east = DBL_MAX, double west = -DBL_MAX)
{
    ccl::ObjLog log;
    auto tiles = cognitics::cdb::FeatureTileInfoForTiledDataset(cdb, 100, north, south, east, west);
    auto model_filenames = std::vector<std::string>();
    for(auto tile : tiles)
    {
        auto tile_models = cognitics::cdb::GSModelReferencesForTile(cdb, tile);
        if(tile_models.empty())
            continue;
        log << "GS TILE: " << cognitics::cdb::FileNameForTileInfo(tile) << " (" << tile_models.size() << " model references)" << log.endl;
        model_filenames.insert(model_filenames.end(), tile_models.begin(), tile_models.end());

        if(model_filenames.size() > 10)
            break;
    }

    std::sort(model_filenames.begin(), model_filenames.end());
    model_filenames.erase(std::unique(model_filenames.begin(), model_filenames.end()), model_filenames.end());
    log << model_filenames.size() << " models" << log.endl;

    auto texture_filenames = std::vector<std::string>();
    for(auto model_filename : model_filenames)
    {
        auto filenames = cognitics::cdb::TextureFileNamesForModel(model_filename);
        if(!filenames.first)
        {
            log << "MODEL MISSING: " << model_filename << log.endl;
            continue;
        }
        for (auto filename : filenames.second)
        {
            auto fn = ccl::FileInfo(ccl::FileInfo(model_filename).getDirName()).getDirName() + "/" + filename;
            texture_filenames.push_back(fn);
        }
    }

    std::sort(texture_filenames.begin(), texture_filenames.end());
    texture_filenames.erase(std::unique(texture_filenames.begin(), texture_filenames.end()), texture_filenames.end());
    log << texture_filenames.size() << " textures" << log.endl;

    for(auto texture_filename : texture_filenames)
    {
        if(!cognitics::cdb::TextureExists(texture_filename))
            log << "TEXTURE MISSING: " << texture_filename << log.endl;
    }
}

void ReportMissingGTFeatureData(const std::string& cdb, double north = DBL_MAX, double south = -DBL_MAX, double east = DBL_MAX, double west = -DBL_MAX)
{
    ccl::ObjLog log;
    auto tiles = cognitics::cdb::FeatureTileInfoForTiledDataset(cdb, 101, north, south, east, west);
    auto model_filenames = std::vector<std::string>();
    for(auto tile : tiles)
    {
        auto tile_models = cognitics::cdb::GTModelReferencesForTile(cdb, tile);
        if(tile_models.empty())
            continue;
        log << "GT TILE: " << cognitics::cdb::FileNameForTileInfo(tile) << " (" << tile_models.size() << " model references)" << log.endl;
        model_filenames.insert(model_filenames.end(), tile_models.begin(), tile_models.end());

        if(model_filenames.size() > 10)
            break;
    }

    std::sort(model_filenames.begin(), model_filenames.end());
    model_filenames.erase(std::unique(model_filenames.begin(), model_filenames.end()), model_filenames.end());
    log << model_filenames.size() << " models" << log.endl;

    auto texture_filenames = std::vector<std::string>();
    for(auto model_filename : model_filenames)
    {
        auto filenames = cognitics::cdb::TextureFileNamesForModel(model_filename);
        if(!filenames.first)
        {
            log << "MODEL MISSING: " << model_filename << log.endl;
            continue;
        }
        for (auto filename : filenames.second)
        {
            auto fn = ccl::FileInfo(model_filename).getDirName() + "/" + filename;
            texture_filenames.push_back(fn);
        }
    }

    std::sort(texture_filenames.begin(), texture_filenames.end());
    texture_filenames.erase(std::unique(texture_filenames.begin(), texture_filenames.end()), texture_filenames.end());
    log << texture_filenames.size() << " textures" << log.endl;

    for(auto texture_filename : texture_filenames)
    {
        if(!ccl::fileExists(texture_filename))
            log << "TEXTURE MISSING: " << texture_filename << log.endl;
    }
}






int main(int argc, char** argv)
{
    std::string logfilename;
    std::ofstream logfile;
    ccl::ObjLog log;
    std::string cdb;

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    // TODO: parameters (cdb)
    // TODO: parameters (bounds)
    // TODO: parameters (-logfile <name>)




    if (!logfilename.empty())
    {
        ccl::FileInfo fi(logfilename);
        if (!ccl::directoryExists(fi.getDirName()))
        {
            ccl::makeDirectory(fi.getDirName());
        }
        std::string outfilename = logfilename;
        logfile.open(outfilename.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }


    auto yemen_cdb_path = std::string("D:/CDB/CDB_Yemen_4.0.0");
    //auto yemen_cdb_bounds = cognitics::cdb::CoordinatesRange(44, 45, 12, 13);

    auto sandiego_cdb_path = std::string("D:/CDB/CDB_san_diego_v3.5");
    //auto sandiego_cdb_bounds = cognitics::cdb::CoordinatesRange(-118, -117, 32, 33);

    auto la_cdb_path = std::string("D:/CDB/LosAngeles_CDB");

    cdb = sandiego_cdb_path;

    // TODO: verify bounds are working correctly for tiles
    // TODO: apply bounds to shapefile filter
    // TODO: TextureExists() implementation: zip testing for GS textures
    // TODO: linux testing

    auto ts_start = std::chrono::steady_clock::now();
    ReportMissingGTFeatureData(cdb);
    ReportMissingGSFeatureData(cdb);
    //ReportMissingGSFeatureData(cdb_path, 34.4, 34.2, -117.6, -117.8);
    //ReportMissingGSFeatureData(cdb_path, 13, 12.8, 45.2, 45);
    //ReportMissingGTFeatureData(cdb_path, 13, 12.8, 45.2, 45);
    auto ts_stop = std::chrono::steady_clock::now();
    log << "ReportMissingFeatureData(): " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;

    
    return EXIT_SUCCESS;
}
