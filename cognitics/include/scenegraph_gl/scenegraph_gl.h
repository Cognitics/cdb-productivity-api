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
