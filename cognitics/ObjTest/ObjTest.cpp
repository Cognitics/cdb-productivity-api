#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"
#include "OBJRender.h"
#include "Obj2CDB.h"
#include "ccl/StringUtils.h"
#include <scenegraph/ExtentsVisitor.h>
#include "sfa/BSP.h"
#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
ccl::ObjLog logger;

int main(int argc, char **argv)
{
    logger.init("main");
    logger << ccl::LINFO;

    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    std::string rootCDBOutput = "E:/TestData/output/cdbRefactored";
    std::string objRootDir = "E:/TestData/MUTC_50m_OBJ";
    Obj2CDB obj2_cdb(objRootDir, rootCDBOutput);

    renderJobList_t renderJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Imagery, 11);
    logger << "Rendering " << renderJobs.size() << " CDB tiles." << logger.endl;
    if(renderJobs.size()>0)
        renderInit(argc, argv, renderJobs);//objFiles);
    logger << "Rendering completed!" << logger.endl;
    renderJobList_t demJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Elevation, 11);
    for( auto&& job : demJobs)
    {
        OBJBuildDEM buildDEM(job);
        buildDEM.build();
        
    }
    return 0;
}