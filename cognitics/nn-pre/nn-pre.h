#pragma once
#pragma once

#include <string>
#include <vector>
#include <list>
#include "sfa/BSP.h"
#include "ccl/ObjLog.h"
#include "cdb_tile/Tile.h"
#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "scenegraph/Visitor.h" 
#include "scenegraphobj/quickobj.h"



// Transform all points in a scene using the specified math transform
class ENUTransformVisitor : public scenegraph::Visitor
{
private:
	Cognitics::CoordinateSystems::EllipsoidTangentPlane *etp;
	OGRCoordinateTransformation *coordTrans;
	sfa::Point offset;
public:
	virtual ~ENUTransformVisitor(void)
	{

	}
	ENUTransformVisitor(Cognitics::CoordinateSystems::EllipsoidTangentPlane *_etp,
		OGRCoordinateTransformation *_coordTrans,
		const sfa::Point &_offset) : etp(_etp), coordTrans(_coordTrans), offset(_offset)
	{

	}

	virtual void visiting(scenegraph::Scene *scene)
	{
		for (size_t i = 0, c = scene->faces.size(); i < c; ++i)
		{
			scenegraph::Face &face = scene->faces.at(i);
			// update face coordinates based on transform
			int numVerts = face.getNumVertices();
			for (int i = 0; i < numVerts; i++)
			{
				sfa::Point &pt = face.verts.at(i);
				// Add offset
				pt += offset;

				double x = pt.X();
				double y = pt.Y();
				double z = pt.Z();
				double local_x = 0, local_y = 0, local_z = 0;
				coordTrans->Transform(1, &x, &y, &z);
				etp->GeodeticToLocal(y,
					x,
					z,
					local_x,
					local_y,
					local_z);
				pt.setX(local_x);
				pt.setY(local_y);
				pt.setZ(local_z);
			}
		}

		traverse(scene);
	}

	scenegraph::Scene *transform(scenegraph::Scene *scene)
	{
		visit(scene);
		return scene;
	}
};

class SimpleFace
{
public:
	unsigned int v1, v2, v3;
	unsigned int vN1, vN2, vN3;
	unsigned int vT1, vT2, vT3;
	scenegraph::Face *faceRef;
	scenegraph::Material *matRef;
};

class OptimizeVisitor : public scenegraph::Visitor
{
private:
	std::vector<sfa::Point> verts;
	std::vector<sfa::Point> normals;
	std::vector<sfa::Point> textureCoords;
	std::vector<scenegraph::Material> materials;
public:
	virtual ~OptimizeVisitor(void)
	{

	}
	OptimizeVisitor()
	{

	}

	virtual void visiting(scenegraph::Scene *scene)
	{
		for (size_t i = 0, c = scene->faces.size(); i < c; ++i)
		{
			scenegraph::Face &face = scene->faces.at(i);
			// update face coordinates based on transform

		}

		traverse(scene);
	}

	scenegraph::Scene *collect(scenegraph::Scene *scene)
	{
		visit(scene);
		return scene;
	}
};

class RenderJob
{
public:
	std::string obj_filename;
	double enuMinX;
	double enuMinY;
	double enuMaxX;
	double enuMaxY;

	cognitics::cdb::Tile cdbTile;
	std::vector<ObjFileInfo> objFiles;
	ObjSrs srs;

	std::string ToString()
	{
		std::stringstream ss;
		ss.precision(6);
		ss << "<RenderJob ";
		ss << "enu-max-x=\"" << enuMinX << "\" ";
		ss << "enu-max-x=\"" << enuMaxX << "\" ";
		ss << "enu-min-y=\"" << enuMinY << "\" ";
		ss << "enu-max-y=\"" << enuMaxY << "\" ";
		ss << ">\n";

		ss << "\t<CDBTileName>";
		ss << cdbTile.getFilename();
		ss << "</CDBTileName>\n";	
		for (auto &&inputFilename : objFiles)
		{
			ss << "\t<InputFile>";
			ss << inputFilename.fi.getFileName();
			ss << "</InputFile>\n";
		}
		ss << srs.ToString();
		ss << "</RenderJob>\n";
		return ss.str();
	}

	RenderJob(const cognitics::cdb::Tile tile, const ObjSrs &_srs) : cdbTile(tile), srs(_srs)
	{
		enuMinX = 0;
		enuMinY = 0;
		enuMaxX = 0;
		enuMaxY = 0;
	}
};
typedef std::list<RenderJob> renderJobList_t;

class ObjFileInfo
{
public:
	int lod;
	ccl::FileInfo fi;
};

class OBJBuildDEM
{
	ccl::ObjLog log;
	RenderJob job;
	bool writeDEM(float *grid, int width, int height);
public:
	OBJBuildDEM(RenderJob job);
	bool build();

};

#if 0
class ObjXXXX
{
	ccl::ObjLog log;
	std::string objRootDir;

	double dbOriginLat;
	double dbOriginLon;
	double dbTop;
	double dbBottom;
	double dbLeft;
	double dbRight;
	double dbMinZ;
	double dbMaxZ;
	double dbLeftLon;
	double dbRightLon;
	double dbBottomLat;
	double dbTopLat;
	double dbMinZElev;
	double dbMaxZElev;
	double offsetX;
	double offsetY;
	double offsetZ;

	std::string metadataFilename;
	std::vector<ccl::FileInfo> objFiles;
	sfa::BSP bsp;
	std::map<sfa::Geometry *, sfa::LineString *> envelopes;
	std::map<sfa::Geometry *, ccl::FileInfo> bestTileLOD;
	Cognitics::CoordinateSystems::EllipsoidTangentPlane *ltp_ellipsoid;

	bool readMetadataXML(const std::string &sourceDir);
public:
	Obj2CDB(const std::string &inputOBJDir,
		const std::string &outputCDBDir, std::string metadataFilename = std::string(), bool hiveMapperMode = false);
	~Obj2CDB();

	renderJobList_t collectRenderJobs(cognitics::cdb::Dataset dataset, int lodNum);
};

#endif
