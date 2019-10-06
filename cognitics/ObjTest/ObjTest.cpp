#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"
#include "OBJRender.h"
#include "ccl/StringUtils.h"
#include <scenegraph/ExtentsVisitor.h>
ccl::ObjLog logger;

int main(int argc, char **argv)
{
    logger.init("main");
    logger << ccl::LINFO;   

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles("E:/TestData/MUTC_50m_OBJ/Data", "*.*", true);
    std::vector<ccl::FileInfo> objFiles;
    for (auto&& fi : files)
    {
        if (ccl::stringEndsWith(fi.getFileName(),"L20.obj"))
        {
            objFiles.push_back(fi);
        }
    }
    /*
    logger << "Found " << objFiles.size() << " matching files." << logger.endl;
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
        extentsVisitor.visit(scene);
        extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);
        logger << fi.getBaseName() << 
            " minX=" << left << " maxX=" << right << 
            " minY=" << bottom << " maxY=" << top << 
            " minZ=" << minZ << " maxZ=" << maxZ 
            << logger.endl;

    }
    */
    scenegraph::Scene *scene = scenegraph::buildSceneFromOBJ("E:/TestData/MUTC_50m_OBJ/Data/Tile_+005_+021/Tile_+005_+021_L21.obj",true);

    //E:/TestData/Tile_+001_+024/Tile_+001_+024_L20.obj
    //E:/TestData/MUTC_50m_OBJ/Data/Tile_+005_+021/Tile_+005_+021_L21.obj
    //scenegraph::buildOpenFlightFromScene("e:/testdata/test.flt", scene);
    //ip::ImageInfo info;
    //ccl::binary buf;
    //ip::GetPNGImagePixels("e:/test.png", info, buf);

    renderInit(argc, argv, scene);    
    return 0;
} 