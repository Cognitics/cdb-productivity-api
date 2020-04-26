/*************************************************************************
Copyright (c) 2020 Cognitics, Inc.

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

#include <scenegraphflt/scenegraphflt.h>
#include <scenegraph/FlattenVisitor.h>
#include <scenegraph/TransformVisitor.h>
#include "ModelKit/FlightToCSM.h"

#ifdef WIN32
#define snprintf _snprintf
#endif

namespace modelkit
{
	static double epsilon = 0.05;

	int FlightToCSMImporter::countPlanePoints(scenegraph::Face &face)
	{
		int count = 0;
		int numVerts = face.getNumVertices();
		for(int i=0;i<numVerts;i++)
		{
			sfa::Point pt = face.getVertN(i);
			if(abs(pt.Z())<epsilon)
				count ++;
		}
		return count;
	}

	modelkit::Material *FlightToCSMImporter::getMaterial(scenegraph::Face &face,modelkit::CrossSection &csm)
	{
		modelkit::Material *mkMat = NULL;
		for(size_t mi=0;mi<face.materials.size();mi++)
		{
			scenegraph::Material &material = face.materials.at(mi);
			//find the current MaterialInfo instance in the list of unique material Infos, then use the corresponding
			//index into that list as the ID to get the reference to the modelkit material from the cross section.
			size_t mIdx;
			bool found = false;
			for (mIdx = 0; mIdx < uniqueMaterials.size(); mIdx++)
			{
				if (uniqueMaterials[mIdx] == material)
				{
					found = true;
					break;
				}
			}
			if (found)
			{
				char id[30];
				//itoa((int)mIdx, id, 10);
				snprintf(id,10,"%d",int(mIdx));
				mkMat = csm.getMaterial(id);
			}
		}
		return mkMat;
	}

	bool FlightToCSMImporter::ImportFltXZ(const std::string &fltfile, const std::string &csmfile)
	{
		//swap y and z
		sfa::Matrix mat(-1,0,0,0,
						0,0,1,0,
						0,1,0,0);
		modelkit::CrossSection csm;
		bool ret = ImportFlt(fltfile,csm,mat);
		if(ret)
		{
			csm.write(csmfile);
		}
		
		return ret;
	}

	bool FlightToCSMImporter::ImportFlt(const std::string &filename, modelkit::CrossSection &csm, const sfa::Matrix &transform)
	{
		//Use scengraphflt to read the flight file
		scenegraph::Scene *scene = scenegraph::buildSceneFromOpenFlight(filename,true);
		
		bool ret = ImportFlt(scene,csm,transform);
		delete scene;
		return ret;
	}

	bool FlightToCSMImporter::ImportFlt(scenegraph::Scene *scene, modelkit::CrossSection &csm, const sfa::Matrix &transform)
	{	    
		scenegraph::FlattenVisitor flattenVisitor;
		scenegraph::Scene *flatScene = flattenVisitor.flatten(scene);
		//Project all the points with the transformation matrix
		scenegraph::TransformVisitor transformVisitor(transform);
		flatScene = transformVisitor.transform(flatScene);
		//scenegraph::buildOpenFlightFromScene("C:\\dev\\cpr\\data\\modelkits\\test-xform.flt",flatScene);
		//Collect textures
		scenegraph::MappedTextureList textures = flatScene->getUniqueTextures();
		BOOST_FOREACH(scenegraph::MappedTexture tex,textures)
		{
			std::string texname = tex.GetTextureName();
			ccl::FileInfo fileinfo(texname);
			if(csm.getTexture(fileinfo.getBaseName()) == NULL)
				csm.addTexture(fileinfo.getBaseName());
		}

		// Gather all unique materials.
		int id = 0;
		uniqueMaterials = flatScene->getUniqueMaterials();
		for(scenegraph::MaterialList::iterator it = uniqueMaterials.begin(), end = uniqueMaterials.end(); it != end; ++it, ++id)
		{
			char idstr[30];
			//itoa(id, idstr, 10);
			snprintf(idstr,30,"%d",id);
			modelkit::Material *mkMat = new modelkit::Material();
			mkMat->setID(idstr);
			mkMat->setAmbient(modelkit::RGBA(it->ambient.r, it->ambient.g, it->ambient.b, it->ambient.a));
			mkMat->setDiffuse(modelkit::RGBA(it->diffuse.r, it->diffuse.g, it->diffuse.b, it->diffuse.a));
			mkMat->setSpecular(modelkit::RGBA(it->specular.r, it->specular.g, it->specular.b, it->specular.a));
			mkMat->setEmission(modelkit::RGBA(it->emission.r, it->emission.g, it->emission.b, it->emission.a));
			mkMat->setTransparency(it->transparency);
			mkMat->setShine(it->shine);
			csm.addMaterial(mkMat);  //add the material to the cross section materials map.
		}

		int edgeNodeID = 0;
		BOOST_FOREACH(scenegraph::Face face,flatScene->faces)
		{			
			char edgeNodeName[1000];
			int numPlanePoints = countPlanePoints(face);
			if(numPlanePoints==3)
			{
				//Collect all faces with 3 points with y=0 (use epsilon 10e-4)
				modelkit::Face *mkface = new modelkit::Face;
				mkface->setFlags(face.attributes);
				if(face.drawBothSides)
					mkface->getFlags().setAttribute("bothsides",true);
				mkface->addPoint(face.getVertN(0));
				mkface->addPoint(face.getVertN(1));
				mkface->addPoint(face.getVertN(2));
				mkface->setFID(face.featureID);
				mkface->setSMC(face.smc);
				for(scenegraph::MappedTextureList::iterator tit = face.textures.begin(), tend = face.textures.end(); tit != tend; ++tit)
				{
					modelkit::TextureReference *textureReference = new modelkit::TextureReference;
					ccl::FileInfo fileinfo(tit->GetTextureName());
					textureReference->setTexture(csm.getTexture(fileinfo.getBaseName()));
					for(std::vector<sfa::Point>::iterator pit = tit->uvs.begin(), pend = tit->uvs.end(); pit != pend; ++pit)
						textureReference->addUV(*pit);
					mkface->addTextureReference(textureReference);
				}

				//Add material as unique to the modelkit cross section and a reference to the face.                                                     
				modelkit::Material *mkMat = getMaterial(face,csm);
				if (mkMat)
					mkface->setMaterialRef(mkMat);  //set material reference on the face.

				if(face.primaryColor.isInitialized())
				{
					mkface->setPrimaryColor(face.primaryColor);
				}
				csm.addFace(mkface);
			}
			if(numPlanePoints==2)
			{
				sfa::Point A,B,C;
				unsigned int a_idx;
				unsigned int b_idx;
				unsigned int c_idx;
				sfa::Point normal = face.GetFaceNormal();
				//Find the 2 points on the plane
				//Find the one point not on the plane and set it up 
				//from there
				if(abs(face.getVertN(0).Z())>=epsilon)
				{
					A = face.getVertN(2);
					a_idx = 2;
					B = face.getVertN(1);
					b_idx = 1;
					C = face.getVertN(0);
					c_idx = 0;
				}
				else if(abs(face.getVertN(1).Z())>=epsilon)
				{
					A = face.getVertN(0);
					a_idx = 0;
					B = face.getVertN(2);
					b_idx = 2;
					C = face.getVertN(1);
					c_idx = 1;
				}
				else if(abs(face.getVertN(2).Z())>=epsilon)
				{
					a_idx = 1;
					b_idx = 0;
					c_idx = 2;
					A = face.getVertN(1);
					B = face.getVertN(0);
					C = face.getVertN(2);
				}
				else
				{
					//error! someone is lying to us
				}

				//Collect all edges with 2 points with z=0
				modelkit::Edge *edge = new modelkit::Edge;
				edge->setFlags(face.attributes);
				if(face.drawBothSides)
					edge->getFlags().setAttribute("bothsides",true);
				edge->setFID(face.featureID);
				edge->setSMC(face.smc);
				edge->setProjectedZ(C.Z());
				edge->addPoint(sfa::Point(A.X(),A.Y()));
				edge->addPoint(sfa::Point(B.X(),B.Y()));
				edge->setNormal(sfa::Point(normal.X(), normal.Y()));
				sfa::Point otherpt(A.X(),A.Y(),A.Z()+C.Z());//C.Z() is the projected Z
				edge->addPoint(sfa::Point(otherpt.X(),otherpt.Z()));

				scenegraph::Face face2d(face);
				face2d.verts.clear();
				face2d.addVert(sfa::Point(A.X(), A.Y(), A.Z()));
				face2d.addVert(sfa::Point(B.X(), B.Y(), B.Z()));
				face2d.addVert(sfa::Point(C.X(), C.Y(), C.Z()));
				face2d.RemapTextureCoords(a_idx, b_idx, c_idx);
				
				sprintf(edgeNodeName,"Edge%d",edgeNodeID++);

				for(size_t ti = 0; ti < face2d.textures.size(); ++ti)
				{
					scenegraph::MappedTexture &texture = face.textures.at(ti);
					modelkit::TextureReference *textureReference = new modelkit::TextureReference;
					ccl::FileInfo fileinfo(texture.GetTextureName());
					textureReference->setTexture(csm.getTexture(fileinfo.getBaseName()));
					textureReference->addUV(sfa::Point(texture.uvs.at(a_idx).X(), texture.uvs.at(a_idx).Y()));
					textureReference->addUV(sfa::Point(texture.uvs.at(b_idx).X(), texture.uvs.at(b_idx).Y()));
					textureReference->addUV(face.getUVForVertex(sfa::Point(otherpt.X(),otherpt.Y(),otherpt.Z()),ti));
					edge->addTextureReference(textureReference);
				}
			
				edge->setName(std::string(edgeNodeName));
			
				//Add material as unique to the modelkit cross section and a reference to the face.                   
				modelkit::Material *mkMat = getMaterial(face,csm);            
				if (mkMat)
					edge->setMaterialRef(mkMat);  //set material reference on the edge.
				if(face.primaryColor.isInitialized())
				{
					edge->setPrimaryColor(face.primaryColor);
				}
				csm.addEdge(edge);
			}		
			
		}
		
		//TODO:
		//Collect attachment points with z=0

		delete flatScene;;
		return true;
	}


}
