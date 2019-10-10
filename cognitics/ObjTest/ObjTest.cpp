#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"
#include "OBJRender.h"
#include "ccl/StringUtils.h"
#include <scenegraph/ExtentsVisitor.h>
#include "sfa/BSP.h"
#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
ccl::ObjLog logger;


int getLODFromFilename(const std::string &filename)
{
    if (ccl::stringEndsWith(filename, ".obj", false))
    {
        std::string truncFilename = filename.substr(0, filename.size() - 4);
        std::string::size_type n = truncFilename.rfind("L");
        if (n != std::string::npos)
        {
            std::string lodNumStr = truncFilename.substr(n + 1);
            int lodNum = atoi(lodNumStr.c_str());
            return lodNum;
        }
    }
    return 0;
}

int main(int argc, char **argv)
{
    logger.init("main");
    logger << ccl::LINFO;

    std::map<sfa::Geometry *, ccl::FileInfo> bestTileLOD;
    sfa::BSP bsp;
    bsp.targetCount = 1;
    std::map<std::string, int> highestTileLODNum;
    std::map<std::string, std::string> highestTileLODFilename;

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles("E:/TestData/MUTC_50m_OBJ/Data", "*.*", true);
    std::vector<ccl::FileInfo> objFiles;
    for (auto&& fi : files)
    {
        if (ccl::stringEndsWith(fi.getFileName(), ".obj"))
        {
            int lod = getLODFromFilename(fi.getFileName());
            //logger << fi.getFileName() << " is LOD " << lod << logger.endl;
            if (highestTileLODNum.find(fi.getDirName()) == highestTileLODNum.end())
            {
                highestTileLODNum[fi.getDirName()] = lod;
                highestTileLODFilename[fi.getDirName()] = fi.getFileName();
            }
            else
            {
                if (lod > highestTileLODNum[fi.getDirName()])
                {
                    highestTileLODNum[fi.getDirName()] = lod;
                    highestTileLODFilename[fi.getDirName()] = fi.getFileName();
                }
            }
        }
    }
    for (auto&& fileLODPair : highestTileLODFilename)
    {
        objFiles.push_back(fileLODPair.second);
        //if (objFiles.size() > 5)
        //    break;
    }
    double origin_lat = 39.05011;
    double origin_lon = -85.53082;
    auto ltp_ellipsoid = new Cognitics::CoordinateSystems::EllipsoidTangentPlane(origin_lat, origin_lon);

#if 1
    double dbTop = -DBL_MAX;
    double dbBottom = DBL_MAX;
    double dbLeft = DBL_MAX;
    double dbRight = -DBL_MAX;
    double dbMinZ = DBL_MAX;
    double dbMaxZ = -DBL_MAX;

    //objFiles
    for (auto&& fi : objFiles)
    {
        scenegraph::ExtentsVisitor extentsVisitor;
        scenegraph::Scene *scene = scenegraph::buildSceneFromOBJ(fi.getFileName(), true);
        double top = -DBL_MAX;
        double bottom = DBL_MAX;
        double left = DBL_MAX;
        double right = -DBL_MAX;
        double minZ = DBL_MAX;
        double maxZ = -DBL_MAX;
        extentsVisitor = scenegraph::ExtentsVisitor();
        extentsVisitor.visit(scene);
        if(!extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ) ||
            top <= bottom ||
            right <= left)
        {
            continue;
        }
        logger << fi.getBaseName() << " : " << left << " <-> " << right << " | " << top << " ^ " << bottom << logger.endl;
        dbLeft = std::min<double>(left, dbLeft);
        dbRight = std::max<double>(right,dbRight);
        dbBottom = std::min<double>(bottom, dbBottom);
        dbTop = std::max<double>(top, dbTop);
        dbMinZ = std::min<double>(minZ, dbMinZ);
        dbMaxZ = std::max<double>(maxZ, dbMaxZ);

        sfa::Polygon *aoi_poly = new sfa::Polygon;
        sfa::LineString *ring = new sfa::LineString;
        ring->addPoint(sfa::Point(left, bottom));
        ring->addPoint(sfa::Point(right, bottom));
        ring->addPoint(sfa::Point(right, top));
        ring->addPoint(sfa::Point(left, top));
        ring->addPoint(sfa::Point(left, bottom));
        aoi_poly->addRing(ring);
        //Keep track of the geometry and its associated file
        bestTileLOD[aoi_poly] = fi;
        bsp.addGeometry(aoi_poly);
        delete scene;
    }
    std::map<sfa::Geometry *, sfa::LineString *> envelopes;
    bsp.generate(envelopes);

    double dbLeftLon = 0;
    double dbRightLon = 0;
    double dbBottomLat = 0;
    double dbTopLat = 0;
    double dbMinZElev = 0;
    double dbMaxZElev = 0;
    
    ltp_ellipsoid->LocalToGeodetic(dbLeft, dbBottom, 0, dbBottomLat, dbLeftLon, dbMinZElev);
    double e, n, u;
    ltp_ellipsoid->GeodeticToLocal(dbBottomLat, dbLeftLon, 0, e, n, u);



    ltp_ellipsoid->LocalToGeodetic(dbRight, dbTop, dbMaxZ, dbTopLat, dbRightLon, dbMaxZElev);

    logger << "Database Extents (Cartesian):" << logger.endl;
    logger << "LL: " << dbLeft << " , " << dbBottom << logger.endl;
    logger << "UR: " << dbRight << " , " << dbTop << logger.endl;
    
    logger << "Database Extents (Geo):" << logger.endl;
    logger << "LL: " << dbLeftLon << " , " << dbBottomLat << logger.endl;
    logger << "UR: " << dbRightLon << " , " << dbTopLat << logger.endl;

#endif

    std::vector<cognitics::cdb::Tile> cdbTiles;
    cognitics::cdb::LOD lod(8);
    cognitics::cdb::Coordinates cdbLL(dbBottomLat, dbLeftLon);
    cognitics::cdb::Coordinates cdbUR(dbTopLat,dbRightLon);
    cognitics::cdb::CoordinatesRange cdbAOI(cdbLL, cdbUR);
    cdbTiles = cognitics::cdb::generate_tiles(cdbAOI, cognitics::cdb::Dataset::Imagery, lod);

    std::string cdbRootPath = "e:/testdata/output/test_cdb";
    logger << "Found " << cdbTiles.size() << " CDB tiles:" << logger.endl;

    renderJobList_t renderJobs;
    for (auto&& tile : cdbTiles)
    {
        RenderJob renderJob(tile);
        sfa::BSPCollectGeometriesVisitor bspVisitor;
        std::string absoluteFilePath = ccl::joinPaths(cdbRootPath, tile.getFilename());
        ccl::FileInfo tileFi(absoluteFilePath);
        ccl::makeDirectory(tileFi.getDirName());
        renderJob.cdbFilename = absoluteFilePath;
        logger << "\t" << absoluteFilePath << logger.endl;
        //get all scenes that intersect with this tile.
        //bsp.
        //
        double tileLocalBottom = 0;
        double tileLocalTop = 0;
        double tileLocalLeft = 0;
        double tileLocalRight = 0;
        double localZ;
        ltp_ellipsoid->GeodeticToLocal(tile.getCoordinates().low().latitude().value(),
            tile.getCoordinates().low().longitude().value(),
            0,
            tileLocalLeft,
            tileLocalBottom,
            localZ);
        ltp_ellipsoid->GeodeticToLocal(tile.getCoordinates().high().latitude().value(),
            tile.getCoordinates().high().longitude().value(),
            0,
            tileLocalRight,
            tileLocalTop,
            localZ);
        renderJob.enuMinX = tileLocalLeft;
        renderJob.enuMaxX = tileLocalRight;
        renderJob.enuMinY = tileLocalBottom;
        renderJob.enuMaxY = tileLocalTop;

        logger << "Tile Extents (Cartesian):" << logger.endl;
        logger << "LL: " << tileLocalLeft << " , " << tileLocalBottom << logger.endl;
        logger << "UR: " << tileLocalRight << " , " << tileLocalTop << logger.endl;

        bspVisitor.setBounds(tileLocalLeft,
            tileLocalBottom, 
            tileLocalRight,
            tileLocalTop);
        bspVisitor.visiting(&bsp);
        scenegraph::Scene *parentScene = new scenegraph::Scene();
        for (auto&& geometry : bspVisitor.results)
        {            
            std::string sourceOBJ = bestTileLOD[geometry].getFileName();
            logger << "\t\tUsing " << sourceOBJ << " as source file." << logger.endl;
            renderJob.objFiles.push_back(sourceOBJ);
            //scenegraph::Scene *childScene = scenegraph::buildSceneFromOBJ(sourceOBJ, true);
            //parentScene->addChild(childScene);
        }
        if (!renderJob.objFiles.empty())
        {
            renderJobs.push_back(renderJob);
        }
    }
    renderInit(argc, argv, renderJobs);//objFiles);
    return 0;
}