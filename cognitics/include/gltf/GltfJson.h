#pragma once
#include "gltf/GltfData.h"

namespace gltf
{
	class GltfJson
	{
	private:
		void writeScenes(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeNodes(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeMeshes(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeMaterials(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeTextures(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeImages(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeBuffers(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeBufferViews(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeBufferView(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, int offset, int length, int target);
		void writeAccessors(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);
		void writeAccessor(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, int bufferView, int componentType, std::string type);
		void writeAccessorMinMax(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, std::vector<float>& min, std::vector<float>& max);
		void writeAccessorMinMax(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, std::vector<unsigned short>& min, std::vector<unsigned short>& max);
		void writeAsset(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter);

		GltfData& data;
	public:
		GltfJson(GltfData& dataA) : data(dataA) {	}
		bool write();
	};

}