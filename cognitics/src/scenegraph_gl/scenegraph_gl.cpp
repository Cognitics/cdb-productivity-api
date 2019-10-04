/****************************************************************************
Copyright 2013 Cognitics, Inc.
Contract No:  N61340-11-C-0008
Contractor Name: Cognitics, Inc.
Contractor Address: 4250 N Glenwood St, Boise ID 83704

Expiration of SBIR Data Rights Period: February 28, 2019

SBIR DATA RIGHTS - The Government's rights to use, modify, reproduce,
release, perform, display, or disclose technical data or computer software
marked with this legend are restricted during the period shown as provided
in paragraph (b)(4) of the Rights in Noncommercial Technical Data and
Computer Software -- Small Business Innovative Research (SBIR) Program
clause contained in the above identified contract. No restrictions apply
after the expiration date shown above. Any reproduction of technical data,
computer software, or portions thereof marked with this legend must also
reproduce the markings.

This software contains inseparable components developed commercially, and 
may not be reproduced or used without a written license from Cognitics, Inc.
****************************************************************************/

#include "scenegraph_gl.h"
#include <ip/jpgwrapper.h>

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

	GLuint SimpleGLRenderVisitor::getOrLoadTextureID(const std::string &texname)
	{
		std::string texpath = ccl::joinPaths(texturedir,texname);	
		if(textures.find(texname)!=textures.end())
			return textures[texname];
		ccl::binary buffer;
		ccl::binary buffer2;
		ip::ImageInfo info;
		if(ip::GetImagePixels(texpath,info,buffer))
		{
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



}
