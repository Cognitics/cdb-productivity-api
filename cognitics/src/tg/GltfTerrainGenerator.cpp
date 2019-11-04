#include <tg/GltfTerrainGenerator.h>
#include <ctl/ctl.h>
#include <ip/pngwrapper.h>
#include <ip/jpgwrapper.h>
//#include <filesystem>


#undef max
#undef min
#define NOMINMAX
#include <scenegraphgltf/scenegraphgltf.h>

using namespace cognitics;

void GltfTerrainGenerator::AdjustJSON_UV(double& u, double& v)
{
    //gltf uses upper left origin for uvs
    //https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#images
    v = 1.0 - v;
}

void GltfTerrainGenerator::BuildFromScene(const std::string& outputName, scenegraph::Scene* scene, double localWidth, double localHeight)
{
	logger << ccl::LWARNING << "BuildFromScene: GLTF requires bounding box for export. Make sure bounds are set correctly." << logger.endl;
	std::string name = outputName;
    scenegraph::buildGltfFromScene(name, scene, north, south, east, west);
}

void GltfTerrainGenerator::CreateMasterFile()
{
    scenegraph::buildTilesetFromScene(ccl::joinPaths(outputPath, "tileset.json"), &master, north, south, east, west);
}

void GltfTerrainGenerator::ExportBuilding(FeatureInfo& featureInfo, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, float lat, float lon)
{
    float elev = featureInfo.elev;
    std::string featurefilename = outputPath + fi.getBaseName(true) + ".b3dm";
    scenegraph::buildGltfFromScene(featurefilename, scene, lat, lat, lon, lon, elev, elev+1.0, 2, featureInfo.AO1);

}

void GltfTerrainGenerator::ExportTree(int treeIndex, const ccl::FileInfo& fi, const std::string& outputPath, scenegraph::Scene* scene, elev::Elevation_DSM* edsm, TreePoints_Feature& treePointsFeature)
{
    scenegraph::Scene scene2;
    ParseJSON(fi.getFileName(), outputPath, scene2);

    float lon = treePointsFeature.treePoints[0].worldPosition.fLon;
    float lat = treePointsFeature.treePoints[0].worldPosition.fLat;

    if (edsm != nullptr)
    {
        sfa::Point loc(lon, lat);
        edsm->Get(&loc);
        treePointsFeature.treePoints[0].elev = loc.Z();
    }

    float elev = treePointsFeature.treePoints[0].elev;

    std::stringstream ss;
    ss << outputPath << fi.getBaseName(true) << "_" << treeIndex << ".b3dm";
    std::cout << "Writing tree file: " << fi.getBaseName(true) << "_" << treeIndex << std::endl;
	std::string filename = ss.str();
    scenegraph::buildGltfFromScene(filename, &scene2, lat, lat, lon, lon, elev, elev+1.0, 3);

}
/*
void GltfTerrainGenerator::generateFixedGrid(const std::string & imgFile, const std::string & outputPath, const std::string & outputName, std::string format, elev::Elevation_DSM & edsm, double north, double south, double east, double west)
{
	std::string outputFileName = ccl::joinPaths(outputPath, outputName + format);
	// open imgFile which is a tif.
	GDALDataset  *poDataset;
	GDALAllRegister();
	poDataset = (GDALDataset *)GDALOpen(imgFile.c_str(), GA_ReadOnly);
	if (poDataset == NULL)
	{
		logger << ccl::LERR << "generateFixedGrid: Couldn't open image for dataset" << logger.endl;
		return;
	}
	int rasterWidth = poDataset->GetRasterXSize();
	int rasterHeight = poDataset->GetRasterYSize();
	int len = rasterWidth * rasterHeight * 3;
	unsigned char* buf = new unsigned char[len];
	unsigned char* bufPtr = buf;

	for (int i = 0; i < 3; i++)
	{
		auto pBand = poDataset->GetRasterBand(i + 1);
		pBand->RasterIO(GF_Read, 0, 0, rasterWidth, rasterHeight, bufPtr + i, rasterWidth, rasterHeight, GDT_Byte, 3, 3 * rasterWidth);
	}

	std::string jpgFilename = ccl::joinPaths(outputPath, outputName + ".jpg");
	ExportTextureMetaData(jpgFilename);
	ip::ImageInfo info;
	info.width = rasterWidth;
	info.height = rasterHeight;
	info.depth = 3;
	info.interleaved = true;
	info.dataType = ip::ImageInfo::UBYTE;
	ccl::binary buffer;
	buffer.resize(len);
	int counter = 0;
	for (int i = 0; i < len; i++)
	{
		buffer[i] = buf[i];
	}
	ip::WriteJPG24(jpgFilename, info, buffer);
	ExportTextureMetaData(jpgFilename);
	delete[] buf;
	ccl::FileInfo fi(outputName);
	std::string tileName = fi.getBaseName(true);

	flatEarth.setOrigin((north + south) / 2, (east + west) / 2);

	double tileWorldNorth = north;
	double tileWorldSouth = south;
	double tileWorldWest = west;
	double tileWorldEast = east;

	localWest = flatEarth.convertGeoToLocalX(west);
	localEast = flatEarth.convertGeoToLocalX(east);
	localNorth = flatEarth.convertGeoToLocalY(north);
	localSouth = flatEarth.convertGeoToLocalY(south);
	double localWidth = localEast - localWest;
	double localHeight = localNorth - localSouth;
	logger << ccl::LINFO << "Using Elevation File MBR: N:" << north << "(" << localNorth << ") S:" << south << "(" << localSouth << ") W:" << west << "(" << localWest << ") E:" << east << "(" << localEast << ")" << logger.endl;

	int nSamples = 100;

	double spacingX = (north - south) / nSamples;
	double spacingY = -(east - west) / nSamples;

	ctl::PointList workingPoints;

	sfa::Point p;
	ctl::PointList gamingArea;
	{
		double z = 0;
		p.setX(west);
		p.setY(south);
		edsm.Get(&p);
		ctl::Point southwest(localWest, localSouth, p.Z());
		p.setX(east);
		p.setY(south);
		edsm.Get(&p);
		ctl::Point southeast(localEast, localSouth, p.Z());
		p.setX(east);
		p.setY(north);
		edsm.Get(&p);
		ctl::Point northeast(localEast, localNorth, p.Z());
		p.setX(west);
		p.setY(north);
		edsm.Get(&p);
		ctl::Point northwest(localWest, localNorth, p.Z());
		gamingArea.push_back(southwest);
		gamingArea.push_back(southeast);
		gamingArea.push_back(northeast);
		gamingArea.push_back(northwest);
	}

	ctl::PointList boundaryPoints;
	{
		sfa::LineString boundaryLineString;
		// Left boundary
		int col = 0;
		double lon = west;
		p.setX(lon);
		double localPostX = flatEarth.convertGeoToLocalX(lon);
		for (int row = 0; row < nSamples; row++)
		{   // Go from pixel space to geo
			double lat = (row * spacingY) + north;
			// Go from geo to local                
			double localPostY = flatEarth.convertGeoToLocalY(lat);
			p.setY(lat);
			edsm.Get(&p);
			boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
		}
		// Right boundary
		col = nSamples - 1;
		lon = east;
		p.setX(lon);
		localPostX = flatEarth.convertGeoToLocalX(lon);
		for (int row = 0; row < nSamples; row++)
		{   // Go from pixel space to geo
			double lat = (row * spacingY) + north;
			// Go from geo to local
			p.setY(lat);
			edsm.Get(&p);
			double localPostY = flatEarth.convertGeoToLocalY(lat);
			boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
		}
		// Bottom boundary
		double lat = south;
		p.setY(lat);
		double localPostY = flatEarth.convertGeoToLocalY(lat);
		for (int col = 0; col < nSamples; col++)
		{   // Go from pixel space to geo
			double lon = (col * spacingX) + west;
			// Go from geo to local
			p.setX(lon);
			edsm.Get(&p);
			double localPostX = flatEarth.convertGeoToLocalX(lon);
			boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
		}
		// Top boundary
		lat = north;
		p.setY(lat);
		localPostY = flatEarth.convertGeoToLocalY(lat);
		for (int col = 0; col < nSamples; col++)
		{   // Go from pixel space to geo
			double lon = (col * spacingX) + west;
			// Go from geo to local
			p.setX(lon);
			edsm.Get(&p);
			double localPostX = flatEarth.convertGeoToLocalX(lon);
			boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, p.Z()));
		}

		//            boundaryLineString.removeColinearPoints(0, 0.5);
		for (int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
		{
			sfa::Point *p = boundaryLineString.getPointN(i);
			boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
		}
	}


	int delaunayResizeIncrement = 100;
	{
		for (int row = 1; row < nSamples - 2; ++row)
		{
			printf("");
			for (int col = 1; col < nSamples - 2; ++col)
			{
				// Go from pixel space to geo
				double lat = (row * spacingY) + north;
				double lon = (col * spacingX) + west;
				// Go from geo to local
				double localPostX = flatEarth.convertGeoToLocalX(lon);
				double localPostY = flatEarth.convertGeoToLocalY(lat);
				p.setX(lon);
				p.setY(lat);
				edsm.Get(&p);
				workingPoints.push_back(ctl::Point(localPostX, localPostY, p.Z()));
			}
		}

		delaunayResizeIncrement = (nSamples * nSamples) / 8;
	}

	// TODO: Allocate this based on a polygon budget
	ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(gamingArea, delaunayResizeIncrement);

	//Randomly the order of point insertions to avoid worst case performance of DelaunayTriangulation
	std::random_shuffle(boundaryPoints.begin(), boundaryPoints.end());
	std::random_shuffle(workingPoints.begin(), workingPoints.end());

	{
		//Alternate inserting boundary and working points to avoid worst case performance of DelaunayTriangulation
		size_t i = 0;
		size_t j = 0;
		while (i < boundaryPoints.size() || j < workingPoints.size())
		{
			if (i < boundaryPoints.size())
				dt->InsertConstrainedPoint(boundaryPoints[i++]);
			if (j < workingPoints.size())
				dt->InsertWorkingPoint(workingPoints[j++]);
		}
	}

	dt->Simplify(1, float(0.05));    // simplify based on coplanar points
									 //dt->Simplify(20000, 0.5);        // if we still have over 20k triangles, simplify using the triangle budget

	ctl::TIN *tin = new ctl::TIN(dt);
	scenegraph::Scene *scene = new scenegraph::Scene;
	scene->faces.reserve(tin->triangles.size() / 3);
	for (size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
	{
		// get the triangle points from the ctl tin
		ctl::Point pa = tin->verts[tin->triangles[i * 3 + 0]];
		ctl::Point pb = tin->verts[tin->triangles[i * 3 + 1]];
		ctl::Point pc = tin->verts[tin->triangles[i * 3 + 2]];
		sfa::Point sfaA = sfa::Point(pa.x, pa.y, pa.z);
		sfa::Point sfaB = sfa::Point(pb.x, pb.y, pb.z);
		sfa::Point sfaC = sfa::Point(pc.x, pc.y, pc.z);

		// get the normals from the ctl tin
		ctl::Vector na = tin->normals[tin->triangles[i * 3 + 0]];
		ctl::Vector nb = tin->normals[tin->triangles[i * 3 + 1]];
		ctl::Vector nc = tin->normals[tin->triangles[i * 3 + 2]];
		sfa::Point sfaAN = sfa::Point(na.x, na.y, na.z);
		sfa::Point sfaBN = sfa::Point(nb.x, nb.y, nb.z);
		sfa::Point sfaCN = sfa::Point(nc.x, nc.y, nc.z);

		// create the new face
		scenegraph::Face face;
		face.verts.push_back(sfaA);
		face.verts.push_back(sfaB);
		face.verts.push_back(sfaC);
		face.vertexNormals.push_back(sfaAN);
		face.vertexNormals.push_back(sfaBN);
		face.vertexNormals.push_back(sfaCN);

		face.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
		face.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);

		// Add texturing
		scenegraph::MappedTexture mt;
		mt.SetTextureName(jpgFilename);
		// Each texture should map to the tile extents directly
		// So create a transform from the tile boundaries to local coordinates
		
		mt.uvs.push_back(sfa::Point((sfaA.X() - localWest) / localWidth, (localNorth - sfaA.Y()) / localHeight));
		mt.uvs.push_back(sfa::Point((sfaB.X() - localWest) / localWidth, (localNorth - sfaB.Y()) / localHeight));
		mt.uvs.push_back(sfa::Point((sfaC.X() - localWest) / localWidth, (localNorth - sfaC.Y()) / localHeight));
		
		face.textures.push_back(mt);

		scene->faces.push_back(face);
	}

	double minElev = DBL_MAX;
	double maxElev = DBL_MIN;
	for (int row = 0; row < nSamples; ++row)
	{
		for (int col = 0; col < nSamples; ++col)
		{
			// Go from pixel space to geo
			double lat = (row * spacingY) + north;
			double lon = (col * spacingX) + west;
			sfa::Point p1;
			p1.setX(lon);
			p1.setY(lat);
			edsm.Get(&p1);
			auto z = p1.Z();
			minElev = std::min(z, minElev);
			maxElev = std::max(z, maxElev);
		}
	}

	std::string outputExportName = ccl::joinPaths(outputPath, outputName + format);

	logger << "Writing " << outputExportName << "..." << logger.endl;

	scenegraph::buildGltfFromScene(outputExportName, scene, north, south, east, west, minElev, maxElev, 1);

	delete tin;
	delete dt;
}
*/

void GltfTerrainGenerator::generateFixedGridWithLOD(std::string geoServerURL, double north, double south, double east, double west, std::string format, const std::string& outputTmpPath, const std::string& outputPath, const std::string&outputFormat, int lodDepth, int textureHeight, int textureWidth)
{
	double deltaX = east - west;
	double deltaY = north - south;

	double delta = std::max(deltaX, deltaY) / 2;

	double centerLat = (north - south) / 2 + south;
	double centerLon = (east - west) / 2 + west;

	east = centerLon + delta;
	west = centerLon - delta;
	north = centerLat + delta;
	south = centerLat - delta;

	centerLat = (north - south) / 2 + south;
	centerLon = (east - west) / 2 + west;

	std::vector<TileInfo> infos;
	TileInfo info;
	info.extents.east = east;
	info.extents.north = north;
	info.extents.south = south;
	info.extents.west = west;
	info.quadKey = std::to_string(0);
	GenerateLODBounds(infos, info, lodDepth);
	GenerateFileNames(infos, outputTmpPath, format);
	ComputeCenterPosition(infos, centerLat, centerLon);

	for (auto info : infos)
	{
		std::cout << info.elevationFileName << std::endl;
	}

	GetData(geoServerURL, 0, infos.size(), &infos);

	std::cout << "DONE" << std::endl;
	elev::DataSourceManager dsm(1000000);

	for (auto& info : infos)
	{
		dsm.AddFile_Raster_GDAL(info.elevationFileName);
	}

	elev::Elevation_DSM edsm(&dsm, elev::elevation_strategy::ELEVATION_BILINEAR);

	for (auto& info : infos)
	{
		generateFixedGrid(info.imageFileName, outputPath, info.quadKey, outputFormat, edsm, info.extents.north, info.extents.south, info.extents.east, info.extents.west);
	}

	createFeatures(edsm);
	//WriteLODfile(infos, outputPath + "/lodFile.txt", lodDepth);
	setBounds(north, south, east, west);
	CreateMasterFile();

	bool removeTextures = true;
	ccl::FileInfo fi(outputPath);
	if (removeTextures)
	{
		if(ccl::directoryExists(fi.getDirName()))
		{
			std::cout << "generate Cesium Lods: Removing image files from " << outputPath << std::endl;
			auto files = ccl::FileInfo::getAllFiles(fi.getDirName(),"*.*");
			for(auto&& file : files)
			{
				if(ccl::FileInfo::fileExists(fi.getFileName()))
				{
					auto ext = fi.getSuffix();
					if (ext == "jpg"
						|| ext == "jpeg"
						|| ext == "png")
					{
						ccl::deleteFile(fi.getFileName());
					}
				}
			}
		}
	}
	auto tmpDir = ccl::joinPaths(outputPath,"/tmp");
	if(ccl::directoryExists(tmpDir))
	{
		auto files = ccl::FileInfo::getAllFiles(fi.getDirName(),"*.*");
		for(auto&& file : files)
		{
			std::cout << "generate Cesium Lods: Removing tmp directory " << tmpDir << std::endl;
			ccl::deleteFile(file.getFileName());
		}
	}
}
