
#include "cdb_tile/LOD.h"
#include "cdb_tile/Tile.h"
#include "cdb_tile/CoordinatesRange.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>
#include <math.h>

namespace cognitics {
namespace cdb {

LOD::LOD(void) : _value(0)
{
}

LOD::LOD(int8_t value) : _value(value)
{
    _value = std::min<int8_t>(_value, 23);
    _value = std::max<int8_t>(_value, -10);
}

int8_t LOD::value(void) const
{
    return _value;
}

uint32_t LOD::dimensions(void) const
{
    return (int)pow(2, std::min<double>(_value + 10, 10));
}

uint32_t LOD::rows(void) const
{
    return std::max<uint32_t>(uint32_t(std::pow(2, _value)), 1);
}

uint32_t LOD::cols(void) const
{
    return std::max<uint32_t>(uint32_t(std::pow(2, _value)), 1);
}

std::string LOD::subdir(void) const
{
    if(_value < 0)
        return "LC";
    std::stringstream ss;
    ss << "L" << std::setw(2) << std::setfill('0') << int(_value);
    return ss.str();
}

std::vector<Tile> generate_tiles(const CoordinatesRange &geographicBounds, Dataset ds, LOD lod)
{
    std::vector<Tile> result;

    double south = geographicBounds.low().latitude().value();
    double north = geographicBounds.high().latitude().value();
    double west = geographicBounds.low().longitude().value();
    double east = geographicBounds.high().longitude().value();
    int32_t isouth = (int32_t)floor(south);
    int32_t iwest = (int32_t)floor(west);
    int32_t inorth = (int32_t)ceil(north);
    int32_t ieast = (int32_t)ceil(east);

    int lodRows = lod.rows();
    int lodCols = lod.cols();
    double rowHeight = 1.0f / lodRows;
    for (int ilat = isouth; ilat <= inorth; ++ilat)
    {
        Latitude latitude(ilat);
        int ilon_step = get_tile_width(latitude);
        double col_width = (double)ilon_step / lodCols;
        for (int ilon = iwest; ilon <= ieast; ilon += ilon_step)
        {
            int usouth = (int)std::floor((south - ilat) / rowHeight);
            usouth = std::max<int>(usouth, 0);
            int unorth = (int)std::ceil((north - ilat) / rowHeight);
            unorth = std::min<int>(unorth, lodRows);
            for (int32_t uref = usouth; uref < unorth; ++uref)
            {
                double tile_south = ilat + (rowHeight * uref);
                double tile_north = ilat + (rowHeight * (uref + 1));
                int uwest = (int)std::floor((west - ilon) / col_width);
                uwest = std::max<int>(uwest, 0);
                int ueast = (int)std::ceil((east - ilon) / col_width);
                ueast = std::min<int>(ueast, lodCols);
                for (int32_t rref = uwest; rref < ueast; ++rref)
                {
                    double tile_west = ilon + (col_width * rref);
                    double tile_east = ilon + (col_width * (rref + 1));
                    CoordinatesRange tileBounds(tile_west, tile_east, tile_south, tile_north);
                    Tile tile(tileBounds, ds, lod.value(),uref,rref);
                    tile.postSpaceX = col_width / 1024;
                    tile.postSpaceY = rowHeight / 1024;
                    result.push_back(tile);
                }
            }
        }
    }
    return result;
}


} }
