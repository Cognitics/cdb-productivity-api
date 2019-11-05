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

        float offsetX;
        float offsetY;
        float offsetZ;

        std::vector<QuickVert> verts;
        std::vector<QuickVert> norms;
        std::vector<QuickVert> uvs;
        std::vector<ushort> vertIdxs;
        std::vector<ushort> uvIdxs;
        std::vector<ushort> normIdxs;
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
        QuickObj(const std::string &objFilename,
        float offsetX=0, float offsetY=0, float offsetZ=0,
        const std::string &textureDirectory="", 
        bool loadTextures=false);

        void getBounds(float &minX, float &maxX,
                       float &minY, float &maxY,
                       float &minZ, float &maxZ);
        bool glRender();
        bool isValid() { return _isValid;}
    };


}