#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"

#include "ccl/StringUtils.h"
#include "ccl/ArgumentParser.h"
// #include <scenegraph/ExtentsVisitor.h>
// #include "sfa/BSP.h"
#include "sfa/sfa.h"
#include <sfa_file_factory/sfa_file_factory.h>
#include <ip/jpgwrapper.h>
#include <ip/rgb.h>
#include <cts/CS_CoordinateSystemFactory.h>
#include <sfa/Layer.h>
#include <sfa/File.h>
#include <ogr/ogr.h>
#include "ogr/File.h"
// #include "rapidjson/document.h"
// #include "rapidjson/writer.h"
// #include "rapidjson/stringbuffer.h"
//#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>
#include <boost/lexical_cast.hpp>

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#include "obj-flt.h"
#include "scenegraphobj/quickobj.h"
ccl::ObjLog logger;

sfa::Point readOffsetXYZ(const std::string &filename)
{
    sfa::Point offset;
    std::ifstream prjfile(filename.c_str());
    std::string data = "";
    std::stringstream buffer;
    buffer << prjfile.rdbuf();
    data = buffer.str();
    prjfile.close();

    // Split with whitespace
    std::vector<std::string> parts = ccl::splitString(data, " \t");
    if (parts.size() != 3)
    {
        //Error, not enough parts in the xyz file
        return offset;
    }
    offset.setX(atof(parts[0].c_str()));
    offset.setY(atof(parts[1].c_str()));
    offset.setZ(atof(parts[2].c_str()));
    return offset;
}

std::string fileToWKT(const std::string &filename)
{
	std::string data = "";

	std::ifstream prjfile(filename.c_str());
	if (!prjfile)
		return NULL;
	std::stringstream buffer;
	buffer << prjfile.rdbuf();
	data = buffer.str();
	prjfile.close();
	const std::string prjstr = data.c_str();

	return prjstr;
}

OGRSpatialReference *LoadProjectionFromPRJ(const std::string &filename)
{
    std::string data = "";
    
    std::ifstream prjfile(filename.c_str());
    if (!prjfile)
        return NULL;
    std::stringstream buffer;
    buffer << prjfile.rdbuf();
    data = buffer.str();
    prjfile.close();

    OGRSpatialReference *file_srs = new OGRSpatialReference;
    const char *prjstr = data.c_str();
    OGRErr err = file_srs->importFromWkt((char **)&prjstr);
    if (err != OGRERR_NONE)
    {
        delete file_srs;
        return NULL;
    }
    return file_srs;
}

void printUsage()
{
	std::cout << "Usage:" << std::endl;
	std::cout << "obj-flt </path/to/obj-file> </path/to/shp-file> </path/to/output_directory>" << std::endl;
}

std::pair<double,double> originFromObj(std::string obj_path)
{
	std::pair<double,double> origin = std::pair<double, double>();
	std::ifstream obj_file(obj_path);
	std::string line_text;

	while(std::getline(obj_file, line_text))
	{
		// std::cout << line_text << std:: endl;
		std::vector<std::string> parts = ccl::splitString(line_text, " ");
		if (parts[0].compare("ORIGIN:") == 0) {
			std::stringstream output;
			std::stringstream output_two;
			std::string first = parts[1];
			std::string second = parts[2];
			std::cout.precision(10);

			output << std::left << std::setfill('0') << std::setw(10) << first;
			origin.first = std::stod(output.str());
			output_two << std::left << std::setfill('0') << std::setw(10) << second;
			origin.second = std::stod(output_two.str());


			obj_file.close();
			return origin;
		}
	}

	obj_file.close();
	return origin;
} 

int main(int argc, char **argv)
{
    // cognitics::ArgumentParser args;

	if (argc < 4) {
		printUsage();
		return EXIT_FAILURE;
	}
	if (argc > 4) {
		printUsage();
		return EXIT_FAILURE;
	}

	std::string slash = "/";
	std::string input_obj = argv[1];
	std::string shapefile_path = argv[2];
	std::string output_directory = argv[3];
	std::string filename_no_extension;
	std::string input_directory;

	std::size_t extension_index = input_obj.find_last_of('.');

	std::string directory_no_extension = input_obj.substr(0, input_obj.find_last_of('/'));
	input_directory = directory_no_extension[directory_no_extension.length() - 1] == '/' ? directory_no_extension : directory_no_extension + slash;
	
	if (extension_index != -1 && extension_index != 0) {
		if (input_obj.substr(extension_index + 1).compare("obj") != 0) {
			std::cout << "Please use an obj filetype..." << std::endl;
			printUsage();
			return EXIT_FAILURE;
		}
		filename_no_extension = input_obj.substr(input_obj.find_last_of('/') + 1, extension_index - (input_obj.find_last_of('/') + 1));
	}

	if (extension_index == -1 || extension_index == 0) {
		filename_no_extension = input_obj.substr(input_obj.find_last_of('/') + 1, input_obj.length() -1);
		std::string extension = ".obj";
		input_obj = input_obj + extension;
	}

	extension_index = shapefile_path.find_last_of('.');
	
	if (extension_index != -1 && extension_index != 0) {
		if (shapefile_path.substr(extension_index + 1).compare("shp") != 0) {
			std::cout << "Please use an shp filetype..." << std::endl;
			printUsage();
			return EXIT_FAILURE;
		}
	}

	if (extension_index == -1 || extension_index == 0) {
		std::string extension = ".shp";
		shapefile_path = shapefile_path + extension;
	}


	output_directory = output_directory[output_directory.length() - 1] == '/' ? output_directory : output_directory + slash;

	std::cout << input_obj << std::endl;
	std::cout << filename_no_extension << std::endl;
	std::cout << shapefile_path << std::endl;
	std::cout << output_directory << std::endl;
	std::cout << input_directory << std::endl;

	auto origin = originFromObj(input_obj);
    std::cout << "Latitude: " << std::setprecision(10) << origin.first << std::endl;
	std::cout << "Longitude: " << std::setprecision(10) << origin.second << std::endl;

	std::string lat_lon_wkt = "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
	sfa::Point offset; // no offset provided from OBJ for now -- will make this and the wkt more portable in the future by looking for file in given directory
	offset.setX(1.0);
	offset.setY(1.0);
	offset.setZ(1.0);

	ObjSrs mesh_srs;
	mesh_srs.srsWKT = lat_lon_wkt;
	mesh_srs.offsetPt = offset;

	auto global_mesh_collection = cognitics::QuickObj(input_obj, mesh_srs, input_directory);

	return EXIT_SUCCESS;
}

	// InitOGR();
	// std::string shapeFilename = visitor_center_location + "output.shp";
	// sfa::File *output_sfa_file = sfa::FileRegistry::instance()->getFile(shapeFilename);
	// if (!output_sfa_file->open(shapeFilename, true))
	// {
	// 	if (!output_sfa_file || !output_sfa_file->create(shapeFilename))
	// 	{
	// 		logger << ccl::LERR << "failed to open " << shapeFilename << "." << logger.endl;
	// 		return -1;
	// 	}
	// }
	// output_sfa_file->beginUpdating();
	// sfa::Layer *output_layer_point = output_sfa_file->addLayer("points", sfa::wkbPointZ);

	// logger << "Rewriting PRJ file" << logger.endl;
	// std::ofstream ofs(visitor_center_location + "output.prj", std::ofstream::trunc);
	// ofs << "GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137,298.257223563]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
	// ofs.close();
 
	// logger << "Outputting cut meshes to OBJ files..." << logger.endl;
	// auto exportable_mesh_collection_filenames = std::vector<std::string>(exportable_meshes.size());
	// for (int i = 0; i < exportable_meshes.size(); ++i)
	// {
	// 	ccl::uuid uuid = cognitics::uuid::random_generator<ccl::uuid>()();
	// 	std::string uuidstr = boost::lexical_cast<std::string>(uuid);

	// 	std::string filename = uuidstr + std::to_string(i);
	// 	logger << "\tWriting " + filename << logger.endl;

	// 	auto& mesh = exportable_meshes.at(i);
	// 	mesh.materialMap = mesh_obj.materialMap;
	// 	mesh.exportObj(visitor_center_location + filename); // OBJ

	// 	ObjSrs srs;
	// 	OGRSpatialReference *srf = LoadProjectionFromPRJ(visitor_center_location + "output.prj");
	// 	char *pszSRS_WKT = NULL;
	// 	srf->exportToWkt(&pszSRS_WKT);
	// 	if (pszSRS_WKT)
	// 		srs.srsWKT = std::string(pszSRS_WKT);
	// 	else
	// 		srs.srsWKT = std::string("ENU");
	// 	srs.offsetPt = readOffsetXYZ(visitor_center_location + "output.xyz");
	// 	cognitics::QuickObj qo(visitor_center_location + filename + ".obj", srs, visitor_center_location, true);

	// 	qo.expandCoordinates();
	// 	//Translate to the centroid and get the origin in lat/lon
	// 	sfa::Point geoOrigin = qo.findCenterAndReOrigin();
	// 	cognitics::QuickObj2Flt qo_flt;
	// 	qo_flt.convertTextures(&qo, visitor_center_location);
	// 	qo_flt.convert(&qo, visitor_center_location + filename + ".flt");

	// 	//Now, write a shapefile with the point feature and the attribute of the filename.
	// 	sfa::Feature instanceFeature;
	// 	instanceFeature.setAttribute("MODL", filename + ".flt");
	// 	instanceFeature.setAttribute("FACC", "AL015");
	// 	instanceFeature.setAttribute("FSC", 0);
	// 	instanceFeature.setAttribute("A01", 0);
	// 	instanceFeature.geometry = geoOrigin.copy();
	// 	sfa::Feature *new_feature = output_layer_point->addFeature(&instanceFeature);
	// 	delete new_feature;
	// 	output_sfa_file->commitUpdates();

	// 	logger << "\tFinished writing " + filename << logger.endl;
	// 	logger.endl;
	// }

	
// #ifndef WIN32
//     char *gdal_data_var = getenv("GDAL_DATA");
//     if(gdal_data_var==NULL)
//     {        
//         putenv("GDAL_DATA=/usr/local/share/gdal");
//     }
//     char *gdal_plugins_var = getenv("GDAL_DRIVER_PATH");
//     if(gdal_plugins_var==NULL)
//     {        
//         putenv("GDAL_DRIVER_PATH=/usr/local/bin/gdalplugins");
//     }
// #else
//     size_t requiredSize;
//     getenv_s(&requiredSize, NULL, 0, "GDAL_DATA");
//     if (requiredSize == 0)
//     {
//         ccl::FileInfo fi(argv[0]);
//         int bufSize = 1024;
//         char *envBuffer = new char[bufSize];
//         std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");
//         sprintf_s(envBuffer, bufSize, "GDAL_DATA=%s", dataDir.c_str());
//         _putenv(envBuffer);        
//     }
// #endif
//     logger.init("main");
//     logger << ccl::LINFO;
//     GDALAllRegister();
//     ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));


//     return EXIT_SUCCESS;
// }
