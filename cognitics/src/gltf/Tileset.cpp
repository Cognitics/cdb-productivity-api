#include "gltf\GltfInfo.h"
#include "gltf\Tileset.h"
#include "ccl\FileInfo.h"

namespace gltf
{
	std::vector<TileInfo> Tileset::tiles;

	void Tileset::GetRadianRectFromExtRef(TileInfo& ref, GeoRect & out_rect)
	{
		double pi = 3.14159265358979323846264338327950288;

		out_rect.west = ref.west   * pi / 180;
		out_rect.south = ref.south * pi / 180;
		out_rect.east = ref.east 	* pi / 180;
		out_rect.north = ref.north	* pi / 180;
	}

	void Tileset::writeLeafChild(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, TileInfo& ref, int geometricError)
	{

		writer.StartObject();

		writer.Key("boundingVolume");
		writer.StartObject();
		writer.Key("region");
		writer.StartArray();
		GeoRect bounds;
		GetRadianRectFromExtRef(ref, bounds);
		writer.Double(bounds.west);
		writer.Double(bounds.south);
		writer.Double(bounds.east);
		writer.Double(bounds.north);
		writer.Double(ref.minElev);
		writer.Double(ref.maxElev);
		writer.EndArray();//region
		writer.EndObject();//boudingVolume

		writer.Key("geometricError");
		writer.Int(geometricError);

		writer.Key("content");
		writer.StartObject();
		writer.Key("uri");
		writer.String(ref.relativePathName.c_str());
		writer.EndObject();//content

		writer.EndObject();//child
	}

	void Tileset::writeLods(rapidjson::PrettyWriter<rapidjson::FileWriteStream>& writer, 
		TileInfo& ref, int geometricError)
	{

		writer.StartObject();

		writer.Key("boundingVolume");
		writer.StartObject();
		writer.Key("region");
		writer.StartArray();
		GeoRect bounds;
		GetRadianRectFromExtRef(ref, bounds);
		writer.Double(bounds.west);
		writer.Double(bounds.south);
		writer.Double(bounds.east);
		writer.Double(bounds.north);
		writer.Double(ref.minElev);
		writer.Double(ref.maxElev);
		writer.EndArray();//region
		writer.EndObject();//boudingVolume

		writer.Key("geometricError");
		writer.Int(geometricError);
		writer.Key("refine");
		writer.String("REPLACE");

		writer.Key("content");
		writer.StartObject();
		writer.Key("uri");
		writer.String(ref.relativePathName.c_str());

		writer.EndObject();//content

		bool childrenCreated = false;

		for (int i = 0; i < 4; ++i)
		{
			ccl::FileInfo fi(ref.relativePathName);
			std::string quadkey = fi.getBaseName(true);
			auto child = tileLods.find(quadkey + std::to_string(i));
			if (child != tileLods.end())
			{
				if (!childrenCreated)
				{
					writer.Key("children");
					writer.StartArray();
					childrenCreated = true;
				}
				writeLods(writer, child->second, geometricError / 2);
			}

		}

		if (childrenCreated)
		{
			writer.EndArray();//children
		}


		writer.EndObject();//child
	}

	void Tileset::write()
	{
		double pi = 3.14159265358979323846264338327950288;
		int baseError = 65536;

		FILE* fp = fopen(name.c_str(), "wb");
		char writeBuffer[65536];
		rapidjson::FileWriteStream os(fp, writeBuffer, sizeof(writeBuffer));
		rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(os);

		writer.StartObject();

		writer.Key("asset");
		writer.StartObject();
		writer.Key("version");
		writer.String("1.0");
		writer.Key("tileset");
		writer.String("1.2.3");
		writer.EndObject();

		writer.Key("extras");
		writer.StartObject();
		writer.Key("name");
		writer.String("Sample Tileset");
		writer.EndObject();

		writer.Key("geometricError");
		writer.Int(baseError);
		

		writer.Key("root");
		writer.StartObject();
		writer.Key("boundingVolume");
		writer.StartObject();
		writer.Key("region");
		writer.StartArray();
		writer.Double(bounds.west * pi / 180);
		writer.Double(bounds.south * pi / 180);
		writer.Double(bounds.east * pi / 180);
		writer.Double(bounds.north * pi / 180);
		writer.Double(tiles[0].minElev);
		writer.Double(tiles[0].maxElev);
		writer.EndArray();//region
		writer.EndObject();//boudingVolume

		writer.Key("geometricError");
		writer.Int(baseError);
		writer.Key("refine");
		writer.String("ADD");
		writer.Key("children");
		writer.StartArray();

		int maxLod = 0;
		for (int i = 0; i < tiles.size(); ++i)
		{
			//create searchable map of lods/quadkeys
			if (tiles[i].typeId == 1)
			{
				ccl::FileInfo fi(tiles[i].relativePathName);
				std::string quadkey = fi.getBaseName(true);
				tileLods.insert(std::pair<std::string, 
					TileInfo&>(quadkey, tiles[i]));
				maxLod = max(maxLod, quadkey.size());
			}
			else
			{
				writeLeafChild(writer, tiles[i], 0);
			}
		}

		auto root = tileLods.find("0");
		if (root != tileLods.end())
		{
			writeLods(writer, root->second, baseError / 2);
		}
		else
		{
			std::cout << "Tileset: couldn't find terrain lods" << std::endl;
		}



		writer.EndArray();//children

		writer.EndObject();//root

		writer.EndObject();//file

		fclose(fp);
	}
}