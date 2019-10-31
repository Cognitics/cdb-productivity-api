#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"
#include "MeshRender.h"
#include "mesh2cdb.h"
#include "ccl/StringUtils.h"
#include <scenegraph/ExtentsVisitor.h>
#include "sfa/BSP.h"
#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
#include <cstdlib>

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
    if (argc < 4)
    {
        std::cout << "Usage: obj2cdb <input obj dir> <output CDB dir> <lod number> [metadata.xml] [hivemapper]\n";
        std::cout << "\n\n";
        std::cout << "The input OBJ directory is where metadata.xml exists.\n";
        std::cout << "The output CDB directory is the parent directory where Tiles lives.\n";
        std::cout << "LOD is between -10 and 23.\n";
        std::cout << "Metadata XML file (default is the input obj dir + 'Metadata.xml').\n";
        std::cout << "hivemapper: If specified, hivemapper mode is used.\n";
        return 1;
    }
    std::string metadataXML;
    bool hiveMapperMode = false;
    std::string rootCDBOutput = argv[2];
    std::string objRootDir = argv[1];
    int cdbLOD = atoi(argv[3]);
    if(argc > 4)
    {
        metadataXML = argv[4];
        logger << "Using metadata file: " << metadataXML << "." << logger.endl;
    }
    if (argc > 5)
    {
        if(ccl::stringCompareNoCase(argv[5],"hivemapper")==0)
        {
            hiveMapperMode = true;
            logger << "Using HiveMapper Mode." << logger.endl;
        }
    }

#ifndef WIN32
    char *gdal_data_var = getenv("GDAL_DATA");
    if(gdal_data_var==NULL)
    {
        ccl::FileInfo fi(argv[0]);
        int bufSize = 1024;
        //This memory becomes owned by the environment,
        //so do not delete the pointer.
        char *envBuffer = new char[bufSize];      
        std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");  
        sprintf(envBuffer, "GDAL_DATA=%s", dataDir.c_str());
        putenv(envBuffer);
    }
#else
    size_t requiredSize;
    getenv_s(&requiredSize, NULL, 0, "GDAL_DATA");
    if (requiredSize == 0)
    {
        ccl::FileInfo fi(argv[0]);
        int bufSize = 1024;
        char *envBuffer = new char[bufSize];
        std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");
        sprintf_s(envBuffer, bufSize, "GDAL_DATA=%s", dataDir.c_str());
        _putenv(envBuffer);        
    }
#endif
    logger.init("main");
    logger << ccl::LINFO;
    GDALAllRegister();
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    Obj2CDB obj2_cdb(objRootDir, rootCDBOutput,metadataXML,hiveMapperMode);

    CPLSetConfigOption("LODMIN", "-10");
    CPLSetConfigOption("LODMAX", argv[3]);

    renderJobList_t renderJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Imagery, cdbLOD);
    logger << "Rendering " << renderJobs.size() << " CDB imagery tiles." << logger.endl;
    if (renderJobs.size() > 0)
        renderInit(argc, argv, renderJobs, rootCDBOutput);
    

    return 0;
}