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

int getLODFromFilename(std::string filename)
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

    std::map<sfa::Polygon *, ccl::FileInfo> bestTileLOD;
    sfa::BSP bsp;
    std::map<std::string, int> highestTileLODNum;
    std::map<std::string, std::string> highestTileLODFilename;

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles("f:/MUTC_50m_OBJ/Data", "*.*", true);
    std::vector<ccl::FileInfo> objFiles;
    for (auto&& fi : files)
    {
        if (ccl::stringEndsWith(fi.getFileName(), ".obj"))
        {
            int lod = getLODFromFilename(fi.getFileName());
            logger << fi.getFileName() << " is LOD " << lod << logger.endl;
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
        if (objFiles.size() > 5)
            break;
    }
#if 0
    
    double lat = 47;
    double lon = -120;
    double elev = 0;
    //ltp_ellipsoid->GeodeticToLocal(lat, lon, elev, north, east, up);
    //ltp_ellipsoid->LocalToGeodetic(north, east, up, lat, lon, elev)


#endif
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
        extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);

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

    double dbLeftLon = 0;
    double dbRightLon = 0;
    double dbBottomLat = 0;
    double dbTopLat = 0;
    double dbMinZElev = 0;
    double dbMaxZElev = 0;
    
    ltp_ellipsoid->LocalToGeodetic(dbBottom, dbLeft, dbMinZ, dbBottomLat, dbLeftLon, dbMinZElev);
    ltp_ellipsoid->LocalToGeodetic(dbTop, dbRight, dbMaxZ, dbTopLat, dbRightLon, dbMaxZElev);

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
    //lod.cognitics::cdb::LODRange cdbLODRange(cognitics::cdb::LOD(-10), cognitics::cdb::LOD(8));
    //cognitics::cdb::generate_tiles(cdbTiles, cdbAOI, cognitics::cdb::Dataset::Imagery, cdbLODRange);
    logger << "Found " << cdbTiles.size() << " CDB tiles:" << logger.endl;
    for (auto&& tile : cdbTiles)
    {
        logger << "\t" << tile.getFilename() << logger.endl;
    }
    renderInit(argc, argv, objFiles);
    return 0;
}