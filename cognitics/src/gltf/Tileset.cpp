#include "gltf/GltfInfo.h"
#include "gltf/Tileset.h"
#include "rapidjson/prettywriter.h"

namespace gltf
{

	void Tileset::GetRadianRectFromExtRef(scenegraph::ExternalReference & ref, GeoRect & out_rect)
	{
		double pi = 3.14159265358979323846264338327950288;

		out_rect.west = ref.position.X();
		out_rect.south = ref.position.Y();
		out_rect.east = ref.position.X() + ref.scale.X();
		out_rect.north = ref.position.Y() + ref.scale.Y();
		out_rect.west = out_rect.west   * pi / 180;
		out_rect.south = out_rect.south * pi / 180;
		out_rect.east = out_rect.east 	* pi / 180;
		out_rect.north = out_rect.north	* pi / 180;
	}

	void Tileset::write()
	{

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
		writer.Int(240);

		writer.Key("root");
		writer.StartObject();
		writer.Key("boundingVolume");
		writer.StartObject();
		writer.Key("region");
		writer.StartArray();
		GeoRect bound1;
		GeoRect bound2;
		GetRadianRectFromExtRef(scene->externalReferences[0], bound1);
		GetRadianRectFromExtRef(scene->externalReferences[scene->externalReferences.size() - 1], bound2);
		writer.Double(bound1.west);
		writer.Double(bound1.south);
		writer.Double(bound2.east);
		writer.Double(bound2.north);
		writer.Double(scene->externalReferences[0].position.Z());
		writer.Double(scene->externalReferences[0].position.Z() - scene->externalReferences[0].scale.Z());
		writer.EndArray();//region
		writer.EndObject();//boudingVolume

		writer.Key("geometricError");
		writer.Int(70);
		writer.Key("refine");
		writer.String("ADD");
		writer.Key("children");
		writer.StartArray();

		for (int i = 0; i < scene->externalReferences.size(); ++i)
		{
			writer.StartObject();

			writer.Key("boundingVolume");
			writer.StartObject();
			writer.Key("region");
			writer.StartArray();
			GeoRect bounds;
			GetRadianRectFromExtRef(scene->externalReferences[i], bounds);
			writer.Double(bounds.west);
			writer.Double(bounds.south);
			writer.Double(bounds.east);
			writer.Double(bounds.north);
			writer.Double(scene->externalReferences[i].position.Z());
			writer.Double(scene->externalReferences[i].position.Z() - scene->externalReferences[i].scale.Z());
			writer.EndArray();//region
			writer.EndObject();//boudingVolume

			writer.Key("geometricError");
			writer.Int(0);

			writer.Key("content");
			writer.StartObject();
			writer.Key("uri");
			ccl::FileInfo fi(scene->externalReferences[i].filename);
			writer.String(fi.getBaseName().c_str());
			writer.EndObject();//content

			writer.EndObject();//child
		}

		writer.EndArray();//children

		writer.EndObject();//root

		writer.EndObject();//file

		fclose(fp);
	}
}