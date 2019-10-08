
#include "scenegraphgltf\scenegraphgltf.h"
#include "gltf\GltfJson.h"
#include "gltf\Tileset.h"

namespace scenegraph
{
	bool buildGltfFromScene(std::string &filename, Scene* scene,
		double north, double south, double east, double west, double minElev, double maxElev, int id, double angle)
	{
		GeoRect tilePos;
		tilePos.east = east;
		tilePos.north = north;
		tilePos.south = south;
		tilePos.west = west;
		tilePos.elev = minElev;

		gltf::GltfInfo info(filename, tilePos, angle);
		gltf::GltfData data(scene, info);
		gltf::GltfJson json(data);

		info.init();
		data.init();

		info.createFile();

		json.write();

		if (info.gltfBinary)
		{
			info.finalizeJsonChunk();
			info.beginBinaryChunk();

			data.write();
		}

		info.finalizeFile();

		filename = info.name;
		
		gltf::TileInfo ti;
		ti.typeId = id;
		ti.relativePathName = info.relativePathName;
		ti.north = north;
		ti.south = south;
		ti.east = east;
		ti.west = west;
		ti.minElev = minElev;
		ti.maxElev = maxElev;
		gltf::Tileset::tiles.push_back(ti);

		return true;
	}

	bool buildTilesetFromScene(const std::string &filename, Scene* scene, double north, double south, double east, double west)
	{
		GeoRect bounds;
		bounds.east = east;
		bounds.north = north;
		bounds.south = south;
		bounds.west = west;
		bounds.elev = 0;

		gltf::Tileset tileset(filename, scene, bounds);
		
		tileset.write();
		
		return true;
	}
}