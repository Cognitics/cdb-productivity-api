#include <fstream>
#include <iomanip>
#include <ccl/FileInfo.h>
#include "scenegraphobj/scenegraphobj.h"
#include <ctl/TIN.h>

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
		// constructor
		SceneObjBuilder(const std::string outputName, ctl::DelaunayTriangulation *dt, double lWest, double lNorth, double width, double height) : tin(dt)
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
		}

		// destructor
		~SceneObjBuilder()
		{

		}

		bool build()
		{
			tileInfo.open(outputNameInfo, std::ofstream::out | std::ofstream::app);
			tileInfo << localWest << " " << localNorth << "\n";
			tileInfo.close();
			std::ofstream file(outputNameObj);
			file << std::setprecision(9);
			file << "mtllib material.mtl\n";
			
			int nverts = tin.verts.size();
			for (int i = 0; i < nverts; i++)
			{
				auto&vert = tin.verts[i];
				auto&normal = tin.normals[i];
                float u = -(vert.x - localWest) / localWidth;
                u = 1 - u;
                float v = (localNorth - vert.y) / localHeight;
                v = 1 - v;
				file << "v " << vert.x << " " << vert.z << " " << vert.y << "\n";
				file << "vn " << normal.x << " " << normal.z << " " << normal.y << "\n";
				file << "vt " << u << " " << v << " " << 0 << "\n";
			}
			
			file << "usemtl " << tileName << "\n";

			for (int i = 0; i < tin.triangles.size(); i += 3)
			{
				file << "f ";
				auto index1 = tin.triangles[i] + 1;
				auto index2 = tin.triangles[i + 1] + 1;
				auto index3 = tin.triangles[i + 2] + 1;
				file << index3 << "/" << index3 << "/" << index3 << " ";
				file << index2 << "/" << index2 << "/" << index2 << " ";
				file << index1 << "/" << index1 << "/" << index1 << "\n";

			}
			file.close();

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
	};

	bool buildObjFromScene(const std::string outputName, ctl::DelaunayTriangulation *dt, double lWest, double lNorth, double width, double height)
	{
		SceneObjBuilder objBuilder(outputName, dt, lWest, lNorth, width, height);
		return objBuilder.build();
	}
}