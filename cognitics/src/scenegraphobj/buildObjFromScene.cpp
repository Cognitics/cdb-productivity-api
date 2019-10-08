#include <fstream>
#include <iomanip>
#include <ccl/FileInfo.h>
#include "scenegraphobj/scenegraphobj.h"
#include "sfa\Point.h"
#include "sfa\PointMath.h"
#include "ctl\Vector.h"
#include <ctl/TIN.h>
#include <boost/lexical_cast.hpp>

using namespace ctl;

namespace scenegraph
{
    struct SceneObjBuilder
    {
        std::string filename;
        std::string outputNameObj;
        std::string outputNameMtl;
        std::string outputNameInfo;
        std::string tileName;
        ctl::TIN tin;
        double localWest;
        double localNorth;
        double localWidth;
        double localHeight;
        std::ofstream material;
        std::ofstream tileInfo;
        int elevWidth;
        int elevHeight;
        ctl::PointList workingPts;
        scenegraph::Scene scene;
        // constructor
        SceneObjBuilder(const std::string outputName, double lWest, double lNorth, double width, double height, int elevationWidth, int elevationHeight, const ctl::PointList &workingPoints)
        {
            ccl::FileInfo fi(outputName);
            outputNameObj = fi.getFileName();
            outputNameMtl = ccl::joinPaths(fi.getDirName(), "material.mtl");
            outputNameInfo = ccl::joinPaths(fi.getDirName(), "tileInfo.txt");
            tileName = fi.getBaseName(true);
            localWest = lWest;
            localNorth = lNorth;
            localWidth = width;
            localHeight = height;
            elevWidth = elevationWidth;
            elevHeight = elevationHeight;
            workingPts = workingPoints;
            scene = NULL;
        }
        SceneObjBuilder(const std::string outputName, double lWest, double lNorth, double width, double height, scenegraph::Scene *inScene)
        {
            ccl::FileInfo fi(outputName);
            outputNameObj = fi.getFileName();
            outputNameMtl = ccl::joinPaths(fi.getDirName(), "material.mtl");
            outputNameInfo = ccl::joinPaths(fi.getDirName(), "tileInfo.txt");
            tileName = fi.getBaseName(true);
            localWest = lWest;
            localNorth = lNorth;
            localWidth = width;
            localHeight = height;
            scene = *inScene;
        }

        // destructor
        ~SceneObjBuilder()
        {

        }

        bool buildFromScene()
        {
            //tileInfo.open(outputNameInfo, std::ofstream::out | std::ofstream::app);
            //tileInfo << localWest << " " << localNorth << "\n";
            //tileInfo.close();
            std::ofstream file(outputNameObj);
            file << std::setprecision(9);
            file << "mtllib material.mtl\n";

            std::map<std::string, std::vector<scenegraph::Face*>> seperatedFaces;

            for (auto& face : scene.faces)
            {
                auto string = face.getMappedTextureN(0).GetTextureName();
                if (seperatedFaces.find(string) == seperatedFaces.end())
                {
                    seperatedFaces[string] = std::vector<scenegraph::Face*>();
                }
                seperatedFaces[string].push_back(&face);
            }

            int index = 0;

            for (auto& faceList : seperatedFaces)
            {
                for (auto&face : faceList.second)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        file << "v " << face->verts[i].X() << " " << face->verts[i].Y() << " " << face->verts[i].Z() << "\n";
                    }
                }
            }

            for (auto& faceList : seperatedFaces)
            {
                for (auto&face : faceList.second)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        float u = face->textures[0].uvs[i].X();
                        float v = face->textures[0].uvs[i].Y();
                        file << "vt " << u << " " << v << " " << 0 << "\n";
                    }
                }
            }

            index = 0;

            for (auto& kvp : seperatedFaces)
            {
                file << "usemtl " << kvp.first << "\n";
                for (auto& face : kvp.second)
                {
                    index += 3;
                    int index1 = index - 2;
                    int index2 = index - 1;
                    int index3 = index;

                    file << "f ";
                    file << index1 << "/" << index1 << " ";
                    file << index2 << "/" << index2 << " ";
                    file << index3 << "/" << index3 << "\n";
                }
            }

            // print out material file 
            file.close();
			if (material.is_open())
			{
				material.close();
			}
            material.open(outputNameMtl, std::ofstream::out | std::ofstream::app);
            material << std::setprecision(9);
            for (auto& kvp : seperatedFaces)
            {
                material << "newmtl " << kvp.first << "\n";
                material << "Ka " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
                material << "Kd " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
                material << "Ks " << 0.0 << " " << 0.0 << " " << 0.0 << "\n";
                material << "Tr " << 1.0 << "\n";
                material << "illum " << 1 << "\n";
                material << "Ns " << 0.0 << "\n";
                material << "map_Kd " << kvp.first << "\n";
            }
            material.close();

            return true;
        }

        bool buildXZY()
        {
            std::ofstream file(outputNameObj);
            file << std::setprecision(9);
            file << "mtllib material.mtl\n";

            std::map<std::string, std::vector<scenegraph::Face*>> seperatedFaces;

            for (auto& face : scene.faces)
            {
                auto string = face.getMappedTextureN(0).GetTextureName();
                if (seperatedFaces.find(string) == seperatedFaces.end())
                {
                    seperatedFaces[string] = std::vector<scenegraph::Face*>();
                }
                seperatedFaces[string].push_back(&face);
            }

            int index = 0;

            for (auto& faceList : seperatedFaces)
            {
                for (auto&face : faceList.second)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        file << "v " << -face->verts[i].X() << " " << face->verts[i].Z() << " " << face->verts[i].Y() << "\n";
                        file << "vn " << -face->vertexNormals[i].X() << " " << face->vertexNormals[i].Z() << " " << face->vertexNormals[i].Y() << "\n";
                    }
                }
            }

            for (auto& faceList : seperatedFaces)
            {
                for (auto&face : faceList.second)
                {
                    for (int i = 0; i < 3; i++)
                    {
                        float u = face->textures[0].uvs[i].X();
                        float v = face->textures[0].uvs[i].Y();
                        file << "vt " << u << " " << v << " " << 0 << "\n";
                    }
                }
            }

            index = 0;

            for (auto& kvp : seperatedFaces)
            {
                file << "usemtl " << kvp.first << "\n";
                for (auto& face : kvp.second)
                {
                    index += 3;
                    int index1 = index - 2;
                    int index2 = index - 1;
                    int index3 = index;

                    file << "f ";
                    file << index1 << "/" << index1 << " ";
                    file << index2 << "/" << index2 << " ";
                    file << index3 << "/" << index3 << "\n";
                }
            }

            // print out material file 
            file.close();
            if (material.is_open())
            {
                material.close();
            }
            material.open(outputNameMtl, std::ofstream::out | std::ofstream::app);
            material << std::setprecision(9);
            for (auto& kvp : seperatedFaces)
            {
                material << "newmtl " << kvp.first << "\n";
                material << "Ka " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
                material << "Kd " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
                material << "Ks " << 0.0 << " " << 0.0 << " " << 0.0 << "\n";
                material << "Tr " << 1.0 << "\n";
                material << "illum " << 1 << "\n";
                material << "Ns " << 0.0 << "\n";
                material << "map_Kd " << kvp.first << "\n";
            }
            material.close();

            return true;
        }

        bool build()
        {
            std::ofstream file(outputNameObj);
            file << std::setprecision(9);
            file << "mtllib material.mtl\n";

            auto localSouth = localNorth - localHeight;

            for (auto &point : workingPts)
            {
                file << "v " << -point.x << " " << point.z << " " << point.y << "\n";
                file << "vn " << "0 1 0\n";
                float u = (point.x - localWest) / localWidth;
                float v = (point.y - localSouth) / localHeight;
                file << "vt " << u << " " << v << " " << 0 << "\n";
            }

            file << "usemtl " << tileName << "\n";

            // Writing out triangles 
            for (int i = 0; i < elevHeight - 1; i++)
            {
                for (int j = 1; j < elevWidth; j++)
                {

                    int index1 = (i * elevWidth) + j;
                    int index2 = index1 + 1;
                    int index3 = index2 + elevWidth;
                    file << "f ";
                    file << index3 << "/" << index3 << "/" << index3 << " ";
                    file << index2 << "/" << index2 << "/" << index2 << " ";
                    file << index1 << "/" << index1 << "/" << index1 << "\n";

                    index1 = (i * elevWidth) + j;
                    index2 = index1 + elevWidth;
                    index3 = index2 + 1;
                    file << "f ";
                    file << index2 << "/" << index2 << "/" << index2 << " ";
                    file << index3 << "/" << index3 << "/" << index3 << " ";
                    file << index1 << "/" << index1 << "/" << index1 << "\n";
                }
            }

            file.close();

			if (material.is_open())
			{
				material.close();
			}
            material.open(outputNameMtl, std::ofstream::out | std::ofstream::app);
            material << std::setprecision(9);
            material << "newmtl " << tileName << "\n";
            material << "Ka " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
            material << "Kd " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
            material << "Ks " << 0.0 << " " << 0.0 << " " << 0.0 << "\n";
            material << "Tr " << 1.0 << "\n";
            material << "illum " << 1 << "\n";
            material << "Ns " << 0.0 << "\n";
            material << "map_Kd " << tileName << ".jpg" << "\n";
            material.close();

            return true;
        }

   //     bool build(const std::string imgFile)
   //     {
   //         std::ofstream file(outputNameObj);
   //         file << std::setprecision(9);
   //         file << "mtllib material.mtl\n";

   //         auto localSouth = localNorth - localHeight;

   //         for (auto &point : workingPts)
   //         {
   //             file << "v " << -point.x << " " << point.z << " " << point.y << "\n";
   //             file << "vn " << "0 1 0\n";
   //             float u = (point.x - localWest) / localWidth;
   //             float v = (point.y - localSouth) / localHeight;
   //             file << "vt " << u << " " << v << " " << 0 << "\n";
   //         }

   //         file << "usemtl " << tileName << "\n";

   //         // Writing out triangles 
   //         for (int i = 0; i < elevHeight - 1; i++)
   //         {
   //             for (int j = 1; j < elevWidth; j++)
   //             {

   //                 int index1 = (i * elevWidth) + j;
   //                 int index2 = index1 + 1;
   //                 int index3 = index2 + elevWidth;
   //                 file << "f ";
   //                 file << index3 << "/" << index3 << "/" << index3 << " ";
   //                 file << index2 << "/" << index2 << "/" << index2 << " ";
   //                 file << index1 << "/" << index1 << "/" << index1 << "\n";

   //                 index1 = (i * elevWidth) + j;
   //                 index2 = index1 + elevWidth;
   //                 index3 = index2 + 1;
   //                 file << "f ";
   //                 file << index2 << "/" << index2 << "/" << index2 << " ";
   //                 file << index3 << "/" << index3 << "/" << index3 << " ";
   //                 file << index1 << "/" << index1 << "/" << index1 << "\n";
   //             }
   //         }

   //         file.close();

			//if (material.is_open())
			//{
			//	material.close();
			//}
   //         material.open(outputNameMtl, std::ofstream::out | std::ofstream::app);
   //         material << std::setprecision(9);
   //         material << "newmtl " << tileName << "\n";
   //         material << "Ka " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
   //         material << "Kd " << 1.0 << " " << 1.0 << " " << 1.0 << "\n";
   //         material << "Ks " << 0.0 << " " << 0.0 << " " << 0.0 << "\n";
   //         material << "Tr " << 1.0 << "\n";
   //         material << "illum " << 1 << "\n";
   //         material << "Ns " << 0.0 << "\n";
   //         material << "map_Kd " << imgFile << "\n";
   //         material.close();

   //         return true;
   //     }
    };

    bool buildObjFromTriangulation(const std::string outputName, double lWest, double lNorth, double width, double height, int elevWidth, int elevHeight, const ctl::PointList &workingPts)
    {
        SceneObjBuilder objBuilder(outputName, lWest, lNorth, width, height, elevWidth, elevHeight, workingPts);
        return objBuilder.build();
    }

    bool buildObjFromScene(const std::string outputName, double lWest, double lNorth, double width, double height, scenegraph::Scene *inscene)
    {
        SceneObjBuilder objBuilder(outputName, lWest, lNorth, width, height, inscene);
        return objBuilder.buildFromScene();
    }

    bool buildObjXZY(const std::string outputName, double lWest, double lNorth, double width, double height, scenegraph::Scene *inscene, bool xzy)
    {
        SceneObjBuilder objBuilder(outputName, lWest, lNorth, width, height, inscene);
        return objBuilder.buildXZY();
    }
    
    void CreateMetaForObj(std::string filename)
    {
        std::ofstream outFile;
        std::string metafile = filename + ".meta";
        outFile.open(metafile, std::ofstream::out);

        boost::uuids::uuid guid = boost::uuids::random_generator()();
        std::string guidString = boost::lexical_cast<std::string>(guid);

        std::string chars = "-";
        for (char c : chars)
        {
            guidString.erase(std::remove(guidString.begin(), guidString.end(), c), guidString.end());
        }

        outFile
            << "fileFormatVersion: " << std::endl
            << "guid: " << guidString << std::endl
            << "TextureImporter:" << std::endl
            << "  internalIDToNameTable: []" << std::endl
            << "  externalObjects: {}" << std::endl
            << "  serializedVersion: 10" << std::endl
            << "  mipmaps:" << std::endl
            << "    mipMapMode: 0" << std::endl
            << "    enableMipMap: 1" << std::endl
            << "    sRGBTexture: 1" << std::endl
            << "    linearTexture: 0" << std::endl
            << "    fadeOut: 0" << std::endl
            << "    borderMipMap: 0" << std::endl
            << "    mipMapsPreserveCoverage: 0" << std::endl
            << "    alphaTestReferenceValue: 0.5" << std::endl
            << "    mipMapFadeDistanceStart: 1" << std::endl
            << "    mipMapFadeDistanceEnd: 3" << std::endl
            << "  bumpmap:" << std::endl
            << "    convertToNormalMap: 0" << std::endl
            << "    externalNormalMap: 0" << std::endl
            << "    heightScale: 0.25" << std::endl
            << "    normalMapFilter: 0" << std::endl
            << "  isReadable: 0" << std::endl
            << "  streamingMipmaps: 0" << std::endl
            << "  streamingMipmapsPriority: 0" << std::endl
            << "  grayScaleToAlpha: 0" << std::endl
            << "  generateCubemap: 6" << std::endl
            << "  cubemapConvolution: 0" << std::endl
            << "  seamlessCubemap: 0" << std::endl
            << "  textureFormat: 1" << std::endl
            << "  maxTextureSize: 2048" << std::endl
            << "  textureSettings:" << std::endl
            << "    serializedVersion: 2" << std::endl
            << "    filterMode: -1" << std::endl
            << "    aniso: -1" << std::endl
            << "    mipBias: -100" << std::endl
            << "    wrapU: 1" << std::endl // sets 1: clamp on Unity
            << "    wrapV: 1" << std::endl
            << "    wrapW: 1" << std::endl
            << "  nPOTScale: 1" << std::endl
            << "  lightmap: 0" << std::endl
            << "  compressionQuality: 50" << std::endl
            << "  spriteMode: 0" << std::endl
            << "  spriteExtrude: 1" << std::endl
            << "  spriteMeshType: 1" << std::endl
            << "  alignment: 0" << std::endl
            << "  spritePivot: {x: 0.5, y: 0.5}" << std::endl
            << "  spritePixelsToUnits: 100" << std::endl
            << "  spriteBorder: {x: 0, y: 0, z: 0, w: 0}" << std::endl
            << "  spriteGenerateFallbackPhysicsShape: 1" << std::endl
            << "  alphaUsage: " << std::endl
            << "  alphaIsTransparency: 0" << std::endl
            << "  spriteTessellationDetail: -1" << std::endl
            << "  textureType: 0" << std::endl
            << "  textureShape: 1" << std::endl
            << "  singleChannelComponent: 0" << std::endl
            << "  maxTextureSizeSet: 0" << std::endl
            << "  compressionQualitySet: 0" << std::endl
            << "  textureFormatSet: 0" << std::endl
            << "  platformSettings:" << std::endl
            << "  - serializedVersion: 2" << std::endl
            << "    buildTarget: DefaultTexturePlatform" << std::endl
            << "    maxTextureSize: 2048" << std::endl
            << "    resizeAlgorithm: 0" << std::endl
            << "    textureFormat: -1" << std::endl
            << "    textureCompression: 1" << std::endl
            << "    compressionQuality: 50" << std::endl
            << "    crunchedCompression: 0" << std::endl
            << "    allowsAlphaSplitting: 0" << std::endl
            << "    overridden: 0" << std::endl
            << "    androidETC2FallbackOverride: 0" << std::endl
            << "  spriteSheet:" << std::endl
            << "    serializedVersion: 2" << std::endl
            << "    sprites: []" << std::endl
            << "    outline: []" << std::endl
            << "    physicsShape: []" << std::endl
            << "    bones: []" << std::endl
            << "    spriteID: " << std::endl
            << "    internalID: 0" << std::endl
            << "    vertices: []" << std::endl
            << "    indices: " << std::endl
            << "    edges: []" << std::endl
            << "    weights: []" << std::endl
            << "    secondaryTextures: []" << std::endl
            << "  spritePackingTag: " << std::endl
            << "  pSDRemoveMatte: 0" << std::endl
            << "  pSDShowRemoveMatteOption: 0" << std::endl
            << "  userData: " << std::endl
            << "  assetBundleName: " << std::endl
            << "  assetBundleVariant: " << std::endl;

        outFile.close();
    }
    
    /*bool buildObjFromTriangulation(const std::string outputName, const std::string imgName, double lWest, double lNorth, double width, double height, int elevWidth, int elevHeight, const ctl::PointList &workingPts)
    {
        SceneObjBuilder objBuilder(outputName, lWest, lNorth, width, height, elevWidth, elevHeight, workingPts);
        return objBuilder.build(imgName);
    }*/
}