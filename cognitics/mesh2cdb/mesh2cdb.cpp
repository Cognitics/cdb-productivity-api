#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
//#include "scenegraphobj/scenegraphobj.h"
//#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"
#include "MeshRender.h"
#include "mesh2cdb.h"
#include "ccl/StringUtils.h"
#include "ccl/ArgumentParser.h"
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
    cognitics::ArgumentParser args;
    args.AddOption("dry-run",0,"","Analyze data and dump a job list, without processing any jobs");
    args.AddOption("metadata",1,"<metadata-filename>","Specify metadata file with the origin and offsets.");
    args.AddOption("hivemapper",0,"","Hivemapper compatability mode (all OBJ files are used, no LOD sorting).");
    args.AddArgument("Input OBJ Directory");
    args.AddArgument("Output CDB directory");
    args.AddArgument("Output LOD number");
    if(args.Parse(argc,argv)==EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }
    std::string metadataXML;
    bool dryRun = false;
    bool hiveMapperMode = false;
    std::string rootCDBOutput = args.Arguments()[1];
    std::string objRootDir = args.Arguments()[0];
    int cdbLOD = atoi(args.Arguments()[2].c_str());
    if(args.Option("hivemapper"))
    {
        hiveMapperMode = true;
    }
    if(args.Option("dry-run"))
    {
        dryRun = true;
    }
    if(args.Option("metadata"))
    {
        metadataXML = args.Parameters("metadata")[0];
    }
    if(argc > 4)
    {
        metadataXML = argv[4];
        logger << "Using metadata file: " << metadataXML << "." << logger.endl;
    }

#ifndef WIN32
    char *gdal_data_var = getenv("GDAL_DATA");
    if(gdal_data_var==NULL)
    {        
        putenv("GDAL_DATA=/usr/local/share/gdal");
    }
    char *gdal_plugins_var = getenv("GDAL_DRIVER_PATH");
    if(gdal_plugins_var==NULL)
    {        
        putenv("GDAL_DRIVER_PATH=/usr/local/bin/gdalplugins");
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
    ObjSrs srs;
    srs.srsWKT = "PROJCS[\"WGS 84 / UTM zone 18N\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-75],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AUTHORITY[\"EPSG\",\"32618\"]]";
    srs.offsetPt = sfa::Point(309113.64844575466, 4291674.189701308, 65.70969764043629);
    //Obj2CDB obj2_cdb(objRootDir, rootCDBOutput, srs,metadataXML,hiveMapperMode);
    Obj2CDB obj2_cdb(objRootDir, rootCDBOutput, srs, metadataXML, hiveMapperMode);

    CPLSetConfigOption("LODMIN", "-10");
    CPLSetConfigOption("LODMAX", argv[3]);

    renderJobList_t renderJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Imagery, cdbLOD);
    logger << "Rendering " << renderJobs.size() << " CDB imagery tiles." << logger.endl;
    if(dryRun)
    {
        for(auto&& job : renderJobs)
        {
            logger << job.ToString() << logger.endl;
        }
    }
    else
    {
        if (renderJobs.size() > 0)
           renderInit(argc, argv, renderJobs, rootCDBOutput);
    }

    return 0;
}