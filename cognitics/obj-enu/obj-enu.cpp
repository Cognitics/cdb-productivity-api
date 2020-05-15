#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include <ccl/FileInfo.h>
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"

#include "ccl/StringUtils.h"
#include "ccl/ArgumentParser.h"
#include <scenegraph/ExtentsVisitor.h>
#include "sfa/BSP.h"
#include "sfa/sfa.h"
#include <sfa_file_factory/sfa_file_factory.h>
#include <ip/jpgwrapper.h>
#include <ip/rgb.h>
#include <cts/CS_CoordinateSystemFactory.h>
#include <sfa/Layer.h>
#include <sfa/File.h>
#include <ogr/ogr.h>
#include "ogr/File.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
//#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#include "obj-enu.h"
#include "quickobj.h"
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
		return false;
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
        return false;
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


int main(int argc, char **argv)
{
	/**
    cognitics::ArgumentParser args;
    
    if(args.Parse(argc,argv)==EXIT_FAILURE)
    {
		return args.Usage("failure");
    }
	*/
	putenv("GDAL_DATA=./gdal-data");

	printf("%s\n", argv[1]);
	logger << argv[0] << logger.endl;
	printf("This microservices assumes that for each separate mesh there exists:\n- an mtl file\n- texture(s)\n- an xyz file\n- an obj file,\n- a prj file\n");

	std::string visitor_center_location = "C:/Users/mkrentz/Development/cdb_mesh/cdb-productivity-api/cognitics/build/data/";
	// C:\Users\mkrentz\Development\cdb_mesh\cdb-productivity-api\cognitics\build\data

	auto fileread = ccl::FileInfo();
	std::string arg_val = argv[1];
	auto add_str = arg_val.find_last_of("/") == arg_val.size() - 1 ? "final/" : "/final/";
	std::string new_dir = arg_val + add_str;
	printf("%s\n", new_dir.c_str());

	auto new_dir_obj = ccl::makeDirectory(new_dir);
	auto all_files = fileread.getAllFiles(argv[1], "*");

	auto& global_mesh_collection = cognitics::QuickObj();

	auto mesh_offsets = sfa::Point();
	auto prj_str = std::string();
	auto obj_str = std::string();
	auto texture_dir = new_dir;
	auto sub_val = 0;

	for (auto file_info : all_files)
	{
		auto name = file_info.getFileName();
		auto base_name = file_info.getBaseName();
		auto extension = file_info.getSuffix();
		if (!extension.compare("jpg") || !extension.compare("png") || !extension.compare("rgb"))
		{
			auto new_dest = texture_dir + base_name;
			ccl::copyFile(name, new_dest);
		}
		if (!extension.compare("xyz"))
		{
			mesh_offsets = readOffsetXYZ(name);
		}
		if (!extension.compare("obj"))
		{
			obj_str = name;
		}
		if (!extension.compare("prj"))
		{
			prj_str = name;
		}
		if (!mesh_offsets.isEmpty() && !prj_str.empty() && !obj_str.empty())
		{
			printf("New Mesh Found --- Combining\n");
			auto to_print = fileToWKT(prj_str);

			ObjSrs mesh_srs;
			mesh_srs.srsWKT = to_print;
			mesh_srs.offsetPt = mesh_offsets;

			auto local_mesh_collection = cognitics::QuickObj(obj_str, mesh_srs, visitor_center_location, true);
			auto vertex = local_mesh_collection.verts[0];

			double origin_x = vertex.x;
			double origin_y = vertex.y;
			if (!mesh_srs.getOrigin(origin_x, origin_y)) {
				return EXIT_FAILURE;
			}

			for (auto& mesh : local_mesh_collection.subMeshes)
			{
				if (global_mesh_collection.materialMap[mesh.materialName].textureFile.compare(""))
				{
					auto material = local_mesh_collection.materialMap[mesh.materialName];
					mesh.materialName = "material_" + std::to_string(global_mesh_collection.subMeshes.size());
					printf("%s", mesh.materialName.c_str());
					global_mesh_collection.materialMap[mesh.materialName] = material;

					auto vert_size = global_mesh_collection.verts.size();
					printf("\n%d\n", vert_size);
					auto uv_size = global_mesh_collection.uvs.size();
					auto norm_size = global_mesh_collection.norms.size();
					

					for (int i = 0, c = mesh.vertIdxs.size(); i < c; i++)
						mesh.vertIdxs[i] = mesh.vertIdxs[i] + vert_size - sub_val;
					for (int i = 0, c = mesh.uvIdxs.size(); i < c; i++)
						mesh.uvIdxs[i] = mesh.uvIdxs[i] + uv_size - sub_val;
					for (int i = 0, c = mesh.normIdxs.size(); i < c; i++)
						mesh.normIdxs[i] = mesh.normIdxs[i] + norm_size - sub_val;

					global_mesh_collection.subMeshes.push_back(mesh);
				}
				else
				{
					auto material = local_mesh_collection.materialMap[mesh.materialName];
					global_mesh_collection.materialMap[mesh.materialName] = material;
					printf("%s", mesh.materialName.c_str());

					auto vert_size = global_mesh_collection.verts.size();
					printf("\n%d\n", vert_size);
					auto uv_size = global_mesh_collection.uvs.size();
					auto norm_size = global_mesh_collection.norms.size();

					for (int i = 1, c = mesh.vertIdxs.size(); i < c; i++)
						mesh.vertIdxs[i] = mesh.vertIdxs[i] + vert_size - sub_val;
					for (int i = 1, c = mesh.uvIdxs.size(); i < c; i++)
						mesh.uvIdxs[i] = mesh.uvIdxs[i] + uv_size - sub_val;
					for (int i = 1, c = mesh.normIdxs.size(); i < c; i++)
						mesh.normIdxs[i] = mesh.normIdxs[i] + norm_size - sub_val;

					global_mesh_collection.subMeshes.push_back(mesh);
				}
			}


			global_mesh_collection.verts.insert(global_mesh_collection.verts.end(), local_mesh_collection.verts.begin(), local_mesh_collection.verts.end());
			global_mesh_collection.uvs.insert(global_mesh_collection.uvs.end(), local_mesh_collection.uvs.begin(), local_mesh_collection.uvs.end());
			global_mesh_collection.norms.insert(global_mesh_collection.norms.end(), local_mesh_collection.norms.begin(), local_mesh_collection.norms.end());
			++sub_val;
			mesh_offsets = sfa::Point();
			prj_str = std::string();
			obj_str = std::string();
		}
	}

	auto output = new_dir + "output";

	global_mesh_collection.exportObj(output);

	return EXIT_SUCCESS;


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


    return EXIT_SUCCESS;
}
