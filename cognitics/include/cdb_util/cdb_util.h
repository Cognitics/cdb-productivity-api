
#pragma once

#include <cdb_tile/Tile.h>
#include <sfa/Feature.h>
#include <ip/GDALRasterSampler.h>
#include <elev/DataSourceManager.h>
#include <elev/Elevation_DSM.h>

#include <vector>

namespace cognitics {
namespace cdb {

template <int DS, int CS1, int CS2>
struct Component
{
    const int Dataset = DS;
    const int Selector1 = CS1;
    const int Selector2 = CS2;
};

struct TileInfo
{
    int latitude;
    int longitude;
    int dataset;
    int selector1;
    int selector2;
    int lod;
    int uref;
    int rref;
};

struct RasterInfo
{
    int Width { 0 };
    int Height { 0 };
    double OriginX { 0.0 };
    double OriginY { 0.0 };
    double PixelSizeX { 0.0 };
    double PixelSizeY { 0.0 };
    double South { 0 };
    double West { 0 };
    double North { 0 };
    double East { 0 };
};

TileInfo TileInfoForTile(const Tile& tile);
int LodForPixelSize(double pixel_size);
int TileDimensionForLod(int lod);
double PixelSizeForLod(int lod);
double MinimumPixelSizeForLod(int lod, double latitude);

std::vector<std::string> FileNamesForTiledDataset(const std::string& cdb, int dataset);

std::vector<TileInfo> FeatureTileInfoForTiledDataset(const std::string& cdb, int dataset, std::tuple<double, double, double, double> nsew = std::make_tuple(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX) );

TileInfo TileInfoForFileName(const std::string& filename);
std::string FilePathForTileInfo(const TileInfo& tileinfo);
std::string FileNameForTileInfo(const TileInfo& tileinfo);
std::tuple<double, double, double, double> NSEWBoundsForTileInfo(const TileInfo& tileinfo);
std::string BoundsStringForTileInfo(const TileInfo& tileinfo);

std::vector<TileInfo> TileInfoForFileNames(const std::vector<std::string>& filenames);

std::vector<sfa::Feature*> FeaturesForOGRFile(const std::string& filename, std::tuple<double, double, double, double> nsew = std::make_tuple(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX) );

std::vector<ccl::AttributeContainer> AttributesForDBF(const std::string& filename);
std::map<std::string, ccl::AttributeContainer> AttributesByCNAM(const std::vector<ccl::AttributeContainer>& attrvec);

std::pair<bool, std::vector<std::string>> TextureFileNamesForModel(const std::string& filename);

std::vector<std::string> GSModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, std::tuple<double, double, double, double> nsew = std::make_tuple(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX) );
std::vector<std::string> GTModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, std::tuple<double, double, double, double> nsew = std::make_tuple(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX) );

bool TextureExists(const std::string& filename);

RasterInfo ReadRasterInfo(const std::string& filename);
bool WriteBytesToJP2(const std::string& filename, const RasterInfo& rasterinfo, const std::vector<unsigned char>& bytes);
bool WriteFloatsToTIF(const std::string& filename, const RasterInfo& rasterinfo, const std::vector<float>& floats);
RasterInfo RasterInfoFromTileInfo(const cognitics::cdb::TileInfo& tileinfo);
std::vector<unsigned char> FlippedVertically(const std::vector<unsigned char>& bytes, size_t width, size_t height, size_t depth);
std::vector<float> FlippedVertically(const std::vector<float>& bytes, size_t width, size_t height, size_t depth);

bool BuildImageryTileBytesFromSampler(GDALRasterSampler& sampler, const TileInfo& tileinfo, std::vector<unsigned char>& bytes);
bool BuildImageryTileFromSampler(const std::string& cdb, GDALRasterSampler& sampler, const TileInfo& tileinfo);

bool BuildElevationTileFloatsFromSampler(GDALRasterSampler& sampler, const TileInfo& tileinfo, std::vector<float>& floats);
bool BuildElevationTileFromSampler(const std::string& cdb, GDALRasterSampler& sampler, const TileInfo& tileinfo);

bool BuildElevationTileFloatsFromSampler2(elev::Elevation_DSM& sampler, const TileInfo& tileinfo, std::vector<float>& floats);
bool BuildElevationTileFromSampler2(const std::string& cdb, elev::Elevation_DSM& sampler, const TileInfo& tileinfo);

bool BuildOverviews(const std::string& cdb, const std::string& component);
bool BuildImageryOverviews(const std::string& cdb);
bool BuildElevationOverviews(const std::string& cdb);

bool IsCDB(const std::string& cdb);
bool MakeCDB(const std::string& cdb);

std::vector<std::pair<std::string, std::string>> GeocellsForCdb(const std::string& cdb);
int MaxLodForDatasetPath(const std::string& path);





}
}
