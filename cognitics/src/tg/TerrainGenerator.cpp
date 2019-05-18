/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
****************************************************************************/

//#pragma optimize("", off)

#pragma warning (disable : 4996)

#include "tg/TerrainGenerator.h"
#include <ccl/FileInfo.h>
#include <ip/pngwrapper.h>
#include <ip/jpgwrapper.h>
#include <limits>
#include <boost/lexical_cast.hpp>
#include <ctl/ctl.h>

#include <scenegraphflt/scenegraphflt.h>
#include <scenegraph/SceneCropper.h>

#include <scenegraph/FlattenVisitor.h>
#include <scenegraph/TransformVisitor.h>

namespace cognitics
{
    

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#if 0
    class RemoveFacesProcessor : public spawn::IFeatureProcessor
    {
        fgs::ModelGenerator generator;
        scenegraph::Scene *scene;
        std::list<scenegraph::Face> faces;
        double translate_x;
        double translate_y;
        cad::AttributeDictionary dictionary;

    public:
        dom::DocumentSP cerDocument;

        virtual bool IgnoreRoad(ccl::uuid id) { return true; }
        virtual bool IgnoreJunction(ccl::uuid id) { return false; }

        RemoveFacesProcessor(scenegraph::Scene* _scene, cts::FlatEarthProjection *proj, spawn::OpenDriveDB *_db) : scene(_scene)
        {
            double origin_lat = 0;
            double origin_lon = 0;
            SetDBPointer(_db);
            db->getOrigin(origin_lat,origin_lon);
            translate_x = proj->convertGeoToLocalX(origin_lon);
            translate_y = proj->convertGeoToLocalY(origin_lat);
            for (size_t i = 0; i < scene->faces.size(); i++)
                faces.push_back(scene->faces[i]);
        }

        void removeFaces(sfa::Polygon outline)
        {
            sfa::LineString* linestring = outline.getExteriorRing();
            if (!linestring) return;

            for (int i = 0; i < linestring->getNumPoints(); i++)
            {
                sfa::Point* p = linestring->getPointN(i);
                p->setX(p->X() + translate_x);
                p->setY(p->Y() + translate_y);
            }

            for (std::list<scenegraph::Face>::iterator it = faces.begin(); it != faces.end(); ++it)
            {
                const scenegraph::Face& face = *it;
                if (face.verts.empty()) continue;

                sfa::Point c;
                for (size_t j = 0; j < face.verts.size(); j++)
                    c = c + face.verts[j];
                c = c / face.verts.size();
                if (outline.intersects(&c))
                {
                    it = faces.erase(it);
                    if (it != faces.begin())
                        it--;
                }
            }
        }

        virtual bool ProcessRoad(fgs::Road* road) { return true; }

        virtual bool ProcessJunction(fgs::Junction *junction) 
        {
            double lat=0;
            double lon=0;
            db->ProjectGeoFromOrtho(junction->getY(), junction->getX(), lat, lon);
            cer::Ruleset rules = cer::generateRuleset(cerDocument, lat, lon, dictionary.getRoadCategory(junction->getAttributes()), dictionary.getRoadEnvironment(junction->getAttributes()));
            generator.getSettings()->setJunctionSplineDS(rules.cutfill.junctionSplineDS);
            generator.getSettings()->setJunctionSplineMode(rules.cutfill.junctionSplineMode);
            generator.getSettings()->setJunctionSplineThreshold(rules.cutfill.junctionSplineThreshold);
            sfa::Polygon polygon = generator.generate2DModel(junction, db);
            removeFaces(polygon);
            return true;
        }

        virtual bool ProcessFeature(sfa::Layer *layer, sfa::Feature *feature)
        {
            if((dictionary.getFeatureType(feature->attributes)=="roadbase")
             || (dictionary.getFeatureType(feature->attributes)=="hydro")
             || (dictionary.getFeatureType(feature->attributes)=="hydro_shore"))
            {
                sfa::Polygon* polygon = dynamic_cast<sfa::Polygon*>(feature->geometry);
                if (polygon)
                    removeFaces(*polygon);
            }
            return true;
        }

        void saveFaces(void)
        {
            scene->faces.clear();
            scene->faces.insert(scene->faces.begin(), faces.begin(), faces.end());
        }
    };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class InsertFacesProcessor : public spawn::IFeatureProcessor
    {
        modelkit::ModelGenerator mk_generator;
        std::string mk_path;
        fgs::ModelGenerator fgs_generator;
        scenegraph::Scene *scene;
        double translate_x;
        double translate_y;
        double left, right, top, bottom;
        cad::AttributeDictionary dictionary;

    public:
        dom::DocumentSP cerDocument;

        virtual bool IgnoreRoad(ccl::uuid id) { return false; }
        virtual bool IgnoreJunction(ccl::uuid id) { return false; }

        InsertFacesProcessor(
            scenegraph::Scene *_scene, 
            cts::FlatEarthProjection *proj, 
            spawn::OpenDriveDB *_db,
            double _left, double _right, double _top, double _bottom,
            const std::string& model_kit_path,
            const std::string& texture_path
            ) : scene(_scene)
        {
            double origin_lat = 0;
            double origin_lon = 0;
            SetDBPointer(_db);
            db->getOrigin(origin_lat,origin_lon);
            translate_x = proj->convertGeoToLocalX(origin_lon);
            translate_y = proj->convertGeoToLocalY(origin_lat);
            left = _left - translate_x;
            right = _right - translate_x;
            top = _top - translate_y;
            bottom = _bottom - translate_y;
            mk_path = model_kit_path;
            fgs_generator.getSettings()->setTexturePath(texture_path);
            fgs_generator.getSettings()->setJunctionSplineDS(1);

            // defaults
            fgs_generator.getRegistry()->SetDefaultTexture("1lane1way_concrete.jpg");

            fgs_generator.readRegistryAndSettings("spawn.xml");
        
        }

        void insertFaces(scenegraph::Scene* child)
        {
            if (!child) return;

            scenegraph::FlattenVisitor flattenVisitor;
            child = flattenVisitor.flatten(child);
            scenegraph::TransformVisitor transformVisitor(scene->matrix);
            child = transformVisitor.transform(child);
            scene->matrix = sfa::Matrix();

            scenegraph::Scene* cropped = scenegraph::cropScene(child, left, right, bottom, top);
             
            for (size_t i = 0; i < cropped->faces.size(); i++)
            {
                scenegraph::Face& face = cropped->faces[i];
                if (face.verts.empty()) continue;

                for (size_t j = 0; j < face.verts.size(); j++)
                    sfa::Point &p = face.verts[j];

                scene->faces.push_back(face);
            }

            delete cropped;
        }

        virtual bool ProcessRoad(fgs::Road *road)
        {
            double lat=0;
            double lon=0;
            db->ProjectGeoFromOrtho(road->getTrack()->front()->getY(), road->getTrack()->front()->getX(), lat, lon);
            cer::Ruleset rules = cer::generateRuleset(cerDocument, lat, lon, dictionary.getRoadCategory(road->getAttributes()), dictionary.getRoadEnvironment(road->getAttributes()));
            fgs::RoadTypeList roadTypeList = road->getRoadTypes();
            int roadtype = (roadTypeList.size() > 0) ? roadTypeList[0]->getType() : fgs::RoadType::UNKNOWN;
            fgs_generator.getSettings()->setRoadWidthTestDistance(rules.cutfill.roadWidthTestDistance, roadtype);
            fgs_generator.getSettings()->setRoadWidthTestThreshold(rules.cutfill.roadWidthTestThreshold, roadtype);
            fgs_generator.getSettings()->setRoadSlicesPerQuadrant(rules.cutfill.roadSlicesPerQuadrant, roadtype);

            modelkit::ModelGeneratorParameters modelGeneratorParameters;
            modelGeneratorParameters.featureAttributes = road->getAttributes();

            scenegraph::Scene* child = NULL;
            if(dictionary.getBridge(road->getAttributes()))
            {
                child = new scenegraph::Scene();
                int path_count = 1;
                path_count = road->getAttributes().getAttributeAsInt("CIlanes",true,1);
                char kitfile[1024];
                sprintf(kitfile,"Default%dLaneBridge.kit",path_count);
                sfa::LineString ortho_line = getLineFromTrack(road->getTrack(), 0, 0);
                mk_generator.BuildModelForLinearFromKIT(&ortho_line,ccl::joinPaths(mk_path,kitfile),*child, modelGeneratorParameters);
            }
            else if(dictionary.getTunnel(road->getAttributes()))
            {
                child = new scenegraph::Scene();
                int path_count = 1;
                path_count = road->getAttributes().getAttributeAsInt("CIlanes",true,1);
                char kitfile[1024];
                sprintf(kitfile,"Default%dLaneTunnel.kit",path_count);
                sfa::LineString ortho_line = getLineFromTrack(road->getTrack(), 0, 0);
                mk_generator.BuildModelForLinearFromKIT(&ortho_line,ccl::joinPaths(mk_path,kitfile),*child, modelGeneratorParameters);
            }
            else
            {
                child = fgs_generator.generate3DModel(road);
                child->matrix.PushTranslate(sfa::Point(translate_x, translate_y));
            }
            insertFaces(child);
            delete child;
            return true;
        }

        virtual bool ProcessJunction(fgs::Junction *junction)
        {
            double lat=0;
            double lon=0;
            db->ProjectGeoFromOrtho(junction->getY(), junction->getX(), lat, lon);
            cer::Ruleset rules = cer::generateRuleset(cerDocument, lat, lon, dictionary.getRoadCategory(junction->getAttributes()), dictionary.getRoadEnvironment(junction->getAttributes()));
            fgs_generator.getSettings()->setJunctionSplineDS(rules.cutfill.junctionSplineDS);
            fgs_generator.getSettings()->setJunctionSplineMode(rules.cutfill.junctionSplineMode);
            fgs_generator.getSettings()->setJunctionSplineThreshold(rules.cutfill.junctionSplineThreshold);
            scenegraph::Scene* child = fgs_generator.generate3DModel(junction, db);
            child->matrix.PushTranslate(sfa::Point(translate_x, translate_y));
            insertFaces(child);
            delete child;
            return true;
        }

        virtual bool ProcessFeature(sfa::Layer *Layer, sfa::Feature *feature)
        {
            if(!feature->attributes.getAttributeAsBool("CIhydro"))
                return true;
            scenegraph::Scene *scene = fgs_generator.generate3DHydroModel(feature, db);
            if(!scene)
                return true;
            insertFaces(scene);
            delete scene;
            return true;
        }

    };

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    class IntegrateRoadsProcessor : public spawn::IFeatureProcessor
    {
        fgs::ModelGenerator generator;
        ctl::DelaunayTriangulation *delaunay;
        cts::FlatEarthProjection *delaunayProj;
        double translate_x;
        double translate_y;
        cad::AttributeDictionary dictionary;

        int sidewalls;

    public:
        virtual ~IntegrateRoadsProcessor()
        {

        }

        virtual bool IgnoreRoad(ccl::uuid id) 
        {
            return false;
        }

        virtual bool IgnoreJunction(ccl::uuid id) 
        {
            return false;
        }

        IntegrateRoadsProcessor(ctl::DelaunayTriangulation *_delaunay, cts::FlatEarthProjection *proj, spawn::OpenDriveDB *_db) : delaunay(_delaunay),delaunayProj(proj)
        {
            sidewalls = 0;


            double origin_lat = 0;
            double origin_lon = 0;
            SetDBPointer(_db);
            db->getOrigin(origin_lat,origin_lon);
            translate_x = proj->convertGeoToLocalX(origin_lon);
            translate_y = proj->convertGeoToLocalY(origin_lat);
        }

        virtual bool ProcessRoad(fgs::Road *road)
        {
            /* //insert only road-bases
            sfa::Polygon outline = generator.generate2DModel(road);
            if(outline.isEmpty())
                return true;
            sfa::LineString *line = outline.getExteriorRing();
            if(line)
            {
                ctl::PointList pl;
                for(int i=0,ic=line->getNumPoints();i<ic;i++)
                {
                    sfa::Point *sfaPoint = line->getPointN(i);
                    ctl::Point pt(sfaPoint->X()+translate_x,sfaPoint->Y()+translate_y,sfaPoint->Z());
                    pl.push_back(pt);
                }
                delaunay->InsertConstrainedLineString(pl);
            }
            */
            return true;
        }

        virtual bool ProcessJunction(fgs::Junction *junction)
        {
            //insert only road-bases
            return true;
        }

        virtual bool ProcessFeature(sfa::Layer *layer, sfa::Feature *feature)
        {
            if(dictionary.getFeatureType(feature->attributes)=="roadbase")
            {
                sfa::Polygon *outline = dynamic_cast<sfa::Polygon *>(feature->geometry);
                if(!outline)
                    return true;
                if(outline->isEmpty())
                    return true;
                sfa::LineString *line = outline->getExteriorRing();
                if(line)
                {
                    line->removeColinearPoints(0.05);
                    ctl::PointList pl;
                    for(int i=0,ic=line->getNumPoints();i<ic;i++)
                    {
                        sfa::Point *sfaPoint = line->getPointN(i);
                        ctl::Point pt(sfaPoint->X()+translate_x,sfaPoint->Y()+translate_y,sfaPoint->Z());
                        pl.push_back(pt);
                    }
                    delaunay->InsertConstrainedLineString(pl);
                    if (delaunay->error())
                        log << "Error inserting roadbase" << log.endl;
                    delaunay->RemoveWorkingPoints(pl);
                }
            }
            else if(dictionary.getFeatureType(feature->attributes)=="sidewall")
            {
    #if 1
                sidewalls++;

                sfa::Polygon *outline = dynamic_cast<sfa::Polygon *>(feature->geometry);
                if(!outline)
                    return true;
                if(outline->isEmpty())
                    return true;
                if (abs(outline->getArea()) < 10)
                    return true;

                sfa::LineString *line = outline->getExteriorRing();
                if(line)
                {
                    ctl::PointList pl;
                    for(int i=0,ic=line->getNumPoints();i<ic;i++)
                    {
                        sfa::Point *sfaPoint = line->getPointN(i);
                        ctl::Point pt(sfaPoint->X()+translate_x,sfaPoint->Y()+translate_y,sfaPoint->Z());
                        pl.push_back(pt);
                    }
                    //TODO: mark faces for retexturing
                    delaunay->InsertConstrainedLineString(pl);
                    if (delaunay->error())
                        log << "Error inserting sidewall" << log.endl;
                    delaunay->RemoveWorkingPoints(pl);
                }
    #endif
            }
            else if(dictionary.getFeatureType(feature->attributes)=="hole")
            {
                sfa::Polygon *outline = dynamic_cast<sfa::Polygon *>(feature->geometry);
                if(!outline)
                    return true;
                if(outline->isEmpty())
                    return true;
                sfa::LineString *line = outline->getExteriorRing();
                if(line)
                {
                    ctl::PointList pl;
                    for(int i=0,ic=line->getNumPoints();i<ic;i++)
                    {
                        sfa::Point *sfaPoint = line->getPointN(i);
                        ctl::Point pt(sfaPoint->X()+translate_x,sfaPoint->Y()+translate_y,sfaPoint->Z());
                        pl.push_back(pt);
                    }
                    //TODO: Mark faces for removal
    #if 0
                    delaunay->InsertConstrainedPolygon(pl);
    #endif
                }
            }
            else if((dictionary.getFeatureType(feature->attributes)=="hydro") || (dictionary.getFeatureType(feature->attributes)=="hydro_shore"))
            {
                sfa::Polygon *outline = dynamic_cast<sfa::Polygon *>(feature->geometry);
                if(!outline)
                    return true;
                if(outline->isEmpty())
                    return true;
                sfa::LineString *line = outline->getExteriorRing();
                if(line)
                {
                    line->removeColinearPoints(0.05);
                    ctl::PointList pl;
                    for(int i=0,ic=line->getNumPoints();i<ic;i++)
                    {
                        sfa::Point *sfaPoint = line->getPointN(i);
                        ctl::Point pt(sfaPoint->X()+translate_x,sfaPoint->Y()+translate_y,sfaPoint->Z());
                        pl.push_back(pt);
                    }
                    delaunay->InsertConstrainedLineString(pl);
                    if (delaunay->error())
                        log << "Error inserting hydro" << log.endl;
                    delaunay->RemoveWorkingPoints(pl);
                }
            }
            return true;
        }

    };
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    TerrainGenerator::~TerrainGenerator(void)
    {
    }


    TerrainGenerator::TerrainGenerator(void) :
        north(DBL_MAX), south(-DBL_MAX), east(DBL_MAX), west(-DBL_MAX),
        localNorth(DBL_MAX), localSouth(-DBL_MAX), localEast(DBL_MAX), localWest(-DBL_MAX),
        /*modelKitPath("modelkits/"), texturePath("textures/"), */outputPath("output/"), 
        textureWidth(1024), textureHeight(1024), texelSize(5.0f),
        elevationDSM(100 * 1024 * 1024), elevationSampler(&elevationDSM, elev::ELEVATION_BILINEAR)
    {
        logger.init("TerrainGenerator");
        //InitODDBSQLite();            
        rasterExtensions.insert("tif");
        rasterExtensions.insert("sid");
        rasterExtensions.insert("ecw");
    }

    void TerrainGenerator::setOrigin(double lat, double lon)
    {
        double oldLat = flatEarth.getOriginLatitude();
        double oldLon = flatEarth.getOriginLongitude();
        flatEarth.setOrigin(lat, lon);
        if(((oldLat != lat) || (oldLon != lon)) && ((oldLat != 0.0f) || (oldLon != 0.0f)))
            logger << ccl::LWARNING << "WARNING: origin change (OLD: " << oldLat << ", " << oldLon << ")" << logger.endl;
        logger << ccl::LINFO << "setOrigin(" << lat << ", " << lon << ")" << logger.endl;
    }

    void TerrainGenerator::setBounds(double north, double south, double west, double east)
    {
        this->north = north;
        this->south = south;
        this->west = west;
        this->east = east;
        localNorth = flatEarth.convertGeoToLocalY(north);
        localSouth = flatEarth.convertGeoToLocalY(south);
        localWest = flatEarth.convertGeoToLocalX(west);
        localEast = flatEarth.convertGeoToLocalX(east);
        logger << ccl::LINFO << "setBounds(): N:" << north << "(" << localNorth << ") S:" << south << "(" << localSouth << ") W:" << west << "(" << localWest << ") E:" << east << "(" << localEast << ")" << logger.endl;
    }

    void TerrainGenerator::addImageryPath(const std::string &imageryPath)
    {
        rasterSampler.AddDirectory(imageryPath, rasterExtensions);
        logger << ccl::LINFO << "addImageryPath(" << imageryPath << ")" << logger.endl;
    }

    void TerrainGenerator::addElevationFile(const std::string &elevationFile)
    {
        elevationDSM.AddFile_Raster_GDAL(elevationFile);
        logger << ccl::LINFO << "addElevationFile(" << elevationFile << ")" << logger.endl;
    }
    

    void TerrainGenerator::setOutputPath(const std::string &outputPath)
    {
        this->outputPath = outputPath;
        logger << ccl::LINFO << "setOutputPath(" << outputPath << ")" << logger.endl;
    }

    void TerrainGenerator::setTextureSize(int width, int height)
    {
        this->textureWidth = width;
        this->textureHeight = height;
        logger << ccl::LINFO << "setTextureSize(" << width << ", " << height << ")" << logger.endl;
    }

    void TerrainGenerator::setTexelSize(double texelSize)
    {
        this->texelSize = texelSize;
        logger << ccl::LINFO << "setTexelSize(" << texelSize << ")" << logger.endl;
    }

    void TerrainGenerator::generate(int row, int col)
    {
        master.externalReferences.clear();
        int rows = ceil((localNorth - localSouth) / (textureHeight * texelSize));
        for(int r = 0; r < rows; ++r)
        {
            if((row == -1) || (row == r))
                generateRow(r, col);
        }
        scenegraph::buildOpenFlightFromScene(ccl::joinPaths(outputPath, "master.flt"), &master);
        master.externalReferences.clear();
    }

    void TerrainGenerator::generateRow(int row, int col)
    {
        int cols = ceil((localEast - localWest) / (textureWidth * texelSize));
        for(int c = 0; c < cols; ++c)
        {
            if((col == -1) || (col == c))
                generateRowColumn(row, c);
        }
    }

    void TerrainGenerator::generateRowColumn(int row, int col)
    {
        std::string tileName = "tile_" + boost::lexical_cast<std::string>(row) + "_" + boost::lexical_cast<std::string>(col);
        std::string jpgFilename = ccl::joinPaths(outputPath, tileName + ".jpg");
        std::string attrFilename = jpgFilename + ".attr";

        logger << ccl::LINFO << "Generating Tile (" << row << ", " << col << ")" << logger.endl;

        double tileLocalNorth = localNorth - (row * (textureHeight * texelSize));
        double tileLocalSouth = tileLocalNorth - (textureHeight * texelSize);
        double tileWorldNorth = flatEarth.convertLocalToGeoLat(tileLocalNorth);
        double tileWorldSouth = flatEarth.convertLocalToGeoLat(tileLocalSouth);
        double tileLocalHeight = tileLocalNorth - tileLocalSouth;

        double tileLocalWest = localWest + (col * (textureWidth * texelSize));
        double tileLocalEast = tileLocalWest + (textureWidth * texelSize);
        double tileWorldWest = flatEarth.convertLocalToGeoLon(tileLocalWest);
        double tileWorldEast = flatEarth.convertLocalToGeoLon(tileLocalEast);
        double tileLocalWidth = tileLocalEast - tileLocalWest;

        // TODO: Add the ability to introspect the elevation sampler to get the range of
        //       elevation post space.
        double postSpacingX = 30.0f;
        double postSpacingY = 30.0f;

        // create texture
        {
            gdalsampler::GeoExtents window;
            window.north = tileWorldNorth;
            window.south = tileWorldSouth;
            window.west = tileWorldWest;
            window.east = tileWorldEast;
            window.filename = tileName;
            window.width = textureWidth;
            window.height = textureHeight;
            int len = window.height * window.width * 3;
            u_char *buf = new u_char[len];
            memset(buf, 255, len);
            ip::ImageInfo info;
            info.width = window.width;
            info.height = window.height;
            info.depth = 3;
            info.interleaved = true;
            info.dataType = ip::ImageInfo::UBYTE;
            ccl::binary buffer;
            buffer.resize(len);
            rasterSampler.Sample(window,buf);
            for(int i = 0; i < len; ++i) 
                buffer[i] = buf[i];
            ip::WriteJPG24(jpgFilename, info, buffer);
            ip::attrFile attr;
            attr.wrapMode = ip::attrFile::WRAP_CLAMP;
            attr.wrapMode_u = ip::attrFile::WRAP_CLAMP;
            attr.wrapMode_v = ip::attrFile::WRAP_CLAMP;
            attr.Write(attrFilename);
            delete buf;
        }

        ctl::PointList gamingArea;
        {
            double z = 0;
            ctl::Point southwest(tileLocalWest, tileLocalSouth, getZ(tileLocalWest, tileLocalSouth));
            ctl::Point southeast(tileLocalEast, tileLocalSouth, getZ(tileLocalEast, tileLocalSouth));
            ctl::Point northeast(tileLocalEast, tileLocalNorth, getZ(tileLocalEast, tileLocalNorth));
            ctl::Point northwest(tileLocalWest, tileLocalNorth, getZ(tileLocalWest, tileLocalNorth));
            gamingArea.push_back(southwest);
            gamingArea.push_back(southeast);
            gamingArea.push_back(northeast);
            gamingArea.push_back(northwest);
        }

        ctl::PointList boundaryPoints;
        {
            sfa::LineString boundaryLineString;
            for(double x = tileLocalWest + postSpacingX; x < tileLocalEast; x += postSpacingX)
                boundaryLineString.addPoint(sfa::Point(x, tileLocalNorth, getZ(x, tileLocalNorth)));
            for(double y = tileLocalSouth + postSpacingY; y < tileLocalNorth; y += postSpacingY)
                boundaryLineString.addPoint(sfa::Point(tileLocalWest, y, getZ(tileLocalWest, y)));
            for(double x = tileLocalWest + postSpacingX; x < tileLocalEast; x += postSpacingX)
                boundaryLineString.addPoint(sfa::Point(x, tileLocalSouth, getZ(x, tileLocalSouth)));
            for(double y = tileLocalSouth + postSpacingY; y < tileLocalNorth; y += postSpacingY)
                boundaryLineString.addPoint(sfa::Point(tileLocalEast, y, getZ(tileLocalEast, y)));
            boundaryLineString.removeColinearPoints(0, 0.5);
            for(int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
            {
                sfa::Point *p = boundaryLineString.getPointN(i);
                boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
            }
        }

        ctl::PointList workingPoints;
        int delaunayResizeIncrement = 100;
        {
            int postRows = ceil((tileLocalNorth - tileLocalSouth) / postSpacingY);
            int postCols = ceil((tileLocalEast - tileLocalWest) / postSpacingX);
            for(int row = 0; row < postRows; ++row)
            {
                double tileY = tileLocalSouth + (postSpacingY * row);
                if((tileY == tileLocalNorth) || (tileY == tileLocalSouth))
                    continue;    //The boundary constraints already handle this

                sfa::Point pt;
                double lat = flatEarth.convertLocalToGeoLat(tileY);
                pt.setY(lat);
                for(int col = 0; col < postCols; ++col)
                {
                    double tileX = tileLocalWest + (postSpacingX * col);
                    if((tileX == tileLocalWest) || (tileX == tileLocalEast))
                        continue;    //The boundary constraints already handle this

                    double lon = flatEarth.convertLocalToGeoLon(tileX);
                    pt.setX(lon);
                    if(elevationSampler.Get(&pt))
                        workingPoints.push_back(ctl::Point(tileX, tileY, pt.Z()));
                }
            }    

            delaunayResizeIncrement = (postRows * postCols) / 8;
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
            while(i < boundaryPoints.size() || j < workingPoints.size())
            {
                if(i < boundaryPoints.size())
                    dt->InsertConstrainedPoint(boundaryPoints[i++]);
                if(j < workingPoints.size())
                    dt->InsertWorkingPoint(workingPoints[j++]);
            }
        }

        /*
        for(std::set<std::string>::iterator oddbIT = oddbFilenames.begin(), oddbEND = oddbFilenames.end(); oddbIT != oddbEND; ++oddbIT)
        {
            std::string oddbFilename = *oddbIT;
            spawn::OpenDriveDB *oddb = new spawn::OpenDriveDB;
            if(!oddb->open(oddbFilename))
            {
                logger << ccl::LERR << "failed to open " + oddbFilename + "." << logger.endl;
                continue;
            }

            double oddbNorth, oddbSouth, oddbWest, oddbEast;
            oddb->ProjectOrthoFromGeo(tileWorldSouth, tileWorldWest, oddbSouth, oddbWest);
            oddb->ProjectOrthoFromGeo(tileWorldNorth, tileWorldEast, oddbNorth, oddbEast);

            logger << "Running IntegrateRoadsProcessor" << logger.endl;
            IntegrateRoadsProcessor integrate_proc(dt,&flatEarth,oddb);
            integrate_proc.SetDBPointer(oddb);
            oddb->ProcessFeaturesInEnvelope(&integrate_proc, oddbNorth, oddbSouth, oddbWest, oddbEast);
        }
        */
        dt->Simplify(1, float(0.05));    // simplify based on coplanar points
        dt->Simplify(20000, 0.5);        // if we still have over 20k triangles, simplify using the triangle budget

        ctl::TIN *tin = new ctl::TIN(dt);
        scenegraph::Scene *scene = new scenegraph::Scene;
        scene->faces.reserve(tin->triangles.size() / 3);
        for(size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
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
            mt.uvs.push_back(sfa::Point((sfaA.X() - tileLocalWest) / tileLocalWidth, (tileLocalNorth - sfaA.Y()) / tileLocalHeight));
            mt.uvs.push_back(sfa::Point((sfaB.X() - tileLocalWest) / tileLocalWidth, (tileLocalNorth - sfaB.Y()) / tileLocalHeight));
            mt.uvs.push_back(sfa::Point((sfaC.X() - tileLocalWest) / tileLocalWidth, (tileLocalNorth - sfaC.Y()) / tileLocalHeight));
            face.textures.push_back(mt);

            scene->faces.push_back(face);
        }
       /*
        for(std::set<std::string>::iterator oddbIT = oddbFilenames.begin(), oddbEND = oddbFilenames.end(); oddbIT != oddbEND; ++oddbIT)
        {
            std::string oddbFilename = *oddbIT;
            spawn::OpenDriveDB *oddb = new spawn::OpenDriveDB;
            if(!oddb->open(oddbFilename))
            {
                logger << ccl::LERR << "failed to open " + oddbFilename + "." << logger.endl;
                continue;
            }

            double oddbNorth, oddbSouth, oddbWest, oddbEast;
            oddb->ProjectOrthoFromGeo(tileWorldSouth, tileWorldWest, oddbSouth, oddbWest);
            oddb->ProjectOrthoFromGeo(tileWorldNorth, tileWorldEast, oddbNorth, oddbEast);

            logger << "Running RemoveFacesProcessor" << logger.endl;
            RemoveFacesProcessor remove_proc(scene, &flatEarth, oddb);
            remove_proc.cerDocument = cerDocument;
            oddb->ProcessFeaturesInEnvelope(&remove_proc, oddbNorth, oddbSouth, oddbWest, oddbEast);
            oddb->ProcessJunctionsInEnvelope(&remove_proc, oddbNorth, oddbSouth, oddbWest, oddbEast);
            remove_proc.saveFaces();

            logger << "Running InsertFacesProcessor" << logger.endl;
            InsertFacesProcessor insert_proc(scene, &flatEarth, oddb, tileLocalWest, tileLocalEast, tileLocalNorth, tileLocalSouth, modelKitPath, texturePath);
            insert_proc.cerDocument = cerDocument;
            oddb->ProcessRoadsInEnvelope(&insert_proc, oddbNorth, oddbSouth, oddbWest, oddbEast);
            oddb->ProcessJunctionsInEnvelope(&insert_proc, oddbNorth, oddbSouth, oddbWest, oddbEast);
            oddb->ProcessFeaturesInEnvelope(&insert_proc, oddbNorth, oddbSouth, oddbWest, oddbEast);

        }
        */
        std::string fltfilename = ccl::joinPaths(outputPath, tileName + std::string(".flt"));
        logger << "Writing " << fltfilename << "..." << logger.endl;
        scenegraph::buildOpenFlightFromScene(fltfilename, scene);
        scenegraph::ExternalReference ext;
        ext.scale = sfa::Point(1.0, 1.0, 1.0);
        ext.filename = fltfilename;
        master.externalReferences.push_back(ext);

        delete scene;
        delete tin;
        delete dt;
    }

    double TerrainGenerator::getZ(double x, double y)
    {
        sfa::Point p(flatEarth.convertLocalToGeoLon(x), flatEarth.convertLocalToGeoLat(y));
        return elevationSampler.Get(&p) ? p.Z() : 0.0f;
    }


}

