#pragma once
#pragma once

#include <string>
#include <vector>
#include <list>
#include "sfa/BSP.h"
#include "ccl/ObjLog.h"
#include "cdb_tile/Tile.h"
#include "CoordinateSystems/EllipsoidTangentPlane.h"

class RenderJob
{
public:
    std::string cdbFilename;
    double enuMinX;
    double enuMinY;
    double enuMaxX;
    double enuMaxY;
    double offsetX;
    double offsetY;
    double offsetZ;
    cognitics::cdb::Tile cdbTile;
    std::vector<std::string> objFiles;


    RenderJob(const cognitics::cdb::Tile tile) : cdbTile(tile)
    {
        enuMinX = 0;
        enuMinY = 0;
        enuMaxX = 0;
        enuMaxY = 0;
        offsetX = 0;
        offsetY = 0;
        offsetZ = 0;
    }
};
typedef std::list<RenderJob> renderJobList_t;

class Obj2CDB
{
    ccl::ObjLog log;
    std::string objRootDir;
    std::string cdbOutputDir;
    double dbOriginLat;
    double dbOriginLon;
    double dbTop;
    double dbBottom;
    double dbLeft;
    double dbRight;
    double dbMinZ;
    double dbMaxZ;
    double dbLeftLon;
    double dbRightLon;
    double dbBottomLat;
    double dbTopLat;
    double dbMinZElev;
    double dbMaxZElev;
    double offsetX;
    double offsetY;
    double offsetZ;

    std::vector<ccl::FileInfo> objFiles;
    sfa::BSP bsp;
    std::map<sfa::Geometry *, sfa::LineString *> envelopes;
    std::map<sfa::Geometry *, ccl::FileInfo> bestTileLOD;
    Cognitics::CoordinateSystems::EllipsoidTangentPlane *ltp_ellipsoid;

    int getLODFromFilename(const std::string &filename);
    void collectHighestLODTiles();
    void buildBSP();
    bool readMetadataXML(const std::string &sourceDir);
public:
    Obj2CDB(const std::string &inputOBJDir,
        const std::string &outputCDBDir);
    ~Obj2CDB();

    renderJobList_t collectRenderJobs(cognitics::cdb::Dataset dataset, int lodNum);
};

class OBJBuildDEM
{
    ccl::ObjLog log;
    RenderJob job;
    bool writeDEM(float *grid, int width, int height);
public:
    OBJBuildDEM(RenderJob job);
    bool build();

};