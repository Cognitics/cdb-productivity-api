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

#include <algorithm>

#include "ModelKit/ModelKit.h"
#include <dom/dom.h>
#include <boost/foreach.hpp>

namespace modelkit
{
	ModelKit::~ModelKit(void)
	{
		for(std::vector<CrossSectionReference *>::iterator it = crossSectionReferences.begin(), end = crossSectionReferences.end(); it != end; ++it)
			delete *it;
		for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it)
			delete *it;
		delete origin;
	}

	ModelKit::ModelKit(void) : origin(NULL)
	{
		valid = true;
        log.init("ModelGenerator");
	    log << ccl::LINFO;
	}

	ModelKit::ModelKit(const std::string &file) : filename(file), origin(NULL)
	{
		valid = false;
		log.init("ModelGenerator");
	    log << ccl::LINFO;

        dom::DocumentSP doc = dom::DOMImplementation::readXML(filename);
		if(!doc)
        {
            log << ccl::LERR << "Cannot read the specified model kit file (" << filename << ")." << log.endl;		
			valid = false;
            return;
        }
		dom::ElementSP element = doc->getDocumentElement();
		if (!element || (element->getNodeName() != "ModelKit"))
		{
			valid = false;
			return;
		}
		if(element->hasAttribute("name"))
			setName(element->getAttribute("name").as_string());
		std::map<int, CrossSectionReference *> csMap;
		dom::ElementList children = element->getChildElements();
		for(dom::ElementList::iterator it = children.begin(), end = children.end(); it != end; ++it)
		{
			dom::ElementSP child = *it;
			if((child->getNodeName() == "CrossSectionReference") && child->hasAttribute("id"))
			{
				CrossSectionReference *crossSectionReference = addCrossSectionReference(child->getTextContent(), "");
                dom::Variant idVar = child->getAttribute("id");
                if (idVar.as_string() != "0" && idVar.as_int() == 0) 
                    log << ccl::LWARNING << "Invalid ID=" << idVar.as_string() << " specified for Cross Section Reference." << log.endl;
                
                csMap[child->getAttribute("id").as_int()] = crossSectionReference;                    
				if(child->hasAttribute("alias"))
					crossSectionReference->setAlias(child->getAttribute("alias").as_string());
			}
			if(child->getNodeName() == "Connection")
			{
				Connection *connection = new Connection;
				dom::ElementList childChildren = child->getChildElements();
				for(dom::ElementList::iterator cit = childChildren.begin(), cend = childChildren.end(); cit != cend; ++cit)
				{
					dom::ElementSP childChild = *cit;
					if((childChild->getNodeName() == "Attachment") && childChild->hasAttribute("crossSectionID"))
					{
						Attachment *attachment = new Attachment;
                        int csID = childChild->getAttribute("crossSectionID").as_int();
                        std::string apName;  
                        if(childChild->hasAttribute("attachmentPoint"))
                        {
							apName = childChild->getAttribute("attachmentPoint").as_string();
                            attachment->setAttachmentPointName(apName);
                        }
                        else
                            log << ccl::LWARNING << "No attachment point name defined for Cross Section Reference with crossSectionID=" << csID << log.endl;
                        dom::Variant csIDVar = childChild->getAttribute("crossSectionID");
                        if (csIDVar.as_string() != "0" && csIDVar.as_int() == 0) 
                            log << ccl::LWARNING << "Invalid crossSectionID=" << csIDVar.as_string() << " specified for Attachment with attachmentPoint=" << apName << log.endl;
                        modelkit::CrossSectionReference *csRef = csMap[csIDVar.as_int()];
                        if (csRef)
                            attachment->setCrossSectionReference(csRef);
                        else
                            log << ccl::LWARNING << "No cross section associated with Cross Section Reference crossSectionID=" << csID << log.endl;                                                                         
						connection->addAttachment(attachment);
					}
				}
				addConnection(connection);
			}
			if((child->getNodeName() == "ModelInstance") && (child->hasAttribute("crossSectionID")))
            {	
	            ModelInstance *modelInstance = new ModelInstance;
                dom::Variant csIDVar = child->getAttribute("crossSectionID");
                if (csIDVar.as_string() != "0" && csIDVar.as_int() == 0) 
                    log << ccl::LWARNING << "Invalid crossSectionID=" << csIDVar.as_string() << " specified for Model Instance." << log.endl;
                modelkit::CrossSectionReference *csRef = csMap[csIDVar.as_int()];
                if (csRef)
                    modelInstance->setCrossSectionReference(csRef);
                else
                    log << ccl::LWARNING << "No cross section associated with Cross Section Reference crossSectionID=" << csIDVar.as_string() << log.endl;                         
                if(child->hasAttribute("source"))
			        modelInstance->setSource(child->getAttribute("source").as_string());
	            if(child->hasAttribute("attachmentPoint"))
		            modelInstance->setAttachmentPointName(child->getAttribute("attachmentPoint").as_string());
	            if(child->hasAttribute("autoPlacement"))
	            {
		            std::string str = child->getAttribute("autoPlacement").as_string();
		            std::transform(str.begin(), str.end(), str.begin(), ::toupper); 
                    if(str == "TRUE")
						modelInstance->setAutoPlaced(true);
                    else if (str == "FALSE")
                        modelInstance->setAutoPlaced(false);                    
                    else
                    {
                        log << ccl::LWARNING << "Invalid attribute 'setAutoPlaced=" << str << "' specified for Model Instance, expecting TRUE or FALSE." << log.endl;
                        modelInstance->setAutoPlaced(false);                    
                    }
	            }
                if (child->hasAttribute("position"))
                {
                    std::string str = child->getAttribute("position").as_string();
                    std::transform(str.begin(), str.end(), str.begin(), ::tolower); 
                    modelInstance->setPositionString(str);
                }
                if (child->hasAttribute("clampToTerrain"))
                {
                    std::string str = child->getAttribute("clampToTerrain").as_string();
		            std::transform(str.begin(), str.end(), str.begin(), ::toupper); 
                    if(str == "TRUE")
						modelInstance->setClampToTerrain(true);
                    else if (str == "FALSE")
                        modelInstance->setClampToTerrain(false);
                    else
                    {
                        log << ccl::LWARNING << "Invalid attribute 'clampToTerrain=" << str << "' specified for Model Instance, expecting TRUE or FALSE." << log.endl;
                        modelInstance->setClampToTerrain(false);
                    }
                }
                dom::ElementList grandchildren = child->getChildElements();
                for(dom::ElementList::iterator cit = grandchildren.begin(), cend = grandchildren.end(); cit != cend; ++cit)
				{
					dom::ElementSP grandchild = *cit;
					if(grandchild->getNodeName() == "Tag")
                    {
                        std::string tname = grandchild->getAttribute("name").as_string();
                        modelInstance->setTagName(tname);
                    }
                    else if(grandchild->getNodeName() == "Spacing")
					{
                        double sStdDev = grandchild->getAttribute("deviation").as_double();
                        double sMean = grandchild->getAttribute("mean").as_double();
                        modelInstance->setSpacingStandardDeviation(sStdDev);
                        modelInstance->setSpacingMean(sMean);
                    }
                    else if (grandchild->getNodeName() == "Offset")
                    {
                        double oStdDev = grandchild->getAttribute("deviation").as_double();
                        double oMean = grandchild->getAttribute("mean").as_double();
                        modelInstance->setOffsetStandardDeviation(oStdDev);
                        modelInstance->setOffsetMean(oMean);
                    }
                    else if (grandchild->getNodeName() == "Distribution")
                    {
                        std::string t = grandchild->getAttribute("type").as_string();
                        modelInstance->setDistributionString(t);
                    }
                    else if (grandchild->getNodeName() == "Scale")
                    {
                        double x = grandchild->getAttribute("x").as_double();
                        double y = grandchild->getAttribute("y").as_double();
                        double z = grandchild->getAttribute("z").as_double();

                        //Ensure the scale factor is greater than 0.
                       if (x <= 0.0)
                            x = 1.0;
                        if (y <= 0.0)
                            y = 1.0;
                        if (z <= 0.0)
                            z = 1.0;
                       
                        modelInstance->setScaleFactor(x, y, z);
                    }
                    else if (grandchild->getNodeName() == "Buffer")
                    {
                        double r = grandchild->getAttribute("radius").as_double();
                        double h = grandchild->getAttribute("height").as_double();
                        modelInstance->setBufferRadius(r);
                        modelInstance->setBufferHeight(h);
                    }
                }
	            addModelInstance(modelInstance);
            }
			if((child->getNodeName() == "Origin") && child->hasAttribute("crossSectionID"))
			{
				delete origin;
				origin = new Attachment;
                dom::Variant csIDVar = child->getAttribute("crossSectionID");
                if (csIDVar.as_string() != "0" && csIDVar.as_int() == 0) 
                    log << ccl::LWARNING << "Invalid crossSectionID=" << csIDVar.as_string() << " specified for Model Origin." << log.endl;
                modelkit::CrossSectionReference *csRef = csMap[csIDVar.as_int()];
                if (csRef)
                    origin->setCrossSectionReference(csRef);
                else
                    log << ccl::LWARNING << "No cross section associated with crossSectionID=" << csIDVar.as_string() << " in the Model Origin." << log.endl;                         
				if(child->hasAttribute("attachmentPoint"))
					origin->setAttachmentPointName(child->getAttribute("attachmentPoint").as_string());
			}
		}
		valid = true;
	}

	bool ModelKit::write(const std::string &file)
	{
		if(file.size())
			filename = file;
		if(filename.empty())
			return false;
		dom::DOMImplementationSP domImplementation(new dom::DOMImplementation);
		dom::DocumentSP doc = domImplementation->createDocument(dom::DOMString(), "ModelKit");
		dom::ElementSP element = doc->getDocumentElement();
		if(name.size())
			element->setAttribute("name", name);
		int i = 0;
		for(std::vector<CrossSectionReference *>::iterator it = crossSectionReferences.begin(), end = crossSectionReferences.end(); it != end; ++it, ++i)
		{
			dom::ElementSP child = doc->createElement("CrossSectionReference");
			child->setAttribute("id", i);
			if((*it)->getAlias().size())
				child->setAttribute("alias", (*it)->getAlias());
			child->setTextContent((*it)->getSource());
			element->appendChild(child);
		}
		for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it, ++i)
		{
			Connection *connection = *it;
			dom::ElementSP child = doc->createElement("Connection");
			std::vector<Attachment *> attachments = connection->getAttachments();
			for(std::vector<Attachment *>::iterator ait = attachments.begin(), aend = attachments.end(); ait != aend; ++ait)
			{
				Attachment *attachment = *ait;
				dom::ElementSP attachmentChild = doc->createElement("Attachment");
				attachmentChild->setAttribute("crossSectionID", getCrossSectionReferenceIndex(attachment->getCrossSectionReference()));
				attachmentChild->setAttribute("attachmentPoint", attachment->getAttachmentPointName());
				child->appendChild(attachmentChild);
			}
			element->appendChild(child);
		}
		for(std::vector<ModelInstance *>::iterator it = modelInstances.begin(), end = modelInstances.end(); it != end; ++it, ++i)
		{
			ModelInstance *modelInstance = *it;
			dom::ElementSP child = doc->createElement("ModelInstance");
			child->setAttribute("crossSectionID", getCrossSectionReferenceIndex(modelInstance->getCrossSectionReference()));
			child->setAttribute("source", modelInstance->getSource());
			child->setAttribute("attachmentPoint", modelInstance->getAttachmentPointName());
			child->setAttribute("autoPlacement", modelInstance->getAutoPlaced() ? "true" : "false");
            child->setAttribute("position", modelInstance->getPositionStr());
            child->setAttribute("clampToTerrain", modelInstance->isClampedToTerrain() ? "true" : "false");
            dom::ElementSP spacingChild = doc->createElement("Spacing");
            spacingChild->setAttribute("deviation", modelInstance->getSpacingStandardDeviation());
            spacingChild->setAttribute("mean", modelInstance->getSpacingMean());
			child->appendChild(spacingChild);
            dom::ElementSP offsetChild = doc->createElement("Offset");
            offsetChild->setAttribute("standardDeviation", modelInstance->getOffsetStandardDeviation());
            offsetChild->setAttribute("mean", modelInstance->getOffsetMean());
			child->appendChild(offsetChild);
            dom::ElementSP distributionChild = doc->createElement("Distribution");
            distributionChild->setAttribute("type", modelInstance->getDistributionString());
            child->appendChild(distributionChild);
            dom::ElementSP tagChild = doc->createElement("Tag");
            tagChild->setAttribute("name", modelInstance->getTagName());
            child->appendChild(tagChild);
            dom::ElementSP scaleChild = doc->createElement("Scale");
            scaleChild->setAttribute("x", modelInstance->getScaleFactor().X());
            scaleChild->setAttribute("y", modelInstance->getScaleFactor().Y());
            scaleChild->setAttribute("z", modelInstance->getScaleFactor().Z());
            child->appendChild(scaleChild);
            dom::ElementSP bufferChild = doc->createElement("Buffer");
            bufferChild->setAttribute("radius", modelInstance->getBufferRadius());
            bufferChild->setAttribute("height", modelInstance->getBufferHeight());
            child->appendChild(bufferChild);

            element->appendChild(child);
		}
		if(origin)
		{
			dom::ElementSP child = doc->createElement("Origin");
			child->setAttribute("crossSectionID", getCrossSectionReferenceIndex(origin->getCrossSectionReference()));
			child->setAttribute("attachmentPoint", origin->getAttachmentPointName());
			element->appendChild(child);
		}
		
        return dom::DOMImplementation::writeXML(this->filename, doc);
	}

	void ModelKit::setFilename(const std::string &file)
	{
		filename = file;
	}

	std::string ModelKit::getFilename(void)
	{
		return filename;
	}

	void ModelKit::setName(const std::string &n)
	{
		name = n;
	}

	std::string ModelKit::getName(void)
	{
		return name;
	}

	bool ModelKit::addCrossSectionReference(CrossSectionReference *crossSectionReference)
	{
        if (!crossSectionReference)
		{
            log << ccl::LWARNING << "Cannot add a NULL cross section reference." << log.endl;
			return false;
		}
        else
        {
            //Make sure the specified cross section reference isn't already in the list.

            for (std::vector<CrossSectionReference*>::iterator itr = crossSectionReferences.begin(); itr != crossSectionReferences.end(); itr++)
            {
                if (crossSectionReference->getAlias() == (*itr)->getAlias() && crossSectionReference->getSource() == (*itr)->getSource())
                {
                    log << ccl::LWARNING << "Cannot add the specified cross section becauae one with the same alias and source already exists." << log.endl;
                    
                    return false;      
                }
            }
            
            crossSectionReferences.push_back(crossSectionReference);
            
            return true;
        }
	}

	CrossSectionReference *ModelKit::addCrossSectionReference(CrossSection *crossSection)
	{
		return addCrossSectionReference(crossSection->getFilename(), "");
	}

	CrossSectionReference *ModelKit::addCrossSectionReference(const std::string &src, const std::string &alias)
	{
		CrossSectionReference *crossSectionReference = new CrossSectionReference(src, alias);
		if (addCrossSectionReference(crossSectionReference))
		    return crossSectionReference;
        else
            return NULL;
	}

	std::vector<CrossSectionReference *> ModelKit::getCrossSectionReferences(void)
	{
		return crossSectionReferences;
	}

	CrossSectionReference *ModelKit::getCrossSectionReference(const std::string &alias)
	{
		for(std::vector<CrossSectionReference *>::iterator it = crossSectionReferences.begin(), end = crossSectionReferences.end(); it != end; ++it)
		{
			CrossSectionReference *crossSectionReference = *it;
			if(crossSectionReference->getAlias() == alias)
				return crossSectionReference;
		}
		return NULL;
	}

	int ModelKit::getCrossSectionReferenceIndex(CrossSectionReference *crossSectionReference)
	{
		int i = 0;
		for(std::vector<CrossSectionReference *>::iterator it = crossSectionReferences.begin(), end = crossSectionReferences.end(); it != end; ++it, ++i)
		{
			if(crossSectionReference == *it)
				return i;
		}
		return -1;
	}

	bool ModelKit::removeCrossSectionReference(CrossSectionReference *crossSectionReference)
	{
		for(std::vector<CrossSectionReference *>::iterator it = crossSectionReferences.begin(), end = crossSectionReferences.end(); it != end; ++it)
		{
			if(*it == crossSectionReference)
			{
				//We found the cross section in the list.

                //First, go through the model kit and update all cross section reference pointers that point to this object to NULL.
                for (size_t i = 0; i < connections.size(); i++)
                {
                    std::vector<Attachment *> attachs = connections[i]->getAttachments();
                    for (size_t j = 0; j < attachs.size(); j++)
                    {
                        if (crossSectionReference == attachs[j]->getCrossSectionReference())
                            attachs[j]->setCrossSectionReference(NULL);
                    }
                }
                for (size_t i = 0; i < modelInstances.size(); i++)
                {
                    if (crossSectionReference == modelInstances[i]->getCrossSectionReference())
                        modelInstances[i]->setCrossSectionReference(NULL);
                }
                if (origin && crossSectionReference == origin->getCrossSectionReference())
                    origin->setCrossSectionReference(NULL);
                
                //Now delete the crossSectionReference object from memory and remove it from the crossSectionReferences list.
                delete *it;
				crossSectionReferences.erase(it);

				return true;
			}
		}
		return false;
	}

	bool ModelKit::removeCrossSectionReference(const std::string &src)
	{
		return removeCrossSectionReference(getCrossSectionReference(src));
	}

    void ModelKit::addModelInstance(ModelInstance *mi)
    {
        modelInstances.push_back(mi);
    }

    ModelInstance *ModelKit::addModelInstance(std::string src, CrossSectionReference *csr, std::string apName, 
                                                bool autoPlaced, std::string pos, bool clamp, sfa::Point &scale, 
                                                std::string tName, double ssDev, double sm, double osDev, 
                                                double om, std::string dist,  double buffRad, double buffHgt)
	{
		ModelInstance *modelInstance = new ModelInstance(src, csr, apName, autoPlaced, pos, clamp, scale, tName, ssDev, sm, osDev, om, dist, buffRad, buffHgt);
		addModelInstance(modelInstance);
		return modelInstance;
	}
    
    bool ModelKit::removeModelInstance(ModelInstance *mi)
    {
	    for(std::vector<ModelInstance *>::iterator it = modelInstances.begin(), end = modelInstances.end(); it != end; ++it)
		{
			if(*it == mi)
			{
				delete *it;
				modelInstances.erase(it);
                return true;
			}
		}
		return false;
    }
    
    std::vector<ModelInstance *> ModelKit::getModelInstances()
    {
        return modelInstances;
    }

    ModelInstance *ModelKit::getModelInstance(int idx)
    {
        if (idx < 0 || idx >= (int)modelInstances.size())
            return NULL;

        return modelInstances[idx];
    }

    ModelInstance *ModelKit::getModelInstance(const std::string &src, const std::string &csAlias, const std::string &apName, bool ap, std::string &distribution)
    {
        for(std::vector<ModelInstance *>::iterator it = modelInstances.begin(), end = modelInstances.end(); it != end; ++it)
		{
            if( (*it)->getSource() == src && (*it)->getCrossSectionReference() 
                    && (*it)->getCrossSectionReference()->getAlias() == csAlias
                    && (*it)->getAttachmentPointName() == apName 
                    && (*it)->getAutoPlaced() == ap 
                    && (*it)->getDistributionString() == distribution ) 
            {
                return *it;
            }
        }
        return NULL;
    }

	void ModelKit::addConnection(Connection *connection)
	{
		connections.push_back(connection);
	}

	Connection *ModelKit::addConnection(Attachment *a, Attachment *b)
	{
		Connection *connection = new Connection(a, b);
		addConnection(connection);
		return connection;
	}

	Connection *ModelKit::addConnection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB)
	{
		Connection *connection = new Connection(crossSectionReferenceA, attachmentPointNameA, crossSectionReferenceB, attachmentPointNameB);
		addConnection(connection);
		return connection;
	}

    std::vector<Connection *> ModelKit::getConnections(void)
	{
		return connections;
	}

	std::vector<Connection *> ModelKit::getConnections(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName)
	{
		std::vector<Connection *> result;
		for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it)
		{
			if((*it)->hasAttachment(crossSectionReference, attachmentPointName))
				result.push_back(*it);
		}
		return result;
	}

    Connection *ModelKit::removeConnection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB)
    {
        Connection *result = NULL;
        for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it)
		{
			if((*it)->getAttachments().size() == 2 
                    && (*it)->hasAttachment(crossSectionReferenceA, attachmentPointNameA) 
                    && (*it)->hasAttachment(crossSectionReferenceB, attachmentPointNameB) )
            {
                result = *it;
                connections.erase(it);
                
                return result;
            }
		}
		return result;
    }

    Connection *ModelKit::getConnection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB)
    {
        for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it)
		{
			if((*it)->getAttachments().size() == 2 
                    && (*it)->hasAttachment(crossSectionReferenceA, attachmentPointNameA) 
                    && (*it)->hasAttachment(crossSectionReferenceB, attachmentPointNameB) )
            {
                return *it;	
            }
		}
		return NULL;
    }



	bool ModelKit::removeConnection(Connection *connection)
	{
		for(std::vector<Connection *>::iterator it = connections.begin(), end = connections.end(); it != end; ++it)
		{
			if(*it == connection)
			{
				delete *it;
				connections.erase(it);
				return true;
			}
		}
		return false;
	}

	void ModelKit::setOrigin(Attachment *origin)
	{
		delete this->origin;
		this->origin = origin;
	}

	Attachment *ModelKit::setOrigin(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName)
	{
		setOrigin(new Attachment(crossSectionReference, attachmentPointName));
		return getOrigin();
	}

	Attachment *ModelKit::getOrigin(void)
	{
		return origin;
	}

	void ModelKit::removeOrigin(void)
	{
		setOrigin(NULL);
	}

	std::string ModelKit::getReferenceSource(Reference *reference)
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

	std::string ModelKit::getReferenceSource(ModelInstance *mi)
	{
		if(!mi)
			return std::string();
		Reference ref(mi->getSource());
		return getReferenceSource(&ref);
	}

	CrossSectionSP ModelKit::consolidate(bool useCachedCrossSection)
	{
		if(consolidatedCrossSection && useCachedCrossSection)
			return consolidatedCrossSection;
        std::string errMsg("Cannot consolidate the model kit:");
        if(!origin)
        {
            //log << ccl::LWARNING << errMsg << " no origin specified." << log.endl;
            return CrossSectionSP();
        }
		CrossSectionReference *originReference = origin->getCrossSectionReference();
		if(!originReference)
        {
			log << ccl::LWARNING << errMsg << " no origin associated with a cross section." << log.endl;
            return CrossSectionSP();
        }
		CrossSection originCS(getReferenceSource(originReference)); 
		AttachmentPoint *ap = originCS.getAttachmentPoint(origin->getAttachmentPointName());
		if(!ap)
        {
            log << ccl::LWARNING << errMsg << " origin does not have a valid attachment point." << log.endl;		
            return CrossSectionSP();
        }
		CrossSectionSP result(new CrossSection);
		result->setFilename(filename);
		result->addAttachmentPoint(ap->getName(), sfa::Point());
		Attachment attachment(NULL, ap->getName());
		result->attach(&attachment, origin, connections);

		consolidatedCrossSection = result;
		return result;
	}

	ModelKitReference::ModelKitReference(void) : min_dist(0.0f), max_dist(DBL_MAX), underground(true)
	{
	}

	bool read_kits(ModelKitReferenceList &kits, const std::string &filename)
	{
		dom::DocumentSP doc = dom::DOMImplementation::readXML(filename);
		if(!doc)
            return false;
		dom::ElementSP element = doc->getDocumentElement();
		if(!element || (element->getNodeName() != "ModelKits"))
			return false;
		dom::ElementList children = element->getChildElements();
		for(dom::ElementList::iterator it = children.begin(), end = children.end(); it != end; ++it)
		{
			dom::ElementSP child = *it;
			if(child->getNodeName() == "ModelKit")
			{
				ModelKitReference mkref;
				mkref.filename = child->getTextContent();
				if(child->hasAttribute("min_dist"))
					mkref.min_dist = child->getAttribute("min_dist").as_double();
				if(child->hasAttribute("max_dist"))
					mkref.max_dist = child->getAttribute("max_dist").as_double();
				if(child->hasAttribute("underground"))
				{
		            std::string underground = child->getAttribute("underground").as_string();
		            std::transform(underground.begin(), underground.end(), underground.begin(), ::tolower); 
					mkref.underground = (underground == "true") || (underground == "yes");
				}
				kits.push_back(mkref);
			}
		}
		return true;
	}

	bool write_kits(const ModelKitReferenceList &kits, const std::string &filename)
	{
		dom::DOMImplementationSP domImplementation(new dom::DOMImplementation);
		dom::DocumentSP doc = domImplementation->createDocument(dom::DOMString(), "ModelKits");
		dom::ElementSP element = doc->getDocumentElement();
		for(ModelKitReferenceList::const_iterator it = kits.begin(), end = kits.end(); it != end; ++it)
		{
			const ModelKitReference &mkref = *it;
			dom::ElementSP child = doc->createElement("ModelKit");
			child->setTextContent(mkref.filename);
			child->setAttribute("min_dist", mkref.min_dist);
			child->setAttribute("max_dist", mkref.max_dist);
			child->setAttribute("underground", mkref.underground ? "true" : "false");
			element->appendChild(child);
		}
        return dom::DOMImplementation::writeXML(filename, doc);
	}


}

