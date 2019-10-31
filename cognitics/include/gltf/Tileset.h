#pragma once
#include <string>
#include "scenegraph/Scene.h"
#include "scenegraph/ExternalReference.h"
#include "rapidjson/prettywriter.h"
#include <map>

namespace gltf
{
	struct TileInfo
	{
		int typeId;
		std::string relativePathName;
		double north;
		double south;
		double east;
		double west;
		double minElev;
		double maxElev;

		TileInfo() :
			typeId(0), relativePathName(""), north(0), south(0), east(0), west(0), minElev(0), maxElev(0)
		{}
	};

	class Tileset
	{
		std::string name;
		scenegraph::Scene* scene;
		GeoRect& bounds;
		std::map<std::string, TileInfo&> tileLods;
		bool removeTextures;

	public:
		Tileset(std::string filename, scenegraph::Scene* sceneA, GeoRect boundsA) :
			name(filename), scene(sceneA), bounds(boundsA), removeTextures(true)
		{}
		void GetRadianRectFromExtRef(TileInfo& ref, GeoRect& out_rect);
		void writeLeafChild(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, TileInfo& ref, int geometricError);
		void writeLods(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, TileInfo& ref, int geometricError);
		void write();

		static std::vector<TileInfo> tiles;
	};
}