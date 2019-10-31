
#include "cdb_tile/LOD.h"
#include "cdb_tile/Tile.h"
#include "cdb_tile/CoordinatesRange.h"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <vector>

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

    int32_t isouth = (int32_t)floor(geographicBounds.low().latitude().value());
    int32_t iwest = (int32_t)floor(geographicBounds.low().longitude().value());
    int32_t inorth = (int32_t)ceil(geographicBounds.high().latitude().value());
    int32_t ieast = (int32_t)ceil(geographicBounds.high().longitude().value());

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
            for (int32_t uref = 0; uref < lodRows; ++uref)
            {
                double tile_south = ilat + (rowHeight * uref);
                if (tile_south >= geographicBounds.high().latitude().value())
                    continue;
                double tile_north = ilat + (rowHeight * (uref + 1));
                if (tile_north <= geographicBounds.low().latitude().value())
                    continue;
                for (int32_t rref = 0; rref < lodCols; ++rref)
                {
                    double tile_west = ilon + (col_width * rref);
                    if (tile_west >= geographicBounds.high().longitude().value())
                        continue;
                    double tile_east = ilon + (col_width * (rref + 1));
                    if (tile_east <= geographicBounds.low().longitude().value())
                        continue;
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
