#pragma once
#include "rapidjson/writer.h"
#include "rapidjson/filewritestream.h"
#include <string>
#include <scenegraph/Scene.h>
#include "ogr_spatialref.h"
#include <fstream>


struct GeoRect
{
	double north;
	double south;
	double west;
	double east;
	double elev;
};
namespace gltf
{
	class GltfInfo
	{
	public:
		std::string name;
		std::ofstream file;
		std::string format;
		GeoRect& bounds;
		bool gltfBinary;
		sfa::Point rtcCenter;

		OGRSpatialReference wgs;
		OGRSpatialReference ecef;
		OGRCoordinateTransformation* coordTrans;
		cts::FlatEarthProjection flatEarth;

		std::vector<float> maxVertexValues;
		std::vector<float> minVertexValues;
		std::vector<float> maxNormalValues;
		std::vector<float> minNormalValues;
		std::vector<float> maxUvValues;
		std::vector<float> minUvValues;

		GltfInfo(std::string filename, GeoRect& pos);
		~GltfInfo();
		void calcRtcCenter();
		int addPadding(char paddingData, int alignmentRequirement);
		void writeFeatureTable();
		void writeB3dmHeader();
		void finalizeB3dmSizes();
		void writeGltfBinaryHeader();
		void finalizeJsonChunk();
		void beginBinaryChunk();
		void finalizeBinaryChunk();
		void finalizeGltfBinarySizes();
		bool init();
		void createFile();
		void finalizeFile();

	private:
		std::streampos b3dmSizePos;
		std::streampos b3dmStart;
		std::streampos gltfSizePos;
		std::streampos gltfStart;
		std::streampos jsonChunkSizePos;
		std::streampos jsonChunkStart;
		std::streampos binChunkSizePos;
		std::streampos binChunkStart;

		std::streampos featureJsonSizePos;
		std::streampos featureJsonStart;
		std::streampos featureBinSizePos;
		std::streampos featureBinStart;
		std::streampos batchJsonSizePos;
		std::streampos batchJsonStart;
		std::streampos batchBinSizePos;
		std::streampos batchBinStart;

		std::streampos fileEnd;
	};
}