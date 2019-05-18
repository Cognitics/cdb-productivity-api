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
#pragma once

#include <ccl/ObjLog.h>
#include <cts/FlatEarthProjection.h>
#include <ip/GDALRasterSampler.h>
#include <elev/Elevation.h>
#include <elev/DataSourceManager.h>
#include <elev/Elevation_DSM.h>
#include <scenegraph/Scene.h>
#include <dom/dom.h>
//#include <cer/cer.h>

namespace cognitics
{
    class TerrainGenerator
    {
    public:
        ~TerrainGenerator(void);
        TerrainGenerator(void);

        void setOrigin(double lat, double lon);
        void setBounds(double north, double south, double west, double east);
        void addImageryPath(const std::string &imageryPath);
        void addElevationFile(const std::string &elevationFile);
        void setOutputPath(const std::string &outputPath);
        void setTextureSize(int width, int height);
        void setTexelSize(double texelSize);
        void generate(int row = -1, int col = -1);

    private:
        ccl::ObjLog logger;
        cts::FlatEarthProjection flatEarth;
        GDALRasterSampler rasterSampler;
        std::set<std::string> rasterExtensions;
        std::set<std::string> oddbFilenames;
        elev::DataSourceManager elevationDSM;
        elev::Elevation_DSM elevationSampler;
        double north, south, east, west;
        double localNorth, localSouth, localEast, localWest;
        std::string modelKitPath;        // "modelkits/"
        std::string texturePath;        // "textures/"
        std::string outputPath;            // "output/"
        int textureHeight;                // 1024
        int textureWidth;                // 1024
        double texelSize;                // 5.0f
        scenegraph::Scene master;
        dom::DocumentSP cerDocument;

        void generateRow(int row, int col = -1);
        void generateRowColumn(int row, int col);
        double getZ(double x, double y);

    };

}
