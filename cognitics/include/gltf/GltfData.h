#pragma once
#include "gltf/GltfInfo.h"

namespace gltf
{
	class GltfData
	{
	public:
		scenegraph::Scene* scene;
		int numVerts;
		float* vertexBuffer;
		float* normalsBuffer;
		float* uvBuffer;
		unsigned short* batchBuffer;
		int vertexBufferLength;
		int normalsBufferLength;
		int uvBufferLength;
		int batchBufferLength;
		GltfInfo& info;


		GltfData(scenegraph::Scene* sceneA, GltfInfo& infoA);
		~GltfData();

		void init();
		void convertLocalToEcef(sfa::Point& p);
		bool convertSceneToEcef();
		void initMinMaxs();
		bool fillBuffers();
		int getTotalBufferLength()
		{
			return vertexBufferLength + normalsBufferLength + uvBufferLength + batchBufferLength;
		}
		void write();
		void getBase64BufferData(std::string& out_data);
	};
}