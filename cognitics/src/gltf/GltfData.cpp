#include "gltf/GltfData.h"
#include "b64/base64.h"

namespace gltf
{

	GltfData::GltfData(scenegraph::Scene* sceneA, GltfInfo& infoA)
		: scene(sceneA), info(infoA)
	{

	}

	GltfData::~GltfData()
	{
		for (int p = 0; p < primitives.size(); ++p)
		{
			if (primitives[p].vertexBuffer != NULL)
			{
				delete[] primitives[p].vertexBuffer;
			}
			if (primitives[p].normalsBuffer != NULL)
			{
				delete[] primitives[p].normalsBuffer;
			}
			if (primitives[p].uvBuffer != NULL)
			{
				delete[] primitives[p].uvBuffer;
			}
			if (primitives[p].batchBuffer != NULL)
			{
				delete[] primitives[p].batchBuffer;
			}
			if (info.embedTextures && primitives[p].textureBufferLength > 0)
			{
				delete[] primitives[p].textureBuffer;
			}
		}
	}

	void GltfData::init()
	{
		if (scene == NULL || scene->faces.size() == 0)
		{
			return;
		}


		if (info.format == "b3dm" || info.format == "i3dm")
		{
			convertSceneToEcef();
		}

		applyRotation();

		definePrimitives();
		initMinMaxs();
		initPrimitivesBuffers();
		fillBuffers();
		initTextureBuffers();
	}

	void GltfData::multiplyMatrix()
	{
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 1; j++) {
				outputMatrix[i][j] = 0;
				for (int k = 0; k < 4; k++) {
					outputMatrix[i][j] += rotationMatrix[i][k] * inputMatrix[k][j];
				}
			}
		}
	}

	void GltfData::setUpRotationMatrix(float angle, float u, float v, float w)
	{
		float L = (u*u + v * v + w * w);
		angle = angle * M_PI / 180.0; //converting to radian value
		float u2 = u * u;
		float v2 = v * v;
		float w2 = w * w;

		rotationMatrix[0][0] = (u2 + (v2 + w2) * std::cos(angle)) / L;
		rotationMatrix[0][1] = (u * v * (1 - std::cos(angle)) - w * std::sqrt(L) * std::sin(angle)) / L;
		rotationMatrix[0][2] = (u * w * (1 - std::cos(angle)) + v * std::sqrt(L) * std::sin(angle)) / L;
		rotationMatrix[0][3] = 0.0;

		rotationMatrix[1][0] = (u * v * (1 - std::cos(angle)) + w * std::sqrt(L) * std::sin(angle)) / L;
		rotationMatrix[1][1] = (v2 + (u2 + w2) * std::cos(angle)) / L;
		rotationMatrix[1][2] = (v * w * (1 - std::cos(angle)) - u * std::sqrt(L) * std::sin(angle)) / L;
		rotationMatrix[1][3] = 0.0;

		rotationMatrix[2][0] = (u * w * (1 - std::cos(angle)) - v * std::sqrt(L) * std::sin(angle)) / L;
		rotationMatrix[2][1] = (v * w * (1 - std::cos(angle)) + u * std::sqrt(L) * std::sin(angle)) / L;
		rotationMatrix[2][2] = (w2 + (u2 + v2) * std::cos(angle)) / L;
		rotationMatrix[2][3] = 0.0;

		rotationMatrix[3][0] = 0.0;
		rotationMatrix[3][1] = 0.0;
		rotationMatrix[3][2] = 0.0;
		rotationMatrix[3][3] = 1.0;
	}

	void GltfData::applyRotation()
	{
		setUpRotationMatrix(-info.angle, info.rtcCenter.X(), info.rtcCenter.Y(), info.rtcCenter.Z());

		for (int i = 0; i < scene->faces.size(); ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				sfa::Point& p = scene->faces[i].verts[j];

				inputMatrix[0][0] = p.X();
				inputMatrix[1][0] = p.Y();
				inputMatrix[2][0] = p.Z();
				inputMatrix[3][0] = 1.0;

				multiplyMatrix();

				p.setX(outputMatrix[0][0]);
				p.setY(outputMatrix[1][0]);
				p.setZ(outputMatrix[2][0]);

			}
		}
	}

	void GltfData::convertLocalToEcef(sfa::Point& p)
	{
		double x = info.flatEarth.convertLocalToGeoLon(p.X());
		double y = info.flatEarth.convertLocalToGeoLat(p.Y());
		double z = p.Z() + info.bounds.elev;

		//TODO store std::min/max lat/lon for tileset bounding box

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
		if (scene->faces.size() < 200)
		{
			//std::cout << "setting vertex normals" << std::endl;
			scene->setVertexNormals();
			//std::cout << "finished setting vertex normals" << std::endl;
		}

		return true;
	}

	void GltfData::initMinMaxs()
	{
		for (int p = 0; p < primitives.size(); ++p)
		{
			auto firstFace = primitives[p].faces[0];
			auto firstVert = firstFace.verts[0];
			sfa::Point firstUv = firstFace.getUVForVertex(firstVert, 0);

			primitives[p].maxVertexValues.clear();
			primitives[p].minVertexValues.clear();
			primitives[p].maxNormalValues.clear();
			primitives[p].minNormalValues.clear();
			primitives[p].maxUvValues.clear();
			primitives[p].minUvValues.clear();

			primitives[p].maxVertexValues.push_back(static_cast<float>(firstVert.X()));
			primitives[p].maxVertexValues.push_back(static_cast<float>(firstVert.Y()));
			primitives[p].maxVertexValues.push_back(static_cast<float>(firstVert.Z()));
			primitives[p].minVertexValues.push_back(static_cast<float>(firstVert.X()));
			primitives[p].minVertexValues.push_back(static_cast<float>(firstVert.Y()));
			primitives[p].minVertexValues.push_back(static_cast<float>(firstVert.Z()));
			primitives[p].maxNormalValues.push_back(static_cast<float>(firstFace.vertexNormals[0].X()));
			primitives[p].maxNormalValues.push_back(static_cast<float>(firstFace.vertexNormals[0].Y()));
			primitives[p].maxNormalValues.push_back(static_cast<float>(firstFace.vertexNormals[0].Z()));
			primitives[p].minNormalValues.push_back(static_cast<float>(firstFace.vertexNormals[0].X()));
			primitives[p].minNormalValues.push_back(static_cast<float>(firstFace.vertexNormals[0].Y()));
			primitives[p].minNormalValues.push_back(static_cast<float>(firstFace.vertexNormals[0].Z()));
			primitives[p].maxUvValues.push_back(static_cast<float>(firstUv.X()));
			primitives[p].maxUvValues.push_back(static_cast<float>(firstUv.Y()));
			primitives[p].minUvValues.push_back(static_cast<float>(firstUv.X()));
			primitives[p].minUvValues.push_back(static_cast<float>(firstUv.Y()));
		}
	}

	void GltfData::definePrimitives()
	{
		//group faces into primitives by texture
		for (int i = 0; i < scene->faces.size(); ++i)
		{
			std::string texName = scene->faces[i].getMappedTextureN(0).GetTextureName();
			if (texName != "InvalidTextureID")
			{
				bool addedToPrim = false;
				for (int j = 0; j < primitives.size(); ++j)
				{
					if (primitives[j].textureName == texName)
					{
						primitives[j].faces.push_back(scene->faces[i]);
						addedToPrim = true;
						break;
					}
				}
				if (!addedToPrim)
				{
					GltfPrimitive prim;
					prim.textureName = texName;
					prim.faces.push_back(scene->faces[i]);
					primitives.push_back(prim);
				}
			}
		}
	}

	void GltfData::initPrimitivesBuffers()
	{
		for (int p = 0; p < primitives.size(); ++p)
		{
			primitives[p].numVerts = static_cast<int>(primitives[p].faces.size()) * 3;
			primitives[p].vertexBuffer = new float[primitives[p].numVerts * 3];
			primitives[p].normalsBuffer = new float[primitives[p].numVerts * 3];
			primitives[p].uvBuffer = new float[primitives[p].numVerts * 2];
			primitives[p].batchBuffer = new unsigned short[primitives[p].numVerts];
			primitives[p].vertexBufferLength = primitives[p].numVerts * 3 * sizeof(float);
			primitives[p].normalsBufferLength = primitives[p].numVerts * 3 * sizeof(float);
			primitives[p].uvBufferLength = primitives[p].numVerts * 2 * sizeof(float);
			primitives[p].batchBufferLength = primitives[p].numVerts * sizeof(unsigned short);
		}
	}

	bool GltfData::fillBuffers()
	{
		for (int p = 0; p < primitives.size(); ++p)
		{
			float* currentVertexPointer = primitives[p].vertexBuffer;
			float* currentNormalsPointer = primitives[p].normalsBuffer;
			float* currentUvPointer = primitives[p].uvBuffer;
			unsigned short* currentBatchPointer = primitives[p].batchBuffer;
			for (int i = 0; i < primitives[p].faces.size(); ++i)
			{
				for (int j = 0; j < 3; ++j)
				{
					float x = static_cast<float>(primitives[p].faces[i].verts[j].X());
					float y = static_cast<float>(primitives[p].faces[i].verts[j].Y());
					float z = static_cast<float>(primitives[p].faces[i].verts[j].Z());
					*currentVertexPointer++ = x;
					*currentVertexPointer++ = y;
					*currentVertexPointer++ = z;
					primitives[p].maxVertexValues[0] = std::max<float>(primitives[p].maxVertexValues[0], x);
					primitives[p].maxVertexValues[1] = std::max<float>(primitives[p].maxVertexValues[1], y);
					primitives[p].maxVertexValues[2] = std::max<float>(primitives[p].maxVertexValues[2], z);
					primitives[p].minVertexValues[0] = std::min<float>(primitives[p].minVertexValues[0], x);
					primitives[p].minVertexValues[1] = std::min<float>(primitives[p].minVertexValues[1], y);
					primitives[p].minVertexValues[2] = std::min<float>(primitives[p].minVertexValues[2], z);


					x = static_cast<float>(primitives[p].faces[i].vertexNormals[j].X());
					y = static_cast<float>(primitives[p].faces[i].vertexNormals[j].Y());
					z = static_cast<float>(primitives[p].faces[i].vertexNormals[j].Z());
					*currentNormalsPointer++ = x;
					*currentNormalsPointer++ = y;
					*currentNormalsPointer++ = z;
					primitives[p].maxNormalValues[0] = std::max<float>(primitives[p].maxNormalValues[0], x);
					primitives[p].maxNormalValues[1] = std::max<float>(primitives[p].maxNormalValues[1], y);
					primitives[p].maxNormalValues[2] = std::max<float>(primitives[p].maxNormalValues[2], z);
					primitives[p].minNormalValues[0] = std::min<float>(primitives[p].minNormalValues[0], x);
					primitives[p].minNormalValues[1] = std::min<float>(primitives[p].minNormalValues[1], y);
					primitives[p].minNormalValues[2] = std::min<float>(primitives[p].minNormalValues[2], z);

					sfa::Point uv = primitives[p].faces[i].textures[0].uvs[j];
					x = static_cast<float>(uv.X());
					y = static_cast<float>(uv.Y());
					*currentUvPointer++ = x;
					*currentUvPointer++ = y;
					primitives[p].maxUvValues[0] = std::max<float>(primitives[p].maxUvValues[0], x);
					primitives[p].maxUvValues[1] = std::max<float>(primitives[p].maxUvValues[1], y);
					primitives[p].minUvValues[0] = std::min<float>(primitives[p].minUvValues[0], x);
					primitives[p].minUvValues[1] = std::min<float>(primitives[p].minUvValues[1], y);

					*currentBatchPointer++ = 0;
				}
			}
		}
		return true;
	}

	void GltfData::initTextureBuffers()
	{
		if (info.embedTextures)
		{
			for (int p = 0; p < primitives.size(); ++p)
			{
				std::string texName;
				ccl::FileInfo fi(primitives[p].textureName);
				if (fi.getDirName() == "")
				{
					texName = ccl::joinPaths(info.outputPath, primitives[p].textureName);
				}
				else
				{
					texName = primitives[p].textureName;
				}
				std::ifstream file(texName, std::ios::binary | std::ios::ate);
				std::streamsize size = file.tellg();
				if (size <= 0)
				{
					std::cout << "GltfData: Couldn't find texture file " << primitives[p].textureName << std::endl;
					continue;
				}
				file.seekg(0, std::ios::beg);
				
				primitives[p].textureBufferLength = size;
				primitives[p].textureBuffer = new char[size];
				if (!file.read(primitives[p].textureBuffer, size))
				{
					std::cout << "GltfData: Error reading texture file " << primitives[p].textureName << std::endl;
				}
			}
		}
	}

	void GltfData::write()
	{
		for (int p = 0; p < primitives.size(); ++p)
		{
			info.file.write(reinterpret_cast<char*>(primitives[p].vertexBuffer), primitives[p].vertexBufferLength);
			info.file.write(reinterpret_cast<char*>(primitives[p].normalsBuffer), primitives[p].normalsBufferLength);
			info.file.write(reinterpret_cast<char*>(primitives[p].uvBuffer), primitives[p].uvBufferLength);
			info.file.write(reinterpret_cast<char*>(primitives[p].batchBuffer), primitives[p].batchBufferLength);
			if (info.embedTextures)
			{
				info.file.write(reinterpret_cast<char*>(primitives[p].textureBuffer), primitives[p].textureBufferLength);
			}
		}
	}

	void GltfData::getBase64BufferData(std::string& out_data)
	{
		out_data = "";
		for (int p = 0; p < primitives.size(); ++p)
		{
			std::ostringstream ss1(std::ios::binary);
			ss1.write(reinterpret_cast<char*>(primitives[p].vertexBuffer), primitives[p].vertexBufferLength);
			out_data += base64Encode(ss1.str());

			std::ostringstream ss2(std::ios::binary);
			ss2.write(reinterpret_cast<char*>(primitives[p].normalsBuffer), primitives[p].vertexBufferLength);
			out_data += base64Encode(ss2.str());

			std::ostringstream ss3(std::ios::binary);
			ss3.write(reinterpret_cast<char*>(primitives[p].uvBuffer), primitives[p].uvBufferLength);
			out_data += base64Encode(ss3.str());

			std::ostringstream ss4(std::ios::binary);
			ss4.write(reinterpret_cast<char*>(primitives[p].batchBuffer), primitives[p].batchBufferLength);
			out_data += base64Encode(ss4.str());

			if (info.embedTextures)
			{
				std::ostringstream ss5(std::ios::binary);
				ss5.write(reinterpret_cast<char*>(primitives[p].textureBuffer), primitives[p].textureBufferLength);
				out_data += base64Encode(ss5.str());
			}
		}
	}
}