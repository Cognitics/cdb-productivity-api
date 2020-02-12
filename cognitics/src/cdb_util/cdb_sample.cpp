
#include <cdb_util/cdb_sample.h>

#include <cdb_util/cdb_util.h>

#include <ccl/FileInfo.h>

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

namespace cognitics {
namespace cdb {

bool cdb_sample(cdb_sample_parameters& params)
{
    auto pixel_size_x = std::abs((params.east - params.west) / params.width);
    auto pixel_size_y = std::abs((params.north - params.south) / params.height);
    auto pixel_size = std::min<double>(pixel_size_x, pixel_size_y);
    auto target_lod = cognitics::cdb::LodForPixelSize(pixel_size);

    auto coords = cognitics::cdb::CoordinatesRange(params.west, params.east, params.south, params.north);
    auto tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)params.dataset), target_lod);

    GDALRasterSampler sampler;

    while(!tiles.empty())
    {
        auto parent_tiles = std::vector<cognitics::cdb::Tile>();
        for(auto tile : tiles)
        {
            auto tile_info = cognitics::cdb::TileInfoForTile(tile);
            auto tile_filepath = cognitics::cdb::FilePathForTileInfo(tile_info);
            auto tile_filename = cognitics::cdb::FileNameForTileInfo(tile_info);
            auto filename = params.cdb + "/Tiles/" + tile_filepath + "/" + tile_filename;
            if(tile_info.dataset == 1)
                filename += ".tif";
            if(tile_info.dataset == 4)
                filename += ".jp2";
            if(std::filesystem::exists(filename))
            {
                sampler.AddFile(filename);
                std::cout << filename << "\n";
                continue;
            }
            if(tile_info.lod - 1 < -10)
                continue;
            auto tile_bounds = cognitics::cdb::NSEWBoundsForTileInfo(tile_info);
            auto parent_coords = cognitics::cdb::CoordinatesRange(std::get<3>(tile_bounds), std::get<2>(tile_bounds), std::get<1>(tile_bounds), std::get<0>(tile_bounds));
            auto parent_tile = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)params.dataset), tile_info.lod - 1).at(0);
            if(std::find(parent_tiles.begin(), parent_tiles.end(), parent_tile) == parent_tiles.end())
                parent_tiles.push_back(parent_tile);
        }
        tiles = parent_tiles;
    }

    auto extents = gdalsampler::GeoExtents();
    extents.north = params.north;
    extents.south = params.south;
    extents.east = params.east;
    extents.west = params.west;
    extents.height = params.height;
    extents.width = params.width;
    auto raster_info = cognitics::cdb::RasterInfo();
    raster_info.North = params.north;
    raster_info.South = params.south;
    raster_info.East = params.east;
    raster_info.West = params.west;
    raster_info.Height = params.height;
    raster_info.Width = params.width;
    raster_info.OriginX = raster_info.West;
    raster_info.OriginY = raster_info.North;
    raster_info.PixelSizeX = (raster_info.East - raster_info.West) / raster_info.Width;
    raster_info.PixelSizeY = (raster_info.South - raster_info.North) / raster_info.Height;

    if(params.dataset == 1)
    {
        auto floats = std::vector<float>(extents.width * extents.height);
        bool result = sampler.Sample(extents, &floats[0]);
        floats = cognitics::cdb::FlippedVertically(floats, raster_info.Width, raster_info.Height, 1);
        cognitics::cdb::WriteFloatsToTIF(params.outfile, raster_info, floats);
    }

    if(params.dataset == 4)
    {
        auto bytes = std::vector<unsigned char>(extents.width * extents.height * 3);
        bool result = sampler.Sample(extents, &bytes[0]);
        bytes = cognitics::cdb::FlippedVertically(bytes, raster_info.Width, raster_info.Height, 3);
        cognitics::cdb::WriteBytesToJP2(params.outfile, raster_info, bytes);
    }

    return true;
}

}
}