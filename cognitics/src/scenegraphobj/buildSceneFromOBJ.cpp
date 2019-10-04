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

#include "scenegraphflt/scenegraphflt.h"
#include "ccl/StringUtils.h"
#include <scenegraph/LOD.h>
#include <boost/tokenizer.hpp>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <dom/dom.h>
#include <boost/algorithm/string.hpp>

namespace scenegraph
{
    class OBJSceneBuilder
    {
        ccl::ObjLog log;
        std::string filename;
        bool setTexturePath;
        std::string objFilePath;
        

        std::vector<sfa::Point> verts;
        std::vector<sfa::Point> uvs;
        std::vector<sfa::Point> vertexNormals;

        //MappedTextureList mappedTextures;
        //MaterialList materials;
        Material currentMaterial;
        std::map<std::string, Material> materialMap;
        
    public:
        Scene *rootScene;
        Scene *currentScene;
        OBJSceneBuilder(const std::string &filename, bool setTexturePath) : filename(filename), setTexturePath(setTexturePath), rootScene(NULL), currentScene(NULL) 
        {
            ccl::FileInfo fileinfo(filename);
            objFilePath = fileinfo.getDirName();
            log.init("OBJSceneBuilder", this);
            log << ccl::LINFO;
            rootScene = currentScene = new scenegraph::Scene();
        }

        ~OBJSceneBuilder(void)
        {
            delete currentScene;
        }

        bool parseMtlFile(const std::string &mtlFilename)
        {
            Material material;
            std::string matName = "";
            std::ifstream infile;
            infile.open(mtlFilename, std::ios::in);
            std::string line;
            while (std::getline(infile, line))
            {
                line = ccl::trim_string(line);
                if (line.empty())
                {
                    continue;
                }
                std::vector<std::string> parts = ccl::splitString(line, " ");
                if (parts.size() < 2)
                {
                    continue;
                }
                if (parts[0] == "newmtl")
                {
                    if (matName.size() > 0)
                    {
                        materialMap[matName] = material;
                        material = Material();
                    }
                    matName = parts[1];
                }
                //Don't parse anything else until we have a material.
                else if (matName.size() > 0)
                {
                    if (parts[0] == "Ka")
                    {
                        if (parts.size() > 3)
                        {
                            if (parts[1] == "spectral")
                            {
                                log << "Spectral not a supported ambient value." << log.endl;
                            }
                            else if (parts[1] == "xyz")
                            {
                                log << "xyz not a supported ambient value." << log.endl;
                            }
                            else
                            {
                                double r = atof(parts[1].c_str());
                                double g = atof(parts[2].c_str());
                                double b = atof(parts[3].c_str());
                                material.ambient = Color(r, g, b);
                            }
                        }
                        else
                        {
                            log << "Not enough values for ambient [Ka] material record." << log.endl;
                        }
                    }
                    if (parts[0] == "Kd")
                    {
                        if (parts.size() > 3)
                        {
                            if (parts[1] == "spectral")
                            {
                                log << "Spectral not a supported diffuse value." << log.endl;
                            }
                            else if (parts[1] == "xyz")
                            {
                                log << "xyz not a supported diffuse value." << log.endl;
                            }
                            else
                            {
                                double r = atof(parts[1].c_str());
                                double g = atof(parts[2].c_str());
                                double b = atof(parts[3].c_str());
                                material.diffuse = Color(r, g, b);
                            }
                        }
                        else
                        {
                            log << "Not enough values for ambient [Ka] material record." << log.endl;
                        }
                    }
                    if (parts[0] == "Ks")
                    {
                        if (parts.size() > 3)
                        {
                            if (parts[1] == "spectral")
                            {
                                log << "Spectral not a supported specular value." << log.endl;
                            }
                            else if (parts[1] == "xyz")
                            {
                                log << "xyz not a supported specular value." << log.endl;
                            }
                            else
                            {
                                double r = atof(parts[1].c_str());
                                double g = atof(parts[2].c_str());
                                double b = atof(parts[3].c_str());
                                material.specular = Color(r, g, b);
                            }
                        }
                        else
                        {
                            log << "Not enough values for ambient [Ka] material record." << log.endl;
                        }
                    }
                    if (parts[0] == "d")
                    {
                        //Dissolve unsupported
                    }
                    if (parts[0] == "Ns")
                    {
                        //Specular Exponent Unsupported
                    }
                    if (parts[0] == "illum")
                    {
                        /*
                         0        Color on and Ambient off
                         1        Color on and Ambient on
                         2        Highlight on
                         3        Reflection on and Ray trace on
                         4        Transparency: Glass on
                                  Reflection: Ray trace on
                         5        Reflection: Fresnel on and Ray trace on
                         6        Transparency: Refraction on
                                  Reflection: Fresnel off and Ray trace on
                         7        Transparency: Refraction on
                                  Reflection: Fresnel on and Ray trace on
                         8        Reflection on and Ray trace off
                         9        Transparency: Glass on
                                  Reflection: Ray trace off
                         10       Casts shadows onto invisible surfaces
                        */
                    }
                    if (parts[0] == "map_Kd")
                    {
                        //Full path, relative to the mtl file (and the obj)
                        material.textureFile = ccl::joinPaths(objFilePath,parts[1]);
                    }
                }
            }
            //Save the last one.
            if (matName.size() > 0)
            {
                materialMap[matName] = material;
            }
            return true;
        }


        bool build(void)
        {

            std::ifstream infile;
            infile.open(filename, std::ios::in);
            std::string line;
            while (std::getline(infile, line))
            {
                line = ccl::trim_string(line);
                if (line.empty())
                {
                    //New group?
                    continue;
                }
                std::vector<std::string> parts = ccl::splitString(line, " ");
                if (parts.size()==0)
                {
                    continue;
                }
                //Vertex
                if (parts[0] == "v")
                {
                    if (parts.size() > 3)
                    {
                        sfa::Point v(atof(parts[1].c_str()), atof(parts[2].c_str()), atof(parts[3].c_str()));
                        verts.push_back(v);
                    }
                    else
                    {
                        log << ccl::LERR << "Not enough value for [v] in OBJ file: (" <<
                            line << ")" << log.endl;
                    }
                }
                //Vertex Normal
                if (parts[0] == "vn")
                {
                    if (parts.size() > 3)
                    {
                        sfa::Point v(atof(parts[1].c_str()), atof(parts[2].c_str()), atof(parts[3].c_str()));
                        vertexNormals.push_back(v);
                    }
                    else
                    {
                        log << ccl::LERR << "Not enough value for [vn] in OBJ file: (" <<
                            line << ")" << log.endl;
                    }
                }
                //Texture coordinate (uv)
                else if (parts[0] == "vt")
                {
                    if (parts.size() > 2)
                    {
                        sfa::Point vt(atof(parts[1].c_str()), atof(parts[2].c_str()));
                        uvs.push_back(vt);
                    }
                    else
                    {
                        log << ccl::LERR << "Not enough value for [vt] in OBJ file: (" <<
                            line << ")" << log.endl;
                    }
                }
                //Polygon/Face
                else if (parts[0] == "f")
                {
                    Face face;
                    bool hasTextureCood = false;
                    MappedTexture mt;
                    // OBJ only supports one texture.
                    if (currentMaterial.textureFile != "")
                    {
                        mt.SetTextureName(currentMaterial.textureFile);
                        face.addMappedTexture(mt);
                        hasTextureCood = true;
                    }
                    for (size_t i = 1, ic = parts.size(); i < ic; i++)
                    {
                        std::vector<std::string> coords = ccl::splitString(parts[i],"/");
                        int v = atoi(coords[0].c_str());
                        // Make sure the vert index is valid
                        if (verts.size() > v)
                        {
                            face.addVert(verts[v]);
                        }
                        int vt = -1;
                        int vn = -1;
                        if (coords.size() > 1)
                        {
                            vt = atoi(coords[1].c_str());
                            if (uvs.size() > vt)
                            {
                                if (hasTextureCood)
                                {
                                    face.textures[0].uvs.push_back(uvs[vt]);
                                }
                            }
                        }
                        if (coords.size() > 2)
                        {
                            vn = atoi(coords[2].c_str());
                            if (vertexNormals.size() > vn)
                            {
                                face.setNormalN(i, vertexNormals[vn]);
                            }
                        }
                    }
                    currentScene->faces.push_back(face);
                }
                else if (parts[0] == "mtllib" && parts.size() > 1)
                {
                    std::string mtlPath = ccl::joinPaths(objFilePath, parts[1]);
                    parseMtlFile(mtlPath);
                }
                else if (parts[0] == "usemtl" && parts.size() > 1)
                {
                    if (materialMap.find(parts[1]) != materialMap.end())
                    {
                        currentMaterial = materialMap[parts[1]];
                    }
                }
                else if (parts[0] == "")
                {

                }

            }
            return false;
        }

    };

    Scene *buildSceneFromOBJ(const std::string &filename, bool setTexturePath)
    {
        OBJSceneBuilder builder(filename, setTexturePath);
        return builder.build() ? builder.rootScene : NULL;
    }

}