#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphobj/scenegraphobj.h"
#include <scenegraph/ExtentsVisitor.h>
#include <scenegraph/TransformVisitor.h>
#include "fstream"

#include "ccl/StringUtils.h"
#include "mesh2cdb.h"
#include "MeshRender.h"
#include "ip/pngwrapper.h"
#include "sfa/BSP.h"
#include "rapidxml/rapidxml.hpp"
#include "quickobj.h"

using namespace rapidxml;
bool Obj2CDB::readMetadataXMLxxx(const std::string &sourceDir)
{
    /*
    xml_document<> doc;
    xml_node<> * root_node;
    // Read the xml file into a vector
    std::ifstream theFile(sourceDir.c_str());
    if (!theFile.is_open())
    {
        log << "Unable to open " << sourceDir << log.endl;
        //exit(1);
        return false;
    }
    std::vector<char> buffer((std::istreambuf_iterator<char>(theFile)), std::istreambuf_iterator<char>());
    buffer.push_back('\0');
    // Parse the buffer using the xml file parsing library into doc 
    doc.parse<0>(&buffer[0]);
    // Find our root node
    root_node = doc.first_node("ModelMetadata");
    for (xml_node<> * node = root_node->first_node(0);
        node;
        node = node->next_sibling())
    {
        if (ccl::stringCompareNoCase(node->name(), "SRS") == 0)
        {
            std::string srs_string = node->value();
            //Parse out the ENU as the first 3 bytes
            if (ccl::stringStartsWith(srs_string, "ENU:", false))
            {
                std::vector<std::string> parts = ccl::splitString(srs_string.substr(4), ",");
                if (parts.size() == 2)
                {
                    double originLat = atof(parts[0].c_str());
                    double originLon = atof(parts[1].c_str());
                    srs.geoOrigin.setX(originLon);
                    srs.geoOrigin.setX(originLat);
                }
                else
                {
                    log << "Unable to parse SRS in metadata.xml" << log.endl;
                    return false;
                }
            }
            else
            {
                log << "Unable to parse SRS in metadata.xml" << log.endl;
                return false;
            }
        }
        else if (ccl::stringCompareNoCase(node->name(), "SRSOrigin") == 0)
        {
            std::string srs_offset_string = node->value();
            std::vector<std::string> parts = ccl::splitString(srs_offset_string, ",");
            if (parts.size() == 3)
            {
                srs.offsetPt = sfa::Point(atof(parts[0].c_str()),atof(parts[1].c_str()),atof(parts[2].c_str()));
            }
            else
            {
                log << "Unable to parse SRSOrigin in metadata.xml" << log.endl;
                return false;
            }
        }
    }
    */
    return true;
}



Obj2CDB::Obj2CDB(const Mesh2CDBParams &_parms) : parms(_parms)
{
    ltp_ellipsoid = NULL;
    dbTop = -DBL_MAX;
    dbBottom = DBL_MAX;
    dbLeft = DBL_MAX;
    dbRight = -DBL_MAX;
    dbMinZ = DBL_MAX;
    dbMaxZ = -DBL_MAX;

    std::string versionXmlPath = ccl::joinPaths(parms.outputDirectory, "Metadata");
    if (!ccl::fileExists(versionXmlPath))
    {
        ccl::makeDirectory(versionXmlPath);
        versionXmlPath = ccl::joinPaths(versionXmlPath, "Version.xml");
        std::ofstream outfile(versionXmlPath.c_str());
        outfile << "<?xml version = \"1.0\"?>" << std::endl;
        outfile << "<Version xmlns:xsi = \"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\">" << std::endl;
        outfile << "<PreviousIncrementalRootDirectory name = \"\" />" << std::endl;
        outfile << "<Comment>Created by mesh2cdb</Comment>" << std::endl;
        outfile << "</Version>" << std::endl;

        outfile.close();
    }

    double originLat = parms.origin.Y();
    double originLon = parms.origin.X();
    ltp_ellipsoid = new Cognitics::CoordinateSystems::EllipsoidTangentPlane(originLat, originLon);

    if (parms.highestLODOnly)
    {
        collectHighestLODTiles();
    }
    else
    {
        objFiles = ccl::FileInfo::getAllFiles(parms.objPath, "*.*", parms.searchObjSubdirectories);
    }
    buildBSP();
}

Obj2CDB::~Obj2CDB()
{
    for (auto && tile_lod : bestTileLOD)
    {
        delete tile_lod.first;
    }
}

int Obj2CDB::getLODFromFilename(const std::string &filename)
{
    if (ccl::stringEndsWith(filename, ".obj", false))
    {
        std::string truncFilename = filename.substr(0, filename.size() - 4);
        std::string::size_type n = truncFilename.rfind("L");
        if (n != std::string::npos)
        {
            std::string lodNumStr = truncFilename.substr(n + 1);
            int lodNum = atoi(lodNumStr.c_str());
            return lodNum;
        }
    }
    return 0;
}

void Obj2CDB::buildBSP()
{
    int loop = 0;
    //objFiles
    for (auto&& fi : objFiles)
    {
        // HAAAAAACK
        //if (loop > 10)
//            break;
        loop++;


#if 1//def QUICK_OBJ
        float top = -FLT_MAX;
        float bottom = FLT_MAX;
        float left = FLT_MAX;
        float right = -FLT_MAX;
        float minZ = FLT_MAX;
        float maxZ = -FLT_MAX;
        ObjSrs srs;
        srs.geoOrigin = parms.origin;
        srs.srsWKT = parms.wkt;
        srs.offsetPt = parms.offset;
        cognitics::QuickObj qo(fi.getFileName(),srs);
        if(!qo.isValid())
        {
            log << "Unable to read " << fi.getFileName() << log.endl;
            continue;
        }
        qo.getBounds(left,right,bottom,top,minZ,maxZ);
#else
        double top = -DBL_MAX;
        double bottom = DBL_MAX;
        double left = DBL_MAX;
        double right = -DBL_MAX;
        double minZ = DBL_MAX;
        double maxZ = -DBL_MAX;
        scenegraph::ExtentsVisitor extentsVisitor;
        scenegraph::Scene *scene = scenegraph::buildSceneFromOBJ(fi.getFileName(), true);
        
        extentsVisitor = scenegraph::ExtentsVisitor();
        extentsVisitor.visit(scene);
        if (!extentsVisitor.getExtents(left, right, bottom, top, minZ, maxZ) ||
            top <= bottom ||
            right <= left)
        {
            continue;
        }
        if (offsetX || offsetY || offsetZ)
        {
            sfa::Matrix matrix;
            matrix.PushTranslate(offsetX, offsetY, offsetZ);
            scenegraph::TransformVisitor transform_visitor(matrix);
            transform_visitor.visit(scene);
        }
#endif
        log << fi.getBaseName() << " : " << left << " <-> " << right << " | " << top << " ^ " << bottom << log.endl;
        dbLeft = std::min<double>(left, dbLeft);
        dbRight = std::max<double>(right, dbRight);
        dbBottom = std::min<double>(bottom, dbBottom);
        dbTop = std::max<double>(top, dbTop);
        dbMinZ = std::min<double>(minZ, dbMinZ);
        dbMaxZ = std::max<double>(maxZ, dbMaxZ);

        sfa::Polygon *aoi_poly = new sfa::Polygon;
        sfa::LineString *ring = new sfa::LineString;
        ring->addPoint(sfa::Point(left, bottom));
        ring->addPoint(sfa::Point(right, bottom));
        ring->addPoint(sfa::Point(right, top));
        ring->addPoint(sfa::Point(left, top));
        ring->addPoint(sfa::Point(left, bottom));
        aoi_poly->addRing(ring);
        //Keep track of the geometry and its associated file
        bestTileLOD[aoi_poly] = fi;
        bsp.addGeometry(aoi_poly);
        //delete scene;
    }

    bsp.generate(envelopes);
    dbLeftLon = 0;
    dbRightLon = 0;
    dbBottomLat = 0;
    dbTopLat = 0;
    dbMinZElev = 0;
    dbMaxZElev = 0;

    ltp_ellipsoid->LocalToGeodetic(dbLeft, dbBottom, 0, dbBottomLat, dbLeftLon, dbMinZElev);
    ltp_ellipsoid->LocalToGeodetic(dbRight, dbTop, dbMaxZ, dbTopLat, dbRightLon, dbMaxZElev);

    log << "Database Extents (Cartesian):" << log.endl;
    log << "LL: " << dbLeft << " , " << dbBottom << log.endl;
    log << "UR: " << dbRight << " , " << dbTop << log.endl;

    log << "Database Extents (Geo):" << log.endl;
    log << "LL: " << dbLeftLon << " , " << dbBottomLat << log.endl;
    log << "UR: " << dbRightLon << " , " << dbTopLat << log.endl;
}

void Obj2CDB::collectHighestLODTiles()
{
    std::map<std::string, int> highestTileLODNum;
    std::map<std::string, std::string> highestTileLODFilename;
    std::vector<ccl::FileInfo> files = ccl::FileInfo::getAllFiles(parms.objPath, "*.*", parms.highestLODOnly);

    for (auto&& fi : files)
    {
        if (ccl::stringEndsWith(fi.getFileName(), ".obj"))
        {
            int lod = getLODFromFilename(fi.getFileName());
            //log << fi.getFileName() << " is LOD " << lod << log.endl;
            if (highestTileLODNum.find(fi.getDirName()) == highestTileLODNum.end())
            {
                highestTileLODNum[fi.getDirName()] = lod;
                highestTileLODFilename[fi.getDirName()] = fi.getFileName();
            }
            else
            {
                if (lod > highestTileLODNum[fi.getDirName()])
                {
                    highestTileLODNum[fi.getDirName()] = lod;
                    highestTileLODFilename[fi.getDirName()] = fi.getFileName();
                }
            }
        }
    }
    for (auto&& fileLODPair : highestTileLODFilename)
    {
        objFiles.push_back(fileLODPair.second);
        //"OBJ count limit enabled!!!!"
        //if (objFiles.size() > 10)
         //   break;
    }


}

renderJobList_t Obj2CDB::collectRenderJobs(cognitics::cdb::Dataset dataset, int lodNum)
{
    cognitics::cdb::LOD lod(lodNum);
    cognitics::cdb::Coordinates cdbLL(dbBottomLat, dbLeftLon);
    cognitics::cdb::Coordinates cdbUR(dbTopLat, dbRightLon);
    cognitics::cdb::CoordinatesRange cdbAOI(cdbLL, cdbUR);
    auto cdbTiles = cognitics::cdb::generate_tiles(cdbAOI, dataset, lod);
    renderJobList_t renderJobs;
    ObjSrs srs;
    srs.geoOrigin = parms.origin;
    srs.srsWKT = parms.wkt;
    srs.offsetPt = parms.offset;
    for (auto&& tile : cdbTiles)
    {
        RenderJob renderJob(tile, srs);
        sfa::BSPCollectGeometriesVisitor bspVisitor;
        std::string absoluteFilePath = ccl::joinPaths(parms.outputDirectory, tile.getFilename());
        if(ccl::fileExists(absoluteFilePath))
        {
            log << absoluteFilePath << " already exists. Skipping..." << log.endl;
            continue;
        }
        ccl::FileInfo tileFi(absoluteFilePath);
        ccl::makeDirectory(tileFi.getDirName());
        renderJob.cdbFilename = absoluteFilePath;
        log << "\t" << absoluteFilePath << log.endl;
        //get all scenes that intersect with this tile.
        //bsp.
        double tileLocalBottom = 0;
        double tileLocalTop = 0;
        double tileLocalLeft = 0;
        double tileLocalRight = 0;
        double localZ;
        ltp_ellipsoid->GeodeticToLocal(tile.getCoordinates().low().latitude().value(),
            tile.getCoordinates().low().longitude().value(),
            0,
            tileLocalLeft,
            tileLocalBottom,
            localZ);
        ltp_ellipsoid->GeodeticToLocal(tile.getCoordinates().high().latitude().value(),
            tile.getCoordinates().high().longitude().value(),
            0,
            tileLocalRight,
            tileLocalTop,
            localZ);
        renderJob.enuMinX = tileLocalLeft;
        renderJob.enuMaxX = tileLocalRight;
        renderJob.enuMinY = tileLocalBottom;
        renderJob.enuMaxY = tileLocalTop;
        renderJob.srs = srs;


        log << "Tile Extents (Cartesian):" << log.endl;
        log << "LL: " << tileLocalLeft << " , " << tileLocalBottom << log.endl;
        log << "UR: " << tileLocalRight << " , " << tileLocalTop << log.endl;

        bspVisitor.setBounds(tileLocalLeft,
            tileLocalBottom,
            tileLocalRight,
            tileLocalTop);
        bspVisitor.visiting(&bsp);
        scenegraph::Scene *parentScene = new scenegraph::Scene();
        for (auto&& geometry : bspVisitor.results)
        {
            std::string sourceOBJ = bestTileLOD[geometry].getFileName();
            log << "\t\tUsing " << sourceOBJ << " as source file." << log.endl;
            renderJob.objFiles.push_back(sourceOBJ);
        }
        if (!renderJob.objFiles.empty())
        {
            renderJobs.push_back(renderJob);
        }
    }
    return renderJobs;
}