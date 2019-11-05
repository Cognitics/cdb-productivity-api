
#include <cdb_util/cdb_util.h>

#include <cdb_util/FeatureDataDictionary.h>

#include <cdb_tile/TileLatitude.h>
#include <ccl/miniz.h>

#include <dbflib/DBaseFile.h>

#include <ogr/File.h>
#include <flt/OpenFlight.h>
#include <flt/TexturePalette.h>

#include <cctype>
#include <locale>
#include <iomanip>

namespace cognitics {
namespace cdb {

std::vector<std::string> FileNamesForTiledDataset(const std::string& cdb, int dataset)
{
    auto result = std::vector<std::string>();
    auto tiles_path = cdb + "/Tiles";
    if(!ccl::directoryExists(tiles_path))
        return result;
    for(const auto& lat_entry : ccl::FileInfo::getSubDirectories(tiles_path))
    {
        for(const auto& lon_entry : ccl::FileInfo::getSubDirectories(lat_entry))
        {
            auto dataset_prefix = std::to_string(dataset);
            while(dataset_prefix.length() < 3)
                dataset_prefix = "0" + dataset_prefix;
            for(const auto& dataset_entry : ccl::FileInfo::getSubDirectories(lon_entry))
            {
                auto dataset_dir = ccl::FileInfo(dataset_entry).getBaseName();
                if(dataset_dir.substr(0, 3) != dataset_prefix)
                    continue;
                for(const auto& lod_entry : ccl::FileInfo::getSubDirectories(dataset_entry))
                {
                    for(const auto& uref_entry : ccl::FileInfo::getSubDirectories(lod_entry))
                    {
                        for(const auto& entry : ccl::FileInfo::getAllFiles(uref_entry, "*.*"))
                            result.push_back(entry.getFileName());
                    }
                }
            }
        }
    }
    return result;
}

std::vector<TileInfo> FeatureTileInfoForTiledDataset(const std::string& cdb, int dataset, double north, double south, double east, double west)
{
    auto filenames = cognitics::cdb::FileNamesForTiledDataset(cdb, dataset);
    auto tiles = cognitics::cdb::TileInfoForFileNames(filenames);
    tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [](const TileInfo& tile) { return !((tile.selector2 == 1) || (tile.selector2 == 3) || (tile.selector2 == 5) || (tile.selector2 == 7) || (tile.selector2 == 9)); }), tiles.end());
    if (north != DBL_MAX)
    {
        tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [=](const TileInfo& tile)
            {
                double tile_north, tile_south, tile_east, tile_west;
                std::tie(tile_north, tile_south, tile_east, tile_west) = cognitics::cdb::NSEWBoundsForTileInfo(tile);
                return (tile_north <= south) || (tile_south >= north) || (tile_east <= west) || (tile_west >= east);
            }
        ), tiles.end());
    }
    return tiles;
}

TileInfo TileInfoForFileName(const std::string& filename)
{
    auto tokens = std::vector<std::string>();
    auto token = std::string();
    auto iss = std::istringstream(filename);
    while (std::getline(iss, token, '_'))
        tokens.push_back(token);
    if (tokens.size() < 7)
        throw std::runtime_error("invalid tile filename");
    auto result = TileInfo();
    auto ns = tokens[0][0];
    result.latitude = std::stoi(tokens[0].substr(1));
    if (ns == 'S')
        result.latitude *= -1;
    auto ew = tokens[0][3];
    result.longitude = std::stoi(tokens[0].substr(4));
    if (ew == 'W')
        result.longitude *= -1;
    result.dataset = std::stoi(tokens[1].substr(1));
    result.selector1 = std::stoi(tokens[2].substr(1));
    result.selector2 = std::stoi(tokens[3].substr(1));
    if(tokens[4][1] == 'C')
        result.lod = -std::stoi(tokens[4].substr(2));
    else
        result.lod = std::stoi(tokens[4].substr(1));
    result.uref = std::stoi(tokens[5].substr(1));
    result.rref = std::stoi(tokens[6].substr(1));
    return result;
}

std::string FilePathForTileInfo(const TileInfo& tileinfo)
{
    std::stringstream ss;
    char ns = (tileinfo.latitude >= 0) ? 'N' : 'S';
    int latitude = std::abs(tileinfo.latitude);
    char ew = (tileinfo.longitude >= 0) ? 'E' : 'W';
    int longitude = std::abs(tileinfo.longitude);
    ss << ns << std::setfill('0') << std::setw(2) << latitude;
    ss << "/" << ew << std::setfill('0') << std::setw(3) << longitude;
    ss << "/" << Dataset(tileinfo.dataset).subdir();
    if(tileinfo.lod < 0)
        return ss.str() + "/LC/U0";
    ss << "/L" << std::setfill('0') << std::setw(2) << tileinfo.lod;
    ss << "/U" << tileinfo.uref;
    return ss.str();
}

std::string FileNameForTileInfo(const TileInfo& tileinfo)
{
    std::stringstream ss;
    char ns = (tileinfo.latitude >= 0) ? 'N' : 'S';
    int latitude = std::abs(tileinfo.latitude);
    char ew = (tileinfo.longitude >= 0) ? 'E' : 'W';
    int longitude = std::abs(tileinfo.longitude);
    std::string lc = (tileinfo.lod >= 0) ? "L" : "LC";
    int lod = std::abs(tileinfo.lod);
    ss << ns << std::setfill('0') << std::setw(2) << latitude;
    ss << ew << std::setfill('0') << std::setw(3) << longitude;
    ss << "_D" << std::setfill('0') << std::setw(3) << tileinfo.dataset;
    ss << "_S" << std::setfill('0') << std::setw(3) << tileinfo.selector1;
    ss << "_T" << std::setfill('0') << std::setw(3) << tileinfo.selector2;
    ss << "_" << lc << std::setfill('0') << std::setw(2) << lod;
    ss << "_U" << tileinfo.uref;
    ss << "_R" << tileinfo.rref;
    return ss.str();
}

std::tuple<double, double, double, double> NSEWBoundsForTileInfo(const TileInfo& tileinfo)
{
    auto ref_dimension = std::max<int>(int(std::pow(2, tileinfo.lod)), 1);
    auto lat_spacing = 1.0 / ref_dimension;
    auto tile_width = (int)get_tile_width(double(tileinfo.latitude));
    auto lon_spacing = (double)tile_width / ref_dimension;
    double south = tileinfo.latitude + (lat_spacing * tileinfo.uref);
    double north = south + lat_spacing;
    double west = tileinfo.longitude + (lon_spacing * tileinfo.rref);
    double east = west + lon_spacing;
    return std::make_tuple(north, south, east, west);
}

std::vector<TileInfo> TileInfoForFileNames(const std::vector<std::string>& filenames)
{
    auto files = std::vector<std::string>();
    for (auto entry : filenames)
    {
        auto filename = ccl::FileInfo(entry).getBaseName(true);
        files.push_back(filename);
    }
    std::sort(files.begin(), files.end());
    files.erase(std::unique(files.begin(), files.end()), files.end());

    auto result = std::vector<TileInfo>();
    for (auto filename : files)
        result.push_back(TileInfoForFileName(filename));
    return result;
}

std::vector<sfa::Feature*> FeaturesForOGRFile(const std::string& filename, double north, double south, double east, double west)
{
    auto result = std::vector<sfa::Feature*>();
    if(!ccl::fileExists(filename))
        return result;
    auto file = ogr::File();
    if(!file.open(filename))
        return result;
    auto layers = file.getLayers();
    for(auto layer : layers)
    {
        if(north != -DBL_MAX)
            layer->setSpatialFilter(west, south, east, north);
        layer->resetReading();
        while (sfa::Feature *feature = layer->getNextFeature())
            result.push_back(feature);
    }
    file.close();
    return result;
}

std::vector<ccl::AttributeContainer> AttributesForDBF(const std::string& filename)
{
    auto result = std::vector<ccl::AttributeContainer>();
    try
    {
        if(!ccl::fileExists(filename))
            return result;
        auto dbf = DBaseFile();
        if(!dbf.openFile(filename))
            return result;
        for(DBaseRecord& record : dbf.m_records)
        {
            auto attributes = ccl::AttributeContainer();
            if(record.m_recordData.size() != dbf.m_colDef.size())
                continue;
            for(size_t i = 0, c = dbf.m_colDef.size(); i < c; ++i)
            {
                auto key = dbf.m_colDef[i].m_fieldName;
                auto value = *record.m_recordData[i];
                value.erase(value.begin(), std::find_if(value.begin(), value.end(), [](int ch) { return !std::isspace(ch); }));
                value.erase(std::find_if(value.rbegin(), value.rend(), [](int ch) { return !std::isspace(ch); }).base(), value.end());
                attributes.setAttribute(key, value);
            }
            result.push_back(attributes);
        }
    }
    catch(...)
    {
        std::cerr << "  bad dbf: " << filename << std::endl;
    }
    return result;
}

std::map<std::string, ccl::AttributeContainer> AttributesByCNAM(const std::vector<ccl::AttributeContainer>& attrvec)
{
    auto result = std::map<std::string, ccl::AttributeContainer>();
    for(auto attr : attrvec)
    {
        if(attr.hasAttribute("CNAM"))
            result[attr.getAttributeAsString("CNAM")] = attr;
    }
    return result;
}

std::pair<bool, std::vector<std::string>> TextureFileNamesForModel(const std::string& filename)
{
    auto result = std::vector<std::string>();

    auto flt = (flt::OpenFlight*)nullptr;

    auto parent_path = ccl::FileInfo(filename).getDirName();
    if(ccl::FileInfo(parent_path).getSuffix() == "zip")
    {
        auto model_filename = ccl::FileInfo(filename).getBaseName();
        auto flt_size = size_t(0);
        auto flt_data = mz_zip_extract_archive_file_to_heap(parent_path.c_str(), model_filename.c_str(), &flt_size, 0);
        if(!flt_data)
            return std::make_pair(false, result);
        auto flt_str = std::string((char*)flt_data, flt_size);
        auto flt_ss = std::stringstream(flt_str, std::stringstream::binary | std::stringstream::in);
        flt = flt::OpenFlight::open(flt_ss);
        if (!flt)
            return std::make_pair(false, result);
        while (auto record = flt->getNextRecord())
        {
            if (record->getRecordType() != flt::Record::FLT_TEXTUREPALETTE)
                continue;
            result.push_back(dynamic_cast<flt::TexturePalette*>(record)->fileName);
        }
        flt::OpenFlight::destroy(flt);
        return std::make_pair(true, result);
    }
    flt = flt::OpenFlight::open(filename);
    if (!flt)
        return std::make_pair(false, result);
    while (auto record = flt->getNextRecord())
    {
        if (record->getRecordType() != flt::Record::FLT_TEXTUREPALETTE)
            continue;
        result.push_back(dynamic_cast<flt::TexturePalette*>(record)->fileName);
    }
    flt::OpenFlight::destroy(flt);
    return std::make_pair(true, result);
}

std::vector<std::string> GSModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, double north, double south, double east, double west)
{
    auto result = std::vector<std::string>();
    auto shp_filepath = cognitics::cdb::FilePathForTileInfo(tileinfo);
    auto shp_filename = cognitics::cdb::FileNameForTileInfo(tileinfo);
    auto shp = cdb + "/Tiles/" + shp_filepath + "/" + shp_filename + ".shp";
    auto features = cognitics::cdb::FeaturesForOGRFile(shp, north, south, east, west);
    if(features.empty())
        return result;

    auto dbf_tile = tileinfo;
    ++dbf_tile.selector2;
    auto dbf_filepath = cognitics::cdb::FilePathForTileInfo(dbf_tile);
    auto dbf_filename = cognitics::cdb::FileNameForTileInfo(dbf_tile);
    auto dbf = cdb + "/Tiles/" + dbf_filepath + "/" + dbf_filename + ".dbf";

    auto attrvec = cognitics::cdb::AttributesForDBF(dbf);
    auto attrmap = cognitics::cdb::AttributesByCNAM(attrvec);
    if(attrmap.empty())
        return result;

    auto zip_tile = tileinfo;
    zip_tile.dataset = 300;
    zip_tile.selector1 = 1;
    zip_tile.selector2 = 1;
    auto zip_filepath = cognitics::cdb::FilePathForTileInfo(zip_tile);
    auto zip_filename = cognitics::cdb::FileNameForTileInfo(zip_tile);
    auto zip = cdb + "/Tiles/" + zip_filepath + "/" + zip_filename + ".zip";

    for (auto feature : features)
    {
        auto cnam = feature->attributes.getAttributeAsString("CNAM");
        if(attrmap.find(cnam) == attrmap.end())
            continue;
        auto facc = attrmap[cnam].getAttributeAsString("FACC");
        auto fsc = attrmap[cnam].getAttributeAsString("FSC");
        while (fsc.size() < 3)
            fsc = "0" + fsc;
        auto modl = attrmap[cnam].getAttributeAsString("MODL");
        auto model_filename = zip + "/" + zip_filename + "_" + facc + "_" + fsc + "_" + modl + ".flt";
        result.push_back(model_filename);
    }

    return result;
}

std::vector<std::string> GTModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, double north, double south, double east, double west)
{
    auto result = std::vector<std::string>();
    auto shp_filepath = cognitics::cdb::FilePathForTileInfo(tileinfo);
    auto shp_filename = cognitics::cdb::FileNameForTileInfo(tileinfo);
    auto shp = cdb + "/Tiles/" + shp_filepath + "/" + shp_filename + ".shp";
    auto features = cognitics::cdb::FeaturesForOGRFile(shp, north, south, east, west);
    if(features.empty())
        return result;

    auto dbf_tile = tileinfo;
    ++dbf_tile.selector2;
    auto dbf_filepath = cognitics::cdb::FilePathForTileInfo(dbf_tile);
    auto dbf_filename = cognitics::cdb::FileNameForTileInfo(dbf_tile);
    auto dbf = cdb + "/Tiles/" + dbf_filepath + "/" + dbf_filename + ".dbf";

    auto attrvec = cognitics::cdb::AttributesForDBF(dbf);
    auto attrmap = cognitics::cdb::AttributesByCNAM(attrvec);
    if(attrmap.empty())
        return result;

    auto fdd = cognitics::cdb::FeatureDataDictionary();

    for (auto feature : features)
    {
        auto cnam = feature->attributes.getAttributeAsString("CNAM");
        if(attrmap.find(cnam) == attrmap.end())
            continue;
        auto facc = attrmap[cnam].getAttributeAsString("FACC");
        auto fsc = attrmap[cnam].getAttributeAsString("FSC");
        while (fsc.size() < 3)
            fsc = "0" + fsc;
        auto modl = attrmap[cnam].getAttributeAsString("MODL");
        auto model_filename = cdb + "/GTModel/500_GTModelGeometry/" + fdd.Subdirectory(facc) + "/D500_S001_T001_" + facc + "_" + fsc + "_" + modl + ".flt";
        result.push_back(model_filename);
    }

    return result;
}


bool TextureExists(const std::string& filename)
{
    //you can iterate through all the files in an archive(using mz_zip_reader_get_num_files()) and retrieve detailed info on each file by calling mz_zip_reader_file_stat().

    auto parent_path = ccl::FileInfo(filename).getDirName();
    if(ccl::FileInfo(parent_path).getSuffix() == "zip")
    {
        auto texture_filename = ccl::FileInfo(filename).getBaseName();
        mz_zip_archive zip;
        memset(&zip, 0, sizeof(zip));
        //auto z = "D:/CDB/LosAngeles_CDB/Tiles/N34/W118/301_GSModelTexture/L06/U0/N34W118_D301_S001_T001_L06_U0_R1.zip";
        //if(!mz_zip_reader_init_file(&zip, z, 0))
        if(!mz_zip_reader_init_file(&zip, parent_path.c_str(), 0))
            return false;
        bool result = (mz_zip_reader_locate_file(&zip, texture_filename.c_str(), nullptr, 0) >= 0);
        mz_zip_reader_end(&zip);
        return result;
    }

    return ccl::FileInfo::fileExists(filename);
}




}
}
