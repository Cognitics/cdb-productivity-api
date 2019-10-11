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
    //argv[1] = Input OBJ directory (where metadata.xml exists)
    //argv[2] = Output  CDB directory (the parent directory where Tiles lives)
    //argv[3] = The LOD to generate
    if(argc < 4)
    {
        std::cout << "Usage: obj2cdb <input obj dir> <output CDB dir> <lod number>\n";
        std::cout << "\n\n";
        std::cout << "The input OBJ directory is where metadata.xml exists.\n";
        std::cout << "The output CDB directory is the parent directory where Tiles lives.\n";
        std::cout << "LOD is between -10 and 23.\n";
        return 1;
    }
    std::string rootCDBOutput = argv[2];
    std::string objRootDir = argv[1];
    int cdbLOD = atoi(argv[3]);

    logger.init("main");
    logger << ccl::LINFO;
    GDALAllRegister();
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    //std::string rootCDBOutput = "E:/TestData/output/cdbRefactored2";
    //std::string objRootDir = "E:/TestData/MUTC_50m_OBJ";
    //std::string rootCDBOutput = "j:/output/pinehurst_cdb";
    //std::string objRootDir = "j:/MUTC_OBJ/MUTC_50m_OBJ";
    //std::string objRootDir = "j:/objs/Pinehurst_OBJ_SEGT_ENU_190701_altadjusted/Pinehurst_OBJ_SEGT_ENU_190701";
    //J:\objs\Pinehurst_OBJ_SEGT_ENU_190701_altadjusted\Pinehurst_OBJ_SEGT_ENU_190701
    //J:\objs\CACTIF 50m rapid OBJ
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

    renderJobList_t renderJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Imagery, cdbLOD);
    logger << "Rendering " << renderJobs.size() << " CDB imagery tiles." << logger.endl;
    if(renderJobs.size()>0)
        renderInit(argc, argv, renderJobs);//objFiles);
    logger << "Build completed!" << logger.endl;
    
    return 0;
}