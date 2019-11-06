#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
//#include "ip/pngwrapper.h"
#include <scenegraph/ExtentsVisitor.h>
#include <ccl/JobManager.h>
#ifdef USE_EGL
#define GLEW_EGL 1
#endif //USE_EGL
#include <GL/glew.h>

#include <GL/glut.h>
#ifndef WIN32
#include <GL/glx.h>
#endif
#include <GL/freeglut_ext.h>
#include <scenegraph_gl/scenegraph_gl.h>
#include "MeshRender.h"
#include "ip/pngwrapper.h"
#include "quickobj.h"
#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#ifdef USE_EGL
#define EGL_EXT_PROTOTYPES
#define EGL_EGLEXT_PROTOTYPES
#include <EGL/egl.h>
#include <EGL/eglext.h>

#endif
namespace
{
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
    bool leftMouseDown = false;
    int leftMouseDownX = 0;
    int leftMouseDownY = 0;

    bool rightMouseDown = false;
    int rightMouseDownX = 0;
    int rightMouseDownY = 0;
    int mousex = 0;
    int mousey = 0;

    double llx = 0;
    double lly = 0;
    double urx = 0;
    double ury = 0;
    scenegraph::Scene *scene = NULL;
    scenegraph::SimpleGLRenderVisitor renderVisitor("");
    scenegraph::ExtentsVisitor extentsVisitor;
    scenegraph::Scene *fixedScene = NULL;
    renderJobList_t renderJobs;
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

void setAOI(double llx, double lly, double urx, double  ury)
{
    window_llx = llx;
    window_lly = lly;
    window_urx = urx;
    window_ury = ury;
    aoi_changed = true;
}

void writeJP2(RenderJob &job, unsigned char *pixels, int width, int height);

class JP2WriterThreadDataManager : public ccl::ThreadDataManager
{
public:

    virtual void onThreadFinished(void)
    {
    }

};

bool writeDEM(RenderJob &job, float *grid, int width, int height);
class DEMWriteJob : public ccl::Job
{
    float *grid;
    RenderJob renderJob;
    int width;
    int height;
protected:
    virtual int onChildFinished(ccl::Job *job, int result)
    {
        return 0;
    }

public:
    virtual ~DEMWriteJob() {}
    DEMWriteJob(ccl::JobManager *manager,
        const RenderJob &job,
        float *grid,
        int width,
        int height) :
        ccl::Job(manager, NULL),
        renderJob(job),
        grid(grid),
        width(width),
        height(height)
    {

    }

    int execute(void)
    {
        writeDEM(renderJob, grid, width, height);
        delete[] grid;
        grid = NULL;
        return 0;
    }
};



class JP2WriteJob : public ccl::Job
{
    unsigned char *pixels;
    RenderJob renderJob;
    int width;
    int height;
protected:
    virtual int onChildFinished(ccl::Job *job, int result)
    {
        return 0;
    }

public:
    virtual ~JP2WriteJob() {}
    JP2WriteJob(ccl::JobManager *manager, 
        const RenderJob &job, 
        unsigned char *pixels, 
        int width, 
        int height) :
            ccl::Job(manager, NULL),
            renderJob(job),
            pixels(pixels),
            width(width),
            height(height)
    {
        
    }

    int execute(void)
    {
        writeJP2(renderJob, pixels, width, height);
        delete[] pixels;
        pixels = NULL;
        return 0;
    }
};

JP2WriterThreadDataManager jp2ThreadDataManager;
int num_threads = 4;
ccl::JobManager jobManager(num_threads, NULL, &jp2ThreadDataManager);

ccl::JobManager DEMJobManager(num_threads, NULL);


void queueDEMJob(RenderJob &job, float *grid, int width, int height)
{
    DEMWriteJob *demJob = new DEMWriteJob(&DEMJobManager, job, grid, width, height);
    jobManager.submitJob(demJob);
}

void queueJP2Job(RenderJob &job, unsigned char *pixels, int width, int height)
{
    JP2WriteJob *jp2Job = new JP2WriteJob(&jobManager,job,pixels,width,height);
    jobManager.submitJob(jp2Job);
}

void writeJP2(RenderJob &job, unsigned char *pixels, int width, int height)
{
    const char *pszFormat = "MEM";
    GDALDriver *poMemDriver;
    poMemDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if (poMemDriver == NULL)
    {
        logger << "Unable to get JP2OpenJPEG Driver." << logger.endl;
        return;
    }
    GDALDriver *poJP2Driver;
    poJP2Driver = GetGDALDriverManager()->GetDriverByName("JP2OpenJPEG");
    if (poJP2Driver == NULL)
    {
        logger << "Unable to get JP2OpenJPEG Driver." << logger.endl;
        return;
    }

    char **papszOptions = NULL;
    auto poDstDS = poMemDriver->Create("mem.tmp", width, height, 3, GDT_Byte,
        papszOptions);
    //GDALCreate("JP2OpenJPEG", );
    const cognitics::cdb::CoordinatesRange cdbExtents = job.cdbTile.getCoordinates();
    double adfGeoTransform[6];
    adfGeoTransform[0] = cdbExtents.low().longitude().value();//left geo
    adfGeoTransform[1] = job.cdbTile.postSpaceX; //post spacing x
    adfGeoTransform[2] = 0;
    adfGeoTransform[3] = cdbExtents.high().latitude().value(); //top geo
    adfGeoTransform[4] = 0;
    adfGeoTransform[5] = job.cdbTile.postSpaceY * -1; //post spacing y

    OGRSpatialReference oSRS;
    char *pszSRS_WKT = NULL;
    GDALRasterBand *poBand;
    poDstDS->SetGeoTransform(adfGeoTransform);
    oSRS.SetWellKnownGeogCS("WGS84");
    oSRS.exportToWkt(&pszSRS_WKT);
    poDstDS->SetProjection(pszSRS_WKT);
    CPLFree(pszSRS_WKT);
    poBand = poDstDS->GetRasterBand(1);//Red
    poBand->RasterIO(GF_Write, 0, 0, width, height,
        pixels, width, height, GDT_Byte, 3, width * 3);
    poBand = poDstDS->GetRasterBand(2);//Green
    poBand->RasterIO(GF_Write, 0, 0, width, height,
        pixels + 1, width, height, GDT_Byte, 3, width * 3);
    poBand = poDstDS->GetRasterBand(3);//Blue
    poBand->RasterIO(GF_Write, 0, 0, width, height,
        pixels + 2, width, height, GDT_Byte, 3, width * 3);


    GDALDataset *outDs = poJP2Driver->CreateCopy(job.cdbFilename.c_str(), poDstDS, 1, NULL, NULL, NULL);
    GDALClose((GDALDatasetH)outDs);
    /* Once we're done, close properly the dataset */
    GDALClose((GDALDatasetH)poDstDS);
}

void FlipVertically(unsigned char *pixels, int width, int height, int depth)
{
    unsigned char *copy = new unsigned char[width * height * depth];
    memcpy(copy, pixels, width * height * depth);
    for (int i = 0; i < height; i++)
    {
        int row_len = width * depth;
        int ii = ((height - 1) - i);
        // copy the row
        for (int j = 0; j < row_len; j++)
        {
            pixels[(i*row_len) + j] = copy[(ii*row_len) + j];
        }
    }
    delete copy;
}


void FlipVertically(float *grid, int width, int height)
{
    float *copy = new float[width * height];
    memcpy(copy, grid, width * height * sizeof(float));
    for (int i = 0; i < height; i++)
    {
        int row_len = width;
        int ii = ((height - 1) - i);
        // copy the row
        for (int j = 0; j < row_len; j++)
        {
            grid[(i*row_len) + j] = copy[(ii*row_len) + j];
        }
    }
    delete copy;
}

bool replace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if (start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

bool writeDEM(RenderJob &job, float *grid, int width, int height)
{

    std::string demFileName = job.cdbFilename;
    replace(demFileName, "004_Imagery", "001_Elevation");
    replace(demFileName, "D004", "D001");
    replace(demFileName, ".jp2", ".tif");
    ccl::FileInfo fi(demFileName);
    ccl::makeDirectory(fi.getDirName(), true);
    //std::cout << "Writing " << demFileName << std::endl;
    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if (poDriver == NULL)
    {
        logger << "Unable to get GTiff Driver." << logger.endl;
        return false;
    }
    char **papszOptions = NULL;
    auto poDstDS = poDriver->Create((demFileName).c_str(), width, height, 1, GDT_Float32,
        papszOptions);
    if(!poDstDS)
    {
        logger << "Unable to create file: " << demFileName << logger.endl;
        return false;
    }
    const cognitics::cdb::CoordinatesRange cdbExtents = job.cdbTile.getCoordinates();
    double adfGeoTransform[6];
    adfGeoTransform[0] = cdbExtents.low().longitude().value();//left geo
    adfGeoTransform[1] = job.cdbTile.postSpaceX; //post spacing x
    adfGeoTransform[2] = 0;
    adfGeoTransform[3] = cdbExtents.high().latitude().value(); //top geo
    adfGeoTransform[4] = 0;
    adfGeoTransform[5] = job.cdbTile.postSpaceY * -1; //post spacing y

    OGRSpatialReference oSRS;
    char *pszSRS_WKT = NULL;
    GDALRasterBand *poBand;
    poDstDS->SetGeoTransform(adfGeoTransform);
    oSRS.SetWellKnownGeogCS("WGS84");
    oSRS.exportToWkt(&pszSRS_WKT);
    poDstDS->SetProjection(pszSRS_WKT);
    CPLFree(pszSRS_WKT);
    poBand = poDstDS->GetRasterBand(1);
    poBand->RasterIO(GF_Write, 0, 0, width, height,
        grid, width, height, GDT_Float32, 0, 0);

    /* Once we're done, close properly the dataset */
    GDALClose((GDALDatasetH)poDstDS);
    return true;
}


#define QUICK_OBJ
void renderToFile(RenderJob &job)
{
    int width = 1024;
    int height = 1024;
    int depth = 3;
    GLenum glErr;
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
    glErr = glCheckFramebufferStatus(GL_FRAMEBUFFER);

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
       // GLenum gl_error = glGetError();
        //logger << "Frame Buffer Error! (" << err << "). glError: " << gl_error << logger.endl;
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
    for(auto&& file : job.objFiles)
    {
        ccl::FileInfo objFi(file);
        cognitics::QuickObj qo(file,job.offsetX,job.offsetY,job.offsetZ,objFi.getDirName(),true);
        if(qo.isValid())
        {
            logger << "Rendering " << file << logger.endl;
            qo.glRender();
        }
        else
        {
            logger << "Unable to load " << file << logger.endl;
        }
    }
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
    FlipVertically(pixels, width, height, 3);

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
    FlipVertically(grid, width, height);
    queueDEMJob(job, grid, width, height);
    queueJP2Job(job, pixels, width, height);
    delete scene;
    scene = NULL;
}


bool renderingToFile = true;
float totalCDBTileCount = 0;
std::string rootCDBOutput;

#ifdef USE_EGL

static const EGLint configAttribs[] = {
        EGL_SURFACE_TYPE, EGL_PBUFFER_BIT,
        EGL_BLUE_SIZE, EGL_DONT_CARE,
        EGL_GREEN_SIZE, EGL_DONT_CARE,
        EGL_RED_SIZE, EGL_DONT_CARE,
        EGL_DEPTH_SIZE, EGL_DONT_CARE,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
        EGL_NONE
};

static const int pbufferWidth = 9;
static const int pbufferHeight = 9;

static const EGLint pbufferAttribs[] = {
      EGL_WIDTH, pbufferWidth,
      EGL_HEIGHT, pbufferHeight,
      EGL_NONE,
};

#endif //USE_EGL
void glutRenderScene()
{
    renderScene();
}

#define CHECK_EGL_ERR do { EGLint err = eglGetError(); if (err != EGL_SUCCESS) { printf("Error line %d: 0x%.4x\n", __LINE__, err); } } while (false)
bool renderInit(int argc, char **argv, renderJobList_t &jobs, const std::string &cdbRoot)
{
#ifdef WIN32
#ifdef USE_EGL
    auto eglModule = LoadLibraryA("libEGL.dll");
    PFNEGLGETPROCADDRESSPROC eglGetProcAddress = (PFNEGLGETPROCADDRESSPROC)GetProcAddress(eglModule, "eglGetProcAddress");
    if(!eglGetProcAddress)
    {
        return false;
    } 
    PFNEGLGETDISPLAYPROC eglGetDisplay = (PFNEGLGETDISPLAYPROC)eglGetProcAddress("eglGetDisplay");
    PFNEGLINITIALIZEPROC eglInitialize = (PFNEGLINITIALIZEPROC)eglGetProcAddress("eglInitialize");
    PFNEGLGETPLATFORMDISPLAYEXTPROC eglGetPlatformDisplayEXT = (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    PFNEGLQUERYDEVICESEXTPROC eglQueryDevicesEXT = (PFNEGLQUERYDEVICESEXTPROC)eglGetProcAddress("eglQueryDevicesEXT");
    PFNEGLMAKECURRENTPROC eglMakeCurrent = (PFNEGLMAKECURRENTPROC)eglGetProcAddress("eglMakeCurrent");

    PFNEGLCHOOSECONFIGPROC eglChooseConfig = (PFNEGLCHOOSECONFIGPROC)eglGetProcAddress("eglChooseConfig");
    PFNEGLCREATECONTEXTPROC eglCreateContext = (PFNEGLCREATECONTEXTPROC)eglGetProcAddress("eglCreateContext");
    PFNEGLGETCONFIGSPROC eglGetConfigs = (PFNEGLGETCONFIGSPROC)eglGetProcAddress("eglGetConfigs");

    PFNEGLGETERRORPROC eglGetError = (PFNEGLGETERRORPROC)eglGetProcAddress("eglGetError");
    PFNEGLBINDAPIPROC eglBindAPI = (PFNEGLBINDAPIPROC)eglGetProcAddress("eglBindAPI");
    PFNEGLQUERYSTRINGPROC eglQueryString = (PFNEGLQUERYSTRINGPROC)eglGetProcAddress("eglQueryString");
#endif //USE_EGL
#endif //WIN32

#ifdef USE_EGL
    EGLDisplay eglDpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    CHECK_EGL_ERR;

    EGLint major, minor;
    eglInitialize(eglDpy, &major, &minor);

    char const * client_apis = eglQueryString(eglDpy, EGL_CLIENT_APIS);
    if(!client_apis)
    {
        std::cerr << "Failed to eglQueryString(display, EGL_CLIENT_APIS)" << std::endl;
        return false;
    }
    std::cout << "Supported client rendering APIs: " << client_apis << std::endl;
    CHECK_EGL_ERR;

    static const int MAX_DEVICES = 4;
    EGLDeviceEXT eglDevs[MAX_DEVICES];
    EGLint numDevices;    

    EGLConfig eglCfg;
    EGLint numConfigs = 20;
    eglGetConfigs(eglDpy, NULL, 0, &numConfigs);
    CHECK_EGL_ERR;
    if(numConfigs<1)
    {
        printf("No configs match!\n");
        return false;
    }
    EGLConfig *configs = new EGLConfig[numConfigs];
    if (EGL_FALSE == eglGetConfigs(eglDpy, configs, numConfigs, &numConfigs))
    {
        printf("Error...\n");
    }
    CHECK_EGL_ERR;


    EGLBoolean res = eglChooseConfig(eglDpy, configAttribs, &eglCfg, 1, &numConfigs);
    CHECK_EGL_ERR;
    eglBindAPI(EGL_OPENGL_API);    
    EGLContext eglCtx = eglCreateContext(eglDpy, eglCfg, EGL_NO_CONTEXT,
        NULL);
    CHECK_EGL_ERR;

    res = eglMakeCurrent(eglDpy, EGL_NO_SURFACE, EGL_NO_SURFACE, eglCtx);
    CHECK_EGL_ERR;


    printf("OpenGL version: %s\n", glGetString(GL_VERSION));
    printf("OpenGL vender: %s\n", glGetString(GL_VENDOR));
    printf("OpenGL renderer: %s: \n", glGetString(GL_RENDERER));
    glewInit();
#endif //USE_EGL

#define USE_WINDOW
#ifdef USE_WINDOW
    // init GLUT and create window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

    glutInitWindowPosition(50, 50);
    glutInitWindowSize(1024, 1024);
    glutCreateWindow("OBJ Viewer");

    glewExperimental = GL_TRUE;
    glewInit();

#endif
    totalCDBTileCount = jobs.size();

    rootCDBOutput = cdbRoot;
    GDALAllRegister();

    renderJobs = jobs;

    logger.init("OBJ Render");
    logger << ccl::LINFO;

    // OpenGL init
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    // enter GLUT event processing cycle
       // register callbacks
    glutDisplayFunc(glutRenderScene);
    glutIdleFunc(glutRenderScene);

    glutMainLoop();

    while(true)
    {
        if(!renderScene())
            return true;
    }
    return true;
}


int CPL_STDCALL GDALProgressObserver(CPL_UNUSED double dfComplete,
    CPL_UNUSED const char *pszMessage,
    void * /* pProgressArg */)
{
    logger << (dfComplete * 100.0f) << "% complete..." << logger.endl;
    return TRUE;
}

/*
 * This is a hack function because there isn't a way to return
 * back to the main program loop when rendering is done. So
 * for now, the final code is here.
 */
void finishBuild()
{
    logger << "Waiting for compression of JP2 files to complete..." << logger.endl;
    jobManager.waitForCompletion();
    logger << "============================" << logger.endl;
    logger << "\nBuilding Imagery LODs" << logger.endl;    
    std::string cdbImageryOpenString = "CDB:" + rootCDBOutput + ":Imagery_Yearly";
    auto poDataset = (GDALDataset *)GDALOpen(cdbImageryOpenString.c_str(), GA_Update);
    if (poDataset == NULL)
    {
        logger << ccl::LERR << "Unable to open " << cdbImageryOpenString << logger.endl;
        return;
    }
    if (poDataset->BuildOverviews("average", 0, NULL, 0, NULL, GDALProgressObserver, NULL) != CE_None)
    {
        logger << "Imagery LOD Build failed." << logger.endl;
        const char *gdalErrMsg = CPLGetLastErrorMsg();
        logger << gdalErrMsg << logger.endl;
    }
    logger << "\n============================" << logger.endl;
    logger << "Building Elevation LODs" << logger.endl;
    std::string cdbElevationOpenString = "CDB:" + rootCDBOutput + ":Elevation_PrimaryTerrainElevation";
    auto poElevDataset = (GDALDataset *)GDALOpen(cdbElevationOpenString.c_str(), GA_Update);
    if (poElevDataset == NULL)
    {
        logger << ccl::LERR << "Unable to open " << poElevDataset << logger.endl;
        return;
    }
    if (poElevDataset->BuildOverviews("average", 0, NULL, 0, NULL, GDALProgressObserver, NULL) != CE_None)
    {
        logger << "Elevation LOD Build failed." << logger.endl;
        const char *gdalErrMsg = CPLGetLastErrorMsg();
        logger << gdalErrMsg << logger.endl;
    }

    logger << "Build completed!" << logger.endl;
}



bool renderScene(void)
{
    if (renderJobs.empty())
    {
        //finishBuild();
        //glutLeaveMainLoop();
        return false;
    }
    RenderJob job = renderJobs.back();
    if (renderingToFile)
    {
        renderJobs.pop_back();
        renderToFile(job);
        float jobsLeft = renderJobs.size();
        if((renderJobs.size() % 10)==0)
        {
            std::stringstream ss;
            ss.precision(2);
            ss << ((1.0f - (jobsLeft / totalCDBTileCount))*100.0) << "%" << " complete...";
            logger << ss.str() << logger.endl;
        }
        return true;
    }
    return false;
}

GLuint LoadShaders(std::string vertexShader, std::string fragmentShader)
{

    // Create the shaders
    GLuint VertexShaderID = glCreateShader(GL_VERTEX_SHADER);
    GLuint FragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

    GLint Result = GL_FALSE;
    int InfoLogLength;

    if (vertexShader.size() > 0)
    {
        // Compile Vertex Shader
        logger << "Compiling vertex shader..." << logger.endl;
        char const * VertexSourcePointer = vertexShader.c_str();
        glShaderSource(VertexShaderID, 1, &VertexSourcePointer, NULL);
        glCompileShader(VertexShaderID);

        // Check Vertex Shader
        glGetShaderiv(VertexShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(VertexShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> VertexShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(VertexShaderID, InfoLogLength, NULL, &VertexShaderErrorMessage[0]);
            logger << (&VertexShaderErrorMessage[0]) << logger.endl;
        }
    }

    if (fragmentShader.size() > 0)
    {
        // Compile Fragment Shader
        logger << "Compiling fragment shader..." << logger.endl;
        char const * FragmentSourcePointer = fragmentShader.c_str();
        glShaderSource(FragmentShaderID, 1, &FragmentSourcePointer, NULL);
        glCompileShader(FragmentShaderID);

        // Check Fragment Shader
        glGetShaderiv(FragmentShaderID, GL_COMPILE_STATUS, &Result);
        glGetShaderiv(FragmentShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength);
        if (InfoLogLength > 0) {
            std::vector<char> FragmentShaderErrorMessage(InfoLogLength + 1);
            glGetShaderInfoLog(FragmentShaderID, InfoLogLength, NULL, &FragmentShaderErrorMessage[0]);
            logger << (&FragmentShaderErrorMessage[0]) << logger.endl;
        }
    }
    // Link the program
    logger << "Linking program..." << logger.endl;
    GLuint ProgramID = glCreateProgram();

    if (vertexShader.size() > 0)
        glAttachShader(ProgramID, VertexShaderID);
    if (fragmentShader.size() > 0)
        glAttachShader(ProgramID, FragmentShaderID);

    glLinkProgram(ProgramID);

    // Check the program
    glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
    glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
    if (InfoLogLength > 0) {
        std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
        glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
        logger << (&ProgramErrorMessage[0]) << logger.endl;
    }

    if (vertexShader.size() > 0)
    {
        glDetachShader(ProgramID, VertexShaderID);
        glDeleteShader(VertexShaderID);
    }
    if (fragmentShader.size() > 0)
    {
        glDetachShader(ProgramID, FragmentShaderID);
        glDeleteShader(FragmentShaderID);

    }

    return ProgramID;
}
