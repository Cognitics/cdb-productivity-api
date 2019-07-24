#include "gltf\GltfJson.h"

namespace gltf
{

	bool GltfJson::write()
	{
		FILE* gltfFilePointer = fopen(data.info.name.c_str(), "ab");
		char writeBuffer[65536];
		rapidjson::FileWriteStream jsonOutStream(gltfFilePointer, writeBuffer, sizeof(writeBuffer));
		rapidjson::Writer<rapidjson::FileWriteStream> jsonWriter(jsonOutStream);

		jsonWriter.StartObject();

		writeScenes(jsonWriter);
		writeNodes(jsonWriter);
		writeMeshes(jsonWriter);
		writeMaterials(jsonWriter);
		writeTextures(jsonWriter);
		writeImages(jsonWriter);
		writeBuffers(jsonWriter);
		writeBufferViews(jsonWriter);
		writeAccessors(jsonWriter);
		writeAsset(jsonWriter);

		jsonWriter.EndObject();

		fclose(gltfFilePointer);

		return true;
	}

	void GltfJson::writeScenes(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		int nodeIndex = 0;
		jsonWriter.Key("scenes");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("nodes");
		jsonWriter.StartArray();
		jsonWriter.Int(nodeIndex);
		jsonWriter.EndArray();//node
		jsonWriter.EndObject();//scene
		jsonWriter.EndArray();//scenes
	}

	void GltfJson::writeNodes(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		int meshIndex = 0;
		jsonWriter.Key("nodes");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("mesh");
		jsonWriter.Int(meshIndex);
		jsonWriter.Key("matrix");
		jsonWriter.StartArray();

		//cesium applies y-up to z-up transform for 3dtiles
		//need to compensate for that here
		jsonWriter.Int(1.0);
		jsonWriter.Int(0.0);
		jsonWriter.Int(0.0);
		jsonWriter.Int(0.0);

		jsonWriter.Int(0.0);
		jsonWriter.Int(0.0);
		jsonWriter.Int(-1.0);
		jsonWriter.Int(0.0);

		jsonWriter.Int(0.0);
		jsonWriter.Int(1.0);
		jsonWriter.Int(0.0);
		jsonWriter.Int(0.0);

		jsonWriter.Int(0.0);
		jsonWriter.Int(0.0);
		jsonWriter.Int(0.0);
		jsonWriter.Int(1.0);

		jsonWriter.EndArray();//matrix

		jsonWriter.EndObject();//node
		jsonWriter.EndArray();//nodes
	}

	void GltfJson::writeMeshes(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		int materialIndex = 0;
		int vertexAccessorIndex = 0;
		int normalAccessorIndex = 1;
		int uvAccessorIndex = 2;
		int batchIdAccessorIndex = 3;

		jsonWriter.Key("meshes");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("primitives");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("material");
		jsonWriter.Int(materialIndex);
		jsonWriter.Key("attributes");
		jsonWriter.StartObject();
		jsonWriter.Key("POSITION");
		jsonWriter.Int(vertexAccessorIndex);
		jsonWriter.Key("NORMAL");
		jsonWriter.Int(normalAccessorIndex);
		jsonWriter.Key("TEXCOORD_0");
		jsonWriter.Int(uvAccessorIndex);
		jsonWriter.Key("_BATCHID");
		jsonWriter.Int(batchIdAccessorIndex);
		jsonWriter.EndObject();//attributes
		jsonWriter.EndObject();//primitive
		jsonWriter.EndArray();//primitives
		jsonWriter.EndObject();//mesh
		jsonWriter.EndArray();//meshes
	}

	void GltfJson::writeMaterials(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		float roughnessFactor = 1;
		float metallicFactor = 0;
		int textureIndex = 0;
		jsonWriter.Key("materials");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("pbrMetallicRoughness");
		jsonWriter.StartObject();
		jsonWriter.Key("roughnessFactor");
		jsonWriter.Double(roughnessFactor);
		jsonWriter.Key("metallicFactor");
		jsonWriter.Double(metallicFactor);
		jsonWriter.Key("baseColorTexture");
		jsonWriter.StartObject();
		jsonWriter.Key("index");
		jsonWriter.Int(textureIndex);
		jsonWriter.EndObject();//texture
		jsonWriter.EndObject();//metallic/roughness
		jsonWriter.EndObject();//material
		jsonWriter.EndArray();//materials
	}

	void GltfJson::writeTextures(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		int imageIndex = 0;
		jsonWriter.Key("textures");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("source");
		jsonWriter.Int(imageIndex);
		jsonWriter.EndObject();//texture
		jsonWriter.EndArray();//textures
	}

	void GltfJson::writeImages(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		ccl::FileInfo fi(data.info.name);
		std::string imagePath = fi.getBaseName(true) + ".jpg";

		jsonWriter.Key("images");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("uri");
		jsonWriter.String(imagePath.c_str());
		jsonWriter.EndObject();//image
		jsonWriter.EndArray();//images
	}

	void GltfJson::writeBuffers(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		int totalBufferLength = data.vertexBufferLength + data.normalsBufferLength + data.uvBufferLength + data.batchBufferLength;
		jsonWriter.Key("buffers");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		if (data.info.gltfBinary)
		{
			jsonWriter.Key("name");
			jsonWriter.String("buffer");
		}
		jsonWriter.Key("byteLength");
		jsonWriter.Int(totalBufferLength);
		if (!data.info.gltfBinary)
		{
			jsonWriter.Key("uri");
			std::string base64Data;
			data.getBase64BufferData(base64Data);
			std::string dataMimeType = "data:application/octet-stream;base64,";
			base64Data.insert(0, dataMimeType);
			jsonWriter.String(base64Data.c_str());
		}
		jsonWriter.EndObject();//buffer
		jsonWriter.EndArray();//buffers
	}

	void GltfJson::writeBufferViews(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		const int arrayBufferTarget = 34962;
		const int elementArrayBufferTarget = 34963;

		int vertexBufferOffset = 0;
		int normalsBufferOffset = data.vertexBufferLength;
		int uvBufferOffset = data.vertexBufferLength + data.normalsBufferLength;
		int batchBufferOffset = data.vertexBufferLength + data.normalsBufferLength + data.uvBufferLength;

		jsonWriter.Key("bufferViews");
		jsonWriter.StartArray();
		writeBufferView(jsonWriter, vertexBufferOffset, data.vertexBufferLength, arrayBufferTarget);
		writeBufferView(jsonWriter, normalsBufferOffset, data.normalsBufferLength, arrayBufferTarget);
		writeBufferView(jsonWriter, uvBufferOffset, data.uvBufferLength, arrayBufferTarget);
		writeBufferView(jsonWriter, batchBufferOffset, data.batchBufferLength, arrayBufferTarget);
		jsonWriter.EndArray();//bufferViews
	}

	void GltfJson::writeBufferView(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, int offset, int length, int target)
	{
		int bufferIndex = 0;
		jsonWriter.StartObject();
		jsonWriter.Key("buffer");
		jsonWriter.Int(bufferIndex);
		jsonWriter.Key("byteOffset");
		jsonWriter.Int(offset);
		jsonWriter.Key("byteLength");
		jsonWriter.Int(length);
		jsonWriter.Key("target");
		jsonWriter.Int(target);
		jsonWriter.EndObject();
	}

	void GltfJson::writeAccessors(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		const int ushortComponentType = 5123;
		const int floatComponentType = 5126;
		const std::string scalarType("SCALAR");
		const std::string vec2Type("VEC2");
		const std::string vec3Type("VEC3");
		const std::string mat4Type("MAT4");

		std::vector<unsigned short> zeroesArray;
		zeroesArray.push_back(0);

		jsonWriter.Key("accessors");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		writeAccessor(jsonWriter, 0, floatComponentType, vec3Type);
		writeAccessorMinMax(jsonWriter, data.info.minVertexValues, data.info.maxVertexValues);
		jsonWriter.EndObject();
		jsonWriter.StartObject();
		writeAccessor(jsonWriter, 1, floatComponentType, vec3Type);
		writeAccessorMinMax(jsonWriter, data.info.minNormalValues, data.info.maxNormalValues);
		jsonWriter.EndObject();
		jsonWriter.StartObject();
		writeAccessor(jsonWriter, 2, floatComponentType, vec2Type);
		writeAccessorMinMax(jsonWriter, data.info.minUvValues, data.info.maxUvValues);
		jsonWriter.EndObject();
		jsonWriter.StartObject();
		writeAccessor(jsonWriter, 3, ushortComponentType, scalarType);
		writeAccessorMinMax(jsonWriter, zeroesArray, zeroesArray);
		jsonWriter.EndObject();
		jsonWriter.EndArray();
	}

	void GltfJson::writeAccessor(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, int bufferViewIndex, int componentType, std::string type)
	{
		int byteOffset = 0;
		jsonWriter.Key("bufferView");
		jsonWriter.Int(bufferViewIndex);
		jsonWriter.Key("componentType");
		jsonWriter.Int(componentType);
		jsonWriter.Key("type");
		jsonWriter.String(type.c_str());
		jsonWriter.Key("byteOffset");
		jsonWriter.Int(byteOffset);
		jsonWriter.Key("count");
		jsonWriter.Int(data.numVerts);
	}

	void GltfJson::writeAccessorMinMax(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, std::vector<unsigned short>& min, std::vector<unsigned short>& max)
	{
		jsonWriter.Key("min");
		jsonWriter.StartArray();
		for (int i = 0; i < min.size(); ++i)
		{
			jsonWriter.Int(min[i]);
		}
		jsonWriter.EndArray();
		jsonWriter.Key("max");
		jsonWriter.StartArray();
		for (int i = 0; i < max.size(); ++i)
		{
			jsonWriter.Int(max[i]);
		}
		jsonWriter.EndArray();
	}

	void GltfJson::writeAccessorMinMax(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, std::vector<float>& min, std::vector<float>& max)
	{
		jsonWriter.Key("min");
		jsonWriter.StartArray();
		for (int i = 0; i < min.size(); ++i)
		{
			jsonWriter.Double(min[i]);
		}
		jsonWriter.EndArray();
		jsonWriter.Key("max");
		jsonWriter.StartArray();
		for (int i = 0; i < max.size(); ++i)
		{
			jsonWriter.Double(max[i]);
		}
		jsonWriter.EndArray();
	}

	void GltfJson::writeAsset(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		jsonWriter.Key("asset");
		jsonWriter.StartObject();
		jsonWriter.Key("version");
		jsonWriter.String("2.0");
		jsonWriter.EndObject();
	}


}
