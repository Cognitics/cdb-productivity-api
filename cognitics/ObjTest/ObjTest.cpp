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
    }
#if 0
    //objFiles
    for (auto&& fileLODPair : highestTileLODFilename)
    {
        fileLODPair.first;
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
#endif

    renderInit(argc, argv, objFiles);
    return 0;
} 