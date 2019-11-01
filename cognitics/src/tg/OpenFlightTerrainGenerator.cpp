#include <tg/OpenFlightTerrainGenerator.h>
#include <scenegraphflt/scenegraphflt.h>
#include <ip/attr.h>

using namespace cognitics;

void OpenFlightTerrainGenerator::BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight)
{
    scenegraph::buildOpenFlightFromScene(outputName, scene);
}

void OpenFlightTerrainGenerator::CreateMasterFile()
{
    scenegraph::buildOpenFlightFromScene(ccl::joinPaths(outputPath, "master.flt"), &master);
}

void OpenFlightTerrainGenerator::ExportTextureMetaData(const std::string& filepath)
{
    ip::attrFile attr;
    attr.wrapMode = ip::attrFile::WRAP_CLAMP;
    attr.wrapMode_u = ip::attrFile::WRAP_CLAMP;
    attr.wrapMode_v = ip::attrFile::WRAP_CLAMP;
    std::string attrFilename = filepath + ".attr";
    attr.Write(attrFilename);
}