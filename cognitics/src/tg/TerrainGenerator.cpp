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
    

    TerrainGenerator::~TerrainGenerator(void)
    {
    }


    TerrainGenerator::TerrainGenerator(void) :
        north(DBL_MAX), south(-DBL_MAX), east(DBL_MAX), west(-DBL_MAX),
        localNorth(DBL_MAX), localSouth(-DBL_MAX), localEast(DBL_MAX), localWest(-DBL_MAX),
        outputPath("output/"), 
        textureWidth(1024), textureHeight(1024), texelSize(5.0f),
        elevationDSM(100 * 1024 * 1024), elevationSampler(&elevationDSM, elev::ELEVATION_BILINEAR)
    {
        logger.init("TerrainGenerator");
        rasterExtensions.insert("jp2");
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

