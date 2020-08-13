#ifndef WIN32
#define USE_EGL
#endif

#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include <ccl/FileInfo.h>
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"


#ifdef USE_EGL
#define GLEW_EGL 1
#endif //USE_EGL
#include <GL/glew.h>

#define FREEGLUT_LIB_PRAGMAS 0
#include <GL/freeglut.h>
#ifndef WIN32
#include <GL/glx.h>
#endif
#include <GL/freeglut_ext.h>
#include <scenegraph_gl/scenegraph_gl.h>
#include "ccl/StringUtils.h"
#include "ccl/ArgumentParser.h"
#include <scenegraph/ExtentsVisitor.h>
#include "sfa/BSP.h"
#include "sfa/sfa.h"
#include <sfa_file_factory/sfa_file_factory.h>
#include <ip/jpgwrapper.h>
#include <ip/rgb.h>
#include <cts/CS_CoordinateSystemFactory.h>
#include <sfa/Layer.h>
#include <sfa/File.h>
#include <ogr/ogr.h>
#include "ogr/File.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
//#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#include "nn-pre.h"
#include "scenegraphobj/quickobj.h"
ccl::ObjLog logger;
double window_llx;
double window_lly;
double window_urx;
double window_ury;
double cursor_world_x;
double cursor_world_y;
int width;
int height;
bool aoi_changed = false;


sfa::Point readOffsetXYZ(const std::string &filename)
{
	sfa::Point offset;
	std::ifstream prjfile(filename.c_str());
	std::string data = "";
	std::stringstream buffer;
	buffer << prjfile.rdbuf();
	data = buffer.str();
	prjfile.close();

	// Split with whitespace
	std::vector<std::string> parts = ccl::splitString(data, " \t");
	if (parts.size() != 3)
	{
		//Error, not enough parts in the xyz file
		return offset;
	}
	offset.setX(atof(parts[0].c_str()));
	offset.setY(atof(parts[1].c_str()));
	offset.setZ(atof(parts[2].c_str()));
	return offset;
}

std::string fileToWKT(const std::string &filename)
{
	std::string data = "";

	std::ifstream prjfile(filename.c_str());
	if (!prjfile)
		return "";
	std::stringstream buffer;
	buffer << prjfile.rdbuf();
	data = buffer.str();
	prjfile.close();
	const std::string prjstr = data.c_str();

	return prjstr;
}

OGRSpatialReference *LoadProjectionFromPRJ(const std::string &filename)
{
	std::string data = "";

	std::ifstream prjfile(filename.c_str());
	if (!prjfile)
		return NULL;
	std::stringstream buffer;
	buffer << prjfile.rdbuf();
	data = buffer.str();
	prjfile.close();

	OGRSpatialReference *file_srs = new OGRSpatialReference;
	const char *prjstr = data.c_str();
	OGRErr err = file_srs->importFromWkt((char **)&prjstr);
	if (err != OGRERR_NONE)
	{
		delete file_srs;
		return NULL;
	}
	return file_srs;
}

std::vector<sfa::Geometry*> geometryListFromPolygonShapefile(const std::string &filename)
{
	auto footprint_file = ogr::File::s_open(filename.c_str());
	sfa::LayerList layers = footprint_file->getLayers();
	std::vector<sfa::Geometry*> polygon_list = std::vector<sfa::Geometry*>();
	for (sfa::Layer* layer : layers)
	{
		sfa::Feature *file_feature = layer->getNextFeature();
		while (file_feature != nullptr)
		{
			sfa::Geometry *file_feature_geometry = file_feature->geometry;
			polygon_list.push_back(file_feature_geometry);
			file_feature = layer->getNextFeature();
			// file_feature->geometry = nullptr;
			// delete file_feature;
		}
	}

	return polygon_list;
}

void setAOI(double llx, double lly, double urx, double  ury)
{
	window_llx = llx;
	window_lly = lly;
	window_urx = urx;
	window_ury = ury;
	aoi_changed = true;
}

void resetAOIForScene(RenderJob job)
{
	double top = job.enuMaxY;
	double bottom = job.enuMinY;
	double left = job.enuMinX;
	double right = job.enuMaxX;
	/*
	double minZ = DBL_MAX;
	double maxZ = -DBL_MAX;
	extentsVisitor = scenegraph::ExtentsVisitor();
	extentsVisitor.visit(scene);
	extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ);
	*/
	setAOI(left, bottom, right, top);
}

#define QUICK_OBJ
void renderToFile(RenderJob &job)
{
	int width = 1024;
	int height = 1024;
	int depth = 3;
#ifndef QUICK_OBJ
	if (scene)
		delete scene;
	scene = new scenegraph::Scene();
	for (auto&&obj : job.objFiles)
	{
		scenegraph::Scene *childScene = scenegraph::buildSceneFromOBJ(obj, true);
		if (job.offsetX || job.offsetY || job.offsetZ)
		{
			sfa::Matrix matrix;
			matrix.PushTranslate(job.offsetX, job.offsetY, job.offsetZ);
			scenegraph::TransformVisitor transform_visitor(matrix);
			transform_visitor.visit(childScene);
		}
		scene->addChild(childScene);
	}
#endif
	resetAOIForScene(job);

	// Build the texture that will serve as the depth attachment for the framebuffer.
	GLuint depth_texture;
	glGenTextures(1, &depth_texture);
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	// The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
	GLuint FramebufferName = 0;
	glGenFramebuffers(1, &FramebufferName);
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);

	// The texture we're going to render to
	GLuint renderedTexture;
	glGenTextures(1, &renderedTexture);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, renderedTexture);

	// Poor filtering. Needed !
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	// Give an empty image to OpenGL ( the last "0" )
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);

	// Set "renderedTexture" as our colour attachement #0
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, renderedTexture, 0);

	// Set the list of draw buffers.
	GLenum DrawBuffers[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, DrawBuffers); // "1" is the size of DrawBuffers

	// Always check that our framebuffer is ok
	auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (err != GL_FRAMEBUFFER_COMPLETE)
	{
		GLenum gl_error = glGetError();
		logger << "Frame Buffer Error! (" << err << "). glError: " << gl_error << logger.endl;
		//return;
	}

	//GLuint ProgramID = LoadShaders("", "layout(location = 0) out vec3 color;");

	// Render to our framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_texture, 0);

	glViewport(0, 0, width, height); // Render on the whole framebuffer, complete from the lower left corner to the upper right
	glEnable(GL_DEPTH_TEST);
	// Use our shader
	//glUseProgram(ProgramID);

		// Clear Color and Depth Buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glClearColor(1.0f, 1.0f, 0.0f, 1.0f); //Chromakey background
	// Reset transformations
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glPushMatrix();
	glLoadIdentity();

	glOrtho(0, width, 0, height, -5000, 5000);

	glColor3f(1.0f, 1.0f, 1.0f);
	glRasterPos2i(10, 10);


	glPopMatrix();

	glOrtho(window_llx, window_urx, window_lly, window_ury, -5000, 5000);
	glMatrixMode(GL_MODELVIEW);

	// Draw
	glEnable(GL_BLEND);
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glPushMatrix();
#ifdef QUICK_OBJ
	logger << "Sorting for " << job.cdbFilename << logger.endl;
	/*std::sort(job.objFiles.begin(), job.objFiles.end(), objinfo_compare());
	for (auto&& ofi : job.objFiles)
	{
		std::string file = ofi.fi.getFileName();
		cognitics::QuickObj *qo = cognitics::gObjCache.get(file);
		if (!qo)
		{
			logger << "Loading " << file << logger.endl;
			qo = new cognitics::QuickObj(file, job.srs, ofi.fi.getDirName(), true);
			cognitics::gObjCache.store(qo);
		}
		if (qo->isValid())
		{
			logger << "Rendering " << file << logger.endl;
			qo->glRender();
		}
		else
		{
			logger << "Unable to load " << file << logger.endl;
		}
	}*/
#else
	renderVisitor.visit(scene);
#endif
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glDisable(GL_BLEND);
	glPopMatrix();
	//glutSwapBuffers();
	unsigned char *pixels = new unsigned char[width * height * depth];
	glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	// FlipVertically(pixels, width, height, 3);

	float *grid = new float[width*height];
	glBindTexture(GL_TEXTURE_2D, depth_texture);
	glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, grid);
	double zFar = 5000;
	double zNear = -5000;
	for (int i = 0, ic = width * height; i < ic; i++)
	{
		if (grid[i] == 1)
			grid[i] = -32767.0;
		else
		{
			grid[i] = -1 * (zNear + (grid[i] * (zFar - zNear)));
		}
	}
	glDeleteTextures(1, &depth_texture);
	glDeleteTextures(1, &renderedTexture);
	// FlipVertically(grid, width, height);
	// queueDEMJob(job, grid, width, height);
	// queueJP2Job(job, pixels, width, height);
	//delete scene;
	// scene = NULL;
}

void getThatMFGLReference(sfa::Polygon* polygon)
{
	int width = 1024;
	int height = 1024;
	auto line_strings = polygon->getRingList();
	glOrtho(0, width, 0, height, -5000, 5000);
	glBegin(GL_LINES);

	for (auto& line_string : line_strings)
	{
		int num_points = line_string->getNumPoints();
		for (int point_idx = 0; point_idx < num_points; point_idx++)
		{
			auto point = line_string->getPointN(point_idx);
			glVertex2f(point->X(), point->Y());
		}
	}
	glEnd();
}

int main(int argc, char **argv)
{
	// read shapefile into list of polygons
	auto shp_file = argv[1];
	logger << "STARTED LOADING SHP FILE" << logger.endl;
	printf("Loading SHP File");
	auto polygon_list = geometryListFromPolygonShapefile(shp_file);
	logger << "FINISHED LOADING SHP FILE" << logger.endl;
	printf("Finished Loading SHP File");
#ifndef USE_EGL
	// init GLUT and create window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowPosition(50, 50);
	glutInitWindowSize(1024, 1024);
	glutCreateWindow("OBJ Viewer");

	glewExperimental = GL_TRUE;
	glewInit();

#endif
	for (auto& geometry : polygon_list)
	{


		// find centroid
		auto polygon = dynamic_cast<sfa::Polygon *>(geometry);
		if (polygon == NULL)
		{
			logger << "We need you to ensure that the shp passed contains a polygon." << logger.endl;
			printf("We need you to ensure that the shp passed contains a polygon.");
			return EXIT_FAILURE;
		}
		auto centroid = polygon->getCentroid();

		// getThatMFGLReference(polygon);
		
		int width = 1024;
		int height = 1024;
		auto line_strings = polygon->getRingList();
		glOrtho(0, width, 0, height, -5000, 5000);
		glBegin(GL_LINES);

		for (auto& line_string : line_strings)
		{
			int num_points = line_string->getNumPoints();
			for (int point_idx = 0; point_idx < num_points; point_idx++)
			{
				auto point = line_string->getPointN(point_idx);
				glVertex2f(point->X(), point->Y());
			}
		}
		glEnd();
		// while true (to test output)
		while (true) {
		}
		logger << "POLY" << logger.endl;
	}
	printf("Done with program");
	while(true)
	return EXIT_SUCCESS;


#ifndef WIN32
	char *gdal_data_var = getenv("GDAL_DATA");
	if (gdal_data_var == NULL)
	{
		putenv("GDAL_DATA=/usr/local/share/gdal");
	}
	char *gdal_plugins_var = getenv("GDAL_DRIVER_PATH");
	if (gdal_plugins_var == NULL)
	{
		putenv("GDAL_DRIVER_PATH=/usr/local/bin/gdalplugins");
	}
#else
	size_t requiredSize;
	getenv_s(&requiredSize, NULL, 0, "GDAL_DATA");
	if (requiredSize == 0)
	{
		ccl::FileInfo fi(argv[0]);
		int bufSize = 1024;
		char *envBuffer = new char[bufSize];
		std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");
		sprintf_s(envBuffer, bufSize, "GDAL_DATA=%s", dataDir.c_str());
		_putenv(envBuffer);
	}
#endif
	logger.init("main");
	logger << ccl::LINFO;
	GDALAllRegister();
	ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));


	return EXIT_SUCCESS;
}
