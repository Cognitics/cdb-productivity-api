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
#include  "sfa/BSP.h"


#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )


class CollectFacesVisitor : public scenegraph::Visitor
{
private:
    std::list<sfa::Polygon> faces;

public:
    virtual ~CollectFacesVisitor(void) {}
    CollectFacesVisitor(void) {}

    virtual void visiting(scenegraph::Scene *scene)
    {
        for (auto& face : scene->faces)
        {
            faces.push_front(face.getPolygon());
        }
        traverse(scene);
    }

    std::list<sfa::Polygon> &getFaces() { return faces; }

};

namespace {
    bool interpolatePointInFace(sfa::Point &p, sfa::Polygon *poly)
    {
        sfa::LineString *line = poly->getExteriorRing();
        if (!line || line->getNumPoints()<3)
            return false;

        sfa::Point p1 = line->getPointN(0);
        sfa::Point p2 = line->getPointN(1);
        sfa::Point p3 = line->getPointN(2);
        //TODO: make sure the 3 points are independent
        sfa::Point P = p - p1;
        sfa::Point U = p2 - p1;
        sfa::Point V = p3 - p1;

        double denom = V.X()*U.Y() - V.Y()*U.X();
        double v = (P.X()*U.Y() - P.Y()*U.X()) / denom;
        double u = (P.Y()*V.X() - P.X()*V.Y()) / denom;

        if (v < 0 || u < 0)
            return false;
        else if (u + v > 1)
            return false;
        else
        {
            p.setZ(p1.Z() + u * U.Z() + v * V.Z());
            return true;
        }
    }

}
OBJBuildDEM::OBJBuildDEM(RenderJob _job) : job(_job)
{
    log.init("OBJBuildDEM", this);

}
bool OBJBuildDEM::writeDEM(float *grid, int width, int height)
{
    const char *pszFormat = "GTiff";
    GDALDriver *poDriver;
    poDriver = GetGDALDriverManager()->GetDriverByName(pszFormat);
    if (poDriver == NULL)
    {
        log << "Unable to get GTiff Driver." << log.endl;
        return false;
    }
    char **papszOptions = NULL;
    auto poDstDS = poDriver->Create((job.cdbFilename + ".tif").c_str(), width, height, 1, GDT_Float32,
        papszOptions);
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
        grid, width, height, GDT_Float32, 3, width * 3);
    
    /* Once we're done, close properly the dataset */
    GDALClose((GDALDatasetH)poDstDS);
    return true;
}

bool OBJBuildDEM::build()
{
    int width = 1024;
    int height = 1024;
    int len = width * height;
    float *grid = new float[len];

    //Initialize grid
    for (int i = 0; i < len; i++)
        grid[i] = -32767.0;

    auto scene = new scenegraph::Scene();
    for (auto&&obj : job.objFiles)
    {
        scenegraph::Scene *childScene = scenegraph::buildSceneFromOBJ(obj, true);
        scene->addChild(childScene);
    }
    CollectFacesVisitor collector;
    collector.visit(scene);
    //Make sure the bsp and visitor go out of scope first
    {
        sfa::BSP bsp;
        sfa::BSPCollectGeometriesVisitor bspVisitor;
        for (auto&& face : collector.getFaces())
        {
            bsp.addGeometry(&face);
        }
        std::map<sfa::Geometry *, sfa::LineString *> envelopes;
        bsp.generate(envelopes);
        for (int row = 0; row < height; row++)
        {
            int rowStart = row * width;
            double postY = (job.cdbTile.postSpaceY * row) + job.enuMinY;
            for (int col = 0; col < width; col++)
            {
                int colIndex = rowStart + col;
                double postX = (job.cdbTile.postSpaceX * col) + job.enuMinX;
                sfa::BSPCollectGeometriesVisitor bspVisitor;
                bspVisitor.setBounds(postX - job.cdbTile.postSpaceX,
                    postY - job.cdbTile.postSpaceY,
                    postX + job.cdbTile.postSpaceX,
                    postY + job.cdbTile.postSpaceY);
                bspVisitor.visiting(&bsp);
                for (auto&& geometry : bspVisitor.results)
                {
                    auto *poly = dynamic_cast<sfa::Polygon *>(geometry);
                    if(!poly)
                    {
                        log << "Non-polygon geometry returned from BSPCollectGeometriesVisitor." << log.endl;
                    }
                    sfa::Point p(postX, postY, -32767.0);
                    if(interpolatePointInFace(p, poly))
                    {
                        grid[colIndex] = std::max<float>(p.Z(), grid[colIndex]);
                    }
                }
            }
        }
    }
    writeDEM(grid, width, height);
    delete scene;
    delete[] grid;
    return true;
}

