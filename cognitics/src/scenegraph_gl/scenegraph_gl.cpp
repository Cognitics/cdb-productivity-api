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


#include "scenegraph_gl/scenegraph_gl.h"
#include <ip/jpgwrapper.h>
#include <ccl/StringUtils.h>

#ifdef _WIN32
#pragma comment(lib, "opengl32.lib")
#endif


namespace scenegraph
{
	

	void SimpleGLRenderVisitor::setupGLMatrixFromSFA(const sfa::Matrix &m,double *glmat)
	{
		const double *sfad = m.getData();
		glmat[0] = sfad[0];
		glmat[1] = sfad[4];
		glmat[2] = sfad[8];
		glmat[3] = 0;
		
		glmat[4] = sfad[0+1];
		glmat[5] = sfad[4+1];
		glmat[6] = sfad[8+1];
		glmat[7] = 0;

		glmat[8] = sfad[0+2];
		glmat[9] = sfad[4+2];
		glmat[10] = sfad[8+2];
		glmat[11] = 0;

		glmat[12] = sfad[0+3];
		glmat[13] = sfad[4+3];
		glmat[14] = sfad[8+3];
		glmat[15] = 1.0;
	}

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

    GLuint SimpleGLRenderVisitor::getOrLoadDDSTextureID(const std::string &texname)
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

	GLuint SimpleGLRenderVisitor::getOrLoadTextureID(const std::string &texname)
	{
		std::string texpath = ccl::joinPaths(texturedir,texname);	
		if(textures.find(texname)!=textures.end())
			return textures[texname];
        if(ccl::stringEndsWith(texname, ".dds", false))
        {
            return getOrLoadDDSTextureID(texname);
        }
		ccl::binary buffer;
		ccl::binary buffer2;
		ip::ImageInfo info;
		if(ip::GetImagePixels(texpath,info,buffer))
		{
            ip::FlipVertically(info, buffer);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			GLuint texid = 0;
			glGenTextures(1, &texid);
			glerror = glGetError();
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
			if(glGenerateMipmapfunc)
				glGenerateMipmapfunc(GL_TEXTURE_2D);
			glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
 			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
			glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
			return texid;
		}

		return -1;
	}
	

	SimpleGLRenderVisitor::~SimpleGLRenderVisitor(void)
	{
		// Free up all the textures
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

	SimpleGLRenderVisitor::SimpleGLRenderVisitor(std::string _texturedir) :  texturedir(_texturedir)
	{
#ifdef WIN32	
	  //TODO: Fix for Linux..
		glGenerateMipmapfunc = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmapEXT");
		if(!glGenerateMipmapfunc)
		{
			//printf("");
		}
#endif
	}

	void SimpleGLRenderVisitor::visiting(scenegraph::Scene *scene)
	{
		if(scene->stateMap.empty())
			scene->buildStateFaces();
		bool first = true;

		std::map<std::string,std::vector<Face *> >::iterator texture_iter = scene->stateMap.begin();
		while(texture_iter!=scene->stateMap.end())
		{
			if(!texture_iter->second.size())
			{
				texture_iter++;
				continue;
			}
			Face *firstface = texture_iter->second.at(0);

			int numVerts(firstface->verts.size());

			glPushAttrib(GL_ALL_ATTRIB_BITS);
				
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			if(firstface->textures.size() > 0)
			{
				GLuint texid = getOrLoadTextureID(firstface->textures[0].GetTextureName());
				glBindTexture(GL_TEXTURE_2D, texid);
				glEnable(GL_TEXTURE_2D);
			}
			//glPolygonMode(GL_FRONT_AND_BACK, GL_WIRE);
			switch(numVerts)
			{
				case 1:
				case 2:
					{
						texture_iter++;
						continue;
					}
				case 3:
					glBegin(GL_TRIANGLES);
					break;
				case 4:
					glBegin(GL_QUADS);
					break;
				default:
					glBegin(GL_POLYGON);
			}

			BOOST_FOREACH(Face *face,texture_iter->second)
			{
				for(int j = 0; j < numVerts; ++j)
				{
					if(j < face->vertexNormals.size())
						glNormal3d(face->vertexNormals.at(j).X(), face->vertexNormals.at(j).Y(), face->vertexNormals.at(j).Z());
					if(face->textures.size() > 0)
						glTexCoord2d(face->textures.at(0).uvs.at(j).X(), face->textures.at(0).uvs.at(j).Y());			
					glVertex3d(face->verts.at(j).X(), face->verts.at(j).Y(), face->verts.at(j).Z());
				}
			}
			texture_iter++;

			glEnd();
			glPopAttrib();
		}

		traverse(scene);
	}

	void SimpleGLRenderVisitor::render(scenegraph::Scene *scene)
	{
		if(scene)
			visit(scene);
	}

    void SimpleGLRenderVisitor::setExtensions()
    {
#ifdef WIN32	
        //TODO: Fix for Linux..
        if (!glGenerateMipmapfunc)
            glGenerateMipmapfunc = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmap");
        if (!glGenerateMipmapfunc)
            glGenerateMipmapfunc = (PFNGLGENERATEMIPMAPPROC)wglGetProcAddress("glGenerateMipmapEXT");
        if (!glGenerateMipmapfunc)
        {
            //printf("");
        }
#endif

    }


}
