#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
#include "scenegraphflt/scenegraphflt.h"
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

#include <curl/curl.h>


bool testRequest()
{
    std::string url = "https://jsonplaceholder.typicode.com/todos/1";
    CURLcode res;
    CURLcode code;
    static char errorBuffer[CURL_ERROR_SIZE];
    static std::string buffer;
    //curl_global_init(CURL_GLOBAL_DEFAULT);

    CURL *conn = curl_easy_init();
    if(conn == NULL) {
        fprintf(stderr, "Failed to create CURL connection\n");
        exit(EXIT_FAILURE);
    }

    code = curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer);
        if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set error buffer [%d]\n", code);
        return false;
    }
    code = curl_easy_setopt(conn, CURLOPT_URL, "https://jsonplaceholder.typicode.com/todos/1");
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set URL [%s]\n", errorBuffer);
        return false;
    }
 
    code = curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L);
    if(code != CURLE_OK) {
        fprintf(stderr, "Failed to set redirect option [%s]\n", errorBuffer);
        return false;
    }

    curl_easy_setopt(conn, CURLOPT_WRITEDATA, stdout); 
    code = curl_easy_perform(conn);
    curl_easy_cleanup(conn);
    std::cout << "code: " << code << " res: " << buffer << "\n";
    return true;
}

int main(int argc, char **argv)
{
    testRequest();
    return 1;
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