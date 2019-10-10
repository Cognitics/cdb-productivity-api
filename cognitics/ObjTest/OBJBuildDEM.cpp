#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
//#include "ip/pngwrapper.h"
#include <scenegraph/ExtentsVisitor.h>

#include <GL/glew.h>
#include <GL/glut.h>
#include <scenegraph_gl/scenegraph_gl.h>
#include "OBJRender.h"
#include "ip/pngwrapper.h"

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )


OBJBuildDEM::OBJBuildDEM(RenderJob _job) : job(_job)
{
    log.init("OBJBuildDEM", this);

}

bool OBJBuildDEM::build()
{
    auto scene = new scenegraph::Scene();
    for (auto&&obj : job.objFiles)
    {
        scenegraph::Scene *childScene = scenegraph::buildSceneFromOBJ(obj, true);
        scene->addChild(childScene);
    }
}

