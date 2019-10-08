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
		std::string outputPath;
		std::string relativePathName;
		std::ofstream file;
		std::string format;
		GeoRect& bounds;
		double angle;
		bool gltfBinary;
		bool embedTextures;
		sfa::Point rtcCenter;

		OGRSpatialReference wgs;
		OGRSpatialReference ecef;
		OGRCoordinateTransformation* coordTrans;
		cts::FlatEarthProjection flatEarth;
		
		GltfInfo(std::string filename, GeoRect& pos, double zRotation);
		~GltfInfo();
		void calcRtcCenter();
		int addPadding(char paddingData, int alignmentRequirement);
		void getFeatureJson(std::string & out_json);
		void writeFeatureTable();
		void writeB3dmHeader();
		void writeI3dmHeader();
		void finalizeB3dmSizes();
		void writeGltfBinaryHeader();
		void finalizeJsonChunk();
		void beginBinaryChunk();
		void finalizeBinaryChunk();
		void finalizeGltfBinarySizes();
		bool init();
		void setPath();
		void createFile();
		void finalizeFile();

	private:
		std::streampos tileSizePos;
		std::streampos tileStart;
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