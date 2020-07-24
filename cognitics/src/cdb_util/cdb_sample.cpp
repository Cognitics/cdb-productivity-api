
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
    auto target_lod = params.lod;
    if(target_lod == 24)
        target_lod = cognitics::cdb::LodForPixelSize(pixel_size);

    auto coords = cognitics::cdb::CoordinatesRange(params.west, params.east, params.south, params.north);
    auto tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)params.dataset), target_lod);
    for(auto& tile : tiles)
    {
        tile.setCs1(params.cs1);
        tile.setCs2(params.cs2);
    }
    auto coverage_tiles = cognitics::cdb::CoverageTilesForTiles(params.cdb, tiles);

    GDALRasterSampler sampler;
    for(auto ctile : coverage_tiles)
    {
        auto cdb = ctile.first;
        auto tile = ctile.second;
        auto tile_info = cognitics::cdb::TileInfoForTile(tile);
        auto tile_filepath = cognitics::cdb::FilePathForTileInfo(tile_info);
        auto tile_filename = cognitics::cdb::FileNameForTileInfo(tile_info);
        auto filename = cdb + "/Tiles/" + tile_filepath + "/" + tile_filename;
        if(tile_info.dataset == 1)
            filename += ".tif";
        if(tile_info.dataset == 4)
            filename += ".jp2";
        sampler.AddFile(filename);
        ccl::Log::instance()->write(ccl::LDEBUG, "  " + filename);
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

std::vector<unsigned char> cdb_sample_imagery(cdb_sample_parameters& params)
{
    auto pixel_size_x = std::abs((params.east - params.west) / params.width);
    auto pixel_size_y = std::abs((params.north - params.south) / params.height);
    auto pixel_size = std::min<double>(pixel_size_x, pixel_size_y);
    auto target_lod = cognitics::cdb::LodForPixelSize(pixel_size);

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

    int channels = 3;

    auto bytes = std::vector<unsigned char>(extents.width * extents.height * channels);
    if(params.blue_marble)
    {
        int bm_width = 21600;
        int bm_height = 10800;
        for(int row = 0, rows = params.height; row < rows; ++row)
        {
            double lat = raster_info.North + (row * raster_info.PixelSizeY);
            int bm_row = bm_height - (((lat + 90.0) / 180.0) * bm_height) - 1;
            if(bm_row < 0)
                continue;
            if(bm_row >= bm_height)
                continue;
            for(int col = 0, cols = params.width; col < cols; ++col)
            {
                double lon = raster_info.West + (col * raster_info.PixelSizeX);
                int bm_col = ((lon + 180.0) / 360.0) * bm_width;
                if(bm_col < 0)
                    continue;
                if(bm_col >= bm_width)
                    continue;

                int bytes_offset = (row * params.width * channels) + (col * channels);
                int bm_offset = (bm_row * bm_width * 3) + (bm_col * 3);
                unsigned char r = params.blue_marble[bm_offset + 0];
                unsigned char g = params.blue_marble[bm_offset + 1];
                unsigned char b = params.blue_marble[bm_offset + 2];
                unsigned char a = 255;
                if(!params.population.empty())
                {
                    int ilat = 90 + std::floor(lat);
                    int ilon = 180 + std::floor(lon);
                    if(params.population.at((ilat * 360) + ilon) > 0)
                    {
                        r = std::min<int>(r + 96, 255);
                        b = std::min<int>(b + 96, 255);
                    }
                }
                bytes[bytes_offset + 0] = r;
                bytes[bytes_offset + 1] = g;
                bytes[bytes_offset + 2] = b;
                if(channels > 3)
                    bytes[bytes_offset + 3] = a;
            }
        }
    }

    if(target_lod < -2)
        return bytes;

    auto coords = cognitics::cdb::CoordinatesRange(params.west, params.east, params.south, params.north);
    auto tiles = cognitics::cdb::generate_tiles(coords, cognitics::cdb::Dataset((uint16_t)params.dataset), target_lod);
    auto coverage_tiles = cognitics::cdb::CoverageTilesForTiles(params.cdb, tiles);

    GDALRasterSampler sampler;
    for(auto ctile : coverage_tiles)
    {
        auto cdb = ctile.first;
        auto tile = ctile.second;
        auto tile_info = cognitics::cdb::TileInfoForTile(tile);
        auto tile_filepath = cognitics::cdb::FilePathForTileInfo(tile_info);
        auto tile_filename = cognitics::cdb::FileNameForTileInfo(tile_info);
        auto filename = cdb + "/Tiles/" + tile_filepath + "/" + tile_filename;
        if(tile_info.dataset == 1)
            filename += ".tif";
        if(tile_info.dataset == 4)
            filename += ".jp2";
        sampler.AddFile(filename);
        ccl::Log::instance()->write(ccl::LDEBUG, "  " + filename);
    }

    bytes = cognitics::cdb::FlippedVertically(bytes, raster_info.Width, raster_info.Height, 3);
    bool result = sampler.Sample(extents, &bytes[0]);
    bytes = cognitics::cdb::FlippedVertically(bytes, raster_info.Width, raster_info.Height, 3);
    return bytes;
}

}
}
