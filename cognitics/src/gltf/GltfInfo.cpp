#include "gltf\GltfInfo.h"

namespace gltf
{

	GltfInfo::GltfInfo(std::string filename, GeoRect& pos) :
		name(filename), bounds(pos), gltfBinary(true)
	{
	}

	GltfInfo::~GltfInfo()
	{
		if (coordTrans != NULL)
		{
			delete coordTrans;
		}
	}

	void GltfInfo::calcRtcCenter()
	{
		double x = (bounds.west + bounds.east) / 2;
		double y = (bounds.north + bounds.south) / 2;
		double z = bounds.elev;

		int reprojected = coordTrans->Transform(1, &x, &y, &z);
		if (reprojected)
		{
			rtcCenter.setX(x);
			rtcCenter.setY(y);
			rtcCenter.setZ(z);
		}
	}

	int GltfInfo::addPadding(char paddingData, int alignmentRequirement)
	{
		int padding = 0;
		std::streampos pos = file.tellp();
		while (pos % alignmentRequirement != 0)
		{
			file << paddingData;
			pos = file.tellp();
			padding++;
		}
		return padding;
	}

	void GltfInfo::writeFeatureTable()
	{
		std::stringstream ss;
		ss.precision(5);
		ss << std::fixed;
		ss << "{\"BATCH_LENGTH\":1,\"RTC_CENTER\":[";
		ss << rtcCenter.X();
		ss << ",";
		ss << rtcCenter.Y();
		ss << ",";
		ss << rtcCenter.Z();
		ss << "]}";
		std::string featureJson = ss.str();

		file << featureJson;
		int featurePadding = addPadding(' ', 8);

		std::streampos currentPos = file.tellp();

		unsigned int featureJsonSize = static_cast<unsigned int>(featureJson.size()) + featurePadding;
		file.seekp(featureJsonSizePos);
		file.write(reinterpret_cast<char*>(&featureJsonSize), 4);

		file.seekp(currentPos);
	}

	void GltfInfo::writeB3dmHeader()
	{
		b3dmStart = file.tellp();
		file << "b3dm";
		unsigned int version = 1;
		file.write(reinterpret_cast<char*>(&version), 4);

		//placeholder that will be overwritten with total file size 
		b3dmSizePos = file.tellp();
		file << "plhd";

		unsigned int zero = 0;
		featureJsonSizePos = file.tellp();
		file << "plhd";
		featureBinSizePos = file.tellp();
		file.write(reinterpret_cast<char*>(&zero), 4);
		batchJsonSizePos = file.tellp();
		file.write(reinterpret_cast<char*>(&zero), 4);
		batchBinSizePos = file.tellp();
		file.write(reinterpret_cast<char*>(&zero), 4);

		writeFeatureTable();
	}

	void GltfInfo::finalizeB3dmSizes()
	{
		std::streampos currentPos = file.tellp();

		unsigned int b3dmSize = fileEnd - b3dmStart;
		file.seekp(b3dmSizePos);
		file.write(reinterpret_cast<char*>(&b3dmSize), 4);

		file.seekp(currentPos);
	}

	void GltfInfo::writeGltfBinaryHeader()
	{
		gltfStart = file.tellp();
		file << "glTF";
		unsigned int version = 2;
		file.write(reinterpret_cast<char*>(&version), 4);

		//placeholder that will be overwritten with total file size 
		gltfSizePos = file.tellp();
		file << "plhd";

		//chunk header
		//placeholder for chunk size
		jsonChunkSizePos = file.tellp();
		file << "plhd";
		file << "JSON";
		jsonChunkStart = file.tellp();
	}

	void GltfInfo::finalizeJsonChunk()
	{
		file.seekp(0, std::ios_base::end);
		addPadding(' ', 4);

		std::streampos currentPos = file.tellp();

		//write chunk size
		unsigned int chunkSize = currentPos - jsonChunkStart;
		file.seekp(jsonChunkSizePos);
		file.write(reinterpret_cast<char*>(&chunkSize), 4);

		file.seekp(currentPos);
	}

	void GltfInfo::beginBinaryChunk()
	{
		//placeholder for chunk size
		binChunkSizePos = file.tellp();
		file << "plhd";
		file << "BIN";
		const char zero = 0;
		file.write(&zero, 1);
		binChunkStart = file.tellp();
	}

	void GltfInfo::finalizeBinaryChunk()
	{
		addPadding('\0', 4);

		std::streampos currentPos = file.tellp();

		//write chunk size
		unsigned int chunkSize = currentPos - binChunkStart;
		file.seekp(binChunkSizePos);
		file.write(reinterpret_cast<char*>(&chunkSize), 4);

		file.seekp(currentPos);

	}

	void GltfInfo::finalizeGltfBinarySizes()
	{
		std::streampos currentPos = file.tellp();

		unsigned int gltfSize = fileEnd - gltfStart;
		file.seekp(gltfSizePos);
		file.write(reinterpret_cast<char*>(&gltfSize), 4);

		file.seekp(currentPos);
	}

	bool GltfInfo::init()
	{
		wgs.SetFromUserInput("WGS84");
		ecef.SetFromUserInput("EPSG:4978");
		coordTrans = OGRCreateCoordinateTransformation(&wgs, &ecef);
		if (coordTrans == NULL)
		{
			std::cout << "error creating transform" << std::endl;
			return false;
		}
		flatEarth.setOrigin((bounds.north + bounds.south) / 2, (bounds.east + bounds.west) / 2);

		ccl::FileInfo fi(name);
		format = fi.getSuffix();
		if (format == "glb" || format == "b3dm" || format == "tileset")
		{
			gltfBinary = true;
		}
		else
		{
			gltfBinary = false;
		}


		calcRtcCenter();

		return true;
	}

	void GltfInfo::createFile()
	{

		file.open(name.c_str(), std::ofstream::binary | std::ofstream::out | std::ofstream::trunc);
		if (!file.good())
		{
			std::cout << "Build GLTF: Error opening file" << std::endl;
		}

		if (format == "b3dm")
		{
			writeB3dmHeader();
		}

		if (gltfBinary)
		{
			writeGltfBinaryHeader();
		}

		file.flush();
	}

	void GltfInfo::finalizeFile()
	{
		if (gltfBinary)
		{
			finalizeBinaryChunk();
		}

		fileEnd = file.tellp();

		if (format == "b3dm")
		{
			finalizeB3dmSizes();
		}

		if (gltfBinary)
		{
			finalizeGltfBinarySizes();
		}

		file.close();
	}
}