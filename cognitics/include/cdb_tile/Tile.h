
#pragma once

#include "Dataset.h"
#include "LODRange.h"
#include "CoordinatesRange.h"
#include "TileCoordinates.h"

#include <cstdint>
#include <vector>
#include <string>
#include "cdb_tile/LOD.h"

namespace cognitics {
namespace cdb {


class Tile
{
    CoordinatesRange coordinates;
public:
    const CoordinatesRange& getCoordinates() const
    {
        return coordinates;
    }

    void setCoordinates(const CoordinatesRange& coordinates)
    {
        this->coordinates = coordinates;
    }

    Tile() {}
private:
    Dataset dataset;
    int lod;
    uint32_t uref;
    uint32_t rref;
    uint32_t cs1;
    uint32_t cs2;
public:
    const Dataset& getDataset() const
    {
        return dataset;
    }

    void setDataset(const Dataset& dataset)
    {
        this->dataset = dataset;
    }

    int getLod() const
    {
        return lod;
    }

    void setLod(const int lod)
    {
        this->lod = lod;
    }

    uint32_t getUref() const
    {
        return uref;
    }

    void setUref(const uint32_t uref)
    {
        this->uref = uref;
    }

    uint32_t getRref() const
    {
        return rref;
    }

    void setRref(const uint32_t rref)
    {
        this->rref = rref;
    }

    uint32_t getCs1() const
    {
        return cs1;
    }

    void setCs1(const uint32_t cs1)
    {
        this->cs1 = cs1;
    }

    uint32_t getCs2() const
    {
        return cs2;
    }

    void setCs2(const uint32_t cs2)
    {
        this->cs2 = cs2;
    }


    std::string getFilename(void) const;

    Tile(CoordinatesRange _coordinates, Dataset _ds, int lod, uint32_t _uref, uint32_t _rref, uint32_t _cs1 = 1,
         uint32_t _cs2 = 1);
};

std::vector<Tile> generate_tiles(const CoordinatesRange &coordinates, Dataset ds, LOD lod);





} }
