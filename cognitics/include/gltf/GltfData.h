#pragma once
#include "gltf/GltfInfo.h"
#include <vector>

namespace gltf
{

	struct GltfPrimitive
	{
		int numVerts;
		float* vertexBuffer;
		float* normalsBuffer;
		float* uvBuffer;
		unsigned short* batchBuffer;
		char* textureBuffer;
		int vertexBufferLength;
		int normalsBufferLength;
		int uvBufferLength;
		int batchBufferLength;
		int textureBufferLength;

		std::vector<float> maxVertexValues;
		std::vector<float> minVertexValues;
		std::vector<float> maxNormalValues;
		std::vector<float> minNormalValues;
		std::vector<float> maxUvValues;
		std::vector<float> minUvValues;

		std::vector<scenegraph::Face> faces;
		std::string textureName;

		GltfPrimitive() :
			numVerts(-1), vertexBuffer(NULL), normalsBuffer(NULL), 
			uvBuffer(NULL), batchBuffer(NULL), textureBuffer(NULL),
			vertexBufferLength(-1), normalsBufferLength(-1), uvBufferLength(-1), 
			batchBufferLength(-1), textureBufferLength(-1)
		{}
	};


	class GltfData
	{
	public:
		scenegraph::Scene* scene;
				
		GltfInfo& info;
		std::vector<GltfPrimitive> primitives;
		
		GltfData(scenegraph::Scene* sceneA, GltfInfo& infoA);
		~GltfData();

		void init();
		void applyRotation();
		void convertLocalToEcef(sfa::Point& p);
		bool convertSceneToEcef();
		void initMinMaxs();
		void definePrimitives();
		void initPrimitivesBuffers();
		bool fillBuffers();
		void initTextureBuffers();
		void write();
		void getBase64BufferData(std::string& out_data);

	private:
		void setUpRotationMatrix(float angle, float u, float v, float w);
		void multiplyMatrix();

		float rotationMatrix[4][4];
		float inputMatrix[4][1];
		float outputMatrix[4][1];
	};

}