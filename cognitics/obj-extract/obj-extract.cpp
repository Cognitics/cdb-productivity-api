#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
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

#include "obj-extract.h"
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

std::vector<sfa::Geometry*> geometryListFromPolygonShapefile(const std::string &filename)
{
	auto footprint_file = ogr::File::s_open(filename.c_str());
	sfa::LayerList layers = footprint_file->getLayers();
	std::vector<sfa::Geometry*> polygon_list = std::vector<sfa::Geometry*>();
	for (sfa::Layer* layer: layers)
	{
		sfa::Feature *file_feature = layer->getNextFeature();
		while (file_feature != nullptr)
		{
			sfa::Geometry *file_feature_geometry = file_feature->geometry;
			polygon_list.push_back(file_feature_geometry);
			file_feature = layer->getNextFeature();
		}
	}

	return polygon_list;
}

std::vector<sfa::Geometry*> geometryListFromPolygonJson(const std::string &filename)
{
	auto footprint_file = ogr::File::s_open(filename.c_str());
	sfa::LayerList layers = footprint_file->getLayers();
	std::vector<sfa::Geometry *> polygon_list = std::vector<sfa::Geometry *>();
	for (sfa::Layer* layer : layers)
	{
		sfa::Feature *file_feature = layer->getNextFeature(3);
		while (file_feature != nullptr)
		{
			sfa::Geometry *file_feature_geometry = file_feature->geometry;
			polygon_list.push_back(file_feature_geometry);
			file_feature = layer->getNextFeature();
		}
	}
	return polygon_list;
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


std::vector<uint32_t> buildSolutionVertexArray(cognitics::QuickSubMesh &mesh, const std::vector<uint32_t>& resulting_indices)
{
	logger << "--- Building triangle-index association vector..." << logger.endl;
	auto ret_vertex_array = std::vector<uint32_t>();
	for (int i = 0, c = mesh.vertIdxs.size() / 3; i < c; ++i)
	{
		int t_a = mesh.vertIdxs[i * 3];
		int t_b = mesh.vertIdxs[i * 3 + 1];
		int t_c = mesh.vertIdxs[i * 3 + 2];
		bool skip = true;
		if (std::find(resulting_indices.begin(), resulting_indices.end(), t_a) != resulting_indices.end())
			skip = false;
		if (std::find(resulting_indices.begin(), resulting_indices.end(), t_b) != resulting_indices.end())
			skip = false;
		if (std::find(resulting_indices.begin(), resulting_indices.end(), t_c) != resulting_indices.end())
			skip = false;
		if (skip)
			continue;
		ret_vertex_array.push_back(t_a);
		ret_vertex_array.push_back(t_b);
		ret_vertex_array.push_back(t_c);
	}

	logger << "\t- Finished building triangle-index association vector..." << logger.endl;
	return ret_vertex_array;
}

std::vector<cognitics::QuickObj> initializeExportableMeshesVector(std::vector<sfa::Geometry *> &building_polygons,cognitics::QuickObj &mesh_obj)
{
	logger << "--- Initializing exportable mesh vector..." << logger.endl;
	std::vector<cognitics::QuickObj> exportable_meshes = std::vector<cognitics::QuickObj>();
	for (sfa::Geometry *shape : building_polygons)
	{
		cognitics::QuickObj exportable_mesh = cognitics::QuickObj();
		exportable_meshes.push_back(exportable_mesh);
	}
	logger << "\tFinished initializing exportable mesh vector..." << logger.endl;
	return exportable_meshes;
}

void buildVertIndicesToIndexMap(cognitics::QuickObj &mesh_obj, std::map<uint32_t, std::list<int>> &map)
{
	logger << "--- Mapping vert indices to UV indices..." << logger.endl;
	auto size = mesh_obj.vertIdxs.size();
	for (int i = 0; i < size; ++i)
	{

		auto has_key = map.count(mesh_obj.vertIdxs[i]);
		if (has_key == 0)
		{
			map.insert(std::pair<uint32_t, std::list<int>>(mesh_obj.vertIdxs[i], std::list<int>()));
		}
		auto &_direct_reference = map.at(mesh_obj.vertIdxs[i]);
		_direct_reference.push_back(i);
	}
	logger << "\tFinished mapping vert indices to UV indices..." << logger.endl;
}

void flatten_verts(cognitics::QuickObj &mesh_obj, std::vector<uint32_t> &to_flatten, float min_z)
{
	for (uint32_t vert_idx: to_flatten)
		mesh_obj.flattenVert(vert_idx, min_z);
}

std::map<int, int> meshIdxToSubmeshIdx(cognitics::QuickObj &mesh_obj, cognitics::QuickSubMesh &submesh)
{
	std::map<int, int> return_map = std::map<int, int>();
	for (int i = 0; i < submesh.vertIdxs.size(); ++i)
	{
		return_map.insert(std::pair<int, int>(submesh.vertIdxs.at(i), i));
	}
	return return_map;
}

int main(int argc, char **argv)
{
    cognitics::ArgumentParser args;
    
    if(args.Parse(argc,argv)==EXIT_FAILURE)
    {
		return args.Usage("failure");
    }
	putenv("GDAL_DATA=./gdal-data");

	std::string visitor_center_location = "C:\\Users\\mkrentz\\Development\\3DStuffs\\models\\fort_story\\final\\";
	// C:\Users\mkrentz\Development\cdb_mesh\cdb-productivity-api\cognitics\build\data

	auto building_polygons = geometryListFromPolygonJson(visitor_center_location + "Polygons.json");
	auto mesh_offsets = readOffsetXYZ(visitor_center_location + "output.xyz");
	auto prj_str = visitor_center_location + "output.prj";

	auto to_print = fileToWKT(prj_str);
	const auto file_str = visitor_center_location + "output.obj";
	const auto texture_dir = visitor_center_location;

	ObjSrs mesh_srs;
	mesh_srs.srsWKT = to_print;
	mesh_srs.offsetPt = mesh_offsets;

	auto &mesh_obj = cognitics::QuickObj(file_str, mesh_srs, visitor_center_location, true);
	auto vertex = mesh_obj.verts[0];

	double origin_x = vertex.x;
	double origin_y = vertex.y;
	if (!mesh_srs.getOrigin(origin_x, origin_y)) {
		return EXIT_FAILURE;
	}

	logger << "Origin Discovered, ready for in-place transformations..." << logger.endl;
	logger << std::to_string(building_polygons.size()) + " Polygons found" << logger.endl;
	logger << std::to_string(mesh_obj.subMeshes.size()) + " Submeshes to parse" << logger.endl;
	logger << logger.endl;

	
	logger << mesh_obj.materialMap.size() << logger.endl;
	for (auto str_mat_pair : mesh_obj.materialMap)
	{
		auto file_path = str_mat_pair.first;
		auto& obj_mtl = str_mat_pair.second;
		auto img_info = ip::ImageInfo();
		
		auto img_buffer = ccl::binary();
	

		ip::GetJPGImagePixels(obj_mtl.textureFile, img_info, img_buffer);
		
		auto width = img_info.width;
		auto height = img_info.height;

		auto r = new unsigned char[width * height / 3]();
		auto g = new unsigned char[width * height / 3]();
		auto b = new unsigned char[width * height / 3]();

		if (img_info.interleaved)
		{
			for (int pixel_idx = 0, pixel_count = width * height / 3; pixel_idx < pixel_count; ++pixel_idx)
			{
				r[pixel_idx] = img_buffer.at((pixel_idx * 3));
				g[pixel_idx] = img_buffer.at((pixel_idx * 3) + 1);
				b[pixel_idx] = img_buffer.at((pixel_idx * 3) + 2);
			}
		} 
		else
		{
			for (int pixel_idx = 0, pixel_count = width * height; pixel_idx < pixel_count; ++pixel_idx)
			{
				r[pixel_idx] = img_buffer.at(pixel_idx);
				g[pixel_idx] = img_buffer.at(pixel_idx + pixel_count / 3);
				b[pixel_idx] = img_buffer.at(pixel_idx + (pixel_count / 3) * 2);
			}
		}
		auto extension_pos = obj_mtl.textureFile.find_last_of('.');
		std::string output_rgb_path = obj_mtl.textureFile.substr(0, extension_pos) + ".rgb";
		const std::string output_path = output_rgb_path;
		ip::WriteRGB(output_rgb_path, r, g, b, width, height);

		mesh_obj.materialMap[file_path].textureFile = output_rgb_path;
	}

	auto exportable_meshes = initializeExportableMeshesVector(building_polygons, mesh_obj);

	auto &flatten_indices = std::list<uint32_t>();

	logger << "--- Preparing exportable meshes for cutting..." << logger.endl;
	for (int i = 0; i < building_polygons.size(); ++i)
	{
		cognitics::QuickObj &mesh = exportable_meshes.at(i);
		mesh.materialFilename = mesh_obj.materialFilename;
	}
	logger << "\tFinished preparing exportable meshes for cutting..." << logger.endl;


	logger << "--- Performing mesh cutting operations..." << logger.endl;

	auto points = std::vector<sfa::Point>();
	points.reserve(mesh_obj.verts.size());
	for (int i = 0, c = mesh_obj.verts.size(); i < c; ++i)
		points.emplace_back(mesh_obj.verts[i].x, mesh_obj.verts[i].y, mesh_obj.verts[i].z);

	auto index_by_point = std::map<sfa::Geometry* , int>();
	for (int i = 0, c = points.size(); i < c; ++i)
		index_by_point[&points[i]] = i;

	auto bsp = sfa::BSP();
	for (int i = 0, c = points.size(); i < c; ++i)
		bsp.addGeometry(&points[i]);
	bsp.generate(std::map<sfa::Geometry *, sfa::LineString *>());
	
	auto indices_to_flatten_per_building = std::map<int, std::vector<uint32_t>>();
	for (int i = 0, c = building_polygons.size(); i < c; ++i)
		indices_to_flatten_per_building[i] = std::vector<uint32_t>();

	for (cognitics::QuickSubMesh &mesh : mesh_obj.subMeshes)
	{
		logger << logger.endl;
		logger << "\t--- READING NEW SUBMESH ---" << logger.endl;

		// auto collection_to_mesh = meshIdxToSubmeshIdx(mesh_obj, mesh);

		for (int polygon_idx = 0, polygon_count = building_polygons.size(); polygon_idx < polygon_count; ++polygon_idx)
		{
			auto building_polygon = dynamic_cast<sfa::Polygon *>(building_polygons[polygon_idx]);
			if (building_polygon == NULL)
			{
				logger << "We need you to ensure that the json passed contains a polygon." << logger.endl;
				return EXIT_FAILURE;
			}

			auto visitor = sfa::BSPCollectGeometriesInPolygonVisitor();
			visitor.setBoundingPolygon(*building_polygon);
			visitor.visiting(&bsp);
			auto resulting_geometries = visitor.results;
			if (resulting_geometries.empty())
			{
				logger << "\t!-- Nothing to cut within the bounds given." << logger.endl;
				continue;
			}

			auto resulting_index_set = std::set<uint32_t>();
			for (auto resulting_geometry : resulting_geometries)
				resulting_index_set.insert(index_by_point[resulting_geometry]);
			
			logger << resulting_index_set.size() << logger.endl;

			auto new_mesh = cognitics::QuickSubMesh();
			new_mesh.materialName = mesh.materialName;
			

			for (int i = 0, c = mesh.vertIdxs.size() / 3; i < c; ++i)
			{
				int t_a = mesh.vertIdxs[i * 3];
				int t_b = mesh.vertIdxs[i * 3 + 1];
				int t_c = mesh.vertIdxs[i * 3 + 2];
				bool skip = true;
				if(resulting_index_set.find(t_a) != resulting_index_set.end())
					skip = false;
				if (resulting_index_set.find(t_b) != resulting_index_set.end())
						skip = false;
				if (resulting_index_set.find(t_c) != resulting_index_set.end())
					skip = false;
				if (skip)
					continue;
				new_mesh.vertIdxs.push_back(t_a);
				new_mesh.vertIdxs.push_back(t_b);
				new_mesh.vertIdxs.push_back(t_c);
				new_mesh.uvIdxs.push_back(mesh.uvIdxs[i * 3]);
				new_mesh.uvIdxs.push_back(mesh.uvIdxs[i * 3 + 1]);
				new_mesh.uvIdxs.push_back(mesh.uvIdxs[i * 3 + 2]);
			}

			auto &new_mesh_collection = exportable_meshes.at(polygon_idx);
			new_mesh_collection.addSubMesh(new_mesh);

			for (auto resulting_geometry : resulting_geometries)
				indices_to_flatten_per_building[polygon_idx].push_back(index_by_point[resulting_geometry]);
		}

	}
	logger << "Finished mesh cutting operations..." << logger.endl;


	for (auto& exportable_mesh : exportable_meshes)
	{
		auto referenced_indices = std::vector<uint32_t>();
		for (auto& exportable_submesh : exportable_mesh.subMeshes)
			referenced_indices.insert(referenced_indices.end(), exportable_submesh.vertIdxs.begin(), exportable_submesh.vertIdxs.end());
		std::sort(referenced_indices.begin(), referenced_indices.end());
		auto trim_location = std::unique(referenced_indices.begin(), referenced_indices.end());
		referenced_indices.erase(trim_location, referenced_indices.end());

		exportable_mesh.verts.push_back(mesh_obj.verts[0]);
		auto vert_reference_map = std::map<uint32_t, uint32_t>();
		for (int i = 0, c = referenced_indices.size(); i < c; ++i) 
		{
			auto referenced_index = referenced_indices[i];
			exportable_mesh.verts.push_back(mesh_obj.verts[referenced_index]);
			vert_reference_map[referenced_index] = i + 1;
		}

		for (auto& exportable_submesh : exportable_mesh.subMeshes)
		{
			auto old_idxs = exportable_submesh.vertIdxs;
			exportable_submesh.vertIdxs.clear();
			for (int i = 0, c = old_idxs.size(); i < c; ++i)
			{
				auto old_idx = old_idxs[i];
				auto new_idx = vert_reference_map[old_idx];
				exportable_submesh.vertIdxs.push_back(new_idx);
			}
		}
	}

	for (auto& exportable_mesh : exportable_meshes)
	{
		auto referenced_uv_idxs = std::vector<uint32_t>();
		for (auto& exportable_submesh : exportable_mesh.subMeshes)
			referenced_uv_idxs.insert(referenced_uv_idxs.end(), exportable_submesh.uvIdxs.begin(), exportable_submesh.uvIdxs.end());
		std::sort(referenced_uv_idxs.begin(), referenced_uv_idxs.end());
		auto trim_location = std::unique(referenced_uv_idxs.begin(), referenced_uv_idxs.end());
		referenced_uv_idxs.erase(trim_location, referenced_uv_idxs.end());

		exportable_mesh.uvs.push_back(mesh_obj.uvs[0]);
		auto uv_reference_map = std::map<uint32_t, uint32_t>();
		for (int i = 0, c = referenced_uv_idxs.size(); i < c; ++i)
		{
			auto referenced_index = referenced_uv_idxs[i];
			exportable_mesh.uvs.push_back(mesh_obj.uvs[referenced_index]);
			uv_reference_map[referenced_index] = i + 1;
		}

		for (auto& exportable_submesh : exportable_mesh.subMeshes)
		{
			auto old_idxs = exportable_submesh.uvIdxs;
			exportable_submesh.uvIdxs.clear();
			for (int i = 0, c = old_idxs.size(); i < c; ++i)
			{
				auto old_idx = old_idxs[i];
				auto new_idx = uv_reference_map[old_idx];
				exportable_submesh.uvIdxs.push_back(new_idx);
			}
		}
	}

	for (int building_to_flatten_idx = 0, c = exportable_meshes.size(); building_to_flatten_idx < c; ++building_to_flatten_idx)
	{
		float min = 99999.99999;
		auto& vert_idxs_to_flatten = indices_to_flatten_per_building[building_to_flatten_idx];
		
		auto skip_origin = true;
		for (uint32_t vert_idx : vert_idxs_to_flatten)
		{
			if (skip_origin)
			{
				skip_origin = false;
				continue;
			}
			auto& vert = mesh_obj.verts[vert_idx];
			min = vert.z < min ? vert.z : min;
		}

		flatten_verts(mesh_obj, vert_idxs_to_flatten, min);
	}

	auto exportable_mesh_collection_origins = std::vector<sfa::Point>(exportable_meshes.size());
	for (int exportable_mesh_idx = 0, mesh_collection_count = exportable_meshes.size(); exportable_mesh_idx < mesh_collection_count; ++exportable_mesh_idx)
	{
		auto mesh_origin = sfa::Point();
		double max_dist = 99999.0;
		for (auto vert : exportable_meshes[exportable_mesh_idx].verts)
		{
			auto vert_point = sfa::Point(vert.x, vert.y, vert.z);
			auto point_distance = vert_point.distance2D2(sfa::Point(0.0, 0.0, 0.0));
			if (point_distance < max_dist)
			{
				mesh_origin = vert_point;
				max_dist = point_distance;
			}
		}
		exportable_mesh_collection_origins[exportable_mesh_idx] = mesh_origin; // origin can be used as distance
		for (auto& vert : exportable_meshes[exportable_mesh_idx].verts)
		{
			vert.x = vert.x - mesh_origin.X();
			vert.y = vert.y - mesh_origin.Y();
			vert.z = vert.z - mesh_origin.Z();
		}
	}


	logger << "Outputting cut meshes to OBJ files..." << logger.endl;
	auto exportable_mesh_collection_filenames = std::vector<std::string>(exportable_meshes.size());
	for (int i = 0; i < exportable_meshes.size(); ++i)
	{
		std::string filename = "output_" + std::to_string(i);
		logger << "\tWriting " + filename << logger.endl;

		auto& mesh = exportable_meshes.at(i);
		mesh.exportObj(filename); // OBJ

		auto& converter = cognitics::QuickObj2Flt();
		converter.convert(&mesh, filename + ".flt"); // FLT
		exportable_mesh_collection_filenames[i] = filename;

		logger << "\tFinished writing " + filename << logger.endl;
		logger.endl;
	}
	mesh_obj.exportObj("Original_Mesh_Flattened");
	auto& converter = cognitics::QuickObj2Flt();
	converter.convert(&mesh_obj, "Original_Mesh_Flattened.flt"); // FLT
	logger << "Finished outputting meshes to OBJ files..." << logger.endl;

	
	auto etp_convert = Cognitics::CoordinateSystems::EllipsoidTangentPlane(mesh_srs.geoOrigin.X(), mesh_srs.geoOrigin.Y(), mesh_srs.geoOrigin.Z());
	cts::CS_CoordinateSystem *wgs84;
	auto coordinate_system_factory = cts::CS_CoordinateSystemFactory();
	wgs84 = coordinate_system_factory.createFromWKT("WGS84");

	for (int exportable_mesh_idx = 0, mesh_collection_count = exportable_meshes.size(); exportable_mesh_idx < mesh_collection_count; ++exportable_mesh_idx)
	{

		auto base_filename = exportable_mesh_collection_filenames[exportable_mesh_idx];
		auto featureFilePath(base_filename + ".shp");
		auto featureFile = sfa::FileRegistry::instance()->getFile(featureFilePath);
		if (!featureFile->create(featureFilePath))
		{
			logger << "Unable to create " << featureFilePath << ", file may already exist." << logger.endl;
			return EXIT_FAILURE;
		}


		auto pointLayer = featureFile->addLayer("points", sfa::wkbPointZ, wgs84);
		sfa::Feature feature;

		auto origin_point = exportable_mesh_collection_origins[exportable_mesh_idx];
		double lat;
		double lon;
		double alt;
		etp_convert.LocalToGeodetic(origin_point.X(), origin_point.Y(), origin_point.Z(), lat, lon, alt);
		sfa::Point *point_for_shp = new sfa::Point(lat, lon, alt); // this malloced point gets delted by feature
		feature.geometry = point_for_shp; 
		feature.setAttribute("AO1", 0);
		feature.setAttribute("FACC", "AL013");
		feature.setAttribute("FSC", 0);
		feature.setAttribute("MODL", base_filename + ".flt");
		pointLayer->addFeature(&feature);
	}

	std::string base_filename = "Original_Mesh_Flattened";
	auto featureFilePath(base_filename + ".shp");
	auto featureFile = sfa::FileRegistry::instance()->getFile(featureFilePath);
	if (!featureFile->create(featureFilePath))
	{
		logger << "Unable to create " << featureFilePath << ", file may already exist." << logger.endl;
		return EXIT_FAILURE;
	}

	auto pointLayer = featureFile->addLayer("points", sfa::wkbPointZ, wgs84);
	sfa::Feature feature;
	auto origin_point = mesh_srs.geoOrigin;
	sfa::Point *point_for_shp = new sfa::Point(origin_point.X(), origin_point.Y(), origin_point.Z()); // this malloced point gets delted by feature
	feature.geometry = point_for_shp;
	feature.setAttribute("AO1", 0);
	feature.setAttribute("FACC", "AL013");
	feature.setAttribute("FSC", 0);
	feature.setAttribute("MODL", base_filename + ".flt");
	pointLayer->addFeature(&feature);


	logger << "test" << logger.endl;


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
