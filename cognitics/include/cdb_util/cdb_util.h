
#pragma once

#include <cdb_tile/Tile.h>
#include <sfa/Feature.h>

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


std::vector<std::string> FileNamesForTiledDataset(const std::string& cdb, int dataset);

std::vector<TileInfo> FeatureTileInfoForTiledDataset(const std::string& cdb, int dataset, double north = DBL_MAX, double south = -DBL_MAX, double east = DBL_MAX, double west = -DBL_MAX);

TileInfo TileInfoForFileName(const std::string& filename);
std::string FilePathForTileInfo(const TileInfo& tileinfo);
std::string FileNameForTileInfo(const TileInfo& tileinfo);
std::tuple<double, double, double, double> NSEWBoundsForTileInfo(const TileInfo& tileinfo);

std::vector<TileInfo> TileInfoForFileNames(const std::vector<std::string>& filenames);

std::vector<sfa::Feature*> FeaturesForOGRFile(const std::string& filename, double north = -DBL_MAX, double south = DBL_MAX, double east = -DBL_MAX, double west = DBL_MAX);

std::vector<ccl::AttributeContainer> AttributesForDBF(const std::string& filename);
std::map<std::string, ccl::AttributeContainer> AttributesByCNAM(const std::vector<ccl::AttributeContainer>& attrvec);

std::pair<bool, std::vector<std::string>> TextureFileNamesForModel(const std::string& filename);

std::vector<std::string> GSModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, double north = -DBL_MAX, double south = DBL_MAX, double east = -DBL_MAX, double west = DBL_MAX);
std::vector<std::string> GTModelReferencesForTile(const std::string& cdb, const TileInfo& tileinfo, double north = -DBL_MAX, double south = DBL_MAX, double east = -DBL_MAX, double west = DBL_MAX);

bool TextureExists(const std::string& filename);


}
}
