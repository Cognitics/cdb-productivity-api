
#include <cdb_util/cdb_util.h>
#include <ogr/File.h>

#include <cdb_util/FeatureDataDictionary.h>

#include <cdb_tile/TileLatitude.h>
#include <ccl/miniz.h>

#include <dbflib/DBaseFile.h>

#include <ogr/File.h>
#include <flt/OpenFlight.h>
#include <flt/TexturePalette.h>
#include <flt/Header.h>

#include <array>
#include <cctype>
#include <locale>
#include <iomanip>

#if _WIN32
#include <filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#elif __GNUC__ && (__GNUC__ < 8)
#include <experimental/filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#else
#include <filesystem>
#endif

namespace cognitics {
namespace cdb {

std::string DatasetName(int code)
{
    switch(code)
    {
        case 1: return "Elevation";
        case 2: return "MinMaxElevation";
        case 3: return "MaxCulture";
        case 4: return "Imagery";
        case 5: return "RMTexture";
        case 6: return "RMDescriptor";
        case 100: return "GSFeature";
        case 101: return "GTFeature";
        case 102: return "GeoPolitical";
        case 200: return "VectorMaterial";
        case 201: return "RoadNetwork";
        case 202: return "RailRoadNetwork";
        case 203: return "PowerLineNetwork";
        case 204: return "HydrographyNetwork";
        case 300: return "GSModelGeometry";
        case 301: return "GSModelTexture";
        case 302: return "GSModelSignature";
        case 303: return "GSModelDescriptor";
        case 400: return "NavData";
        case 401: return "Navigation";
        case 500: return "GTModelGeometry";
        case 501: return "GTModelTexture";
        case 502: return "GTModelSignature";
        case 503: return "GTModelDescriptor";
        case 600: return "MModelGeometry";
        case 601: return "MModelTexture";
        case 602: return "MModelSignature";
        case 603: return "MModelDescriptor";
    }
    return "Unknown";
}

int DatasetCode(const std::string& name)
{
    if(name == "Elevation") return 1;
    if(name == "MinMaxElevation") return 2;
    if(name == "MaxCulture") return 3;
    if(name == "Imagery") return 4;
    if(name == "RMTexture") return 5;
    if(name == "RMDescriptor") return 6;
    if(name == "GSFeature") return 100;
    if(name == "GTFeature") return 101;
    if(name == "GeoPolitical") return 102;
    if(name == "VectorMaterial") return 200;
    if(name == "RoadNetwork") return 201;
    if(name == "RailRoadNetwork") return 202;
    if(name == "PowerLineNetwork") return 203;
    if(name == "HydrographyNetwork") return 204;
    if(name == "GSModelGeometry") return 300;
    if(name == "GSModelTexture") return 301;
    if(name == "GSModelSignature") return 302;
    if(name == "GSModelDescriptor") return 303;
    if(name == "NavData") return 400;
    if(name == "Navigation") return 401;
    if(name == "GTModelGeometry") return 500;
    if(name == "GTModelTexture") return 501;
    if(name == "GTModelSignature") return 502;
    if(name == "GTModelDescriptor") return 503;
    if(name == "MModelGeometry") return 600;
    if(name == "MModelTexture") return 601;
    if(name == "MModelSignature") return 602;
    if(name == "MModelDescriptor") return 603;
    return 0;
}

std::string DatasetSubdirectory(int code)
{
    std::stringstream ss;
    ss << std::setw(3) << std::setfill('0') << code;
    ss << "_" << DatasetName(code);
    return ss.str();
}

int ComponentSelector1Code(int dataset, const std::string& name)
{
    if((dataset == 1) && (name == "ManMade")) return 1;
    return 0;
}


int LodForPixelSize(double pixel_size)
{
    for(int i = -10; i <= 23; ++i)
    {
        double lod_pixel_size = 0;
        if(i<0)
        {
            int abspow = std::pow(2, abs(i));
            lod_pixel_size = (1.0 / 1024) / (1/abspow);
        }
        else
        {
            lod_pixel_size = (1.0 / 1024) / std::pow(2, i);
        }
        
        if(lod_pixel_size < pixel_size)
            return i;
    }
    return 23;
}

double PixelSizeForLod(int lod)
{
    auto div = std::max<int>(std::pow(2, lod), 1);
    return 1.0 / div;
}

int TileDimensionForLod(int lod)
{
    return std::min<int>(std::pow(2, lod + 10), 1024);
}

double MinimumPixelSizeForLod(int lod, double latitude)
{
    auto div = std::max<int>(std::pow(2, lod), 1);
    auto lat_spacing = 1.0 / div;
    auto tile_width = (int)get_tile_width(latitude);
    auto lon_spacing = (double)tile_width / div;
    return std::min<double>(lat_spacing, lon_spacing);
}

TileInfo ParentTileInfo(const TileInfo& tileinfo)
{
    auto result = tileinfo;
    --result.lod;
    result.uref = std::floor(result.uref / 2);
    result.rref = std::floor(result.rref / 2);
    return result;
}

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

std::vector<TileInfo> FeatureTileInfoForTiledDataset(const std::string& cdb, int dataset, std::tuple<double, double, double, double> nsew)
{
    auto filenames = FileNamesForTiledDataset(cdb, dataset);
    auto tiles = TileInfoForFileNames(filenames);
    tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [](const TileInfo& tile)
        {
            return !((tile.selector2 == 1) || (tile.selector2 == 3) || (tile.selector2 == 5) || (tile.selector2 == 7) || (tile.selector2 == 9));
        }
    ), tiles.end());
    if (std::get<0>(nsew) != DBL_MAX)
    {
        tiles.erase(std::remove_if(tiles.begin(), tiles.end(), [=](const TileInfo& tile)
            {
                double tile_north, tile_south, tile_east, tile_west;
                std::tie(tile_north, tile_south, tile_east, tile_west) = NSEWBoundsForTileInfo(tile);
                return (tile_north <= std::get<1>(nsew)) || (tile_south >= std::get<0>(nsew)) || (tile_east <= std::get<3>(nsew)) || (tile_west >= std::get<2>(nsew));
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

TileInfo TileInfoForTile(const Tile& tile)
{
    auto info = TileInfo();
    info.latitude = std::floor(tile.getCoordinates().low().latitude().value());
    info.longitude = std::floor(tile.getCoordinates().low().longitude().value());
    info.dataset = tile.getDataset().code();
    info.lod = tile.getLod();
    info.selector1 = tile.getCs1();
    info.selector2 = tile.getCs2();
    info.rref = tile.getRref();
    info.uref = tile.getUref();
    return info;
}

std::tuple<double, double, double, double> NSEWBoundsForTileInfo(const TileInfo& tileinfo)
{
    auto div = std::max<int>(std::pow(2, tileinfo.lod), 1);
    auto lat_spacing = 1.0 / div;
    auto tile_width = (int)get_tile_width(double(tileinfo.latitude));
    auto lon_spacing = (double)tile_width / div;
    double south = tileinfo.latitude + (lat_spacing * tileinfo.uref);
    double north = south + lat_spacing;
    double west = tileinfo.longitude + (lon_spacing * tileinfo.rref);
    double east = west + lon_spacing;
    return std::make_tuple(north, south, east, west);
}

std::string BoundsStringForTileInfo(const TileInfo& tileinfo)
{
    double north, south, east, west;
    std::tie(north, south, east, west) = NSEWBoundsForTileInfo(tileinfo);
    std::stringstream ss;
    ss << "N: " << std::setprecision(9) << north << "  S:" << south << "  E:" << east << "  W:" << west;
    return ss.str();
}

std::vector<TileInfo> TileInfoForFileNames(const std::vector<std::string>& filenames)
{
    auto files = std::vector<std::string>();
    for(auto fn : filenames)
        files.push_back(ccl::FileInfo(fn).getBaseName());
    std::sort(files.begin(), files.end());
    files.erase(std::unique(files.begin(), files.end()), files.end());
    auto result = std::vector<TileInfo>();
    for(auto file : files)
        result.push_back(TileInfoForFileName(file));
    return result;
}

std::vector<sfa::Feature*> FeaturesForOGRFile(const std::string& filename, std::tuple<double, double, double, double> nsew)
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
        if(std::get<0>(nsew) != DBL_MAX)
            layer->setSpatialFilter(std::get<3>(nsew), std::get<1>(nsew), std::get<2>(nsew), std::get<0>(nsew));
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

std::vector<std::string> GSModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, std::tuple<double, double, double, double> nsew)
{
    auto result = std::vector<std::string>();
    auto shp_filepath = FilePathForTileInfo(tileinfo);
    auto shp_filename = FileNameForTileInfo(tileinfo);
    auto shp = cdb + "/Tiles/" + shp_filepath + "/" + shp_filename + ".shp";
    auto features = FeaturesForOGRFile(shp, nsew);
    if(features.empty())
        return result;

    auto dbf_tile = tileinfo;
    ++dbf_tile.selector2;
    auto dbf_filepath = FilePathForTileInfo(dbf_tile);
    auto dbf_filename = FileNameForTileInfo(dbf_tile);
    auto dbf = cdb + "/Tiles/" + dbf_filepath + "/" + dbf_filename + ".dbf";

    auto attrvec = AttributesForDBF(dbf);
    auto attrmap = AttributesByCNAM(attrvec);
    if(!attrmap.empty())
    {
        for (auto feature : features)
        {
            auto cnam = feature->attributes.getAttributeAsString("CNAM");
            if(attrmap.find(cnam) == attrmap.end())
                continue;
            auto attr = attrmap[cnam];
            for(auto key : attr.getKeys())
                feature->attributes.setAttribute(key, attr.getAttributeAsVariant(key));
        }
    }

    auto zip_tile = tileinfo;
    zip_tile.dataset = 300;
    zip_tile.selector1 = 1;
    zip_tile.selector2 = 1;
    auto zip_filepath = FilePathForTileInfo(zip_tile);
    auto zip_filename = FileNameForTileInfo(zip_tile);
    auto zip = cdb + "/Tiles/" + zip_filepath + "/" + zip_filename + ".zip";

    for (auto feature : features)
    {
        auto facc = feature->attributes.getAttributeAsString("FACC");
        auto fsc = feature->attributes.getAttributeAsString("FSC");
        while (fsc.size() < 3)
            fsc = "0" + fsc;
        auto modl = feature->attributes.getAttributeAsString("MODL");
        auto model_filename = zip + "/" + zip_filename + "_" + facc + "_" + fsc + "_" + modl + ".flt";
        result.push_back(model_filename);
    }

    return result;
}

std::vector<std::string> GTModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, std::tuple<double, double, double, double> nsew)
{
    auto result = std::vector<std::string>();
    auto shp_filepath = FilePathForTileInfo(tileinfo);
    auto shp_filename = FileNameForTileInfo(tileinfo);
    auto shp = cdb + "/Tiles/" + shp_filepath + "/" + shp_filename + ".shp";
    auto features = FeaturesForOGRFile(shp, nsew);
    if(features.empty())
        return result;

    auto dbf_tile = tileinfo;
    ++dbf_tile.selector2;
    auto dbf_filepath = FilePathForTileInfo(dbf_tile);
    auto dbf_filename = FileNameForTileInfo(dbf_tile);
    auto dbf = cdb + "/Tiles/" + dbf_filepath + "/" + dbf_filename + ".dbf";

    auto attrvec = AttributesForDBF(dbf);
    auto attrmap = AttributesByCNAM(attrvec);
    if(!attrmap.empty())
    {
        for (auto feature : features)
        {
            auto cnam = feature->attributes.getAttributeAsString("CNAM");
            if(attrmap.find(cnam) == attrmap.end())
                continue;
            auto attr = attrmap[cnam];
            for(auto key : attr.getKeys())
                feature->attributes.setAttribute(key, attr.getAttributeAsVariant(key));
        }
    }

    auto fdd = FeatureDataDictionary();

    for (auto feature : features)
    {
        auto facc = feature->attributes.getAttributeAsString("FACC");
        auto fsc = feature->attributes.getAttributeAsString("FSC");
        while (fsc.size() < 3)
            fsc = "0" + fsc;
        auto modl = feature->attributes.getAttributeAsString("MODL");
        auto modl_base = ccl::FileInfo(modl).getBaseName(true);
        modl = modl_base + ".flt";
        auto model_filename = cdb + "/GTModel/500_GTModelGeometry/" + fdd.Subdirectory(facc) + "/D500_S001_T001_" + facc + "_" + fsc + "_" + modl;
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

bool BuildImageryTileBytesFromSampler(GDALRasterSampler& sampler, const TileInfo& tileinfo, std::vector<unsigned char>& bytes)
{
    auto extents = gdalsampler::GeoExtents();
    std::tie(extents.north, extents.south, extents.east, extents.west) = NSEWBoundsForTileInfo(tileinfo);
    extents.width = TileDimensionForLod(tileinfo.lod);
    extents.height = extents.width;
    return sampler.Sample(extents, &bytes[0]);
}

bool BuildElevationTileFloatsFromSampler(GDALRasterSampler& sampler, const TileInfo& tileinfo, std::vector<float>& floats)
{
    auto extents = gdalsampler::GeoExtents();
    std::tie(extents.north, extents.south, extents.east, extents.west) = NSEWBoundsForTileInfo(tileinfo);
    extents.width = TileDimensionForLod(tileinfo.lod);
    extents.height = extents.width;
    double spacing_x = (extents.east - extents.west) / (extents.width + 1);
    double spacing_y = (extents.north - extents.south) / (extents.height + 1);
    extents.north -= (spacing_y * 0.5);
    extents.south -= (spacing_y * 0.5);
    extents.east -= (spacing_x * 0.5);
    extents.west -= (spacing_x * 0.5);
    return sampler.Sample(extents, &floats[0]);
}

bool BuildElevationTileFloatsFromSampler2(elev::Elevation_DSM& sampler, const TileInfo& tileinfo, std::vector<float>& floats)
{
    auto extents = gdalsampler::GeoExtents();
    std::tie(extents.north, extents.south, extents.east, extents.west) = NSEWBoundsForTileInfo(tileinfo);
    extents.width = TileDimensionForLod(tileinfo.lod);
    extents.height = extents.width;
    double spacing_x = (extents.east - extents.west) / extents.width;
    double spacing_y = (extents.north - extents.south) / extents.height;
    auto point = sfa::Point();
    for(int y = 0; y < extents.height; ++y)
    {
        point.setY(extents.south + (y * spacing_y));
        for(int x = 0; x < extents.width; ++x)
        {
            point.setX(extents.west + (x * spacing_x));
            if(sampler.Get(&point))
                floats[(y * extents.width) + x] = point.Z();
        }
    }
    return true;
}

RasterInfo ReadRasterInfo(const std::string& filename)
{
    GDALDataset *ds = NULL;
    auto info = RasterInfo();
    //Is this a pseudo filename ending with the table name?
    std::string tableName;
    std::string strippedFilename = filename;
    ccl::GetFilenameAndTable(filename, strippedFilename, tableName);
    if (!tableName.empty())
    {
        char** papszOptions = NULL;
        std::string tableStr = "TABLE=" + tableName;
        papszOptions = CSLAddString(papszOptions, tableStr.c_str());

        ds = (GDALDataset*)GDALOpenEx(strippedFilename.c_str(),
            GDAL_OF_READONLY, NULL, papszOptions, NULL);
    }
    else
    {
        ds = (GDALDataset *)GDALOpen(filename.c_str(), GA_ReadOnly);
    }
    if(ds == nullptr)
        return info;

    info.Width = ds->GetRasterXSize();
    info.Height = ds->GetRasterYSize();
    auto geotransform = std::array<double, 6>();
    auto has_geotransform = (ds->GetGeoTransform(&geotransform[0]) == CE_None);
    auto projref = ds->GetProjectionRef();
    if(projref==nullptr)
    {
        projref = "WGS84";
    }
    

    if(!has_geotransform)
        return info;

    auto file_srs = OGRSpatialReference(projref);
    auto app_srs = OGRSpatialReference();
    app_srs.SetWellKnownGeogCS("WGS84");
    auto transform = OGRCreateCoordinateTransformation(&file_srs, &app_srs);

    GDALClose(ds);

    info.OriginX = geotransform[0];
    info.OriginY = geotransform[3];
    info.PixelSizeX = geotransform[1];
    info.PixelSizeY = geotransform[5];

    double x_min = DBL_MAX;
    double x_max = -DBL_MAX;
    double y_min = DBL_MAX;
    double y_max = -DBL_MAX;

    int row_arr[2] = {0,info.Height-1};
    int col_arr[2] = {0,info.Width-1};

/*
    // Every post in the first and last row, 1
    for(int row = 0; row < 2; ++row)
    {
        for(int col = 0; col < 2; ++col)
        {
            //Xgeo = GT(0) + Xpixel * GT(1) + Yline * GT(2)
            //Ygeo = GT(3) + Xpixel * GT(4) + Yline * GT(5)
            auto x = geotransform[0] + (geotransform[1] * col_arr[col]) + (geotransform[2] * row_arr[row]);
            auto y = geotransform[3] + (geotransform[4] * col_arr[col]) + (geotransform[5] * row_arr[row]);
            x_min = std::min<double>(x_min, x);
            x_max = std::max<double>(x_max, x);
            y_min = std::min<double>(y_min, y);
            y_max = std::max<double>(y_max, y);
        }
    }
*/
    /*

    for(int row = 0; row < info.Height; ++row)
    {
        for(int col = 0; col < info.Width; ++col)
        {
            //Xgeo = GT(0) + Xpixel * GT(1) + Yline * GT(2)
            //Ygeo = GT(3) + Xpixel * GT(4) + Yline * GT(5)
            auto x = geotransform[0] + (geotransform[1] * col) + (geotransform[2] * row);
            auto y = geotransform[3] + (geotransform[4] * col) + (geotransform[5] * row);
            x_min = std::min<double>(x_min, x);
            x_max = std::max<double>(x_max, x);
            y_min = std::min<double>(y_min, y);
            y_max = std::max<double>(y_max, y);
        }
    }
    */

    for(int row = 0; row < info.Height; ++row)
    {
        bool only_first_and_last_col = false;
        if((row!=0) && (row != (info.Height-1)))
            only_first_and_last_col = true;

        for(int col = 0; col < info.Width; ++col)
        {
            if(only_first_and_last_col && (col!=0) && (col!=(info.Width-1)))
                continue;
            
            //Xgeo = GT(0) + Xpixel * GT(1) + Yline * GT(2)
            //Ygeo = GT(3) + Xpixel * GT(4) + Yline * GT(5)
            auto x = geotransform[0] + (geotransform[1] * col) + (geotransform[2] * row);
            auto y = geotransform[3] + (geotransform[4] * col) + (geotransform[5] * row);
            x_min = std::min<double>(x_min, x);
            x_max = std::max<double>(x_max, x);
            y_min = std::min<double>(y_min, y);
            y_max = std::max<double>(y_max, y);
        }
    }

    if(y_max < y_min)
        std::swap(y_max, y_min);
    if(x_max < x_min)
        std::swap(x_max, x_min);

    double sw_s = y_min;
    double sw_w = x_min;
    double se_s = y_min;
    double se_e = x_min;
    double nw_n = y_max;
    double nw_w = x_max;
    double ne_n = y_max;
    double ne_e = x_max;

    if(transform)
    {
        transform->Transform(1, &sw_w, &sw_s);
        transform->Transform(1, &se_e, &se_s);
        transform->Transform(1, &nw_w, &nw_n);
        transform->Transform(1, &ne_e, &ne_n);

        transform->Transform(1, &info.OriginX, &info.OriginY);

        info.PixelSizeX = fabs((nw_w - sw_w))/info.Width;
        info.PixelSizeY = fabs((nw_n - sw_s))/info.Height;

        OGRCoordinateTransformation::DestroyCT(transform);
    }

    info.South = std::min<double>(sw_s, se_s);
    info.North = std::max<double>(nw_n, ne_n);
    info.West = std::min<double>(sw_w, nw_w);
    info.East = std::max<double>(ne_e, ne_e);

    return info;
}

std::vector<float> FloatsFromTIF(const std::string& filename)
{
    auto result = std::vector<float>();

    auto dataset = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
    if(!dataset)
        return result;

    auto width = dataset->GetRasterXSize();
    auto height = dataset->GetRasterYSize();
    auto depth = dataset->GetRasterCount();

    double geotransform[6];
    if(dataset->GetGeoTransform(geotransform) != CE_None)
    {
        GDALClose(dataset);
        return result;
    }

    result.resize(width * height);
    auto discard = dataset->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, width, height, (unsigned char*)&result[0], width, height, GDT_Float32, 0, 0);

    GDALClose(dataset);

    return result;
}

std::vector<unsigned char> BytesFromJP2(const std::string& filename)
{
    auto result = std::vector<unsigned char>();

    auto dataset = (GDALDataset*)GDALOpen(filename.c_str(), GA_ReadOnly);
    if(!dataset)
        return result;

    auto width = dataset->GetRasterXSize();
    auto height = dataset->GetRasterYSize();
    auto depth = dataset->GetRasterCount();

    double geotransform[6];
    if(dataset->GetGeoTransform(geotransform) != CE_None)
    {
        GDALClose(dataset);
        return result;
    }

    result.resize(width * height * depth);
    auto discard1 = dataset->GetRasterBand(1)->RasterIO(GF_Read, 0, 0, width, height, (unsigned char*)&result[0], width, height, GDT_Byte, 3, width * 3);
    auto discard2 = dataset->GetRasterBand(2)->RasterIO(GF_Read, 0, 0, width, height, (unsigned char*)&result[1], width, height, GDT_Byte, 3, width * 3);
    auto discard3 = dataset->GetRasterBand(3)->RasterIO(GF_Read, 0, 0, width, height, (unsigned char*)&result[2], width, height, GDT_Byte, 3, width * 3);

    GDALClose(dataset);

    return result;
}


bool WriteBytesToJP2(const std::string& filename, const RasterInfo& rasterinfo, const std::vector<unsigned char>& bytes)
{
    auto mem = GetGDALDriverManager()->GetDriverByName("MEM");
    if(mem == NULL)
        return false;
    auto jp2 = GetGDALDriverManager()->GetDriverByName("JP2OpenJPEG");
    if(jp2 == NULL)
        return false;

    double geotransform[6] = { rasterinfo.OriginX, rasterinfo.PixelSizeX, 0.0, rasterinfo.OriginY, 0.0, rasterinfo.PixelSizeY };

    auto mem_ds = mem->Create("mem.tmp", rasterinfo.Width, rasterinfo.Height, 3, GDT_Byte, nullptr);
    mem_ds->SetGeoTransform(geotransform);

    OGRSpatialReference oSRS;
    oSRS.SetWellKnownGeogCS("WGS84");
    char *wkt = NULL;
    oSRS.exportToWkt(&wkt);
    mem_ds->SetProjection(wkt);
    CPLFree(wkt);

    auto discard1 = mem_ds->GetRasterBand(1)->RasterIO(GF_Write, 0, 0, rasterinfo.Width, rasterinfo.Height, (unsigned char*)&bytes[0], rasterinfo.Width, rasterinfo.Height, GDT_Byte, 3, rasterinfo.Width * 3);
    auto discard2 = mem_ds->GetRasterBand(2)->RasterIO(GF_Write, 0, 0, rasterinfo.Width, rasterinfo.Height, (unsigned char*)&bytes[1], rasterinfo.Width, rasterinfo.Height, GDT_Byte, 3, rasterinfo.Width * 3);
    auto discard3 = mem_ds->GetRasterBand(3)->RasterIO(GF_Write, 0, 0, rasterinfo.Width, rasterinfo.Height, (unsigned char*)&bytes[2], rasterinfo.Width, rasterinfo.Height, GDT_Byte, 3, rasterinfo.Width * 3);

    auto out_ds = jp2->CreateCopy(filename.c_str(), mem_ds, 1, NULL, NULL, NULL);
    GDALClose(out_ds);

    GDALClose(mem_ds);

    return true;
}

void WriteFloatsToText(const std::string& filename, const RasterInfo& rasterinfo, const std::vector<float>& floats)
{
    std::ofstream f;
    f.open(filename);
    for(int y = 0; y < rasterinfo.Height; ++y)
    {
        for(int x = 0; x < rasterinfo.Width; ++x)
            f << std::fixed << std::setw(8) << std::setprecision(2) << floats[(y * rasterinfo.Width) + x];
        f << "\n";
    }
    f.close();
}

bool WriteFloatsToTIF(const std::string& filename, const RasterInfo& rasterinfo, const std::vector<float>& floats, bool pixel_is_point)
{
    auto tif_driver = GetGDALDriverManager()->GetDriverByName("GTiff");
    if(tif_driver == NULL)
        return false;

    double geotransform[6] = { rasterinfo.OriginX - (0.5 * rasterinfo.PixelSizeX), rasterinfo.PixelSizeX, 0.0, rasterinfo.OriginY + (0.5 * rasterinfo.PixelSizeY), 0.0, rasterinfo.PixelSizeY };

    ccl::makeDirectory(ccl::FileInfo(filename).getDirName());
    auto tif_ds = tif_driver->Create(filename.c_str(), rasterinfo.Width, rasterinfo.Height, 1, GDT_Float32, nullptr);
    tif_ds->SetGeoTransform(geotransform);
    if(pixel_is_point)
        tif_ds->SetMetadataItem("AREA_OR_POINT", "POINT");
    else
        tif_ds->SetMetadataItem("AREA_OR_POINT", "AREA");

    OGRSpatialReference oSRS;
    oSRS.SetWellKnownGeogCS("WGS84");
    char *wkt = NULL;
    oSRS.exportToWkt(&wkt);
    tif_ds->SetProjection(wkt);
    CPLFree(wkt);

    auto tif_band = tif_ds->GetRasterBand(1);
    tif_band->SetNoDataValue(-32767.0f);
    auto discard = tif_band->RasterIO(GF_Write, 0, 0, rasterinfo.Width, rasterinfo.Height, (float*)&floats[0], rasterinfo.Width, rasterinfo.Height, GDT_Float32, 0, 0);

    GDALClose(tif_ds);

    return true;
}

RasterInfo RasterInfoFromTileInfo(const TileInfo& tileinfo)
{
    auto result = RasterInfo();
    std::tie(result.North, result.South, result.East, result.West) = NSEWBoundsForTileInfo(tileinfo);
    result.OriginX = result.West;
    result.OriginY = result.North;
    result.Width = TileDimensionForLod(tileinfo.lod);
    result.Height = result.Width;
    result.PixelSizeX = (result.East - result.West) / result.Width;
    result.PixelSizeY = (result.South - result.North) / result.Height;
    return result;
}

std::vector<unsigned char> FlippedVertically(const std::vector<unsigned char>& bytes, size_t width, size_t height, size_t depth)
{
    auto result = std::vector<unsigned char>(bytes.size());
    size_t row_size = width * depth;
    for(size_t y = 0; y < height; ++y)
    {
        size_t source_begin = (height - y - 1) * row_size;
        size_t source_end = source_begin + row_size;
        size_t result_begin = y * row_size;
        std::copy(bytes.begin() + source_begin, bytes.begin() + source_end, result.begin() + result_begin);
    }
    return result;
}

std::vector<float> FlippedVertically(const std::vector<float>& floats, size_t width, size_t height, size_t depth)
{
    auto result = std::vector<float>(floats.size());
    size_t row_size = width * depth;
    for(size_t y = 0; y < height; ++y)
    {
        size_t source_begin = (height - y - 1) * row_size;
        size_t source_end = source_begin + row_size;
        size_t result_begin = y * row_size;
        std::copy(floats.begin() + source_begin, floats.begin() + source_end, result.begin() + result_begin);
    }
    return result;
}

bool BuildImageryTileFromSampler(const std::string& cdb, GDALRasterSampler& sampler, const TileInfo& tileinfo)
{
    auto jp2_filepath = FilePathForTileInfo(tileinfo);
    auto jp2_filename = FileNameForTileInfo(tileinfo);
    auto outfilename = cdb + "/Tiles/" + jp2_filepath + "/" + jp2_filename + ".jp2";
    auto dim = TileDimensionForLod(tileinfo.lod);
    auto bytes = std::vector<unsigned char>();
    if (std::filesystem::exists(outfilename))
    {
        bytes = BytesFromJP2(outfilename);
        if(bytes.size() == dim * dim * 3)
            bytes = FlippedVertically(bytes, dim, dim, 3);
    }

    if(bytes.empty())
    {
        auto dimension = TileDimensionForLod(tileinfo.lod);
        bytes.resize(dimension * dimension * 3);
    }

    BuildImageryTileBytesFromSampler(sampler, tileinfo, bytes);    
    bytes = FlippedVertically(bytes, dim, dim, 3);
    auto info = RasterInfoFromTileInfo(tileinfo);
    ccl::makeDirectory(ccl::FileInfo(outfilename).getDirName());
    std::remove(outfilename.c_str());
    return WriteBytesToJP2(outfilename, info, bytes);
}

bool BuildElevationTileFromSampler(const std::string& cdb, GDALRasterSampler& sampler, const TileInfo& tileinfo)
{
    auto tif_filepath = FilePathForTileInfo(tileinfo);
    auto tif_filename = FileNameForTileInfo(tileinfo);
    auto outfilename = cdb + "/Tiles/" + tif_filepath + "/" + tif_filename + ".tif";
    auto dim = TileDimensionForLod(tileinfo.lod);
    auto floats = std::vector<float>();
    if(std::filesystem::exists(outfilename))
    {
        floats = FloatsFromTIF(outfilename);
        if(floats.size() == dim * dim)
            floats = FlippedVertically(floats, dim, dim, 1);
    }
    if(floats.empty())
    {
        floats.resize(dim * dim);
        std::fill(floats.begin(), floats.end(), -32767.0f);
    }

    std::fill(floats.begin(), floats.end(), -1.0f);

    BuildElevationTileFloatsFromSampler(sampler, tileinfo, floats);
    floats = FlippedVertically(floats, dim, dim, 1);
    auto info = RasterInfoFromTileInfo(tileinfo);
    ccl::makeDirectory(ccl::FileInfo(outfilename).getDirName());
    std::remove(outfilename.c_str());
    //WriteFloatsToText(outfilename + ".txt", info, floats);
    return WriteFloatsToTIF(outfilename, info, floats);
}

bool BuildElevationTileFromSampler2(const std::string& cdb, elev::Elevation_DSM& sampler, const TileInfo& tileinfo)
{
    auto tif_filepath = FilePathForTileInfo(tileinfo);
    auto tif_filename = FileNameForTileInfo(tileinfo);
    auto outfilename = cdb + "/Tiles/" + tif_filepath + "/" + tif_filename + ".tif";

    auto floats = std::vector<float>();
    if(std::filesystem::exists(outfilename))
        floats = FloatsFromTIF(outfilename);
    if(floats.empty())
    {
        auto dimension = TileDimensionForLod(tileinfo.lod);
        floats.resize(dimension * dimension);
        std::fill(floats.begin(), floats.end(), -32767.0f);
    }

    BuildElevationTileFloatsFromSampler2(sampler, tileinfo, floats);
    auto dim = TileDimensionForLod(tileinfo.lod);
    floats = FlippedVertically(floats, dim, dim, 1);
    auto info = RasterInfoFromTileInfo(tileinfo);
    ccl::makeDirectory(ccl::FileInfo(outfilename).getDirName());
    std::remove(outfilename.c_str());
    //WriteFloatsToText(outfilename + ".txt", info, floats);
    return WriteFloatsToTIF(outfilename, info, floats);
}




namespace
{
    ccl::ObjLog log;
    int CPL_STDCALL GDALProgressObserver(CPL_UNUSED double dfComplete, CPL_UNUSED const char *pszMessage, void * /* pProgressArg */)
    {
        log << (dfComplete * 100.0f) << "% complete..." << log.endl;
        return TRUE;
    }
}

bool BuildOverviews(const std::string& cdb, const std::string& component)
{
    CPLSetConfigOption("LODMIN", "-10");
    //CPLSetConfigOption("LODMAX", argv[3]);
    //const char *gdalErrMsg = CPLGetLastErrorMsg();
    auto open = "CDB:" + cdb + ":" + component;
    auto dataset = (GDALDataset *)GDALOpen(open.c_str(), GA_Update);
    if (dataset == NULL)
        return false;
    if (dataset->BuildOverviews("average", 0, NULL, 0, NULL, GDALProgressObserver, NULL) != CE_None)
        return false;
    GDALClose(dataset);
    return true;
}

bool BuildImageryOverviews(const std::string& cdb)
{
    return BuildOverviews(cdb, "Imagery_Yearly");
}

bool BuildElevationOverviews(const std::string& cdb)
{
    return BuildOverviews(cdb, "Elevation_PrimaryTerrainElevation");
}

bool IsCDB(const std::string& cdb)
{
    return ccl::fileExists(cdb + "/Metadata/Version.xml");
}

bool MakeCDB(const std::string& cdb)
{
    if(IsCDB(cdb))
        return false;
    if(!ccl::makeDirectory(cdb + "/Metadata"))
        return false;
    auto filename = cdb + "/Metadata/Version.xml";
    std::ofstream outfile(filename.c_str());
    if(!outfile.good())
        return false;
    outfile << "<?xml version = \"1.0\"?>\n";
    outfile << "<Version xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">\n";
    outfile << "<PreviousIncrementalRootDirectory name = \"\" />\n";
    outfile << "<Comment>Created by mesh2cdb</Comment>\n";
    outfile << "</Version>\n";
    outfile.close();
    return true;
}

std::vector<std::pair<std::string, std::string>> GeocellsForCdb(const std::string& cdb)
{
    auto result = std::vector<std::pair<std::string, std::string>>();
    auto tiles_path = cdb + "/Tiles";
    if (!ccl::directoryExists(tiles_path))
        return result;
    for (const auto& lat_entry : ccl::FileInfo::getSubDirectories(tiles_path))
    {
        for (const auto& lon_entry : ccl::FileInfo::getSubDirectories(lat_entry))
        {
            auto lat_fn = ccl::FileInfo(lat_entry).getBaseName();
            auto lon_fn = ccl::FileInfo(lon_entry).getBaseName();
            result.emplace_back(std::make_pair(lat_fn, lon_fn));
        }
    }
    return result;
}

int MaxLodForDatasetPath(const std::string& path)
{
    int result = INT_MIN;
    for (const auto& lod_entry : ccl::FileInfo::getSubDirectories(path))
    {
        auto lod_base = ccl::FileInfo(lod_entry).getBaseName();
        if(lod_base == "LC")
        {
            result = std::max<int>(result, 0);
            continue;
        }
        if(lod_base[0] != 'L')
            continue;
        auto lodstr = lod_base.substr(1);
        int lod = std::stoi(lodstr);
        if(lod > 23)
            continue;
        result = std::max<int>(result, lod);
    }
    return result;
}

std::tuple<double, double, double, double> NSEWBoundsForCDB(const std::string& cdb)
{
    double north = -DBL_MAX;
    double south = DBL_MAX;
    double east = -DBL_MAX;
    double west = DBL_MAX;
    auto geocells = GeocellsForCdb(cdb);
    for(auto geocell : geocells)
    {
        int lat = std::stoi(geocell.first.substr(1));
        if(geocell.first[0] == 'S')
            lat *= -1;
        int lon = std::stoi(geocell.second.substr(1));
        if(geocell.second[0] == 'W')
            lon *= -1;
        auto tile_width = get_tile_width(double(lat));
        double n = lat + 1;
        double e = lon + tile_width;
        north = std::max<double>(north, n);
        south = std::min<double>(south, lat);
        east = std::max<double>(east, e);
        west = std::min<double>(west, lon);
    }
    return std::make_tuple(north, south, east, west);
}

std::string PreviousIncrementalRootDirectory(const std::string& cdb)
{
    auto version_xml = cdb + "/Metadata/Version.xml";
    if(!std::filesystem::exists(version_xml))
        return "";
    auto ifs = std::ifstream(version_xml);
    auto xml = std::string(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
    auto pos = xml.find("<PreviousIncrementalRootDirectory");
    if(pos == std::string::npos)
        return "";
    pos = xml.find("name=", pos);
    if(pos == std::string::npos)
        return "";
    pos = xml.find('"', pos);
    if(pos == std::string::npos)
        return "";
    auto endpos = xml.find('"', pos + 1);
    if(endpos == std::string::npos)
        return "";
    auto pird_name = xml.substr(pos + 1, endpos - pos - 1);
    if(pird_name.empty())
        return "";
    return cdb + "/" + pird_name;
}

std::vector<std::string> VersionChainForCDB(const std::string& cdb)
{
    auto result = std::vector<std::string>();
    auto entry = cdb;
    while(!entry.empty())
    {
        result.push_back(entry);
        entry = PreviousIncrementalRootDirectory(entry);
        if(!std::filesystem::exists(entry))
            entry.clear();
    }
    return result;
}

std::vector<std::pair<std::string, Tile>> CoverageTilesForTiles(const std::string& cdb, const std::vector<Tile>& source_tiles)
{
    auto cdblist = VersionChainForCDB(cdb);
    auto result = std::vector<std::pair<std::string, Tile>>();
    auto tiles = source_tiles;
    while(!tiles.empty())
    {
        auto parent_tiles = std::vector<Tile>();
        for(auto tile : tiles)
        {
            auto tile_info = TileInfoForTile(tile);
            auto tile_filepath = FilePathForTileInfo(tile_info);
            auto tile_filename = FileNameForTileInfo(tile_info);
            bool found = false;
            for(auto local_cdb : cdblist)
            {
                auto filename = local_cdb + "/Tiles/" + tile_filepath + "/" + tile_filename;
                if(tile_info.dataset == 1)
                    filename += ".tif";
                if(tile_info.dataset == 4)
                    filename += ".jp2";
                if(std::filesystem::exists(filename))
                {
                    result.emplace_back(local_cdb, tile);
                    found = true;
                    break;
                }
            }
            if(found)
                continue;
            if(tile_info.lod - 1 < -10)
                continue;
            auto tile_bounds = NSEWBoundsForTileInfo(tile_info);
            auto parent_coords = CoordinatesRange(std::get<3>(tile_bounds), std::get<2>(tile_bounds), std::get<1>(tile_bounds), std::get<0>(tile_bounds));
            auto parent_tiles_add = generate_tiles(parent_coords, Dataset((uint16_t)tile_info.dataset), tile_info.lod - 1);
            for(auto parent_tile : parent_tiles_add)
            {
                if(std::find(parent_tiles.begin(), parent_tiles.end(), parent_tile) == parent_tiles.end())
                    parent_tiles.push_back(parent_tile);
            }
        }
        tiles = parent_tiles;
    }
    return result;
}

std::vector<std::pair<std::string, TileInfo>> CoverageTileInfosForTileInfo(const std::string& cdb, const TileInfo& source_tileinfo)
{
    auto cdblist = VersionChainForCDB(cdb);
    auto result = std::vector<std::pair<std::string, TileInfo>>();
    auto tileinfos = std::vector<TileInfo>();
    tileinfos.push_back(source_tileinfo);
    while(!tileinfos.empty())
    {
        auto parent_tileinfos = std::vector<TileInfo>();
        for(auto tileinfo : tileinfos)
        {
            auto tile_filepath = FilePathForTileInfo(tileinfo);
            auto tile_filename = FileNameForTileInfo(tileinfo);
            bool found = false;
            for(auto local_cdb : cdblist)
            {
                auto filename = local_cdb + "/Tiles/" + tile_filepath + "/" + tile_filename;
                if(tileinfo.dataset == 1)
                    filename += ".tif";
                if(tileinfo.dataset == 4)
                    filename += ".jp2";
                if(std::filesystem::exists(filename))
                {
                    result.emplace_back(local_cdb, tileinfo);
                    found = true;
                    break;
                }
            }
            if(found)
                continue;
            if(tileinfo.lod - 1 < -10)
                continue;
            auto parent_ti = ParentTileInfo(tileinfo);
            if(std::find(parent_tileinfos.begin(), parent_tileinfos.end(), parent_ti) == parent_tileinfos.end())
                parent_tileinfos.push_back(parent_ti);
        }
        tileinfos = parent_tileinfos;
    }
    return result;
}

bool InjectFeatures(const std::string& cdb, int dataset, int cs1, int cs2, int lod, const std::string& filename, const std::string& models_path, const std::string& textures_path)
{
    if (!IsCDB(cdb))
        MakeCDB(cdb);
    ccl::ObjLog log;
    double north = -DBL_MAX;
    double south = DBL_MAX;
    double east = -DBL_MAX;
    double west = DBL_MAX;
    auto features = std::vector<sfa::Feature*>();
    {
        // TODO: coordinate transforms
        auto file = ogr::File();
        if(!file.open(filename))
            return false;
        auto layers = file.getLayers();
        for(auto layer : layers)
        {
            while(auto feature = layer->getNextFeature())
                features.push_back(feature);
            double left, bottom, right, top;
            layer->getExtent(left, bottom, right, top, false);
            north = std::max<double>(north, top);
            south = std::min<double>(south, bottom);
            east = std::max<double>(east, right);
            west = std::min<double>(west, left);
        }
        file.close();
    }
    auto coords = CoordinatesRange(west, east, south, north);
    auto tiles = generate_tiles(coords, Dataset((uint16_t)dataset), lod);
    log << "INJECT " << filename << " ((" << west << ", " << south << ") (" << east << ", " << north << ")) : " << tiles.size() << " tiles" << log.endl;
    for(auto tile : tiles)
    {
        auto tile_info = TileInfoForTile(tile);
        tile_info.selector1 = cs1;
        tile_info.selector2 = cs2;
        double tile_north, tile_south, tile_east, tile_west;
        std::tie(tile_north, tile_south, tile_east, tile_west) = NSEWBoundsForTileInfo(tile_info);
        auto tile_features = std::vector<sfa::Feature*>();
        for(auto feature : features)
        {
            if(!feature->geometry)
                continue;
            auto envelope = std::make_unique<sfa::LineString>(dynamic_cast<sfa::LineString*>(feature->geometry->getEnvelope()));
            auto point_min = envelope->getPointN(0);
            auto point_max = envelope->getPointN(1);
            if(point_min->Y() > tile_north)
                continue;
            if(point_min->X() > tile_east)
                continue;
            if(point_max->Y() < tile_south)
                continue;
            if(point_max->X() < tile_west)
                continue;
            tile_features.push_back(feature);
        }
        if(tile_features.empty())
            continue;
        if(!models_path.empty())
            InjectGTModels(cdb, tile_features, models_path, textures_path);
        auto tile_filepath = FilePathForTileInfo(tile_info);
        auto tile_filename = FileNameForTileInfo(tile_info);
        auto tile_fn = cdb + "/Tiles/" + tile_filepath + "/" + tile_filename + ".shp";
        log << "    " << tile_filename << ": " << tile_features.size() << " features" << log.endl;
        ccl::makeDirectory(cdb + "/Tiles/" + tile_filepath);
        auto file = ogr::File();
        if(!file.open(tile_fn, true))
        {
            if(!file.create(tile_fn))
                return false;
            // TODO: create layer?
        }
        for(auto feature : tile_features)
        {
            // TODO: attribute handling

            auto new_features = FeaturesForTileCroppedFeature(tile_info, *feature);
            for(auto new_feature : new_features)
            {
                delete file.addFeature(new_feature);
                delete new_feature;
            }
        }
        file.close();
    }
    for(auto feature : features)
        delete feature;
    return true;
}

bool InjectFeatures(const std::string& cdb, int dataset, int cs1, int cs2, int lod, const std::vector<std::string>& filenames, const std::string& models_path, const std::string& textures_path)
{
    bool result = true;
    for(auto fn : filenames)
    {
        if(!InjectFeatures(cdb, dataset, cs1, cs2, lod, fn, models_path, textures_path))
            result = false;
    }
    return result;
}

std::vector<sfa::Feature*> FeaturesForTileCroppedFeature(const TileInfo& tile_info, const sfa::Feature& feature)
{
    if(!feature.geometry)
        return std::vector<sfa::Feature*>();
    double tile_north, tile_south, tile_east, tile_west;
    std::tie(tile_north, tile_south, tile_east, tile_west) = NSEWBoundsForTileInfo(tile_info);
    auto tile_ring = new sfa::LineString;
    tile_ring->addPoint(new sfa::Point(tile_west, tile_south));
    tile_ring->addPoint(new sfa::Point(tile_east, tile_south));
    tile_ring->addPoint(new sfa::Point(tile_east, tile_north));
    tile_ring->addPoint(new sfa::Point(tile_west, tile_north));
    tile_ring->addPoint(new sfa::Point(tile_west, tile_south));
    sfa::Polygon tile_aabb;
    tile_aabb.addRing(tile_ring);
    auto isect_geometry = tile_aabb.intersection(feature.geometry);
    if(!isect_geometry)
        return std::vector<sfa::Feature*>();
    sfa::GeometryCollection* isect_collection = dynamic_cast<sfa::GeometryCollection*>(isect_geometry);
    if(!isect_collection)
    {
        isect_collection = new sfa::GeometryCollection;
        isect_collection->addGeometry(isect_geometry);
    }
    auto result = std::vector<sfa::Feature*>();
    for(int i = 0, c = isect_collection->getNumGeometries(); i < c; ++i)
    {
        auto geometry = isect_collection->getGeometryN(i + 1);
        if(geometry->getWKBGeometryType() != feature.geometry->getWKBGeometryType())
            continue;
        auto rfeature = new sfa::Feature;
        rfeature->attributes = feature.attributes;
        rfeature->geometry = geometry->copy();
        result.push_back(rfeature);
    }
    delete isect_collection;
    return result;
}

void ReportMissingGSFeatureData(const std::string& cdb, std::tuple<double, double, double, double> nsew)
{
    ccl::ObjLog log;
    auto tiles = FeatureTileInfoForTiledDataset(cdb, 100, nsew);
    auto model_filenames = std::vector<std::string>();
    for(auto tile : tiles)
    {
        auto tile_models = GSModelReferencesForTile(cdb, tile, nsew);
        if(tile_models.empty())
            continue;
        log << "GS TILE: " << FileNameForTileInfo(tile) << " (" << tile_models.size() << " model references)" << log.endl;
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
        auto filenames = TextureFileNamesForModel(model_filename);
        if(!filenames.first)
        {
            log << "MODEL MISSING: " << model_filename << log.endl;
            continue;
        }
        for (auto filename : filenames.second)
        {
            auto fn = ccl::FileInfo(ccl::FileInfo(model_filename).getDirName()).getDirName() + "/" + filename;
            if(ccl::fileExists(fn))
            {
                texture_filenames.push_back(fn);
                continue;
            }
            auto base = ccl::FileInfo(fn).getBaseName();
            auto tileinfo = TileInfoForFileName(base);
            auto zipfn = ccl::FileInfo(fn).getDirName() + "/" + FileNameForTileInfo(tileinfo) + ".zip";
            texture_filenames.push_back(zipfn + "/" + base);
        }
    }

    std::sort(texture_filenames.begin(), texture_filenames.end());
    texture_filenames.erase(std::unique(texture_filenames.begin(), texture_filenames.end()), texture_filenames.end());
    log << texture_filenames.size() << " textures" << log.endl;

    for(auto texture_filename : texture_filenames)
    {
        if(!TextureExists(texture_filename))
            log << "TEXTURE MISSING: " << texture_filename << log.endl;
    }
}

void ReportMissingGTFeatureData(const std::string& cdb, std::tuple<double, double, double, double> nsew)
{
    ccl::ObjLog log;
    auto tiles = FeatureTileInfoForTiledDataset(cdb, 101, nsew);
    auto model_filenames = std::vector<std::string>();
    for(auto tile : tiles)
    {
        auto tile_models = GTModelReferencesForTile(cdb, tile, nsew);
        if(tile_models.empty())
            continue;
        //log << "GT TILE: " << FileNameForTileInfo(tile) << " (" << tile_models.size() << " model references)" << log.endl;
        model_filenames.insert(model_filenames.end(), tile_models.begin(), tile_models.end());
    }

    std::sort(model_filenames.begin(), model_filenames.end());
    model_filenames.erase(std::unique(model_filenames.begin(), model_filenames.end()), model_filenames.end());
    log << model_filenames.size() << " models" << log.endl;

    auto texture_filenames = std::vector<std::string>();
    for(auto model_filename : model_filenames)
    {
        auto filenames = TextureFileNamesForModel(model_filename);
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

std::map<std::string, std::pair<std::string, std::string>> FACCandFSCbyMODLforFeatures(const std::vector<sfa::Feature*>& features)
{
    auto result = std::map<std::string, std::pair<std::string, std::string>>();
    for(auto feature : features)
    {
        if(!feature->attributes.hasAttribute("FACC"))
            continue;
        if(!feature->attributes.hasAttribute("FSC"))
            continue;
        if(!feature->attributes.hasAttribute("MODL"))
            continue;
        auto facc = feature->attributes.getAttributeAsString("FACC");
        auto fsc = feature->attributes.getAttributeAsString("FSC");
        auto modl = feature->attributes.getAttributeAsString("MODL");
        result[modl] = std::make_pair(facc, fsc);
    }
    return result;
}

std::string BytesFromFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);
    file.unsetf(std::ios::skipws);
    file.seekg(0, std::ios::end);
    auto len = file.tellg();
    file.seekg(0, std::ios::beg);
    std::string bytes;
    bytes.reserve(len);
    bytes.insert(bytes.begin(), std::istream_iterator<char>(file), std::istream_iterator<char>());
    return bytes;
}

void FileFromBytes(const std::string& filename, const std::string& bytes)
{
    std::ofstream file(filename, std::ios::out | std::ios::binary);
    file.write(&bytes[0], bytes.size());
    file.close();
}


void InjectGTModels(const std::string& cdb, const std::vector<sfa::Feature*>& features, const std::string& source_model_path, const std::string& source_texture_path)
{
    ccl::ObjLog log;
    auto fdd = FeatureDataDictionary();
    auto source_by_target = std::map<std::string, std::string>();
    for(auto feature : features)
    {
        if(!feature->attributes.hasAttribute("FACC"))
            continue;
        if(!feature->attributes.hasAttribute("FSC"))
            continue;
        if(!feature->attributes.hasAttribute("MODL"))
            continue;
        auto facc = feature->attributes.getAttributeAsString("FACC");
        auto fsc = feature->attributes.getAttributeAsString("FSC");
        while (fsc.size() < 3)
            fsc = "0" + fsc;
        auto modl = feature->attributes.getAttributeAsString("MODL");
        auto modl_base = ccl::FileInfo(modl).getBaseName(true);
        auto infile = source_model_path + "/" + modl;
        auto relpath = "/GTModel/500_GTModelGeometry/" + fdd.Subdirectory(facc) + "/D500_S001_T001_" + facc + "_" + fsc + "_" + modl;
        auto outfile = cdb + relpath;
        source_by_target[outfile] = infile;
        feature->attributes.setAttribute("MODL", modl_base);
    }
    for(auto entry : source_by_target)
    {
        auto outfile = entry.first;
        if(ccl::fileExists(outfile))
            continue;
        auto infile = entry.second;
        if(!ccl::fileExists(infile))
        {
            log << "Missing source model: " << infile << log.endl;
            continue;
        }
        auto outpath = ccl::FileInfo(outfile).getDirName();
        ccl::makeDirectory(outpath);

        auto bytes = BytesFromFile(infile);
        auto is = std::istringstream(bytes);
        ccl::BindStream bs(is);
        while(bs.pos() < bytes.size())
        {
            ccl::BigEndian<ccl::uint16_t> opcode;
            ccl::BigEndian<ccl::uint16_t> length;
            bs.bind(opcode);
            bs.bind(length);
            if(opcode == flt::Record::FLT_TEXTUREPALETTE)
            {
                std::string texture_filename;
                auto pos = bs.pos();
                bs.bind(texture_filename, 200);
                bs.seek(pos);
                texture_filename = texture_filename.c_str();
                auto last = texture_filename.find_last_not_of(" \r\n");
                if(last != std::string::npos)
                    texture_filename = texture_filename.substr(0, last + 1);
                auto texture_basename = ccl::FileInfo(texture_filename).getBaseName();

                std::string wslod = "00";
                if(ccl::FileInfo(texture_basename).getSuffix() == "rgb")
                {
                    auto wh = WidthHeightFromRGB(source_texture_path + "/" + texture_basename);
                    int dim = std::max<int>(wh.first, wh.second);
                    int exp = 1;
                    while(std::pow(2, exp) < dim)
                        ++exp;
                    wslod = std::to_string(exp);
                    if(wslod.size() < 2)
                        wslod = "0" + wslod;
                }
                auto char1 = texture_basename.substr(0, 1);
                auto char2 = texture_basename.substr(1, 1);
                auto target_texture_path = "/501_GTModelTexture/" + char1 + "/" + char2 + "/" + ccl::FileInfo(texture_basename).getBaseName(true);

                auto out_texture_basename = "/D501_S001_D001_W" + wslod + "_" + texture_basename;
                texture_filename = "../../../.." + target_texture_path + "/" + out_texture_basename;
                memset(&bytes[pos], 0, 200);
                strncpy(&bytes[pos], texture_filename.c_str(), 200);

                texture_filename = cdb + "/GTModel" + target_texture_path + "/" + out_texture_basename;
                if(!ccl::fileExists(texture_filename))
                {
                    if(ccl::fileExists(source_texture_path + "/" + texture_basename))
                    {
                        auto outpath = ccl::FileInfo(texture_filename).getDirName();
                        ccl::makeDirectory(outpath);
                        log << "  " << source_texture_path << "/" << texture_basename << " -> " << texture_filename << log.endl;
                        ccl::copyFile(source_texture_path + "/" + texture_basename, texture_filename);
                    }
                    else
                    {
                        log << "Missing source texture: " << texture_basename << log.endl;
                    }
                }
            }
            bs.seek(bs.pos() + length - 4);
        }
        FileFromBytes(outfile, bytes);
    }
}

bool WriteFeaturesToOGRFile(const std::string& filename, const std::vector<sfa::Feature*> features)
{
    auto file = ogr::File();
    if (!file.open(filename, true))
    {
        if (!file.create(filename))
            return false;
    }
    for (auto feature : features)
        delete file.addFeature(feature);
    file.close();
    return true;
}

std::vector<sfa::Feature*> FeaturesForTileInfo(const std::string& cdb, const TileInfo& tile_info)
{
    auto tile_filepath = FilePathForTileInfo(tile_info);
    auto tile_filename = FileNameForTileInfo(tile_info);
    auto tile_fn = cdb + "/Tiles/" + tile_filepath + "/" + tile_filename + ".shp";
    auto class_tile = tile_info;
    class_tile.selector2 = tile_info.selector2 + 1;
    auto class_tile_filepath = FilePathForTileInfo(class_tile);
    auto class_tile_filename = FileNameForTileInfo(class_tile);
    auto class_tile_fn = cdb + "/Tiles/" + class_tile_filepath + "/" + class_tile_filename + ".dbf";
    auto ext_tile = tile_info;
    ext_tile.selector2 = 16;
    if(tile_info.selector2 == 3)
        ext_tile.selector2 = 17;
    if(tile_info.selector2 == 5)
        ext_tile.selector2 = 18;
    auto ext_tile_filepath = FilePathForTileInfo(ext_tile);
    auto ext_tile_filename = FileNameForTileInfo(ext_tile);
    auto ext_tile_fn = cdb + "/Tiles/" + ext_tile_filepath + "/" + ext_tile_filename + ".dbf";
    auto features = cognitics::cdb::FeaturesForOGRFile(tile_fn);
    auto class_attr = cognitics::cdb::AttributesForDBF(class_tile_fn);
    auto class_map = cognitics::cdb::AttributesByCNAM(class_attr);
    auto ext_attr = cognitics::cdb::AttributesForDBF(ext_tile_fn);
    auto ext_map = cognitics::cdb::AttributesByCNAM(ext_attr);
    for (auto feature : features)
    {
        auto cnam = feature->attributes.getAttributeAsString("CNAM");
        if (class_map.find(cnam) != class_map.end())
        {
            auto attributes = class_map[cnam].getVariantMap();
            for (auto attr : *attributes)
                feature->attributes.setAttribute(attr.first, attr.second);
        }
        if (ext_map.find(cnam) != ext_map.end())
        {
            auto attributes = ext_map[cnam].getVariantMap();
            for (auto attr : *attributes)
                feature->attributes.setAttribute(attr.first, attr.second);
        }
    }
    return features;
}

std::vector<sfa::Feature*> Features(const std::string& cdb, int dataset, int cs1, int cs2, int lod, std::tuple<double, double, double, double> nsew)
{
    auto result = std::vector<sfa::Feature*>();
    auto coords = CoordinatesRange(std::get<3>(nsew), std::get<2>(nsew), std::get<1>(nsew), std::get<0>(nsew));
    auto tiles = generate_tiles(coords, Dataset((uint16_t)dataset), lod);
    for(auto tile : tiles)
    {
        auto tile_info = TileInfoForTile(tile);
        auto features = FeaturesForTileInfo(cdb, tile_info);
        for(auto feature : features)
            result.push_back(feature);
    }
    return result;
}

std::pair<int, int> WidthHeightFromRGB(const std::string& filename)
{
    int width = 1;
    int height = 1;
    std::ifstream file(filename, std::ios::binary);
    ccl::BindStream bs(file);
    ccl::BigEndian<ccl::int16_t> magic;
    bs.bind(magic);
    if(magic == 474)
    {
        bs.seek(bs.pos() + 4);
        ccl::BigEndian<ccl::uint16_t> w;
        ccl::BigEndian<ccl::uint16_t> h;
        bs.bind(w);
        bs.bind(h);
        width = w;
        height = h;
    }
    return std::pair<int, int>(width, height);
}

namespace
{
    void CollectFilesInTiledDataset(std::vector<std::string>& result, const std::filesystem::path& path, const NSEW& nsew = { DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX })
    {
        for(const auto& entry : std::filesystem::directory_iterator(path))
        {
            if(std::filesystem::is_directory(entry))
            {
                CollectFilesInTiledDataset(result, entry, nsew);
                continue;
            }
            if(!std::filesystem::is_regular_file(entry))
                continue;
            // TODO: filter nsew
            result.push_back(entry.path().string());
        }
    }
}

std::string SubdirectoryForLOD(int lod)
{
    if(lod >= 10)
        return "L" + std::to_string(lod);
    if(lod >= 0)
        return "L0" + std::to_string(lod);
    return "LC";
}

int LatitudeFromSubdirectory(const std::string& subdir)
{
    int ilatitude = std::stoi(subdir.substr(1));
    if(subdir[0] == 'N')
        return ilatitude;
    if(subdir[0] == 'S')
        return -ilatitude;
    return INT_MAX;
}

int LongitudeFromSubdirectory(const std::string& subdir)
{
    int ilongitude = std::stoi(subdir.substr(1));
    if(subdir[0] == 'E')
        return ilongitude;
    if(subdir[0] == 'W')
        return -ilongitude;
    return INT_MAX;
}

std::vector<std::string> FilesInTiledDataset(const std::string& cdb, int dataset)//, const NSEW& nsew)
{
    std::string dataset_subdirectory = DatasetSubdirectory(dataset);
    auto result = std::vector<std::string>();
    for(const auto& latitude_entry : std::filesystem::directory_iterator(cdb + "/Tiles"))
    {
        auto latitude_path = latitude_entry.path();
        if(!std::filesystem::is_directory(latitude_path))
            continue;
        double latitude = LatitudeFromSubdirectory(latitude_path.filename().string());
        if(latitude == INT_MAX)
            continue;
        // TODO: filter nsew
        for(const auto& longitude_entry : std::filesystem::directory_iterator(latitude_path))
        {
            auto longitude_path = longitude_entry.path();
            double longitude = LongitudeFromSubdirectory(longitude_path.filename().string());
            if(longitude == INT_MAX)
                continue;
            // TODO: filter nsew
            auto dataset_path = longitude_path / dataset_subdirectory;
            CollectFilesInTiledDataset(result, dataset_path);//, nsew);
        }
    }
    return result;
}

int DimensionsForLOD(int lod)
{
    return (int)pow(2, std::min<double>(lod + 10, 10));
}

int RowsForLOD(int lod)
{
    return std::max<int>(int(std::pow(2, lod)), 1);
}

int ColumnsForLOD(int lod)
{
    return std::max<int>(int(std::pow(2, lod)), 1);
}

int TileWidthAtLatitude(double latitude)
{
    int lat = (int)std::floor(latitude);
    if (lat >= 90.0f)
        return 0;
    if (lat >= 89.0f)
        return 12;
    if (lat >= 80.0f)
        return 6;
    if (lat >= 75.0f)
        return 4;
    if (lat >= 70.0f)
        return 3;
    if (lat >= 50.0f)
        return 2;
    if (lat >= -50.0f)
        return 1;
    if (lat >= -70.0f)
        return 2;
    if (lat >= -75.0f)
        return 3;
    if (lat >= -80.0f)
        return 4;
    if (lat >= -89.0f)
        return 6;
    if (lat >= -90.0f)
        return 12;
    return 0;
}

std::vector<TileInfo> GenerateTileInfos(int lod, const NSEW& nsew)
{
    auto result = std::vector<TileInfo>();

    int inorth = (int)std::ceil(nsew.north);
    int isouth = (int)std::floor(nsew.south);
    int ieast = (int)std::ceil(nsew.east);
    int iwest = (int)std::floor(nsew.west);

    int lod_rows = RowsForLOD(lod);
    int lod_cols = ColumnsForLOD(lod);
    double row_height = 1.0 / lod_rows;
    for(int ilat = isouth; ilat <= inorth; ++ilat)
    {
        int ilon_step = TileWidthAtLatitude(ilat);
        double col_width = (double)ilon_step / lod_cols;
        for (int ilon = iwest; ilon <= ieast; ilon += ilon_step)
        {
            int usouth = (int)std::floor((nsew.south - ilat) / row_height);
            usouth = std::max<int>(usouth, 0);
            int unorth = (int)std::ceil((nsew.north - ilat) / row_height);
            unorth = std::min<int>(unorth, lod_rows);
            for (int uref = usouth; uref < unorth; ++uref)
            {
                int uwest = (int)std::floor((nsew.west - ilon) / col_width);
                uwest = std::max<int>(uwest, 0);
                int ueast = (int)std::ceil((nsew.east - ilon) / col_width);
                ueast = std::min<int>(ueast, lod_cols);
                for (int rref = uwest; rref < ueast; ++rref)
                {
                    result.emplace_back(TileInfo { ilat, ilon, 0, 0, 0, lod, uref, rref });
                }
            }
        }
    }
    return result;
}

void BuildMinMaxElevation(const std::string& cdb, int lod_offset)
{
    auto elevation_filenames = FileNamesForTiledDataset(cdb, 1);
    auto elevation_tileinfos = TileInfoForFileNames(elevation_filenames);
    auto minmax_tiles = std::vector<Tile>();
    for(auto elevation_tileinfo : elevation_tileinfos)
    {
        auto nsew = NSEWBoundsForTileInfo(elevation_tileinfo);
        auto coords = CoordinatesRange(std::get<3>(nsew), std::get<2>(nsew), std::get<1>(nsew), std::get<0>(nsew));
        auto tiles = generate_tiles(coords, Dataset((uint16_t)2), elevation_tileinfo.lod - lod_offset);
        minmax_tiles.insert(minmax_tiles.end(), tiles.begin(), tiles.end());
    }
    std::sort(minmax_tiles.begin(), minmax_tiles.end());
    minmax_tiles.erase(std::unique(minmax_tiles.begin(), minmax_tiles.end()), minmax_tiles.end());
    std::reverse(minmax_tiles.begin(), minmax_tiles.end());
    for(auto tile : minmax_tiles)
    {
        auto tile_info = TileInfoForTile(tile);
        int dim = TileDimensionForLod(tile_info.lod);
        auto min_floats = std::vector<float>(dim * dim);
        auto max_floats = std::vector<float>(dim * dim);
        {
            auto elevation_tile_info = tile_info;
            elevation_tile_info.dataset = 1;
            elevation_tile_info.selector1 = 1;
            elevation_tile_info.selector2 = 1;
            auto elevation_tif_filepath = FilePathForTileInfo(elevation_tile_info);
            auto elevation_tif_filename = FileNameForTileInfo(elevation_tile_info);
            auto elevation_tif = cdb + "/Tiles/" + elevation_tif_filepath + "/" + elevation_tif_filename + ".tif";
            auto elevation_floats = FloatsFromTIF(elevation_tif);
            for(int row = 0; row < dim; ++row)
            {
                for(int col = 0; col < dim; ++col)
                {
                    // note that this doesn't follow the spec exactly
                    // - we should reference the adjacent tiles to the north/east
                    // - we should upsample from lower LODs if available for the north/east
                    // - coarser LODs in the minmax dataset should be taken from the higher LOD minmax rather than the associated elevation layer
                    int index = (row * dim) + col;
                    float sw = elevation_floats[index + 0];
                    float se = (col + 1 < dim) ? elevation_floats[index + 1] : sw;
                    float nw = (row + 1 < dim) ? elevation_floats[index + dim] : sw;
                    float ne = sw;
                    if(row + 1 < dim)
                        ne = se;
                    if(col + 1 < dim)
                        ne = nw;
                    if((row + 1 < dim) && (col + 1 < dim))
                        elevation_floats[index + dim + 1];
                    float min_value = std::min<float>(sw, se);
                    min_value = std::min<float>(min_value, nw);
                    min_value = std::min<float>(min_value, ne);
                    min_floats[index] = min_value;
                    float max_value = std::max<float>(sw, se);
                    max_value = std::max<float>(max_value, nw);
                    max_value = std::max<float>(max_value, ne);
                    max_floats[index] = max_value;
                }
            }
        }
        tile_info.dataset = 2;
        tile_info.selector1 = 1;
        {
            tile_info.selector2 = 1;
            auto tif_filepath = FilePathForTileInfo(tile_info);
            auto tif_filename = FileNameForTileInfo(tile_info);
            auto tif = cdb + "/Tiles/" + tif_filepath + "/" + tif_filename + ".tif";
            auto raster_info = RasterInfoFromTileInfo(tile_info);
            WriteFloatsToTIF(tif, raster_info, min_floats, false);
        }
        {
            tile_info.selector2 = 2;
            auto tif_filepath = FilePathForTileInfo(tile_info);
            auto tif_filename = FileNameForTileInfo(tile_info);
            auto tif = cdb + "/Tiles/" + tif_filepath + "/" + tif_filename + ".tif";
            auto raster_info = RasterInfoFromTileInfo(tile_info);
            WriteFloatsToTIF(tif, raster_info, max_floats, false);
        }
    }
}


}
}
