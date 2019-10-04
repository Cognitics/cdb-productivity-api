#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"

ccl::ObjLog logger;

int main(int argc, char **argv)
{
    logger.init("main");
    logger << ccl::LINFO;

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    scenegraph::Scene *scene = scenegraph::buildSceneFromOBJ("E:/TestData/Tile_+001_+024/Tile_+001_+024_L20.obj", "E:/TestData/Tile_+001_+024/");

    return 0;
}