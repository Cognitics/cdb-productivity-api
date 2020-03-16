/*************************************************************************
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
#include <string>
#include <vector>
#include <ccl/ObjLog.h>

#include <sfa/Point.h>

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#include "CoordinateSystems/EllipsoidTangentPlane.h"

class ObjSrs
{
public:
    std::string srsWKT;
    sfa::Point offsetPt;
    sfa::Point geoOrigin;

    std::string ToString() const
    {
        std::stringstream ss;
        ss.precision(6);
        ss << "\t<SRS ";
        ss << "offset-x=\"" << offsetPt.X() << "\" ";
        ss << "offset-y=\"" << offsetPt.Y() << "\" ";
        ss << "offset-z=\"" << offsetPt.Z() << "\" ";
        ss << "srsWKT=\"" << srsWKT << "\" ";
        ss << "/> ";
        return ss.str();
    }

    bool getOrigin(double &lat, double &lon)
    {
        if(srsWKT.length())
        {
            OGRSpatialReference wgs;
            OGRCoordinateTransformation* coordTrans;
            wgs.SetFromUserInput("WGS84");
            OGRSpatialReference *file_srs = new OGRSpatialReference;
            const char *prjstr = srsWKT.c_str();
            OGRErr err = file_srs->importFromWkt((char **)&prjstr);
            if (err != OGRERR_NONE)
            {
                delete file_srs;
                return false;
            }
            coordTrans = OGRCreateCoordinateTransformation(file_srs, &wgs);
            double x = offsetPt.X();
            double y = offsetPt.Y();
            double z = offsetPt.Z();
            //Use the offset for the origin
            coordTrans->Transform(1, &x, &y, &z);
            lat = y;
            lon = x;
            
            auto ltp_ellipsoid = new Cognitics::CoordinateSystems::EllipsoidTangentPlane(lat, lon);
            geoOrigin.setX(lon);
            geoOrigin.setY(lat);
            delete file_srs;
            return true;
        }
        else
        {
            lat = this->geoOrigin.Y();
            lon = this->geoOrigin.X();
        }
        return true;
    }
};

namespace cognitics {

    class Color
    {
        bool initialized;
    public:
        double r;
        double g;
        double b;
        double a;
        Color();
        Color(double R, double G, double B, double A=1.0); 
        virtual ~Color();

        bool operator==(const Color &rhs) const;
        bool operator<(const Color &rhs) const;
        bool operator>(const Color &rhs) const;
        bool isInitialized() const;
    }; //Color : class

    class Material
    {
    public:
        Color ambient;
        Color diffuse;
        Color specular;
        Color emission;
        double shine;
        double transparency;
		std::string textureFile;
        bool transparent;
        int illumination;
        std::string mapDiffuse;

        Material();
        virtual ~Material();

        std::string toString() const
        {
            std::stringstream ss;
            ss << "A(" << ambient.r << "," << ambient.g << "," << ambient.b << "," << ambient.a << ")"
                << ":D(" << diffuse.r << "," << diffuse.g << "," << diffuse.b << "," << diffuse.a << ")"
                << ":S(" << specular.r << "," << specular.g << "," << specular.b << "," << specular.a << ")"
                << ":E(" << emission.r << "," << emission.g << "," << emission.b << "," << emission.a << ")"
                << ":shine(" << shine << ")"
                << ":transparency(" << transparency << ")";

            return ss.str();
        }

        bool operator==(const Material &rhs) const;

        bool operator<(const Material &rhs) const;

    }; //Material : class
    
    typedef std::vector<Material> MaterialList;

    typedef struct  {
        float x;
        float y;
        float z;
    } QuickVert;

    class QuickSubMesh
    {
    public:
        std::vector<uint32_t> vertIdxs;
        std::vector<uint32_t> uvIdxs;
        std::vector<uint32_t> normIdxs;
        std::string materialName;
    };

    /*
        QuickObj makes a bunch of assumptions, such as each 
        face being a triangle.
    */
    class QuickObj
    {
        ccl::ObjLog log;
        float minX;
        float maxX;

        float minY;
        float maxY;

        float minZ;
        float maxZ;

        ObjSrs srs;
        std::vector <QuickSubMesh> subMeshes;
        std::vector<QuickVert> verts;
        std::vector<QuickVert> norms;
        std::vector<QuickVert> uvs;
        std::vector<uint32_t> vertIdxs;
        std::vector<uint32_t> uvIdxs;
        std::vector<uint32_t> normIdxs;
        std::string objFilename;
        std::string textureFilename;
        std::string textureDirectory;
        std::string materialFilename;
        std::string materialName;

        Material currentMaterial;
        std::map<std::string, Material> materialMap;

        bool _isValid;
        bool parseMtlFile(const std::string &mtlFilename);

		std::map<std::string,unsigned int> textures;
		uint32_t getOrLoadTextureID(const std::string &texname);
        uint32_t getOrLoadDDSTextureID(const std::string &texname);

    public:
        ~QuickObj();
        QuickObj(const std::string &objFilename, const ObjSrs &_srs,
                const std::string &textureDirectory="", 
                bool loadTextures=false);

        void getBounds(float &minX, float &maxX,
                       float &minY, float &maxY,
                       float &minZ, float &maxZ);
        bool glRender();
        bool isValid() { return _isValid;}

        bool transform(Cognitics::CoordinateSystems::EllipsoidTangentPlane *_etp,
            OGRCoordinateTransformation *_coordTrans,
            const sfa::Point &_offset);
    };


}