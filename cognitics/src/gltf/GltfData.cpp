#include <algorithm>
#include "gltf/GltfData.h"
#include "b64/base64.h"

namespace gltf
{

	GltfData::GltfData(scenegraph::Scene* sceneA, GltfInfo& infoA)
		: scene(sceneA), info(infoA)
	{
		numVerts = static_cast<int>(scene->faces.size()) * 3;
		vertexBuffer = new float[numVerts * 3];
		normalsBuffer = new float[numVerts * 3];
		uvBuffer = new float[numVerts * 2];
		batchBuffer = new unsigned short[numVerts];
		vertexBufferLength = numVerts * 3 * sizeof(float);
		normalsBufferLength = numVerts * 3 * sizeof(float);
		uvBufferLength = numVerts * 2 * sizeof(float);
		batchBufferLength = numVerts * sizeof(unsigned short);
	}

	GltfData::~GltfData()
	{
		delete[] vertexBuffer;
		delete[] normalsBuffer;
		delete[] uvBuffer;
		delete[] batchBuffer;
	}

	void GltfData::init()
	{
		if (scene == NULL || scene->faces.size() == 0)
		{
			return;
		}
		convertSceneToEcef();
		initMinMaxs();
		fillBuffers();
	}

	void GltfData::convertLocalToEcef(sfa::Point& p)
	{
		double x = info.flatEarth.convertLocalToGeoLon(p.X());
		double y = info.flatEarth.convertLocalToGeoLat(p.Y());
		double z = p.Z();

		int reprojected = info.coordTrans->Transform(1, &x, &y, &z);
		if (reprojected)
		{
			if (info.format == "b3dm")
			{
				x -= info.rtcCenter.X();
				y -= info.rtcCenter.Y();
				z -= info.rtcCenter.Z();
			}
			p.setX(x);
			p.setY(y);
			p.setZ(z);
		}
		else
		{
			std::cout << "error transforming" << std::endl;

		}
	}
	bool GltfData::convertSceneToEcef()
	{
		if (scene == NULL || scene->faces.size() == 0)
		{
			return false;
		}

		for (int i = 0; i < scene->faces.size(); ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				convertLocalToEcef(scene->faces[i].verts[j]);
			}
		}

		//TODO should update the vertex normals after transforming, 
		//but this seems to sometimes stall indefinitely
		//std::cout << "setting vertex normals" << std::endl;
		//scene->setVertexNormals();
		//std::cout << "finished setting vertex normals" << std::endl;

		return true;
	}

	void GltfData::initMinMaxs()
	{
		info.maxVertexValues.clear();
		info.maxVertexValues.push_back(static_cast<float>(scene->faces[0].verts[0].X()));
		info.maxVertexValues.push_back(static_cast<float>(scene->faces[0].verts[0].Y()));
		info.maxVertexValues.push_back(static_cast<float>(scene->faces[0].verts[0].Z()));
		info.minVertexValues.clear();
		info.minVertexValues.push_back(static_cast<float>(scene->faces[0].verts[0].X()));
		info.minVertexValues.push_back(static_cast<float>(scene->faces[0].verts[0].Y()));
		info.minVertexValues.push_back(static_cast<float>(scene->faces[0].verts[0].Z()));

		info.maxNormalValues.clear();
		info.maxNormalValues.push_back(static_cast<float>(scene->faces[0].vertexNormals[0].X()));
		info.maxNormalValues.push_back(static_cast<float>(scene->faces[0].vertexNormals[0].Y()));
		info.maxNormalValues.push_back(static_cast<float>(scene->faces[0].vertexNormals[0].Z()));
		info.minNormalValues.clear();
		info.minNormalValues.push_back(static_cast<float>(scene->faces[0].vertexNormals[0].X()));
		info.minNormalValues.push_back(static_cast<float>(scene->faces[0].vertexNormals[0].Y()));
		info.minNormalValues.push_back(static_cast<float>(scene->faces[0].vertexNormals[0].Z()));

		sfa::Point uvPoint = scene->faces[0].getUVForVertex(scene->faces[0].verts[0], 0);
		info.maxUvValues.clear();
		info.maxUvValues.push_back(static_cast<float>(uvPoint.X()));
		info.maxUvValues.push_back(static_cast<float>(uvPoint.Y()));
		info.minUvValues.clear();
		info.minUvValues.push_back(static_cast<float>(uvPoint.X()));
		info.minUvValues.push_back(static_cast<float>(uvPoint.Y()));
	}

	bool GltfData::fillBuffers()
	{
		float* currentVertexPointer = vertexBuffer;
		float* currentNormalsPointer = normalsBuffer;
		float* currentUvPointer = uvBuffer;
		unsigned short* currentBatchPointer = batchBuffer;
		bool showOrigin = false;
		for (int i = 0; i < scene->faces.size(); ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				float x = static_cast<float>(scene->faces[i].verts[j].X());
				float y = static_cast<float>(scene->faces[i].verts[j].Y());
				float z = static_cast<float>(scene->faces[i].verts[j].Z());
				if (i == 0 && showOrigin)
				{
					if (j == 0) { x = 0; }
					if (j == 1) { y = 0; }
					if (j == 2) { z = 0; }
				}
				*currentVertexPointer++ = x;
				*currentVertexPointer++ = y;
				*currentVertexPointer++ = z;
				info.maxVertexValues[0] = std::max<double>(info.maxVertexValues[0], x);
				info.maxVertexValues[1] = std::max<double>(info.maxVertexValues[1], y);
				info.maxVertexValues[2] = std::max<double>(info.maxVertexValues[2], z);
				info.minVertexValues[0] = std::min<double>(info.minVertexValues[0], x);
				info.minVertexValues[1] = std::min<double>(info.minVertexValues[1], y);
				info.minVertexValues[2] = std::min<double>(info.minVertexValues[2], z);


				x = static_cast<float>(scene->faces[i].vertexNormals[j].X());
				y = static_cast<float>(scene->faces[i].vertexNormals[j].Y());
				z = static_cast<float>(scene->faces[i].vertexNormals[j].Z());
				*currentNormalsPointer++ = x;
				*currentNormalsPointer++ = y;
				*currentNormalsPointer++ = z;
				info.maxNormalValues[0] = std::max<double>(info.maxNormalValues[0], x);
				info.maxNormalValues[1] = std::max<double>(info.maxNormalValues[1], y);
				info.maxNormalValues[2] = std::max<double>(info.maxNormalValues[2], z);
				info.minNormalValues[0] = std::min<double>(info.minNormalValues[0], x);
				info.minNormalValues[1] = std::min<double>(info.minNormalValues[1], y);
				info.minNormalValues[2] = std::min<double>(info.minNormalValues[2], z);

				sfa::Point uv = scene->faces[i].getUVForVertex(scene->faces[i].verts[j], 0);
				x = static_cast<float>(uv.X());
				y = static_cast<float>(uv.Y());
				*currentUvPointer++ = x;
				*currentUvPointer++ = y;
				info.maxUvValues[0] = std::max<double>(info.maxUvValues[0], x);
				info.maxUvValues[1] = std::max<double>(info.maxUvValues[1], y);
				info.minUvValues[0] = std::min<double>(info.minUvValues[0], x);
				info.minUvValues[1] = std::min<double>(info.minUvValues[1], y);

				*currentBatchPointer++ = 0;
			}
		}
		return true;
	}
	void GltfData::write()
	{
		info.file.write(reinterpret_cast<char*>(vertexBuffer), vertexBufferLength);
		info.file.write(reinterpret_cast<char*>(normalsBuffer), normalsBufferLength);
		info.file.write(reinterpret_cast<char*>(uvBuffer), uvBufferLength);
		info.file.write(reinterpret_cast<char*>(batchBuffer), batchBufferLength);
	}


	void GltfData::getBase64BufferData(std::string& out_data)
	{
		std::ostringstream ss1(std::ios::binary);
		ss1.write(reinterpret_cast<char*>(vertexBuffer), vertexBufferLength);
		out_data = base64Encode(ss1.str());

		std::ostringstream ss2(std::ios::binary);
		ss2.write(reinterpret_cast<char*>(normalsBuffer), vertexBufferLength);
		out_data += base64Encode(ss2.str());

		std::ostringstream ss3(std::ios::binary);
		ss3.write(reinterpret_cast<char*>(uvBuffer), uvBufferLength);
		out_data += base64Encode(ss3.str());

		std::ostringstream ss4(std::ios::binary);
		ss4.write(reinterpret_cast<char*>(batchBuffer), batchBufferLength);
		out_data += base64Encode(ss4.str());
	}
}