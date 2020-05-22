#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#endif

#include "scenegraphobj/quickobj.h"
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <ccl/FileInfo.h>
#include <ccl/StringUtils.h>

#include <fstream>
#include <GL/glew.h>
#include <GL/gl.h>
#define FREEGLUT_LIB_PRAGMAS 0




#include <GL/freeglut.h>
#include <ip/jpgwrapper.h>
#include "ip/pngwrapper.h"
#include <errno.h>

#ifndef GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_2BPPV1_IMG 0x8C01
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_2BPPV1_IMG 0x8C03
#endif
#ifndef GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGB_PVRTC_4BPPV1_IMG 0x8C00
#endif
#ifndef GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG
#define GL_COMPRESSED_RGBA_PVRTC_4BPPV1_IMG 0x8C02
#endif

// S3TC/DXT (GL_EXT_texture_compression_s3tc) : Most desktop/console gpus
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT1_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT1_EXT 0x83F1
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT3_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT3_EXT 0x83F2
#endif
#ifndef GL_COMPRESSED_RGBA_S3TC_DXT5_EXT
#define GL_COMPRESSED_RGBA_S3TC_DXT5_EXT 0x83F3
#endif

// ATC (GL_AMD_compressed_ATC_texture) : Qualcomm/Adreno based gpus
#ifndef ATC_RGB_AMD
#define ATC_RGB_AMD 0x8C92
#endif
#ifndef ATC_RGBA_EXPLICIT_ALPHA_AMD
#define ATC_RGBA_EXPLICIT_ALPHA_AMD 0x8C93
#endif
#ifndef ATC_RGBA_INTERPOLATED_ALPHA_AMD
#define ATC_RGBA_INTERPOLATED_ALPHA_AMD 0x87EE
#endif

// ETC1 (OES_compressed_ETC1_RGB8_texture) : All OpenGL ES chipsets
#ifndef ETC1_RGB8
#define ETC1_RGB8 0x8D64
#endif



namespace cognitics {

    bool fileToENU(Cognitics::CoordinateSystems::EllipsoidTangentPlane *ltp_ellipsoid, OGRCoordinateTransformation* coordTrans, float &x, float &y, float &z)
    {
        double local_x = 0, local_y = 0, local_z = 0;
        double dx = x;
        double dy = y;
        double dz = z;
        coordTrans->Transform(1, &dx, &dy, &dz);
        ltp_ellipsoid->GeodeticToLocal(dy,
            dx,
            dz,
            local_x,
            local_y,
            local_z);
        x = local_x;
        y = local_y;
        z = local_z;
        return true;
    }

    /*
     * expandCoordinates remaps everything that there are the exact same number
     * of verts,UVs, and normals (unless there are no uvs or normals). This
     * makes it easier to export to something like OpenFlight.
     */
    void QuickObj::expandCoordinates()
    {
        std::vector<QuickVert> newVerts;
        std::vector<QuickVert> newNorms;
        std::vector<QuickVert> newUvs;

        //std::vector<int> indices;
        std::map<coord_tuple_t, int32_t> newMappings;
        for (auto&& submesh : subMeshes)
        {
            std::vector<uint32_t> newVertIdxs;
            std::vector<uint32_t> newUvIdxs;
            std::vector<uint32_t> newNormIdxs;
            /*
            if((vertIdxs.size() != uvIdxs.size()) || (vertIdxs.size() != normIdxs.size()))
            {
                log << "Error, unexpected index array sizes." << log.endl;
                break;
            }
            */
            bool has_uvs = (submesh.uvIdxs.size() > 0);
            bool has_norms = (submesh.normIdxs.size() > 0);
            for (size_t i = 0, ic = submesh.vertIdxs.size(); i < ic; i++)
            {
                int32_t idx = submesh.vertIdxs[i];
                int32_t uvidx = -1;
                int32_t normidx = -1;
                if(has_uvs)
                    uvidx = submesh.uvIdxs[i];
                if (has_norms)
                    normidx = submesh.normIdxs[i];
                coord_tuple_t coord = std::make_tuple(idx, uvidx, normidx);
                int newIdx = newVerts.size();
                if (newMappings.find(coord) == newMappings.end())
                {
                    newMappings[coord] = newIdx;
                    //Add a new vert values
                    newVerts.push_back(verts[idx]);
                    if (has_uvs)
                        newUvs.push_back(uvs[uvidx]);
                    if (has_norms)
                        newNorms.push_back(norms[normidx]);
                }
                else
                {
                    newIdx = newMappings[coord];
                }
                //reset the mesh indexes
                newVertIdxs.push_back(newIdx);
                if (has_uvs)
                {
                    newUvIdxs.push_back(newIdx);
                }
                if (has_norms)
                {
                    newNormIdxs.push_back(newIdx);
                }
            }
            submesh.vertIdxs = newVertIdxs;
            submesh.uvIdxs = newUvIdxs;
            submesh.normIdxs = newNormIdxs;
        }
        verts = newVerts;
        uvs = newUvs;
        norms = newNorms;
    }

    bool QuickObj::parseOBJ(bool loadTextures)
    {
        log.init("QuickObj", this);
        ccl::FileInfo fi(objFilename);
        std::string objFilePath = fi.getDirName();
        //Get the file size
        auto fileSize = ccl::getFileSize(objFilename);
        if (fileSize > (1024 * 1024 * 1024))
        {
            log << "File size of " << fileSize << " for " << objFilename << " exceeds the max of 1gb" << log.endl;
            return false;
        }
        //Make a buffer for the text
        char *fileContents = new char[fileSize + 1];
        //Open the file
        FILE *f = fopen(objFilename.c_str(), "rb");
        if (!f)
        {
            delete[] fileContents;
            log << "Unable to open " << objFilename << ". error: " << strerror(errno) << log.endl;
            return false;
        }
        //Read the entire file
        fread(fileContents, 1, fileSize, f);
        int pos = 0;

        //OBJ indexes start at 1, so we put a placeholder in 0
        QuickVert placeholder3;
        placeholder3.x = 0; placeholder3.y = 0; placeholder3.z = 0;
        verts.push_back(placeholder3);
        uvs.push_back(placeholder3);
        norms.push_back(placeholder3);

        while (pos < fileSize)
        {
            int lineStart = pos;

            //line by line
            while (fileContents[pos] != 0x0a && fileContents[pos] != 0x0d && fileContents[pos] != 0 && pos < fileSize)
            {
                pos++;
            }
            //Null terminate the line
            fileContents[pos] = 0;

            int lineEnd = pos;
            //Process between lineStart and lineEnd
            char *tok = strtok(fileContents + lineStart, " ");
            if (strcmp(tok, "v") == 0)
            {
                char *x = strtok(NULL, " ");
                if (!x)
                    continue;
                char *y = strtok(NULL, " ");
                if (!y)
                    continue;
                QuickVert v;

                v.x = atof(x) + srs.offsetPt.X();
                v.y = atof(y) + srs.offsetPt.Y();
                char *z = strtok(NULL, " ");
                if (!z)
                    v.z = srs.offsetPt.Z();
                else
                    v.z = atof(z) + srs.offsetPt.Z();

                /*
                double tmpv = v.x;
                v.x = v.y;
                v.y = tmpv;
                */
                /*if(v.x < (srs.offsetPt.X()/2))
                {
                    printf("!!!");
                }*/
                minX = std::min<float>(minX, v.x);
                minY = std::min<float>(minY, v.y);
                minZ = std::min<float>(minZ, v.z);

                maxX = std::max<float>(maxX, v.x);
                maxY = std::max<float>(maxY, v.y);
                maxZ = std::max<float>(maxZ, v.z);

                verts.push_back(v);
            }
            else if (strcmp(tok, "vt") == 0)
            {
                char *x = strtok(NULL, " ");
                if (!x)
                    continue;
                char *y = strtok(NULL, " ");
                if (!y)
                    continue;
                QuickVert vt;
                vt.x = atof(x);
                vt.y = atof(y);
                vt.z = 0;
                uvs.push_back(vt);
            }
            else if (strcmp(tok, "vn") == 0)
            {
                char *x = strtok(NULL, " ");
                if (!x)
                    continue;
                char *y = strtok(NULL, " ");
                if (!y)
                    continue;
                QuickVert vn;
                vn.x = atof(x);
                vn.y = atof(y);
                char *z = strtok(NULL, " ");
                if (!z)
                    vn.z = 0;
                else
                    vn.z = atof(z);
                norms.push_back(vn);
            }
            else if (strcmp(tok, "f") == 0)
            {
                tok = strtok(NULL, " ");
                while (tok)
                {
                    char *vp = tok;
                    char *vtp = NULL;
                    char *vnp = NULL;
                    while (*tok != 0)
                    {
                        if (*tok == '/')
                        {
                            //Terminate the previous pointer
                            if (vtp == NULL)
                                vtp = tok + 1;
                            else if (vnp == NULL)
                                vnp = tok + 1;
                            else
                                break;
                            *tok = 0;
                        }
                        tok++;
                    }
                    uint32_t vertId = atoi(vp);
                    vertIdxs.push_back(vertId);
                    auto &lastMesh = subMeshes.back();
                    lastMesh.vertIdxs.push_back(vertId);

                    if (vtp)
                    {
                        uint32_t uvId = atoi(vtp);
                        uvIdxs.push_back(uvId);
                        lastMesh.uvIdxs.push_back(uvId);
                    }
                    if (vnp)
                    {
                        uint32_t normId = atoi(vnp);
                        normIdxs.push_back(normId);
                        lastMesh.normIdxs.push_back(normId);
                    }
                    tok = strtok(NULL, " ");
                }
            }
            else if (strcmp(tok, "mtllib") == 0)
            {
                char *materialFile = strtok(NULL, " ");
                materialFilename = ccl::joinPaths(objFilePath, materialFile);
                if (loadTextures)
                    parseMtlFile(materialFilename);
            }
            else if (strcmp(tok, "usemtl") == 0)
            {
                //Defines a new material from this point on
                char *material = strtok(NULL, " ");
                materialName = std::string(material);
                QuickSubMesh submesh;
                submesh.materialName = materialName;
                subMeshes.push_back(submesh);
                auto lastMesh = subMeshes.back();
            }
            //Skip and remaining cr/lf
            while ((pos < fileSize) &&
                (fileContents[pos] == 0x0a || fileContents[pos] == 0x0d || fileContents[pos] == 0))
            {
                pos++;
            }

        }//end of while parsing lines
        fclose(f);
        _isValid = true;
        delete[] fileContents;
        //Transform if needed. If there is no WKT, assume it's already in ENU
        if (srs.srsWKT.size() > 0 && srs.srsWKT != "ENU")
        {
            OGRSpatialReference wgs;
            OGRCoordinateTransformation* coordTrans;
            wgs.SetFromUserInput("WGS84");
            OGRSpatialReference *file_srs = new OGRSpatialReference;

            const char *prjstr = srs.srsWKT.c_str();
            OGRErr err;
            if (prjstr[0] == '+')
            {
                err = file_srs->importFromProj4(prjstr);
            }
            else
            {
                err = file_srs->importFromWkt((char **)&prjstr);
            }

            if (err != OGRERR_NONE)
            {
                delete file_srs;
                return false;
            }
            coordTrans = OGRCreateCoordinateTransformation(file_srs, &wgs);

            double originLat = 0;
            double originLon = 0;
            double x = srs.offsetPt.X();
            double y = srs.offsetPt.Y();
            double z = srs.offsetPt.Z();
            //Use the offset for the origin
            coordTrans->Transform(1, &x, &y, &z);
            //now x,y,z should be geo
            originLat = y;
            originLon = x;
            std::stringstream ss;
            ss.precision(12);
            ss << "Using an origin of lat=" << originLat << " lon=" << originLon;
            auto ltp_ellipsoid = new Cognitics::CoordinateSystems::EllipsoidTangentPlane(originLat, originLon);

            double local_x = 0, local_y = 0, local_z = 0;

            for (int i = 1, ic = verts.size(); i < ic; i++)
            {
                QuickVert &vert = verts[i];
                fileToENU(ltp_ellipsoid, coordTrans, vert.x, vert.y, vert.z);
            }
            fileToENU(ltp_ellipsoid, coordTrans, minX, minY, minZ);
            fileToENU(ltp_ellipsoid, coordTrans, maxX, maxY, maxZ);
        }
        return true;
    }

    bool QuickObj::parseLMAB(bool loadTextures)
    {
        //Not implemented yet because the lmab file has a number of meshes
        //at different LODs, so it will require some restructuring to use.
        //for now, convert the lmab to obj using the lmbundle2ogj app
        return false;
    }

    QuickObj::QuickObj(const std::string &objFilename,
            const ObjSrs &srs,
            const std::string &_textureDirectory, 
            bool loadTextures) :
                objFilename(objFilename),
                textureDirectory(_textureDirectory),
                minX(FLT_MAX),maxX(-FLT_MAX),minY(FLT_MAX),
                maxY(-FLT_MAX),minZ(FLT_MAX),maxZ(-FLT_MAX),
                _isValid(false)
    {
        log.init("QuickObj",this);
        ccl::FileInfo fi(objFilename);
        std::string objFilePath = fi.getDirName();
        if(ccl::stringCompareNoCase(fi.getSuffix(),"obj")==0)
        {
            parseOBJ(loadTextures);
        }
        else if (ccl::stringCompareNoCase(fi.getSuffix(), "lmab")==0)
        {
            parseLMAB(loadTextures);
        }

        return;
        
    }

    void QuickObj::getBounds(float &minX, float &maxX,
                             float &minY, float &maxY,
                             float &minZ, float &maxZ)
    {
        minX = this->minX;
        minY = this->minY;
        minZ = this->minZ;

        maxX = this->maxX;
        maxY = this->maxY;
        maxZ = this->maxZ;
    }


    bool QuickObj::parseMtlFile(const std::string &mtlFilename)
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
                    ccl::FileInfo fi(objFilename);
                    std::string objFilePath = fi.getDirName();
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



    Material::Material() : shine(0.0), transparency(0.0) 
    {
    }

    Material::~Material()
    {
    }

    bool Material::operator==(const Material &rhs) const
    {
        return (shine == rhs.shine)
            && (transparency == rhs.transparency)
            && (ambient == rhs.ambient)
            && (diffuse == rhs.diffuse)
            && (specular == rhs.specular)
            && (emission == rhs.emission);
    }

    bool Material::operator< (const Material &rhs) const
    {
        if(shine < rhs.shine)
            return true;
        if(shine > rhs.shine)
            return false;
        if(transparency < rhs.transparency)
            return true;
        if(transparency > rhs.transparency)
            return false;
        if(ambient < rhs.ambient)
            return true;
        if(ambient > rhs.ambient)
            return false;
        if(diffuse < rhs.diffuse)
            return true;
        if(diffuse > rhs.diffuse)
            return false;
        if(specular < rhs.specular)
            return true;
        if(specular > rhs.specular)
            return false;
        return emission < rhs.emission;
    }

    
    Color::Color(double R, double G, double B, double A)
        : initialized(true),r(R), g(G), b(B), a(A)
    {
    }
    
    Color::Color() : initialized(false),r(1),g(1),b(1),a(1)
    {
    }

    Color::~Color()
    {
    }

    bool Color::isInitialized() const
    {
        return initialized;
    }

    bool Color::operator==(const Color &rhs) const
    {
        return (r == rhs.r) && (g == rhs.g) && (b == rhs.b) && (a == rhs.a);
    }

    bool Color::operator<(const Color &rhs) const
    {
        if(r < rhs.r)
            return true;
        else if (r > rhs.r)
            return false;
        else if(g < rhs.g)
            return true;
        else if (g > rhs.b)
            return false;
        else if(b < rhs.b)
            return true;
        else if (b > rhs.b)
            return false;
        else return (a < rhs.a);
    }

    bool Color::operator>(const Color &rhs) const
    {
        return !(rhs < *this) && !(rhs == *this);
    }


    bool QuickObj::glRender()
    {
        glPushAttrib(GL_ALL_ATTRIB_BITS);
            
        glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
        glEnable(GL_TEXTURE_2D);
        
        for (auto&& submesh : subMeshes)
        {
            std::string texname = materialMap[submesh.materialName].textureFile;
            if (texname.length() > 0)
            {
                GLuint texid = getOrLoadTextureID(texname);
                if (texid == 0)
                {
                    
                    log << "Invalid texture, unable to render texture in QuickObj::glRender()" << log.endl;
                }
                //std::cout << submesh.materialName << " tex=" << texid << " filename=" << materialMap[submesh.materialName].textureFile << "\n";
                glBindTexture(GL_TEXTURE_2D, texid);
            }
            glBegin(GL_TRIANGLES);
            if (submesh.vertIdxs.size() != submesh.uvIdxs.size())
            {
                log << "The number of vertices does not match the number of UV coordinates." << log.endl;
                return false;
            }

            for (size_t i = 0, ic = submesh.vertIdxs.size(); i < ic; i++)
            {
                uint32_t idx = submesh.vertIdxs[i];
                uint32_t uvidx = submesh.uvIdxs[i];
                if (idx == 0 || uvidx == 0)
                {
                    log << "Vert/UV index of 0 is invalid for OBJ." << log.endl;
                    return false;
                }
                glTexCoord2f(uvs[uvidx].x, uvs[uvidx].y);
                glVertex3f(verts[idx].x, verts[idx].y, verts[idx].z);
            }
            glEnd();
        }
        
        glPopAttrib();
        return true;
    }

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

    GLuint QuickObj::getOrLoadDDSTextureID(const std::string &texname)
    {
        //DDS Reader Borrowed from http://www.opengl-tutorial.org
        unsigned char header[124];
        FILE* fp;

        /* try to open the file */
        fp = fopen(texname.c_str(), "rb");
        if (fp == NULL)
        {
            log << "Unable to open " << texname << log.endl;
            return 0;
        }

        /* verify the type of file */
        char filecode[4];
        fread(filecode, 1, 4, fp);
        if (strncmp(filecode, "DDS ", 4) != 0)
        {
            log << "Not a dds file " << texname << log.endl;
            fclose(fp);
            return 0;
        }

        /* get the surface desc */
        fread(&header, 124, 1, fp);

        unsigned int height = *(unsigned int*)&(header[8]);
        unsigned int width = *(unsigned int*)&(header[12]);
        unsigned int linearSize = *(unsigned int*)&(header[16]);
        unsigned int mipMapCount = *(unsigned int*)&(header[24]);
        unsigned int fourCC = *(unsigned int*)&(header[80]);
        unsigned char* buffer;
        unsigned int bufsize;
        /* how big is it going to be including all mipmaps? */
        bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
        buffer = (unsigned char*)malloc(bufsize * sizeof(unsigned char));
        fread(buffer, 1, bufsize, fp);
        /* close the file pointer */
        fclose(fp);
        unsigned int components = (fourCC == FOURCC_DXT1) ? 3 : 4;
        unsigned int format;
        switch (fourCC)
        {
        case FOURCC_DXT1:
            format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
            break;
        case FOURCC_DXT3:
            format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
            break;
        case FOURCC_DXT5:
            format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
            break;
        default:
            log << "Unknown compression mode " << fourCC << log.endl;
            free(buffer);
            return 0;
        }
        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);
        textures[texname] = textureID;
        // "Bind" the newly created texture : all future texture functions will modify this texture
        glBindTexture(GL_TEXTURE_2D, textureID);
        unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
        unsigned int offset = 0;

        /* load the mipmaps */
        for (unsigned int level = 0; level < mipMapCount && (width || height); ++level)
        {
            unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
            glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
                                   0, size, buffer + offset);

            offset += size;
            width /= 2;
            height /= 2;
        }
        free(buffer);

        return textureID;

    }

    class TexturePixels
    {
    public:
        ccl::binary pixels;
        std::string filename;
        ip::ImageInfo info;
        ~TexturePixels()
        {
            pixels.clear();//Probably not needed, but I'm trying to track down a leak.
        }
    };

    typedef std::map<std::string, TexturePixels> texture_pixels_map_t;

    //Global texture cache:

    /***
     * Warning, this cache isn't thread safe. Nothing is done to control the lifetime
     * of the texture in the cache. To make it thread-safe, a reference count should be used
     * so texture are only removed from the cache inside a mutex, when the reference count is zero.
     * For this to work, all code that uses a cached texture  must dereference the count,
     * by calling a deref inside the cache object, so it can be protected with a mutex.
     * Why haven't I done this yet? Because it isn't needed right now.
     */
    class TextureCache
    {
        int max_textures;
        std::map<std::string, TexturePixels> images;
        std::map<std::string, int> textureAge;
        ccl::mutex mut;
        void removeOldest()
        {
            mut.lock();
            int max_age = 0;
            for(auto && age : textureAge)
            {
                max_age = std::max<int>(age.second,max_age);
            }
            for (auto && age : textureAge)
            {
                if(age.second == max_age)
                {
                    removeTexture(age.first);
                    mut.unlock();
                    return;
                }
            }
            mut.unlock();
        }

        void removeTexture(const std::string &filename)
        {
            mut.lock();
            auto iter = images.find(filename);
            if(iter!=images.end())
            {
                images.erase(iter);
            }

            auto age_iter = textureAge.find(filename);
            if (age_iter != textureAge.end())
            {
                textureAge.erase(age_iter);
            }
            mut.unlock();
        }

        void increaseAge()
        {
            mut.lock();
            for(auto &&tage : textureAge)
            {
                tage.second++;
            }
            mut.unlock();
        }

        void resetAge(const std::string &filename)
        {
            mut.lock();
            auto age_iter = textureAge.find(filename);
            if (age_iter != textureAge.end())
            {
               age_iter->second = 0;
            }
            mut.unlock();
        }
    public:
        TextureCache(int max_textures) : max_textures(max_textures)
        {
            
        }

        void store(TexturePixels image)
        {
            mut.lock();
            if (images.size() > max_textures)
            {
                removeOldest();
            }
            textureAge[image.filename] = 0;
            images[image.filename] = image;
            mut.unlock();
        }

        bool get(const std::string &textureFilename, TexturePixels &image)
        {
            mut.lock();
            auto iter = images.find(textureFilename);
            if (iter == images.end())
            {
                mut.unlock();
                return false;
            }
            increaseAge();
            resetAge(textureFilename);
            image = iter->second;
            mut.unlock();
            return true;
        }
    };

    TextureCache gTextureCache(20);


    GLuint QuickObj::getOrLoadTextureID(const std::string &texname)
    {
        //std::string texpath = ccl::joinPaths(textureDirectory,texname);
        if(textures.find(texname)!=textures.end())
            return textures[texname];
        if(ccl::stringEndsWith(texname, ".dds", false))
        {
            return getOrLoadDDSTextureID(texname);
        }

        TexturePixels tpix;
        ip::ImageInfo info;
        if(!gTextureCache.get(texname,tpix))
        {
            ccl::binary buffer;         
            if (ip::GetImagePixels(texname, info, buffer))
            {
                ip::FlipVertically(info, buffer);
                tpix.pixels = buffer;
                tpix.info = info;
                tpix.filename = texname;
                gTextureCache.store(tpix);
            }
            else
            {
                log << "Error, unable to read texture pixels for " << texname << log.endl;
                return 0;
            }
        }
        info = tpix.info;
        {
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glEnable(GL_TEXTURE_2D);
            GLuint texid = 0;
            GLenum glerror = glGetError();
            glGenTextures(1, &texid);
            //std::cout << "texid=" << texid << " err=" << (int)glerror << "\n";
            glBindTexture(GL_TEXTURE_2D, texid);
            glerror = glGetError();
            textures[texname] = texid;
            
            glerror = glGetError();
            unsigned char *p = (unsigned char *)&tpix.pixels.at(0);

            if (info.depth == 3 && info.interleaved && info.dataType == ip::ImageInfo::UBYTE)
                glTexImage2D(GL_TEXTURE_2D, 0, 3, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, p);
            else if (info.depth == 4 && info.interleaved && info.dataType == ip::ImageInfo::UBYTE)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, p);
            else
                log << "Error, unknown texture format!" << log.endl;
            //if(glGenerateMipmapfunc)
            //    glGenerateMipmapfunc(GL_TEXTURE_2D);
            glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
            glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
            
            return texid;
        }

        return -1;
    }
    QuickObj &QuickObj::operator=(const QuickObj &other)
    {
        this->minX = other.minX;
        this->minY = other.minY;
        this->maxX = other.maxX;
        this->maxY = other.maxY;
        this->minZ = other.minZ;
        this->maxZ = other.maxZ;
        this->srs = other.srs;
        this->subMeshes = other.subMeshes;
        this->verts = other.verts;
        this->norms = other.norms;
        this->uvs = other.uvs;
        this->vertIdxs = other.vertIdxs;
        this->uvIdxs = other.uvIdxs;
        this->normIdxs = other.normIdxs;
        this->textureFilename = other.textureFilename;
        this->textureDirectory = other.textureDirectory;
        this->materialFilename = other.materialFilename;
        this->materialName = other.materialName;
        this->currentMaterial = other.currentMaterial;
        this->materialMap = other.materialMap;
        this->_isValid = other._isValid;
        this->textures = other.textures;
        this->objFilename = other.objFilename;
        return *this;
    }

    QuickObj::QuickObj(const QuickObj &other)
    {
        this->minX = other.minX;
        this->minY = other.minY;
        this->maxX = other.maxX;
        this->maxY = other.maxY;
        this->minZ = other.minZ;
        this->maxZ = other.maxZ;
        this->srs = other.srs;
        this->subMeshes = other.subMeshes;
        this->verts = other.verts;
        this->norms = other.norms;
        this->uvs = other.uvs;
        this->vertIdxs = other.vertIdxs;
        this->uvIdxs = other.uvIdxs;
        this->normIdxs = other.normIdxs;
        this->textureFilename = other.textureFilename;
        this->textureDirectory = other.textureDirectory;
        this->materialFilename = other.materialFilename;
        this->materialName = other.materialName;
        this->currentMaterial = other.currentMaterial;
        this->materialMap = other.materialMap;
        this->_isValid = other._isValid;
        this->textures = other.textures;
        this->objFilename = other.objFilename;
    }

    QuickObj::~QuickObj()
    {
        std::map<std::string,GLuint>::iterator iter = textures.begin();
        while(iter!=textures.end())
        {
            if(iter->second!=-1)
            {
                int texid = iter->second;                
                glDeleteTextures(1,&iter->second);
                //GLenum glerror = glGetError();
                //std::string t = iter->first;
                //std::cout << "deleted texture=" << t << " texid=" << texid << " err=" << (int)glerror << "\n";
            }
            iter++;
        }
    }

    bool QuickObj::transform(Cognitics::CoordinateSystems::EllipsoidTangentPlane *etp,
        OGRCoordinateTransformation *coordTrans,
        const sfa::Point &offset) 
    {
        for(auto& vert : verts)
        {
            double x = vert.x + offset.X();
            double y = vert.y + offset.Y();
            double z = vert.z + offset.Z();
            double local_x = 0, local_y = 0, local_z = 0;
            coordTrans->Transform(1, &x, &y, &z);
            etp->GeodeticToLocal(y,
                x,
                z,
                local_x,
                local_y,
                local_z);
            vert.x = local_x;
            vert.y = local_y;
            vert.z = local_z;
        }
        return true;
    }

    ObjCache::ObjCache(int max_objs) : max_objs(max_objs)
    {

    }

    ObjCache::~ObjCache()
    {
        mut.lock();
        for (auto && age : objAge)
        {
            delete objs[age.first];
        }
        mut.unlock();
    }

    //store takes ownership of obj and will delete it
    void ObjCache::store(QuickObj *obj)
    {
        if (!obj)
            return;
        mut.lock();
        if (objs.size() > max_objs)
        {
            removeOldest();
        }
        objAge[obj->objFilename] = 0;
        objs[obj->objFilename] = obj;
        mut.unlock();
    }

    QuickObj* ObjCache::get(const std::string &objFilename)
    {
        mut.lock();
        QuickObj* obj = NULL;
        auto iter = objs.find(objFilename);
        if (iter == objs.end())
        {
            mut.unlock();
            return NULL;
        }
        increaseAge();
        resetAge(objFilename);
        obj = iter->second;
        mut.unlock();
        return obj;
    }

    void ObjCache::removeOldest()
    {
        mut.lock();
        int max_age = 0;
        for (auto && age : objAge)
        {
            max_age = std::max<int>(age.second, max_age);
        }
        //for (auto && age : objAge)
        for(auto age_iter = objAge.begin(),end=objAge.end();age_iter!=end;age_iter++)
        {
            auto &age = *age_iter;
            if (age.second == max_age)
            {
                auto iter = objs.find(age.first);
                if (iter != objs.end())
                {
                    QuickObj *old = iter->second;
                    if (old)
                    {
                        //log << "Erasing " << old->objFilename << "..." << log.endl;
                        delete old;
                    }
                    else
                    {
                        log << old->objFilename << " doesn't have an object in the cache!" << log.endl;
                    }
                    objs.erase(iter);
                    objAge.erase(age_iter);
                }
                mut.unlock();
                return;
            }
        }
        mut.unlock();
    }

    void ObjCache::increaseAge()
    {
        mut.lock();
        for (auto &&tage : objAge)
        {
            tage.second++;
        }
        mut.unlock();
    }

    void ObjCache::resetAge(const std::string &filename)
    {
        mut.lock();
        auto iter = objAge.find(filename);
        if (iter != objAge.end())
        {
            iter->second = 0;
        }
        mut.unlock();
    }

    ObjCache gObjCache(50);

    QuickObj2Flt::QuickObj2Flt()
    {
        fltFile = NULL;
        header = NULL;
        obj = NULL;
    }

    bool QuickObj2Flt::buildMat(Material &mat)
    {

        return true;
    }

    bool QuickObj2Flt::buildMesh(QuickObj &obj)
    {

        return true;
    }

    bool QuickObj2Flt::buildSubmesh(QuickSubMesh &submesh, const std::string &name)
    {
        flt::Record *container = NULL;
        flt::Object *groupRecord = new flt::Object;
        groupRecord->id = name;
        container = groupRecord;
        records.push_back(container);
        records.push_back(new flt::PushLevel);
        header->nextObjectNodeID = header->nextObjectNodeID + 1;
        
		//Assume that faces are always triangles...
        for(size_t i=0,ic=submesh.vertIdxs.size();i<ic;i+=3)
        {
            flt::Face *faceRecord = new flt::Face;
            header->nextFaceNodeID = header->nextFaceNodeID + 1;
            std::stringstream ss;
            ss << "p" << i;
            faceRecord->id = ss.str();
            flt::VertexList *vertexListRecord = new flt::VertexList;

			int index = submesh.vertIdxs[i];
			vertexListRecord->offsets.push_back(int(8 + (index * 64)));

			index = submesh.vertIdxs[i+1];
			vertexListRecord->offsets.push_back(int(8 + (index * 64)));

			index = submesh.vertIdxs[i+2];
			vertexListRecord->offsets.push_back(int(8 + (index * 64)));

            faceRecord->drawType = 1;
			faceRecord->materialIndex = matIDMap[submesh.materialName];
			faceRecord->texturePatternIndex = texIDMap[submesh.materialName];


			faceRecord->drawType = 1;//both sides
			faceRecord->lightMode = 3;

			records.push_back(faceRecord);

            records.push_back(new flt::PushLevel);
            records.push_back(vertexListRecord);
            records.push_back(new flt::PopLevel);
        }
        records.push_back(new flt::PopLevel);//Pop the container level
        return true;
    }


    ccl::uint32_t getUInt32FromColor(double r, double g, double b, double a)
    {
        return ((unsigned char)(a * 0xFF) << 24)
            | ((unsigned char)(b * 0xFF) << 16)
            | ((unsigned char)(g * 0xFF) << 8)
            | (unsigned char)(r * 0xFF);
    }

    bool QuickObj2Flt::convertTextures(QuickObj *obj, const std::string &outputDir)
    {
        for (auto str_mat_pair : obj->materialMap)
        {
            auto file_path = str_mat_pair.first;
            auto& obj_mtl = str_mat_pair.second;
            auto img_info = ip::ImageInfo();

            auto img_buffer = ccl::binary();
            ccl::FileInfo fi(obj_mtl.textureFile);
            if (ccl::stringEndsWith(fi.getSuffix(), "rgb", false))
            {
                continue;
            }
            else if (ccl::stringEndsWith(fi.getSuffix(), "jpg", false))
            {
                ip::GetJPGImagePixels(obj_mtl.textureFile, img_info, img_buffer);
            }
            else if (ccl::stringEndsWith(fi.getSuffix(), "png", false))
            {
                ip::GetPNGImagePixels(obj_mtl.textureFile, img_info, img_buffer);
            }
            auto width = img_info.width;
            auto height = img_info.height;
            auto depth = img_info.depth;
            auto r = new u_char[width * height];
            auto g = new u_char[width * height];
            auto b = new u_char[width * height];
            ip::FlipVertically(img_info, img_buffer);
            if (img_info.interleaved)
            {
                for (int pixel_idx = 0, pixel_count = width * height * depth,output_idx=0; pixel_idx < pixel_count; pixel_idx+=3,output_idx++)
                {
                    r[output_idx] = img_buffer.at((pixel_idx));
                    g[output_idx] = img_buffer.at((pixel_idx) + 1);
                    b[output_idx] = img_buffer.at((pixel_idx) + 2);
                }
            }
            else
            {
                int r_ofs = 0;
                int g_ofs = width * height;
                int b_ofs = width * height * 2;
                for(int j=0,jc=width*height;j<jc;j++)
                {
                    r[j] = img_buffer.at(r_ofs + j);
                    g[j] = img_buffer.at(g_ofs +j);
                    b[j] = img_buffer.at(b_ofs + j);
                }
            }
            std::string output_rgb_path = ccl::joinPaths(outputDir, fi.getBaseName(true) + ".rgb");
            obj->materialMap[file_path].textureFile = output_rgb_path;
            ip::WriteRGB(output_rgb_path, r, g, b, width, height);
            delete[] r;
            delete[] g;
            delete[] b;
        }
        return true;
    }

    bool QuickObj2Flt::convert(QuickObj *obj, const std::string &outputFltFilename)
    {
        this->obj = obj;
        fltFile = flt::OpenFlight::create(outputFltFilename, 1600);
        if (!fltFile)
        {
            log << "Error: Can't create " << outputFltFilename << log.endl;
            return false;
        }
        header = new flt::Header;        
		header->projectionType = 0;//flat earth
        header->originLatitude = obj->srs.geoOrigin.Y();;
        header->originLongitude = obj->srs.geoOrigin.X();;

        //TODO: need to calculate from transform
        header->southwestLatitude = 0;
        header->southwestLongitude = 0;

        header->northeastLatitude = 0;
        header->northeastLongitude = 0;

        header->flags[0] = true;    // ??
        records.push_back(header);

        //TODO: add colors
        flt::ColorPalette *colorPalette = new flt::ColorPalette;
        //TODO: Do something
        records.push_back(colorPalette);

        //add materials
		
		int currentMatId = 0;
		for (auto&& matPair : obj->materialMap)
		{
			matPair.second.textureFile;
			Material &mat = matPair.second;
			matIDMap[matPair.first] = currentMatId;
			flt::MaterialPalette *materialPalette = new flt::MaterialPalette;
			materialPalette->index = int(currentMatId++);
			materialPalette->ambientRed = mat.ambient.r;
			materialPalette->ambientGreen = mat.ambient.g;
			materialPalette->ambientBlue = mat.ambient.b;
			materialPalette->diffuseRed = mat.diffuse.r;
			materialPalette->diffuseGreen = mat.diffuse.g;
			materialPalette->diffuseBlue = mat.diffuse.b;
			materialPalette->specularRed = mat.specular.r;
			materialPalette->specularGreen = mat.specular.g;
			materialPalette->specularBlue = mat.specular.b;
			materialPalette->emissiveRed = mat.emission.r;
			materialPalette->emissiveGreen = mat.emission.g;
			materialPalette->emissiveBlue = mat.emission.b;
			materialPalette->alpha = mat.diffuse.a;
			materialPalette->shininess = 0;
			records.push_back(materialPalette);

		}

		int currentTexId = 0;
		for (auto&& matPair : obj->materialMap)
		{
			flt::TexturePalette *texturePalette = new flt::TexturePalette;
			texIDMap[matPair.first] = currentTexId;
			texturePalette->textureIndex = currentTexId++;
            ccl::FileInfo fltFI(matPair.second.textureFile);
			texturePalette->fileName = fltFI.getBaseName();
			records.push_back(texturePalette);
		}
		
		flt::LightSourcePalette *lightSourcePalette;
		lightSourcePalette = new flt::LightSourcePalette;
		lightSourcePalette->index = 1;
		lightSourcePalette->ambientAlpha = 1.0f;
		lightSourcePalette->diffuseRed = 1.0f;
		lightSourcePalette->diffuseGreen = 1.0f;
		lightSourcePalette->diffuseBlue = 1.0f;
		lightSourcePalette->diffuseAlpha = 1.0f;
		lightSourcePalette->specularRed = 1.0f;
		lightSourcePalette->specularGreen = 1.0f;
		lightSourcePalette->specularBlue = 1.0f;
		lightSourcePalette->specularAlpha = 1.0f;
		records.push_back(lightSourcePalette);

        flt::RecordList vertexList;
        bool have_norms = false;
        if (obj->norms.size() == obj->verts.size())
        {
            have_norms = true;
        }
        bool have_uvs = false;
        if (obj->uvs.size() == obj->verts.size())
        {
            have_uvs = true;
        }
        const int vertexSize = 64;
        flt::VertexPalette *vertexPalette = new flt::VertexPalette;
        vertexPalette->vertexPaletteLength = int(8 + (obj->verts.size() * vertexSize));
        records.push_back(vertexPalette);

        for(size_t i=0,ic=obj->verts.size();i<ic;i++)
        {
            flt::VertexWithColorNormalUV *vertexRecord = new flt::VertexWithColorNormalUV;
            vertexRecord->x = obj->verts[i].x;
            vertexRecord->y = obj->verts[i].y;
            vertexRecord->z = obj->verts[i].z;
            vertexRecord->i = 0;
            vertexRecord->j = 0;
            vertexRecord->k = 0;
            vertexRecord->u = 0;
            vertexRecord->v = 0;

            if (have_norms)
            {
                vertexRecord->i = obj->norms[i].x;
                vertexRecord->j = obj->norms[i].y;
                vertexRecord->k = obj->norms[i].z;
            }
            if (have_uvs)
            {
                vertexRecord->u = obj->uvs[i].x;
                vertexRecord->v = obj->uvs[i].y;
            }
            vertexRecord->flags[2] = true;    // no color
            vertexRecord->flags[3] = true;    // packed color
            vertexRecord->packedColor = getUInt32FromColor(1.0,1.0,1.0,1.0);
            vertexRecord->colorIndex = -1;
            records.push_back(vertexRecord);
        }
       
		
        
		records.push_back(new flt::PushLevel);//???

//BuildScene Start
		flt::Record *container = NULL;
		flt::Group *groupRecord = new flt::Group;
		groupRecord->id = "root";
		container = groupRecord;
		header->nextGroupNodeID = header->nextGroupNodeID + 1;
		records.push_back(container);
		records.push_back(new flt::PushLevel);//container

        int submeshNo = 0;
        for (auto &&submesh : obj->subMeshes)
        {
            std::stringstream ss;
            ss << "mesh " << submeshNo++;
            buildSubmesh(submesh,ss.str());
        }
//BuildScene End
		records.push_back(new flt::PopLevel);//container

        records.push_back(new flt::PopLevel);//???
        if (!fltFile->addRecords(records))
        {
            log << "Error: Unable to add flt records." << log.endl;
            return false;
        }
		
        flt::OpenFlight::destroy(fltFile);
        return true;
    }

	bool QuickObj::exportObj(const std::string &filename)
	{
		std::string out_name = filename.c_str();
		out_name += ".obj";
		std::ofstream out(out_name);

		out << "# Produced by Cognitics\n";
		std::time_t now = std::time(0);
		std::string now_str = std::ctime(&now);
		out << "# " + now_str;
		out << "# " + std::to_string(this->verts.size()) + " vertices, " + std::to_string(this->vertIdxs.size()) + " faces\n";
		out << "\nmtllib " + this->materialFilename + "\n\n";
		if (this->verts.size() > 0)
		{
			for (cognitics::QuickVert &vertex : this->verts)
			{
				if (vertex.x != 0.000000 && vertex.y != 0.000000) {
					out << "v " + std::to_string(vertex.x) + " " + std::to_string(vertex.y) + " " + std::to_string(vertex.z) + "\n";
				}
			}
		}
		if (this->uvs.size() > 0)
		{
			for (cognitics::QuickVert &uv : this->uvs)
			{
				if (uv.x != 0.000000) {
					std::string w = uv.z == 0 ? "" : " " + std::to_string(uv.z);
					out << "vt " + std::to_string(uv.x) + " " + std::to_string(uv.y) + w + "\n";
				}
			}
		}
		if (this->norms.size() > 0)
		{
			for (cognitics::QuickVert &norm : this->norms)
			{
				if (norm.x != 0.000000) {
					out << "vn " + std::to_string(norm.x) + " " + std::to_string(norm.y) + " " + std::to_string(norm.z) + "\n";
				}
			}
		}

		for (cognitics::QuickSubMesh &subMesh : this->subMeshes)
		{
			if (subMesh.vertIdxs.size() < 1)
			{
				continue;
			}
			out << "\n";
			out << "usemtl " + subMesh.materialName + "\n";
			out << "\n";
			for (int i = 0; i < subMesh.vertIdxs.size(); i += 3)
			{
				std::string vert_idx = std::to_string(subMesh.vertIdxs.at(i));
				std::string uv_idx = i < subMesh.uvIdxs.size() ? "/" + std::to_string(subMesh.uvIdxs.at(i)) : "";
				std::string norm_idx = i < subMesh.normIdxs.size() ? "/" + std::to_string(subMesh.normIdxs.at(i)) + " " : " ";

				std::string vert_idx_one = std::to_string(subMesh.vertIdxs.at(i + 1));
				std::string uv_idx_one = i + 1 < subMesh.uvIdxs.size() ? "/" + std::to_string(subMesh.uvIdxs.at(i + 1)) : "";
				std::string norm_idx_one = i + 1 < subMesh.normIdxs.size() ? "/" + std::to_string(subMesh.normIdxs.at(i + 1)) + " " : " ";

				std::string vert_idx_two = std::to_string(subMesh.vertIdxs.at(i + 2));
				std::string uv_idx_two = i + 2 < subMesh.uvIdxs.size() ? "/" + std::to_string(subMesh.uvIdxs.at(i + 2)) : "";
				std::string norm_idx_two = i + 2 < subMesh.normIdxs.size() ? "/" + std::to_string(subMesh.normIdxs.at(i + 2)) + "\n" : "\n";

                //Ignore degenerate faces (triangles with no volume, because they share a vert).
                if((vert_idx == vert_idx_one ) || (vert_idx == vert_idx_two) || (vert_idx_two == vert_idx_one))
                {
                    continue;
                }
			    out << "f " + vert_idx + uv_idx + norm_idx + vert_idx_one + uv_idx_one + norm_idx_one + vert_idx_two + uv_idx_two + norm_idx_two;
			}
		}
		out.close();
		return true;
	}

	void QuickObj::addSubMesh(const QuickSubMesh &submesh)
	{
		subMeshes.push_back(submesh);
	}

	void QuickObj::flattenVert(uint32_t index, float up_val)
	{
		verts[index].z = up_val;
	}

    };

