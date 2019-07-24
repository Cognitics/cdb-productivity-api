
#include "scenegraphgltf\scenegraphgltf.h"
#include "gltf\GltfJson.h"
#include "gltf\Tileset.h"

namespace scenegraph
{
	bool buildGltfFromScene(const std::string &filename, Scene* scene, double north, double south, double east, double west)
	{
		GeoRect tilePos;
		tilePos.east = east;
		tilePos.north = north;
		tilePos.south = south;
		tilePos.west = west;
		tilePos.elev = 0;

		gltf::GltfInfo info(filename, tilePos);
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

		return true;
	}

	bool buildTilesetFromScene(const std::string &filename, Scene* scene)
	{
		gltf::Tileset tiles(filename, scene);
		tiles.write();
		
		return true;
	}
}