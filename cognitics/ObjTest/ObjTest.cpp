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

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )
ccl::ObjLog logger;

int main(int argc, char **argv)
{
    logger.init("main");
    logger << ccl::LINFO;
    GDALAllRegister();
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    std::string rootCDBOutput = "E:/TestData/output/cdbRefactored2";
    std::string objRootDir = "E:/TestData/MUTC_50m_OBJ";
    Obj2CDB obj2_cdb(objRootDir, rootCDBOutput);
    /*
    renderJobList_t demJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Elevation, 11);
    logger << "Building " << demJobs.size() << " CDB elevation tiles." << logger.endl;
    for (auto&& job : demJobs)
    {
        OBJBuildDEM buildDEM(job);
        buildDEM.build();

    }
    */

    renderJobList_t renderJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Imagery, 11);
    logger << "Rendering " << renderJobs.size() << " CDB imagery tiles." << logger.endl;
    if(renderJobs.size()>0)
        renderInit(argc, argv, renderJobs);//objFiles);
    logger << "Build completed!" << logger.endl;
    
    return 0;
}