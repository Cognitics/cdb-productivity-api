
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

    Tile() : lod(0),
        uref(0),
        rref(0),
        cs1(1),
        cs2(1),
        postSpaceX(0),
        postSpaceY(0)
    {
        
    }

    Tile(int lod, uint32_t uref, uint32_t rref, uint32_t cs1, uint32_t cs2, double post_space_x, double post_space_y)
        : lod(lod),
          uref(uref),
          rref(rref),
          cs1(cs1),
          cs2(cs2),
          postSpaceX(post_space_x),
          postSpaceY(post_space_y)
    {
    }

    Tile(const Tile& rhs) = default;
    bool operator==(const Tile& rhs) { return (coordinates == rhs.coordinates) && (lod == rhs.lod) && (uref == rhs.uref) && (rref == rhs.rref) && (cs1 == rhs.cs1) && (cs2 == rhs.cs2); }
    bool operator!=(const Tile& rhs) { return !(*this == rhs); } 
    bool operator<(const Tile& rhs)
    {
        if(coordinates < rhs.coordinates)
            return true;
        if(coordinates > rhs.coordinates)
            return false;
        if(lod < rhs.lod)
            return true;
        if(lod > rhs.lod)
            return false;
        if(uref < rhs.uref)
            return true;
        if(uref > rhs.uref)
            return false;
        if(rref < rhs.rref)
            return true;
        if(rref > rhs.rref)
            return false;
        if(cs1 < rhs.cs1)
            return true;
        if(cs1 > rhs.cs1)
            return false;
        if(cs2 < rhs.cs2)
            return true;
        if(cs2 > rhs.cs2)
            return false;
        return false;
    }

private:
    Dataset dataset;
    int lod;
    uint32_t uref;
    uint32_t rref;
    uint32_t cs1;
    uint32_t cs2;


public:
    double postSpaceX;
    double postSpaceY;

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

    std::string Path() const;
    std::string Filename() const;

    Tile(CoordinatesRange _coordinates, Dataset _ds, int lod, uint32_t _uref, uint32_t _rref, uint32_t _cs1 = 1, uint32_t _cs2 = 1);
};

std::vector<Tile> generate_tiles(const CoordinatesRange &coordinates, Dataset ds, LOD lod);





} }
