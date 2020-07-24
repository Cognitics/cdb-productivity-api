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

#include <math.h>
#include "ModelKit/CrossSection.h"
#include "ModelKit/Connection.h"
#include <dom/dom.h>
#include <boost/foreach.hpp>

namespace modelkit
{
	CrossSection::~CrossSection(void)
	{
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			delete *it;
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			delete *it;
		for(std::vector<Texture *>::iterator it = textures.begin(), end = textures.end(); it != end; ++it)
			delete *it;
        for(std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
			delete *it;
		for(std::vector<AttachmentPoint*>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			delete *it;

        materialMap.erase(materialMap.begin(), materialMap.end());
	}

    CrossSection::CrossSection(void) : PI(3.14159265358979323846)
	{
	}

    CrossSection::CrossSection(const CrossSection &copy) : PI(copy.PI)
	{
		filename = copy.filename;
        name = copy.name;
        materialMap = copy.materialMap;

		std::map<Texture *, Texture *> textureMap;
		for(std::vector<Texture *>::const_iterator it = copy.textures.begin(), end = copy.textures.end(); it != end; ++it)
			textureMap[*it] = addTexture((*it)->getSource());
		for(std::vector<Edge *>::const_iterator it = copy.edges.begin(), end = copy.edges.end(); it != end; ++it)
		{
			Edge *edge = new Edge(*(*it));
			std::vector<TextureReference *> textureReferences = edge->getTextureReferences();
			for(std::vector<TextureReference *>::iterator tit = textureReferences.begin(), tend = textureReferences.end(); tit != tend; ++tit)
				(*tit)->setTexture(textureMap[(*tit)->getTexture()]);
			edges.push_back(edge);
		}
		for(std::vector<Face *>::const_iterator it = copy.faces.begin(), end = copy.faces.end(); it != end; ++it)
		{
			Face *face = new Face(*(*it));
            face->setMaterialRef((*it)->getMaterialRef());
			std::vector<TextureReference *> textureReferences = face->getTextureReferences();
			for(std::vector<TextureReference *>::iterator tit = textureReferences.begin(), tend = textureReferences.end(); tit != tend; ++tit)
				(*tit)->setTexture(textureMap[(*tit)->getTexture()]);
            faces.push_back(face);
		}
		for(std::vector<AttachmentPoint *>::const_iterator it = copy.attachmentPoints.begin(), end = copy.attachmentPoints.end(); it != end; ++it)
			attachmentPoints.push_back(new AttachmentPoint(*(*it)));
		for(std::vector<AvoidArea *>::const_iterator it = copy.avoidAreas.begin(), end = copy.avoidAreas.end(); it != end; ++it)
			avoidAreas.push_back(new AvoidArea(*(*it)));
	}

	CrossSection::CrossSection(const std::string &file) : filename(file), PI(3.14159265358979323846)
	{
		dom::DocumentSP doc = dom::DOMImplementation::readXML(filename);
		if(!doc)
			return;
		dom::ElementSP element = doc->getDocumentElement();
		if(!element || (element->getNodeName() != "CrossSection"))
			return;
		if(element->hasAttribute("name"))
			setName(element->getAttribute("name").as_string());
		std::map<int, Texture *> textureMap;
		dom::ElementList children = element->getChildElements();
		for(dom::ElementList::iterator it = children.begin(), end = children.end(); it != end; ++it)
		{
			dom::ElementSP child = *it;
			if((child->getNodeName() == "Texture") && child->hasAttribute("id"))
			{
				Texture *texture = addTexture(child->getTextContent());
               if (!texture)
                   continue;
				textureMap[child->getAttribute("id").as_int()] = texture;
				if(child->hasAttribute("alias"))
					texture->setAlias(child->getAttribute("alias").as_string());
			}
            if((child->getNodeName() == "Material") && child->hasAttribute("id"))
			{
                std::string id = child->getAttribute("id").as_string();
                double transparency = child->hasAttribute("transparency") ? child->getAttribute("transparency").as_double() : 0.0;
                double shine = child->hasAttribute("shine") ? child->getAttribute("shine").as_double() : 0.0;
                dom::ElementList grandchildren = child->getChildElements();
				RGBA ambient, diffuse, specular, emission;
                for(dom::ElementList::iterator gItr = grandchildren.begin(), gEnd = grandchildren.end(); gItr != gEnd; ++gItr)
				{
                    double r = (*gItr)->hasAttribute("r") ? (*gItr)->getAttribute("r").as_double() : 0.0;
                    double g = (*gItr)->hasAttribute("g") ? (*gItr)->getAttribute("g").as_double() : 0.0;
                    double b = (*gItr)->hasAttribute("b") ? (*gItr)->getAttribute("b").as_double() : 0.0;
                    double a = (*gItr)->hasAttribute("a") ? (*gItr)->getAttribute("a").as_double() : 0.0;
                    if ( (*gItr)->getNodeName() == "Ambient")
                        ambient = RGBA(r, g, b, a);
                    else if ( (*gItr)->getNodeName() == "Diffuse")
                        diffuse = RGBA(r, g, b, a);
                    else if ( (*gItr)->getNodeName() == "Specular")
                        specular = RGBA(r, g, b, a);
                    else if ( (*gItr)->getNodeName() == "Emission")
                        emission = RGBA(r, g, b, a);
                }
                Material *mat = new Material(id, transparency, shine, ambient, diffuse, specular, emission);
                addMaterial(mat);
            }
			if(child->getNodeName() == "Edge")
			{
				Edge *edge = new Edge;
				if(child->hasAttribute("name"))
					edge->setName(child->getAttribute("name").as_string());
				if(child->hasAttribute("fid"))
					edge->setFID(child->getAttribute("fid").as_int());
				if(child->hasAttribute("smc"))
					edge->setSMC(child->getAttribute("smc").as_int());
				if(child->hasAttribute("isTerrain"))
					edge->setTerrain(child->getAttribute("isTerrain").as_string() == "true");
				if(child->hasAttribute("projectedZ"))
					edge->setProjectedZ(child->getAttribute("projectedZ").as_double());
                 if(child->hasAttribute("materialID"))
                 {
                    std::string id = child->getAttribute("materialID").as_string();
                    if (id != "" && id != "-1")
                    {    
                        Material *mat = this->getMaterial(id);
                        if (!mat)
                            ccl::Log::instance()->write(ccl::LERR, "Error: attempting to load a cross section metarial reference before the material as been loaded.");
                        else
                            edge->setMaterialRef(mat);
                    }
                }
				dom::NamedNodeMap attrlist = child->getAttributes();
				for(dom::NamedNodeMap::iterator it = attrlist.begin(), end = attrlist.end(); it != end; ++it)
				{
					dom::AttrSP attr = std::dynamic_pointer_cast<dom::Attr>(it->second);
					if((attr->getName() == "name")
						|| (attr->getName() == "fid")
						|| (attr->getName() == "smc"))
						continue;
					edge->getAttributes().setAttribute(attr->getName(), attr->getValue().as_string());
				}

				dom::ElementList childChildren = child->getChildElements();
				for(dom::ElementList::iterator cit = childChildren.begin(), cend = childChildren.end(); cit != cend; ++cit)
				{
					dom::ElementSP childChild = *cit;
					if(childChild->getNodeName() == "Flags")
					{
						dom::NamedNodeMap attrlist = childChild->getAttributes();
						for(dom::NamedNodeMap::iterator it = attrlist.begin(), end = attrlist.end(); it != end; ++it)
						{
							dom::AttrSP attr = std::dynamic_pointer_cast<dom::Attr>(it->second);
							if((attr->getValue().as_string() == "true") || (attr->getValue().as_int() > 0))
								edge->getFlags().setAttribute(attr->getName(), true);
						}
					}
					if(childChild->getNodeName() == "Point")
					{
						sfa::Point point;
						if(childChild->hasAttribute("x"))
							point.setX(childChild->getAttribute("x").as_double());
						if(childChild->hasAttribute("y"))
							point.setY(childChild->getAttribute("y").as_double());
						edge->addPoint(point);
					}
					if(childChild->getNodeName() == "Normal")
					{
						sfa::Point normal;
						if(childChild->hasAttribute("x"))
							normal.setX(childChild->getAttribute("x").as_double());
						if(childChild->hasAttribute("y"))
							normal.setY(childChild->getAttribute("y").as_double());
						edge->setNormal(normal);
					}
					if((childChild->getNodeName() == "TextureReference") && childChild->hasAttribute("textureID"))
					{
						TextureReference *textureReference = new TextureReference;
						textureReference->setTexture(textureMap[childChild->getAttribute("textureID").as_int()]);
						dom::ElementList uvChildren = childChild->getChildElements();
						for(dom::ElementList::iterator uvit = uvChildren.begin(), uvend = uvChildren.end(); uvit != uvend; ++uvit)
						{
							dom::ElementSP uvChild = *uvit;
							if(uvChild->getNodeName() == "UV")
							{
								sfa::Point uv;
								if(uvChild->hasAttribute("x"))
									uv.setX(uvChild->getAttribute("x").as_double());
								if(uvChild->hasAttribute("y"))
									uv.setY(uvChild->getAttribute("y").as_double());
								textureReference->addUV(uv);
							}
						}
						edge->addTextureReference(textureReference);
					}
					if(childChild->getNodeName() == "PrimaryColor")
					{
						double r = childChild->hasAttribute("r") ? childChild->getAttribute("r").as_double() : 0.0;
						double g = childChild->hasAttribute("g") ? childChild->getAttribute("g").as_double() : 0.0;
						double b = childChild->hasAttribute("b") ? childChild->getAttribute("b").as_double() : 0.0;
						double a = childChild->hasAttribute("a") ? childChild->getAttribute("a").as_double() : 0.0;
						scenegraph::Color color(r,g,b,a);
						edge->setPrimaryColor(color);
					}
					if(childChild->getNodeName() == "AlternateColor")
					{
						double r = childChild->hasAttribute("r") ? childChild->getAttribute("r").as_double() : 0.0;
						double g = childChild->hasAttribute("g") ? childChild->getAttribute("g").as_double() : 0.0;
						double b = childChild->hasAttribute("b") ? childChild->getAttribute("b").as_double() : 0.0;
						double a = childChild->hasAttribute("a") ? childChild->getAttribute("a").as_double() : 0.0;
						scenegraph::Color color(r,g,b,a);
						edge->setAlternateColor(color);
					}
				}
				addEdge(edge);
			}
			if(child->getNodeName() == "Face")
			{
				Face *face = new Face;
				if(child->hasAttribute("name"))
					face->setName(child->getAttribute("name").as_string());
				if(child->hasAttribute("fid"))
					face->setFID(child->getAttribute("fid").as_int());
				if(child->hasAttribute("smc"))
					face->setSMC(child->getAttribute("smc").as_int());
                if(child->hasAttribute("materialID"))
                {
                    std::string id = child->getAttribute("materialID").as_string();
                    if (id != "" && id != "-1")
                    {
                        Material *mat = this->getMaterial(id);
                        if (!mat)
                            ccl::Log::instance()->write(ccl::LERR, "Error: attempting to load a cross section metarial reference before the material as been loaded.");
                        else
                            face->setMaterialRef(mat);
                    }
                }
				dom::ElementList childChildren = child->getChildElements();
				for(dom::ElementList::iterator cit = childChildren.begin(), cend = childChildren.end(); cit != cend; ++cit)
				{
					dom::ElementSP childChild = *cit;
					if(childChild->getNodeName() == "Flags")
					{
						dom::NamedNodeMap attrlist = childChild->getAttributes();
						for(dom::NamedNodeMap::iterator it = attrlist.begin(), end = attrlist.end(); it != end; ++it)
						{
							dom::AttrSP attr = std::dynamic_pointer_cast<dom::Attr>(it->second);
							if((attr->getValue().as_string() == "true") || (attr->getValue().as_int() > 0))
								face->getFlags().setAttribute(attr->getName(), true);
						}
					}
					if(childChild->getNodeName() == "Point")
					{
						sfa::Point point;
						if(childChild->hasAttribute("x"))
							point.setX(childChild->getAttribute("x").as_double());
						if(childChild->hasAttribute("y"))
							point.setY(childChild->getAttribute("y").as_double());
						face->addPoint(point);
					}
					if((childChild->getNodeName() == "TextureReference") && childChild->hasAttribute("textureID"))
					{
						TextureReference *textureReference = new TextureReference;
						textureReference->setTexture(textureMap[childChild->getAttribute("textureID").as_int()]);
						dom::ElementList uvChildren = childChild->getChildElements();
						for(dom::ElementList::iterator uvit = uvChildren.begin(), uvend = uvChildren.end(); uvit != uvend; ++uvit)
						{
							dom::ElementSP uvChild = *uvit;
							if(uvChild->getNodeName() == "UV")
							{
								sfa::Point uv;
								if(uvChild->hasAttribute("x"))
									uv.setX(uvChild->getAttribute("x").as_double());
								if(uvChild->hasAttribute("y"))
									uv.setY(uvChild->getAttribute("y").as_double());
								textureReference->addUV(uv);
							}
						}
						face->addTextureReference(textureReference);
					}
					if(childChild->getNodeName() == "PrimaryColor")
					{
						double r = childChild->hasAttribute("r") ? childChild->getAttribute("r").as_double() : 0.0;
						double g = childChild->hasAttribute("g") ? childChild->getAttribute("g").as_double() : 0.0;
						double b = childChild->hasAttribute("b") ? childChild->getAttribute("b").as_double() : 0.0;
						double a = childChild->hasAttribute("a") ? childChild->getAttribute("a").as_double() : 0.0;
						scenegraph::Color color(r,g,b,a);
						face->setPrimaryColor(color);
					}
					if(childChild->getNodeName() == "AlternateColor")
					{
						double r = childChild->hasAttribute("r") ? childChild->getAttribute("r").as_double() : 0.0;
						double g = childChild->hasAttribute("g") ? childChild->getAttribute("g").as_double() : 0.0;
						double b = childChild->hasAttribute("b") ? childChild->getAttribute("b").as_double() : 0.0;
						double a = childChild->hasAttribute("a") ? childChild->getAttribute("a").as_double() : 0.0;
						scenegraph::Color color(r,g,b,a);
						face->setAlternateColor(color);
					}
				}
				addFace(face);
			}
            if(child->getNodeName() == "AvoidArea")
            {
                AvoidArea *aa = new AvoidArea();
                dom::ElementList grandchildren = child->getChildElements();
				for(dom::ElementList::iterator gcIt = grandchildren.begin(), gcEnd = grandchildren.end(); gcIt != gcEnd; ++gcIt)
				{
					dom::ElementSP grandchild = *gcIt;
					if(grandchild->getNodeName() == "Point")
					{
                        sfa::Point pt;
                        if(grandchild->hasAttribute("x"))
							pt.setX(grandchild->getAttribute("x").as_double());
						if(grandchild->hasAttribute("y"))
							pt.setY(grandchild->getAttribute("y").as_double());                                   

                        aa->addPoint(pt);
                    }
                }
                addAvoidArea(aa);
            }
			if((child->getNodeName() == "AttachmentPoint") && child->hasAttribute("name"))
			{
				sfa::Point point;
				dom::ElementList childChildren = child->getChildElements();
				for(dom::ElementList::iterator cit = childChildren.begin(), cend = childChildren.end(); cit != cend; ++cit)
				{
					dom::ElementSP pointChild = *cit;
					if(pointChild->getNodeName() == "Point")
					{
						if(pointChild->hasAttribute("x"))
							point.setX(pointChild->getAttribute("x").as_double());
						if(pointChild->hasAttribute("y"))
							point.setY(pointChild->getAttribute("y").as_double());
					}
				}
				addAttachmentPoint(child->getAttribute("name").as_string(), point);
			}
		}
		
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
		{
			//The value in the CSM file is not actually used, we'll always
			//rely on the right handed rule for the normal (the front of the face is counter-clockwise)
			Edge *edge = *it;
			edge->calculateNormals();
		}
		
	}

	bool CrossSection::invertFaceNormals()
	{		
		BOOST_FOREACH(Face *face,faces)
		{
			face->invertNormal();
		}
		return true;
	}

	bool CrossSection::invertEdgeNormals()
	{
		//Flip around each edge
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
		{
			Edge *edge = *it;
			edge->invertNormal();
			edge->calculateNormals();
		}
		
		return true;
	}

	bool CrossSection::write(const std::string &file)
	{
		if(file.size())
			filename = file;
		if(filename.empty())
			return false;
		snapPoints();

		dom::DOMImplementationSP domImplementation(new dom::DOMImplementation);
		dom::DocumentSP doc = domImplementation->createDocument(dom::DOMString(), "CrossSection");
		dom::ElementSP element = doc->getDocumentElement();
		if(name.size())
			element->setAttribute("name", name);
		int i = 0;
		for(std::vector<Texture *>::iterator it = textures.begin(), end = textures.end(); it != end; ++it, ++i)
		{
			dom::ElementSP child = doc->createElement("Texture");
			child->setAttribute("id", i);
			if((*it)->getAlias().size())
				child->setAttribute("alias", (*it)->getAlias());
			child->setTextContent((*it)->getSource());
			element->appendChild(child);
		}
        for (std::map<std::string, Material*>::iterator it = materialMap.begin(), end = materialMap.end(); it != end; ++it)
        {
            Material *mat = it->second;
            dom::ElementSP child = doc->createElement("Material");
            child->setAttribute("id", mat->getID());
            child->setAttribute("shine", mat->getShine());
            child->setAttribute("transparency", mat->getTransparency());
            //ambient
            dom::ElementSP ambientChild = doc->createElement("Ambient");
            ambientChild->setAttribute("r", formatDoubleToStr(mat->getAmbient().getR()));
            ambientChild->setAttribute("b", formatDoubleToStr(mat->getAmbient().getB()));
            ambientChild->setAttribute("g", formatDoubleToStr(mat->getAmbient().getG()));
            ambientChild->setAttribute("a", formatDoubleToStr(mat->getAmbient().getA()));
			child->appendChild(ambientChild);
            //diffuse
            dom::ElementSP diffuseChild = doc->createElement("Diffuse");
            diffuseChild->setAttribute("r", formatDoubleToStr(mat->getDiffuse().getR()));
			diffuseChild->setAttribute("g", formatDoubleToStr(mat->getDiffuse().getG()));
            diffuseChild->setAttribute("b", formatDoubleToStr(mat->getDiffuse().getB()));
            diffuseChild->setAttribute("a", formatDoubleToStr(mat->getDiffuse().getA()));
			child->appendChild(diffuseChild);
            //specular
            dom::ElementSP specularChild = doc->createElement("Specular");
            specularChild->setAttribute("r", formatDoubleToStr(mat->getSpecular().getR()));
			specularChild->setAttribute("g", formatDoubleToStr(mat->getSpecular().getG()));
            specularChild->setAttribute("b", formatDoubleToStr(mat->getSpecular().getB()));
            specularChild->setAttribute("a", formatDoubleToStr(mat->getSpecular().getA()));
			child->appendChild(specularChild);
            //emission
            dom::ElementSP emissionChild = doc->createElement("Emission");
            emissionChild->setAttribute("r", formatDoubleToStr(mat->getEmission().getR()));
			emissionChild->setAttribute("g", formatDoubleToStr(mat->getEmission().getG()));
            emissionChild->setAttribute("b", formatDoubleToStr(mat->getEmission().getB()));
            emissionChild->setAttribute("a", formatDoubleToStr(mat->getEmission().getA()));
			child->appendChild(emissionChild);
            
            element->appendChild(child);
        }

		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it, ++i)
		{
			Edge *edge = *it;
			dom::ElementSP child = doc->createElement("Edge");
			child->setAttribute("name", edge->getName());
			child->setAttribute("fid", edge->getFID());
			child->setAttribute("smc", edge->getSMC());
			child->setAttribute("isTerrain", (edge->isTerrain() ? "true" : "false"));
			child->setAttribute("projectedZ", edge->getProjectedZ());
            std::string id = edge->getMaterialRef() ? edge->getMaterialRef()->getID() : "";
            child->setAttribute("materialID", id) ;
			ccl::AttributeContainer &attributes = edge->getAttributes();
			std::vector<std::string> keys = attributes.getKeys();
			ccl::AttributeContainer &flags = edge->getFlags();
			std::vector<std::string> flagKeys = edge->getFlags().getKeys();
			scenegraph::Color primaryColor = edge->getPrimaryColor();
			if(primaryColor.isInitialized())
			{
				dom::ElementSP colorChild = doc->createElement("PrimaryColor");
				colorChild->setAttribute("r", primaryColor.r);
				colorChild->setAttribute("g", primaryColor.g);
				colorChild->setAttribute("b", primaryColor.b);
				colorChild->setAttribute("a", primaryColor.a);
				child->appendChild(colorChild);
			}
			scenegraph::Color alternateColor = edge->getAlternateColor();
			if(alternateColor.isInitialized())
			{
				dom::ElementSP colorChild = doc->createElement("AlternateColor");
				colorChild->setAttribute("r", alternateColor.r);
				colorChild->setAttribute("g", alternateColor.g);
				colorChild->setAttribute("b", alternateColor.b);
				colorChild->setAttribute("a", alternateColor.a);
				child->appendChild(colorChild);
			}
			for(std::vector<std::string>::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
				child->setAttribute(*it, attributes.getAttributeAsVariant(*it));
			if(edge->getFlags().getVariantMap()->size() > 0)
			{
				dom::ElementSP flagsChild = doc->createElement("Flags");
				for(std::vector<std::string>::iterator it = flagKeys.begin(), end = flagKeys.end(); it != end; ++it)
					if(flags.getAttributeAsBool(*it))
						flagsChild->setAttribute(*it, "true");
				child->appendChild(flagsChild);
			}
			PointList points = edge->getPoints();
			for(PointList::iterator pit = points.begin(), pend = points.end(); pit != pend; ++pit)
			{
				dom::ElementSP pointChild = doc->createElement("Point");
				pointChild->setAttribute("x", pit->X());
				pointChild->setAttribute("y", pit->Y());
				child->appendChild(pointChild);
			}
			sfa::Point normal = edge->getNormal();
			dom::ElementSP normalChild = doc->createElement("Normal");
			normalChild->setAttribute("x", normal.X());
			normalChild->setAttribute("y", normal.Y());
			child->appendChild(normalChild);
            std::vector<TextureReference *> textureReferences = edge->getTextureReferences();
			for(std::vector<TextureReference *>::iterator tit = textureReferences.begin(), tend = textureReferences.end(); tit != tend; ++tit)
			{
				TextureReference *textureReference = *tit;
				dom::ElementSP textureReferenceChild = doc->createElement("TextureReference");
				textureReferenceChild->setAttribute("textureID", getTextureIndex(textureReference->getTexture()));
				PointList uvs = textureReference->getUVs();
				for(PointList::iterator pit = uvs.begin(), pend = uvs.end(); pit != pend; ++pit)
				{
					dom::ElementSP uvChild = doc->createElement("UV");
					uvChild->setAttribute("x", pit->X());
					uvChild->setAttribute("y", pit->Y());
					textureReferenceChild->appendChild(uvChild);
				}
				child->appendChild(textureReferenceChild);
			}
			element->appendChild(child);
		}
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it, ++i)
		{
			Face *face = *it;
			dom::ElementSP child = doc->createElement("Face");
			child->setAttribute("name", face->getName());
			child->setAttribute("fid", face->getFID());
			child->setAttribute("smc", face->getSMC());
            std::string id = face->getMaterialRef() ? face->getMaterialRef()->getID() : "";
            child->setAttribute("materialID", id) ;
			scenegraph::Color primaryColor = face->getPrimaryColor();
			if(primaryColor.isInitialized())
			{
				dom::ElementSP colorChild = doc->createElement("PrimaryColor");
				colorChild->setAttribute("r", primaryColor.r);
				colorChild->setAttribute("g", primaryColor.g);
				colorChild->setAttribute("b", primaryColor.b);
				colorChild->setAttribute("a", primaryColor.a);
				child->appendChild(colorChild);
			}
			scenegraph::Color alternateColor = face->getAlternateColor();
			if(alternateColor.isInitialized())
			{
				dom::ElementSP colorChild = doc->createElement("AlternateColor");
				colorChild->setAttribute("r", alternateColor.r);
				colorChild->setAttribute("g", alternateColor.g);
				colorChild->setAttribute("b", alternateColor.b);
				colorChild->setAttribute("a", alternateColor.a);
				child->appendChild(colorChild);
			}
			if(face->getFlags().getVariantMap()->size() > 0)
			{
				dom::ElementSP flagsChild = doc->createElement("Flags");
				std::vector<std::string> flagKeys = face->getFlags().getKeys();
				for(std::vector<std::string>::iterator it = flagKeys.begin(), end = flagKeys.end(); it != end; ++it)
					if(face->getFlags().getAttributeAsBool(*it))
						flagsChild->setAttribute(*it, "true");
				child->appendChild(flagsChild);
			}
			PointList points = face->getPoints();
			for(PointList::iterator pit = points.begin(), pend = points.end(); pit != pend; ++pit)
			{
				dom::ElementSP pointChild = doc->createElement("Point");
				pointChild->setAttribute("x", pit->X());
				pointChild->setAttribute("y", pit->Y());
				child->appendChild(pointChild);
			}
			std::vector<TextureReference *> textureReferences = face->getTextureReferences();
			for(std::vector<TextureReference *>::iterator tit = textureReferences.begin(), tend = textureReferences.end(); tit != tend; ++tit)
			{
				TextureReference *textureReference = *tit;
				dom::ElementSP textureReferenceChild = doc->createElement("TextureReference");
				textureReferenceChild->setAttribute("textureID", getTextureIndex(textureReference->getTexture()));
				PointList uvs = textureReference->getUVs();
				for(PointList::iterator pit = uvs.begin(), pend = uvs.end(); pit != pend; ++pit)
				{
					dom::ElementSP uvChild = doc->createElement("UV");
					uvChild->setAttribute("x", pit->X());
					uvChild->setAttribute("y", pit->Y());
					textureReferenceChild->appendChild(uvChild);
				}
				child->appendChild(textureReferenceChild);
			}
			element->appendChild(child);
		}
        for(std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it, ++i)
		{
            AvoidArea *aa = *it;
            dom::ElementSP child = doc->createElement("AvoidArea");
            for(int aaIdx = 0; aaIdx < (*it)->getNumPoints(); ++aaIdx)
			{
                sfa::Point pt;
                if (aa->getPoint(aaIdx, pt))
                {
                    dom::ElementSP pointChild = doc->createElement("Point");
				    pointChild->setAttribute("x", pt.X());
				    pointChild->setAttribute("y", pt.Y());
				    child->appendChild(pointChild);
                }
            }
            element->appendChild(child);
        }
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it, ++i)
		{
			AttachmentPoint *attachmentPoint = *it;
			dom::ElementSP child = doc->createElement("AttachmentPoint");
			child->setAttribute("name", attachmentPoint->getName());
			sfa::Point point = attachmentPoint->getPoint();
			dom::ElementSP pointChild = doc->createElement("Point");
			pointChild->setAttribute("x", point.X());
			pointChild->setAttribute("y", point.Y());
			child->appendChild(pointChild);
			element->appendChild(child);
		}
		return dom::DOMImplementation::writeXML(this->filename, doc);
	}

	void CrossSection::setFilename(const std::string &file)
	{
		filename = file;
	}

	std::string CrossSection::getFilename(void)
	{
		return filename;
	}

	void CrossSection::setName(const std::string &n)
	{
		name = n;
	}

	std::string CrossSection::getName(void)
	{
		return name;
	}

	void CrossSection::addTexture(Texture *texture)
	{
		textures.push_back(texture);
	}

	Texture *CrossSection::addTexture(const std::string &src)
	{
		Texture *texture = new Texture(src);
		addTexture(texture);
		return texture;
	}

	std::vector<Texture *> CrossSection::getTextures(void)
	{
		return textures;
	}

	Texture *CrossSection::getTexture(const std::string &src)
	{
		for(std::vector<Texture *>::iterator it = textures.begin(), end = textures.end(); it != end; ++it)
		{
			if(src == (*it)->getSource())
				return *it;
		}
		return NULL;
	}

	int CrossSection::getTextureIndex(Texture *texture)
	{
		int i = 0;
		for(std::vector<Texture *>::iterator it = textures.begin(), end = textures.end(); it != end; ++it, ++i)
		{
			if(texture == *it)
				return i;
		}
		return -1;
	}

	bool CrossSection::removeTexture(Texture *texture)
	{
		for(std::vector<Texture *>::iterator it = textures.begin(), end = textures.end(); it != end; ++it)
		{
			if(*it == texture)
			{
				BOOST_FOREACH(Edge *edge, edges)
				{
					edge->removeTextureReference(texture);
				}
				BOOST_FOREACH(Face *face, faces)
				{
					face->removeTextureReference(texture);
				}
				delete *it;
				textures.erase(it);
				return true;
			}
		}
		return false;
	}

	bool CrossSection::removeTexture(const std::string &src)
	{
		return removeTexture(getTexture(src));
	}

	void CrossSection::addEdge(Edge *edge)
	{
		edges.push_back(edge);
	}

	std::vector<Edge *> CrossSection::getEdges(void)
	{
		return edges;
	}

	Edge *CrossSection::getEdge(const std::string &name)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
		{
			if(name == (*it)->getName())
				return *it;
		}
		return NULL;
	}

	Edge *CrossSection::getEdge(int fid)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
		{
			if(fid == (*it)->getFID())
				return *it;
		}
		return NULL;
	}

	bool CrossSection::removeEdge(Edge *edge)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
		{
			if(*it == edge)
			{
				delete *it;
				edges.erase(it);
				return true;
			}
		}
		return false;
	}

	bool CrossSection::removeEdge(const std::string &name)
	{
		return removeEdge(getEdge(name));
	}

	void CrossSection::addFace(Face *face)
	{
		faces.push_back(face);
	}

	Face *CrossSection::addFace(const std::string name, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c)
	{
		Face *face = new Face(name, a, b, c);
		addFace(face);
		return face;
	}

	std::vector<Face *> CrossSection::getFaces(void)
	{
		return faces;
	}

	Face *CrossSection::getFace(const std::string &name)
	{
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
		{
			if(name == (*it)->getName())
				return *it;
		}
		return NULL;
	}

	bool CrossSection::removeFace(Face *face)
	{
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
		{
			if(*it == face)
			{
				delete *it;
				faces.erase(it);
				return true;
			}
		}
		return false;
	}

	bool CrossSection::removeFace(const std::string &name)
	{
		return removeFace(getFace(name));
	}

    bool CrossSection::addAvoidArea(AvoidArea *a)
    {
        if (!a)
            return false;

        avoidAreas.push_back(a);

        return true;

    }
     
    std::vector<AvoidArea*> CrossSection::getAvoidAreas()
    {
        return avoidAreas;
    }
    
    bool CrossSection::removeAvoidArea(AvoidArea &a)
    {
        for (size_t i = 0; i < avoidAreas.size(); i++)
        {
            if (a == *avoidAreas[i])
            {
                avoidAreas.erase(avoidAreas.begin()+i);
                return true;
            }
        }
        
        return false;
    }

	void CrossSection::addAttachmentPoint(AttachmentPoint *attachmentPoint)
	{
		attachmentPoints.push_back(attachmentPoint);
	}

	AttachmentPoint *CrossSection::addAttachmentPoint(const std::string &name, const sfa::Point &point)
	{
		AttachmentPoint *attachmentPoint = new AttachmentPoint(name, point);
		addAttachmentPoint(attachmentPoint);
		return attachmentPoint;
	}

	std::vector<AttachmentPoint *> CrossSection::getAttachmentPoints(void)
	{
		return attachmentPoints;
	}

	AttachmentPoint *CrossSection::getAttachmentPoint(const std::string &name)
	{
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
		{
			if(name == (*it)->getName())
				return *it;
		}
		return NULL;
	}

	bool CrossSection::removeAttachmentPoint(AttachmentPoint *attachmentPoint)
	{
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
		{
			if(*it == attachmentPoint)
			{
				delete *it;
				attachmentPoints.erase(it);
				return true;
			}
		}
		return false;
	}

	bool CrossSection::removeAttachmentPoint(const std::string &name)
	{
		return removeAttachmentPoint(getAttachmentPoint(name));
	}

     bool CrossSection::addMaterial(Material *mat)
     {
         if (!mat)
             return false;

         const std::string &key = mat->getID();
         materialMap[key] = mat;
         
         return true;
     }

     Material* CrossSection::getMaterial(const std::string &id)
     {
        if (materialMap.find(id) == materialMap.end())
            return NULL;  //not found

        return materialMap[id];
     }

	Material *CrossSection::getMatchingMaterial(Material *mat)
	{
		for(std::map<std::string, Material *>::iterator it = materialMap.begin(), end = materialMap.end(); it != end; ++it)
		{
			if(*(it->second) == *mat)
				return it->second;
		}
		return NULL;
	}

     bool CrossSection::removeMaterial(const std::string &id)
     {
        if (materialMap.find(id) == materialMap.end())
            return false;  //not found

        materialMap.erase(id);
        
        return true;  
     }


	void CrossSection::scale(double factor)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			(*it)->scale(factor);
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			(*it)->scale(factor);
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			(*it)->scale(factor);
        for (std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
            (*it)->scale(factor);
	}

	void CrossSection::scaleX(double factor)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			(*it)->scaleX(factor);
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			(*it)->scaleX(factor);
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			(*it)->scaleX(factor);
        for (std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
            (*it)->scaleX(factor);
	}

	void CrossSection::offsetX(double amount)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			(*it)->offsetX(amount);
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			(*it)->offsetX(amount);
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			(*it)->offsetX(amount);
        for (std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
            (*it)->offsetX(amount);
	}

	void CrossSection::translate(double x, double y)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			(*it)->translate(x, y);
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			(*it)->translate(x, y);
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			(*it)->translate(x, y);
        for (std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
            (*it)->translate(x, y);
	}

	void CrossSection::expand(double x, double y)
	{
		for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			(*it)->expand(x, y);
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			(*it)->expand(x, y);
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			(*it)->expand(x, y);
        for (std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
            (*it)->expand(x, y);
	}

    void CrossSection::rotate(double degrees)
    {
        //Calculate the centroid, used as the "pivot point" to rotate our cross section around.
        PointList edgePts;
        for(std::vector<Edge *>::iterator eIt = edges.begin(), edgesEnd = edges.end(); eIt != edgesEnd; ++eIt)
        {
            PointList pts = (*eIt)->getPoints();
            for (PointList::iterator pIt = pts.begin(), ptsEnd = pts.end(); pIt != ptsEnd; ++pIt)
            {
                edgePts.push_back(*pIt);
            }
        }
        sfa::Point centroid = calculateCentroid(edgePts);
        
        //convert the degrees to radians
        double radians = degrees/180 * PI;

        //Rotate the edges, faces, attachment points, and avoid areas.
        for(std::vector<Edge *>::iterator it = edges.begin(), end = edges.end(); it != end; ++it)
			(*it)->rotate(radians, centroid);
		for(std::vector<Face *>::iterator it = faces.begin(), end = faces.end(); it != end; ++it)
			(*it)->rotate(radians, centroid);
		for(std::vector<AttachmentPoint *>::iterator it = attachmentPoints.begin(), end = attachmentPoints.end(); it != end; ++it)
			(*it)->rotate(radians, centroid);
        for(std::vector<AvoidArea *>::iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end; ++it)
			(*it)->rotate(radians, centroid);
    }

    sfa::Point CrossSection::calculateCentroid(PointList &pts)
    {
        if (pts.size() == 0)
        {
            ccl::Log::instance()->write(ccl::LERR, "Cannot calculate centroid: no points specified.");         
            return sfa::Point();
        }

        double xMin, xMax, yMin, yMax;
        double currX, currY;

        PointList::iterator it = pts.begin(), end = pts.end();
        xMin = xMax = (*it).X();
        yMin = yMax = (*it).Y();
        while (++it != end)
        {
            currX = (*it).X();
            currY = (*it).Y();
            if ( currX > xMax )
                xMax = currX;
            else if ( currX < xMin )
                xMin = currX;

            if ( currY > yMax )
                yMax = currY;
            else if ( currY < yMin )
                yMin = currY;
        }
        
        double centerX = currX / (double)pts.size();
        double centerY = currY / (double)pts.size();

        return sfa::Point(centerX, centerY);
    }

	std::string CrossSection::getReferenceSource(Reference *reference)
	{
		if(!reference)
			return std::string();
		ccl::FileInfo info(filename);
		std::string path = info.getDirName();
		std::string fullpath = path.empty() ? reference->getSource() : path + "/" + reference->getSource();
#ifdef WIN32
		for(size_t i=0,ilen=fullpath.size();i<ilen;i++)
		{
			if(fullpath[i]=='/')
				fullpath[i]='\\';
		}
#endif
		return fullpath;
	}

	bool CrossSection::attach(Attachment *localAttachment, Attachment *remoteAttachment, std::vector<Connection *> connections)
	{
		if(!localAttachment || !remoteAttachment)
			return false;
		CrossSectionReference *csRef = remoteAttachment->getCrossSectionReference();
		if(!csRef)
			return false;
		std::string remoteAlias = csRef->getAlias();
		CrossSection remoteCS(getReferenceSource(csRef));

		// figure out our deltas (remote attachment point minus local attachment point)
		AttachmentPoint *remoteAttachmentPoint = remoteCS.getAttachmentPoint(remoteAttachment->getAttachmentPointName());
		if(!remoteAttachmentPoint)
		{
			ccl::Log::instance()->write(ccl::LWARNING, "invalid attachment point for " + remoteCS.getName() + ": " + remoteAttachment->getAttachmentPointName());
			return false;
		}
		AttachmentPoint *thisAttachmentPoint = getAttachmentPoint(localAttachment->getAttachmentPointName());
		if(!thisAttachmentPoint)
		{
			ccl::Log::instance()->write(ccl::LWARNING, "invalid attachment point for " + getName() + ": " + localAttachment->getAttachmentPointName());
			return false;
		}
		sfa::Point remotePoint = remoteAttachmentPoint->getPoint();
		sfa::Point thisPoint = thisAttachmentPoint->getPoint();
		double deltaX = thisPoint.X() - remotePoint.X();
		double deltaY = thisPoint.Y() - remotePoint.Y();

		// copy textures (and remove duplicates)
		std::map<Texture *, Texture *> textureMap;
		std::vector<Texture *> remoteTextures = remoteCS.getTextures();
		for(std::vector<Texture *>::iterator it = remoteTextures.begin(), end = remoteTextures.end(); it != end; ++it)
		{
			Texture *remoteTexture = *it;
			if(!remoteTexture)
				continue;
			bool mapped = false;
			for(std::vector<Texture *>::iterator tit = textures.begin(), tend = textures.end(); tit != tend; ++tit)
			{
				Texture *localTexture = *tit;
				if(getReferenceSource(remoteTexture) == localTexture->getSource())
				{
					textureMap[remoteTexture] = localTexture;
					mapped = true;
				}
			}
			if(!mapped)
			{
				Texture *newTexture = new Texture(remoteTexture->getSource());
				textures.push_back(newTexture);
				textureMap[remoteTexture] = newTexture;
			}
		}

		// copy edges and apply delta to points
		std::vector<Edge *> remoteEdges = remoteCS.getEdges();
		for(std::vector<Edge *>::iterator it = remoteEdges.begin(), end = remoteEdges.end(); it != end; ++it)
		{
			Edge *remoteEdge = *it;
			if(!remoteEdge)
				continue;
			Edge *edge = new Edge;
			edge->setName(remoteAlias + ":" + remoteEdge->getName());
			edge->setNormal(remoteEdge->getNormal());
			edge->setFID(remoteEdge->getFID());
			edge->setSMC(remoteEdge->getSMC());
			edge->setTerrain(remoteEdge->isTerrain());
			edge->setAttributes(remoteEdge->getAttributes());
			edge->setFlags(remoteEdge->getFlags());
			edge->setPrimaryColor(remoteEdge->getPrimaryColor());
			edge->setAlternateColor(remoteEdge->getAlternateColor());
			edge->setProjectedZ(remoteEdge->getProjectedZ());
			PointList points = remoteEdge->getPoints();

			//Add materials
			if(remoteEdge->getMaterialRef())
			{
				Material *mat = getMatchingMaterial(remoteEdge->getMaterialRef());
				if(!mat)
				{
					mat = new Material(*remoteEdge->getMaterialRef());
					addMaterial(mat);
				}
				edge->setMaterialRef(mat);
			}

			for(PointList::iterator pit = points.begin(), pend = points.end(); pit != pend; ++pit)
				edge->addPoint(sfa::Point(pit->X() + deltaX, pit->Y() + deltaY));
			std::vector<TextureReference *> remoteTextureReferences = remoteEdge->getTextureReferences();
			for(std::vector<TextureReference *>::iterator tit = remoteTextureReferences.begin(), tend = remoteTextureReferences.end(); tit != tend; ++tit)
			{
				TextureReference *remoteTextureReference = *tit;
				TextureReference *newTextureReference = new TextureReference;
				newTextureReference->setTexture(textureMap[remoteTextureReference->getTexture()]);
				PointList uvs = remoteTextureReference->getUVs();
				for(PointList::iterator pit = uvs.begin(), pend = uvs.end(); pit != pend; ++pit)
					newTextureReference->addUV(*pit);
				edge->addTextureReference(newTextureReference);
			}
			addEdge(edge);
		}

		// copy faces and apply delta to points
		std::vector<Face *> remoteFaces = remoteCS.getFaces();
		for(std::vector<Face *>::iterator it = remoteFaces.begin(), end = remoteFaces.end(); it != end; ++it)
		{
			Face *remoteFace = *it;
			if(!remoteFace)
				continue;
			Face *face = new Face;
			face->setName(remoteAlias + ":" + remoteFace->getName());
			face->setFID(remoteFace->getFID());
			face->setSMC(remoteFace->getSMC());
			face->setFlags(remoteFace->getFlags());
			face->setPrimaryColor(remoteFace->getPrimaryColor());
			face->setAlternateColor(remoteFace->getAlternateColor());

			PointList points = remoteFace->getPoints();
			for(PointList::iterator pit = points.begin(), pend = points.end(); pit != pend; ++pit)
				face->addPoint(sfa::Point(pit->X() + deltaX, pit->Y() + deltaY));

			//Add materials
			if(remoteFace->getMaterialRef())
			{
				Material *mat = getMatchingMaterial(remoteFace->getMaterialRef());
				if(!mat)
				{
					mat = new Material(*remoteFace->getMaterialRef());
					addMaterial(mat);
				}
				face->setMaterialRef(mat);
			}

			std::vector<TextureReference *> remoteTextureReferences = remoteFace->getTextureReferences();
			for(std::vector<TextureReference *>::iterator tit = remoteTextureReferences.begin(), tend = remoteTextureReferences.end(); tit != tend; ++tit)
			{
				TextureReference *remoteTextureReference = *tit;
				TextureReference *newTextureReference = new TextureReference;
				newTextureReference->setTexture(textureMap[remoteTextureReference->getTexture()]);
				PointList uvs = remoteTextureReference->getUVs();
				for(PointList::iterator pit = uvs.begin(), pend = uvs.end(); pit != pend; ++pit)
					newTextureReference->addUV(*pit);
				face->addTextureReference(newTextureReference);
			}
			addFace(face);
		}

		// copy avoid areas and apply delta to points
		std::vector<AvoidArea *> remoteAvoidAreas = remoteCS.getAvoidAreas();
		for(std::vector<AvoidArea *>::iterator it = remoteAvoidAreas.begin(), end = remoteAvoidAreas.end(); it != end; ++it)
		{
			AvoidArea *remoteAvoidArea = *it;
			if(!remoteAvoidArea)
				continue;
			AvoidArea *avoidArea = new AvoidArea;
			PointList points = remoteAvoidArea->getPoints();
			for(PointList::iterator pit = points.begin(), pend = points.end(); pit != pend; ++pit)
				avoidArea->addPoint(sfa::Point(pit->X() + deltaX, pit->Y() + deltaY));
			addAvoidArea(avoidArea);
		}

		// copy attachment points
		std::vector<AttachmentPoint *> remoteAttachmentPoints = remoteCS.getAttachmentPoints();
		for(std::vector<AttachmentPoint *>::iterator it = remoteAttachmentPoints.begin(), end = remoteAttachmentPoints.end(); it != end; ++it)
		{
			AttachmentPoint *remoteAttachmentPoint = *it;
			if(!remoteAttachmentPoint)
				continue;
			AttachmentPoint *attachmentPoint = new AttachmentPoint;
			attachmentPoint->setName(remoteAlias + ":" + remoteAttachmentPoint->getName());
			sfa::Point point = remoteAttachmentPoint->getPoint();
			attachmentPoint->setPoint(sfa::Point(point.X() + deltaX, point.Y() + deltaY));
			addAttachmentPoint(attachmentPoint);
		}

		// split connections based on presence of attachment
		std::vector<Connection *> attachmentConnections;
		std::vector<Connection *> otherConnections;
		for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it)
		{
			Connection *connection = *it;
			if(connection->hasAttachment(remoteAttachment->getCrossSectionReference()))
				attachmentConnections.push_back(connection);
			else
				otherConnections.push_back(connection);
		}

		// process connections for the attachment
		for(std::vector<Connection *>::iterator it = attachmentConnections.begin(), end = attachmentConnections.end(); it != end; ++it)
		{
			Connection *connection = *it;
			std::vector<Attachment *> attachments = connection->getAttachments();

			// get the local attachment point
			std::string attachmentPoint;
			for(std::vector<Attachment *>::iterator ait = attachments.begin(), aend = attachments.end(); ait != aend; ++ait)
			{
				Attachment *a = *ait;
				if(a->getCrossSectionReference() == remoteAttachment->getCrossSectionReference())
					attachmentPoint = a->getCrossSectionReference()->getAlias() + ":" + a->getAttachmentPointName();
			}
			if(attachmentPoint.empty())
				continue;

			Attachment attachment(remoteAttachment->getCrossSectionReference(), attachmentPoint);
			for(std::vector<Attachment *>::iterator ait = attachments.begin(), aend = attachments.end(); ait != aend; ++ait)
			{
				if((*ait)->getCrossSectionReference() != remoteAttachment->getCrossSectionReference())
					attach(&attachment, *ait, otherConnections);
			}
		}

		return true;
	}

    std::string CrossSection::formatDoubleToStr(double v)
    {
      std::stringstream ss;
      ss.precision(8);
      ss << v;
      return ss.str();
    }

	double CrossSection::getRadius(void) const
	{
		double radius = 0;
		PointList points;
	//	For all Faces get points
		for (std::vector<Face*>::const_iterator it = faces.begin(), end = faces.end(); it != end; it++)
		{
			points = (*it)->getPoints();
			radius = std::max<double>(radius, points[0].length2());
			radius = std::max<double>(radius, points[1].length2());
			radius = std::max<double>(radius, points[2].length2());
		}
		/*
	//	For all avoid area points
		for (std::vector<AvoidArea*>::const_iterator it = avoidAreas.begin(), end = avoidAreas.end(); it != end ; it++)
		{
			points = (*it)->getPoints();
			for (PointList::const_iterator point = points.begin(), end_points = points.end(); point != end_points; point++)
				radius = std::max<double>(radius, point->length2());
		}
		*/

		return sqrt(radius);
	}

	double CrossSection::getHeight(void) const
	{
		double minY = 0;
		double maxY = 0;
		for (std::vector<Face *>::const_iterator it = faces.begin(), end = faces.end(); it != end; ++it)
		{
			PointList points = (*it)->getPoints();
			minY = (minY < points[0].Y()) ? minY : points[0].Y();
			maxY = (maxY > points[0].Y()) ? maxY : points[0].Y();
			minY = (minY < points[1].Y()) ? minY : points[1].Y();
			maxY = (maxY > points[1].Y()) ? maxY : points[1].Y();
			minY = (minY < points[2].Y()) ? minY : points[2].Y();
			maxY = (maxY > points[2].Y()) ? maxY : points[2].Y();
		}
		return maxY - minY;
	}

	double CrossSection::getWidth(void) const
	{
		double minX = 0;
		double maxX = 0;
		for (std::vector<Face *>::const_iterator it = faces.begin(), end = faces.end(); it != end; ++it)
		{
			PointList points = (*it)->getPoints();
			minX = (minX < points[0].X()) ? minX : points[0].X();
			maxX = (maxX > points[0].X()) ? maxX : points[0].X();
			minX = (minX < points[1].X()) ? minX : points[1].X();
			maxX = (maxX > points[1].X()) ? maxX : points[1].X();
			minX = (minX < points[2].X()) ? minX : points[2].X();
			maxX = (maxX > points[2].X()) ? maxX : points[2].X();
		}
		return maxX - minX;
	}

	double CrossSection::getTrafficableWidth(void) const
	{
		double width = 0;
		for (std::vector<Edge *>::const_iterator it = edges.begin(), end = edges.end(); it != end; ++it)
		{
			Edge *edge = *it;
			if(edge->getAttributes().getAttributeAsString("trafficable") == "true")
			{
				PointList points = edge->getPoints();
				double w = points[1].X() - points[0].X();
				width += (w > 0) ? w : 1 - w;
			}
		}
		return width;
	}

	int CrossSection::snapPoints()
	{
		const double epsilon = 1e-6;
		std::set<sfa::Point> grid;//All unique points here
		BOOST_FOREACH(Edge *edge, edges)
		{
			const PointList &points = edge->getPoints();
			BOOST_FOREACH(const sfa::Point &pt,points)
			{
				double closest_dist = DBL_MAX;
				BOOST_FOREACH(const sfa::Point &gridpt,grid)
				{
					closest_dist = std::min<double>(closest_dist,gridpt.distance(&pt));
				}
				// If pt is not within epsilon distance from any grid point, insert pt into the grid
				if(closest_dist>epsilon)
				{
					grid.insert(pt);
				}
			}

		}
		BOOST_FOREACH(Face *face, faces)
		{
			const PointList &points = face->getPoints();
			BOOST_FOREACH(const sfa::Point &pt,points)
			{
				double closest_dist = DBL_MAX;
				BOOST_FOREACH(const sfa::Point &gridpt,grid)
				{
					closest_dist = std::min<double>(closest_dist,gridpt.distance(&pt));
				}
				// If pt is not within epsilon distance from any grid point, insert pt into the grid
				if(closest_dist>epsilon)
				{
					grid.insert(pt);
				}
			}
		}
		// Now go back through all the edges and faces and snap all points to the closest
		// point in the grid
		BOOST_FOREACH(Edge *edge, edges)
		{
			int numEdgePoints = edge->getNumPoints();
			for(int i=0;i<numEdgePoints;i++)
			{
				sfa::Point closestGridPt;
				double closest_dist = DBL_MAX;
				sfa::Point &edgePt = edge->getPoint(i);
				BOOST_FOREACH(const sfa::Point &gridpt,grid)
				{
					double grid_pt_dist = gridpt.distance(&edgePt);
					if(grid_pt_dist < closest_dist)
					{
						closest_dist = grid_pt_dist;
						closestGridPt = gridpt;
					}
				}
				if(closest_dist < DBL_MAX)
				{
					// Snap to the grid
					edgePt = closestGridPt;
				}
			}
		}

		BOOST_FOREACH(Face *face,faces)
		{
			int numFacePoints = face->getNumPoints();
			for(int i=0;i<numFacePoints;i++)
			{
				sfa::Point closestGridPt;
				double closest_dist = DBL_MAX;
				sfa::Point &facePt = face->getPoint(i);
				BOOST_FOREACH(const sfa::Point &gridpt,grid)
				{
					double grid_pt_dist = gridpt.distance(&facePt);
					if(grid_pt_dist < closest_dist)
					{
						closest_dist = grid_pt_dist;
						closestGridPt = gridpt;
					}
				}
				if(closest_dist < DBL_MAX)
				{
					// Snap to the grid
					facePt = closestGridPt;
				}
			}
		}

		return 0;
	}

}
