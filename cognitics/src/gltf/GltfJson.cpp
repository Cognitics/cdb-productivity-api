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
		jsonWriter.Key("meshes");
		jsonWriter.StartArray();
		jsonWriter.StartObject();
		jsonWriter.Key("primitives");
		jsonWriter.StartArray();

		for (int p = 0; p < data.primitives.size(); ++p)
		{
			int vertexAccessorIndex = p * 4;
			jsonWriter.StartObject();
			jsonWriter.Key("material");
			jsonWriter.Int(p);
			jsonWriter.Key("attributes");
			jsonWriter.StartObject();
			jsonWriter.Key("POSITION");
			jsonWriter.Int(vertexAccessorIndex);
			jsonWriter.Key("NORMAL");
			jsonWriter.Int(vertexAccessorIndex + 1);
			jsonWriter.Key("TEXCOORD_0");
			jsonWriter.Int(vertexAccessorIndex + 2);
			jsonWriter.Key("_BATCHID");
			jsonWriter.Int(vertexAccessorIndex + 3);
			jsonWriter.EndObject();//attributes
			jsonWriter.EndObject();//primitive
		}

		jsonWriter.EndArray();//primitives
		jsonWriter.EndObject();//mesh
		jsonWriter.EndArray();//meshes
	}

	void GltfJson::writeMaterials(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		float roughnessFactor = 1;
		float metallicFactor = 0;

		jsonWriter.Key("materials");
		jsonWriter.StartArray();
		for (int p = 0; p < data.primitives.size(); ++p)
		{
			jsonWriter.StartObject();
			jsonWriter.Key("alphaMode");
			jsonWriter.String("MASK");
			jsonWriter.Key("doubleSided");
			jsonWriter.Bool(true);
			jsonWriter.Key("pbrMetallicRoughness");
			jsonWriter.StartObject();
			jsonWriter.Key("roughnessFactor");
			jsonWriter.Double(roughnessFactor);
			jsonWriter.Key("metallicFactor");
			jsonWriter.Double(metallicFactor);
			jsonWriter.Key("baseColorTexture");
			jsonWriter.StartObject();
			jsonWriter.Key("index");
			jsonWriter.Int(p);
			jsonWriter.EndObject();//texture
			jsonWriter.EndObject();//metallic/roughness
			jsonWriter.EndObject();//material
		}
		jsonWriter.EndArray();//materials
	}

	void GltfJson::writeTextures(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		jsonWriter.Key("textures");
		jsonWriter.StartArray();
		for (int p = 0; p < data.primitives.size(); ++p)
		{
			jsonWriter.StartObject();
			jsonWriter.Key("source");
			jsonWriter.Int(p);
			jsonWriter.EndObject();//texture
		}
		jsonWriter.EndArray();//textures
	}

	void GltfJson::writeImages(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		jsonWriter.Key("images");
		jsonWriter.StartArray();
		for (int p = 0; p < data.primitives.size(); ++p)
		{
			ccl::FileInfo fi(data.primitives[p].textureName);

			jsonWriter.StartObject();
			if (data.info.embedTextures)
			{
				jsonWriter.Key("bufferView");
				jsonWriter.Int( (p * 5) + 4);
				jsonWriter.Key("mimeType");
				if (fi.getSuffix() == "jpg" || fi.getSuffix() == "jpeg")
				{
					jsonWriter.String("image/jpeg");
				}
				else if (fi.getSuffix() == "png")
				{
					jsonWriter.String("image/png");
				}
				else if (fi.getSuffix() == "tif" || fi.getSuffix() == "tiff")
				{
					jsonWriter.String("image/tiff");
				}
				else
				{
					std::cout << "GltfJson::writeImages - unknown/unsupported texture format; jpeg or png recommended" << std::endl;
					jsonWriter.String("application/unknown");
				}
			}
			else
			{
				jsonWriter.Key("uri");
				jsonWriter.String(fi.getBaseName(false).c_str());
			}
			jsonWriter.EndObject();//image
		}
		jsonWriter.EndArray();//images
	}

	void GltfJson::writeBuffers(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter)
	{
		int totalBufferLength = 0;
		for (int p = 0; p < data.primitives.size(); ++p)
		{
			totalBufferLength += data.primitives[p].vertexBufferLength
				+ data.primitives[p].normalsBufferLength
				+ data.primitives[p].uvBufferLength
				+ data.primitives[p].batchBufferLength;
			if (data.info.embedTextures)
			{
				totalBufferLength += data.primitives[p].textureBufferLength;
			}
		}
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

		int currentOffset = 0;

		jsonWriter.Key("bufferViews");
		jsonWriter.StartArray();
		for (int p = 0; p < data.primitives.size(); ++p)
		{
			writeBufferView(jsonWriter, currentOffset, data.primitives[p].vertexBufferLength, arrayBufferTarget);
			currentOffset += data.primitives[p].vertexBufferLength;
			writeBufferView(jsonWriter, currentOffset, data.primitives[p].normalsBufferLength, arrayBufferTarget);
			currentOffset += data.primitives[p].normalsBufferLength;
			writeBufferView(jsonWriter, currentOffset, data.primitives[p].uvBufferLength, arrayBufferTarget);
			currentOffset += data.primitives[p].uvBufferLength;
			writeBufferView(jsonWriter, currentOffset, data.primitives[p].batchBufferLength, arrayBufferTarget);
			currentOffset += data.primitives[p].batchBufferLength;
			if (data.info.embedTextures)
			{
				writeBufferView(jsonWriter, currentOffset, data.primitives[p].textureBufferLength, arrayBufferTarget);
				currentOffset += data.primitives[p].textureBufferLength;
			}
		}
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
		for (int p = 0; p < data.primitives.size(); ++p)
		{
			int buffersPerPrimitive = 4;
			if (data.info.embedTextures)
			{
				++buffersPerPrimitive;
			}
			int vertexBufferIndex = p * buffersPerPrimitive;
			jsonWriter.StartObject();
			writeAccessor(jsonWriter, vertexBufferIndex, floatComponentType, vec3Type, data.primitives[p].numVerts);
			writeAccessorMinMax(jsonWriter, data.primitives[p].minVertexValues, data.primitives[p].maxVertexValues);
			jsonWriter.EndObject();
			jsonWriter.StartObject();
			writeAccessor(jsonWriter, vertexBufferIndex + 1, floatComponentType, vec3Type, data.primitives[p].numVerts);
			writeAccessorMinMax(jsonWriter, data.primitives[p].minNormalValues, data.primitives[p].maxNormalValues);
			jsonWriter.EndObject();
			jsonWriter.StartObject();
			writeAccessor(jsonWriter, vertexBufferIndex + 2, floatComponentType, vec2Type, data.primitives[p].numVerts);
			writeAccessorMinMax(jsonWriter, data.primitives[p].minUvValues, data.primitives[p].maxUvValues);
			jsonWriter.EndObject();
			jsonWriter.StartObject();
			writeAccessor(jsonWriter, vertexBufferIndex + 3, ushortComponentType, scalarType, data.primitives[p].numVerts);
			writeAccessorMinMax(jsonWriter, zeroesArray, zeroesArray);
			jsonWriter.EndObject();
		}
		jsonWriter.EndArray();
	}

	void GltfJson::writeAccessor(rapidjson::Writer<rapidjson::FileWriteStream>& jsonWriter, int bufferViewIndex, int componentType, std::string type, int numVerts)
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
		jsonWriter.Int(numVerts);
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
