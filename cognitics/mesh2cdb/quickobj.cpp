#ifdef WIN32
#define _CRT_SECURE_NO_WARNINGS 1
#include <windows.h>
#endif

#include "quickobj.h"
#include <stdio.h>
#include <string.h>
#include <float.h>
#include <ccl/FileInfo.h>
#include <ccl/StringUtils.h>

#include <fstream>
#include <GL/glew.h>
#include <GL/gl.h>
#include <ip/jpgwrapper.h>

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

    QuickObj::QuickObj(const std::string &objFilename, float _offsetX, float _offsetY, float _offsetZ,const std::string &_textureDirectory, bool loadTextures) :
        objFilename(objFilename),offsetX(_offsetX), offsetY(_offsetY), offsetZ(_offsetZ), textureDirectory(_textureDirectory), minX(FLT_MIN),maxX(-FLT_MAX),minY(FLT_MIN),maxY(-FLT_MAX),minZ(FLT_MIN),maxZ(-FLT_MAX),_isValid(false)
    {
        log.init("QuickObj",this);
        ccl::FileInfo fi(objFilename);
        std::string objFilePath = fi.getDirName();
        //Get the file size
        auto fileSize = ccl::getFileSize(objFilename);
        if(fileSize > (1024 * 1024 * 1024))
        {
            log << "File size of " << fileSize <<  " for " << objFilename << " exceeds the max of 1gb" << log.endl;
            return;
        }
        //Make a buffer for the text
        char *fileContents = new char[fileSize+1];
        //Open the file
        FILE *f = fopen(objFilename.c_str(),"rb");
        if(!f)
        {
            delete[] fileContents;
            log << "Unable to open " << objFilename << ". error: " << strerror(errno) << log.endl;
            return;
        }
        //Read the entire file
        fread(fileContents,1,fileSize,f);
        int pos = 0;

        //OBJ indexes start at 1, so we put a placeholder in 0
        QuickVert placeholder3;
        placeholder3.x = 0; placeholder3.y = 0; placeholder3.z = 0;
        verts.push_back(placeholder3);
        uvs.push_back(placeholder3);
        norms.push_back(placeholder3);

        while(pos < fileSize)
        {
            int lineStart = pos;
            
            //line by line
            while(fileContents[pos]!=0x0a && fileContents[pos]!=0x0d && fileContents[pos]!=0 && pos < fileSize)
            {
                pos++;
            }
            //Null terminate the line
            fileContents[pos] = 0;

            int lineEnd = pos;
            //Process between lineStart and lineEnd
            char *tok = strtok(fileContents+lineStart," ");
            if(strcmp(tok,"v")==0)
            {
                char *x = strtok(NULL," ");
                if(!x)
                    continue;
                char *y = strtok(NULL," ");
                if(!y)
                    continue;
                QuickVert v;
                v.x = atof(x) + offsetX;
                v.y = atof(y) + offsetY;
                char *z = strtok(NULL," ");
                if(!z)
                    v.z = offsetZ;
                else
                    v.z = atof(z) + offsetZ;
                
                minX = std::min<float>(minX,v.x);
                minY = std::min<float>(minY,v.y);
                minZ = std::min<float>(minZ,v.z);

                maxX = std::max<float>(maxX,v.x);
                maxY = std::max<float>(maxY,v.y);
                maxZ = std::max<float>(maxZ,v.z);

                verts.push_back(v);
            }
            else if(strcmp(tok,"vt")==0)
            {
                char *x = strtok(NULL," ");
                if(!x)
                    continue;
                char *y = strtok(NULL," ");
                if(!y)
                    continue;
                QuickVert vt;
                vt.x = atof(x);
                vt.y = atof(y);
                vt.z = 0;
                uvs.push_back(vt);
            }
            else if(strcmp(tok,"vn")==0)
            {
                char *x = strtok(NULL," ");
                if(!x)
                    continue;
                char *y = strtok(NULL," ");
                if(!y)
                    continue;
                QuickVert vn;
                vn.x = atof(x);
                vn.y = atof(y);
                char *z = strtok(NULL," ");
                if(!z)
                    vn.z = 0;
                else
                    vn.z = atof(z);
                norms.push_back(vn);
            }
            else if(strcmp(tok,"f")==0)
            {
                tok = strtok(NULL," ");
                while(tok)
                {
                    char *vp = tok;
                    char *vtp = NULL;
                    char *vnp = NULL;
                    while(*tok != 0)
                    {
                        if(*tok=='/')
                        {
                            //Terminate the previous pointer
                            if(vtp==NULL)
                                vtp = tok+1;
                            else if(vnp==NULL)
                                vnp = tok+1;
                            else
                                break;
                            *tok = 0;
                        }
                        tok++;
                    }
                    uint32_t vertId = atoi(vp);
                    vertIdxs.push_back(vertId);
                    if(vtp)
                    {
                        uint32_t uvId = atoi(vtp);
                        uvIdxs.push_back(uvId);
                    }
                    if(vnp)
                    {
                        uint32_t normId = atoi(vnp);
                        normIdxs.push_back(normId);
                    }
                    tok = strtok(NULL," ");
                }
            }
            else if(strcmp(tok,"mtllib")==0)
            {
                char *materialFile = strtok(NULL," ");
                materialFilename = ccl::joinPaths(objFilePath, materialFile);
                if(loadTextures)
                    parseMtlFile(materialFilename);
            }
            else if(strcmp(tok,"usemtl")==0)
            {
                //Defines a new material from this point on
                //Todo: figure out how to track this (just use the first one?)
                char *material = strtok(NULL," ");
                if(materialName.empty())
                    materialName = std::string(material);
            }
            //Skip and remaining cr/lf
            while((pos < fileSize) && 
                  (fileContents[pos]==0x0a || fileContents[pos]==0x0d || fileContents[pos]==0))
            {
                pos++;
            }

        }//end of while parsing lines
        fclose(f);
        _isValid = true;
        delete[] fileContents;
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
        
        GLuint texid = getOrLoadTextureID(materialMap[materialName].textureFile);
        glBindTexture(GL_TEXTURE_2D, texid);
        glEnable(GL_TEXTURE_2D);
        glBegin(GL_TRIANGLES);
        if(vertIdxs.size()!=uvIdxs.size())
        {
            log << "The number of vertices does not match the number of UV coordinates." << log.endl;
            return false;
        }

        for(size_t i=0,ic=vertIdxs.size();i<ic;i++)
        {
            uint32_t idx = vertIdxs[i];
            uint32_t uvidx = uvIdxs[i];
            if(idx==0 || uvidx==0)
            {
                log << "Vert/UV index of 0 is invalid for OBJ." << log.endl;
                return false;
            }
            glTexCoord2f(uvs[uvidx].x,uvs[uvidx].y);
            glVertex3f(verts[idx].x,verts[idx].y,verts[idx].z);
            
            //glNormal3f(x, y, z);
            
        }

        glEnd();
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
            std::cout << "Unable to open " << texname << std::endl;
            return 0;
        }

        /* verify the type of file */
        char filecode[4];
        fread(filecode, 1, 4, fp);
        if (strncmp(filecode, "DDS ", 4) != 0)
        {
            std::cout << "Not a dds file " << texname << std::endl;
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
            std::cout << "Unknown compression mode " << fourCC << std::endl;
            free(buffer);
            return 0;
        }
        // Create one OpenGL texture
        GLuint textureID;
        glGenTextures(1, &textureID);

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

    GLuint QuickObj::getOrLoadTextureID(const std::string &texname)
    {
        //std::string texpath = ccl::joinPaths(textureDirectory,texname);
        if(textures.find(texname)!=textures.end())
            return textures[texname];
        if(ccl::stringEndsWith(texname, ".dds", false))
        {
            return getOrLoadDDSTextureID(texname);
        }
        ccl::binary buffer;
        ccl::binary buffer2;
        ip::ImageInfo info;
        if(ip::GetImagePixels(texname,info,buffer))
        {
            ip::FlipVertically(info, buffer);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            GLuint texid = 0;
            glGenTextures(1, &texid);
            GLenum glerror = glGetError();
            textures[texname] = texid;
            glEnable(GL_TEXTURE_2D);
            glerror = glGetError();
            glBindTexture(GL_TEXTURE_2D,texid);
            glerror = glGetError();
            unsigned char *p = (unsigned char *)&buffer.at(0);

            if(info.depth==3 && info.interleaved && info.dataType==ip::ImageInfo::UBYTE)
                glTexImage2D(GL_TEXTURE_2D, 0, 3, info.width, info.height, 0, GL_RGB, GL_UNSIGNED_BYTE, p);
            else if(info.depth==4 && info.interleaved && info.dataType==ip::ImageInfo::UBYTE)
                glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.width, info.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, p);
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

    QuickObj::~QuickObj()
    {
        std::map<std::string,GLuint>::iterator iter = textures.begin();
        while(iter!=textures.end())
        {
            if(iter->second!=-1)
            {
                glDeleteTextures(1,&iter->second);
            }
            iter++;
        }
    }


}