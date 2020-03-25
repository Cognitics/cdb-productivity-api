#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>

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
#include <cdb_util/cdb_util.h>
#include <cstdlib>

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )
ccl::ObjLog logger;

void showHelp()
{
    std::cout << "Example Configuration File:\n\n";
    std::cout << "    < ? xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    std::cout << "    < mesh2cdb  highest_lod_only=\"false\" maxlod=\"1\" outputdir=\"\">\n";
    std::cout << "    < origin x=\"0\" y=\"0\"/>\n";
    std::cout << "    < offset x=\"0\" y=\"0\" z=\"0\"/>\n";
    std::cout << "    <wkt>PROJCS[\"WGS 84 / UTM zone 18N\",GEOGCS[\"WGS 84\",DATUM[\"WGS_1984\",SPHEROID[\"WGS 84\",6378137,298.257223563,AUTHORITY[\"EPSG\",\"7030\"]],AUTHORITY[\"EPSG\",\"6326\"]],PRIMEM[\"Greenwich\",0,AUTHORITY[\"EPSG\",\"8901\"]],UNIT[\"degree\",0.0174532925199433,AUTHORITY[\"EPSG\",\"9122\"]],AUTHORITY[\"EPSG\",\"4326\"]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"latitude_of_origin\",0],PARAMETER[\"central_meridian\",-75],PARAMETER[\"scale_factor\",0.9996],PARAMETER[\"false_easting\",500000],PARAMETER[\"false_northing\",0],UNIT[\"metre\",1,AUTHORITY[\"EPSG\",\"9001\"]],AUTHORITY[\"EPSG\",\"32618\"]]</wkt>\n";
    std::cout << "    < sources texturepath=\"\" objdir=\"\" recurse=\"true\">\n";
    std::cout << "    < sourceobj file=\"\"/>\n";
    std::cout << "    < / sources>\n";
    std::cout << "    < / mesh2cdb>\n";
    std::cout << "\n";

}

int main(int argc, char **argv)
{

    cognitics::ArgumentParser args;
    args.AddOption("dry-run",0,"","Analyze data and dump a job list, without processing any jobs");
    args.AddOption("config",1,"<config.xml>","Specify configuration file.");
    //args.AddOption("metadata", 1, "<metadata.xml>", "Specify a metadata.xml file. Assumes ENU srs, use as alternative to -config");
    //args.AddOption("lod", 1, "<LOD>", "Maximum LOD to create, range is -10 through 20 for CDB");
    args.AddOption("cdb", 1, "<output path>", "Use the specified path, ignoring the contents of the config file.");
    args.AddOption("help",0,"","Display help, including sample xml file");

    if(args.Parse(argc,argv)==EXIT_FAILURE)
    {
        return EXIT_FAILURE;
    }

    if (args.Option("help"))
    {
        args.Usage();
        showHelp();
        return 0;
    }
      

    std::string configXML;
    std::string metadataXML;
    bool dryRun = false;
    
    if(args.Option("dry-run"))
    {
        dryRun = true;
    }
    if(args.Option("config"))
    {
        configXML = args.Parameters("config")[0];
    }
    else if(args.Option("metadata"))
    {
        metadataXML = args.Parameters("metadata")[0];
    }
    else
    {
        args.Usage("You must specify a configuration file with -config <file>.");
        return 1;
    }

    Mesh2CDBParams parms;
    if (configXML.length() > 0)
    {
        parms.parse(configXML);
    }

    if (args.Option("cdb"))
    {
        parms.outputDirectory = args.Parameters("cdb")[0];
    }
    if (args.Option("lod"))
    {
        std::string maxLODStr = args.Parameters("lod")[0];
        parms.maxLOD = strtod(maxLODStr.c_str(), nullptr);
    }
    if(!parms.isValid())
    {
        std::cout << "Error: Invalid config file.\n";
        return 1;
    }

    initializeGDALEnvironmentVariables(argv[0]);
    logger.init("main");
    logger << ccl::LINFO;
    GDALAllRegister();
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    ObjSrs srs;
    srs.srsWKT = parms.wkt;
    srs.offsetPt = parms.offset;
    srs.geoOrigin = parms.origin;
    
    //srs.offsetPt = sfa::Point(309113.64844575466, 4291674.189701308, 65.70969764043629);
    //Obj2CDB obj2_cdb(objRootDir, rootCDBOutput, srs,metadataXML,hiveMapperMode);
    Obj2CDB obj2_cdb(parms);

    CPLSetConfigOption("LODMIN", "-10");
    CPLSetConfigOption("LODMAX", argv[3]);

    renderJobList_t renderJobs = obj2_cdb.collectRenderJobs(cognitics::cdb::Dataset::Imagery, parms.maxLOD);
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
           renderInit(argc, argv, renderJobs, parms.outputDirectory);
    }

    return 0;

}