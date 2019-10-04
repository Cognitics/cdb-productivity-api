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
#pragma once

#ifdef WIN32
#include <Windows.h>
#endif

#include <GL/gl.h>
#include <GL/glu.h>

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <scenegraph/Visitor.h>
#include <scenegraph/Face.h>
#include <scenegraph/Scene.h>
#include <sfa/Matrix.h>


namespace scenegraph
{
	typedef void (*PFNGLGENERATEMIPMAPPROC) (GLenum target);
	

	class SimpleGLRenderVisitor : public scenegraph::Visitor
	{
		PFNGLGENERATEMIPMAPPROC glGenerateMipmapfunc;
		std::string texturedir;
		GLenum glerror;
		std::map<std::string,GLuint> textures;
		void setupGLMatrixFromSFA(const sfa::Matrix &m,double *glmat);
		GLuint getOrLoadTextureID(const std::string &texname);
	public:
		void renderFace(scenegraph::Face &face);
		virtual ~SimpleGLRenderVisitor(void);
		SimpleGLRenderVisitor(std::string _texturedir);
		virtual void visiting(scenegraph::Scene *scene);
		void render(scenegraph::Scene *scene);
	};

}
