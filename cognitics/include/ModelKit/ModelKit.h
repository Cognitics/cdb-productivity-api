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
/*! \file ModelKit/ModelKit.h
\headerfile ModelKit/ModelKit.h
\brief Provides modelkit::ModelKit.
\author Aaron Brinton <abrinton@cognitics.net>
\date 25 April 2011

\page modelkit_page Cognitics Model Kit Library
\ref sfa_page

\section Description

The Cognitics Model Kit Library provides tools for manipulating collections of model cross-sections and generating dynamic models, as well as defining the XML schema for storage and retrieval of this information.
A Model Kit contains the parts and metadata required to generate a particular type of model, such as a bridge, overpass, or tunnel.

The core component of the Model Kit is a cross-section, a two-dimensional representation that can be extruded to provide a three-dimensional object.
The cross-section contains information about faces, edges, and points of attachment to other cross sections.
Faces are two-dimensional planes containing the geometry of the cross-section.
Edges represent the "sides" of the cross-section that are extruded.
Faces and edges are associated with textures that are applied dynamically during the model generation process.

The cross-section attachment points specify locations where the cross-section may be attached to other cross-sections.
This allows the Model Kit to combine simple cross-sections, such as a guardrail, sidewalk, or bridge deck, into a composite cross section, such as an entire bridge.
With this design, simple cross-sections may be used across several Model Kits, providing alternative model designs without redefining each component.

In addition to cross-section components, the Model Kit may contain external model references for instantiation over regular intervals.
This enables objects such as street lights or bridge stanchions to be generated along the extrusion distance.

The model generation functionality utilizes the Model Kit architecture to dynamically generate a model for a given geometry.

It includes the following classes:
\li modelkit::ModelGenerator \copybrief modelkit::ModelGenerator
\li modelkit::CrossSection \copybrief modelkit::CrossSection
\li modelkit::ModelKit \copybrief modelkit::ModelKit
\li modelkit::Edge \copybrief modelkit::Edge
\li modelkit::Face \copybrief modelkit::Face
\li modelkit::AttachmentPoint \copybrief modelkit::AttachmentPoint
\li modelkit::Attachment \copybrief modelkit::Attachment
\li modelkit::Connection \copybrief modelkit::Connection
\li modelkit::ModelInstance \copybrief modelkit::ModelInstance
\li modelkit::TextureReference \copybrief modelkit::TextureReference
\li modelkit::Material \copybrief modelkit::Material


\section Usage

The ModelKit classes retain pointer ownership of all child components.

\code
#include <ModelKit/ModelKit.h>

modelkit::CrossSection csDeck("Deck.csm");
modelkit::CrossSection csLeftEdge("LeftEdge.csm");
modelkit::CrossSection csRightEdge("RightEdge.csm");

modelkit::ModelKit modelKit;
modelKit.setName("Bridge");
modelkit::CrossSectionReference *csrDeck = modelKit.addCrossSectionReference(&csDeck);
csrDeck->setAlias("Deck");
modelkit::CrossSectionReference *csrLeftEdge = modelKit.addCrossSectionReference(&csLeftEdge);
csrLeftEdge->setAlias("LeftEdge");
modelkit::CrossSectionReference *csrRightEdge = modelKit.addCrossSectionReference(&csRightEdge);
csrRightEdge->setAlias("RightEdge");
modelKit.setOrigin(csrDeck, "Instance");
modelKit.addConnection(csrDeck, "LowerLeft", csrLeftEdge, "LowerRight");
modelKit.addConnection(csrDeck, "LowerRight", csrRightEdge, "LowerLeft");
modelKit.addModelInstance("lamppost.flt", csrDeck, "LowerLeft", 10, true);
modelKit.addModelInstance("lamppost.flt", csrDeck, "LowerRight", 10, true);
modelKit.write("Bridge.kit");

modelkit::CrossSection *combinedCrossSection = modelKit.consolidate();
combinedCrossSection->write("Bridge.csm");

// note that the application owns the result of consolidate()
delete combinedCrossSection;
\endcode

*/
#pragma once

#include "ModelKit/CrossSection.h"
#include "ModelKit/Connection.h"
#include "ModelKit/ModelInstance.h"
#include <list>

//! \namespace modelkit Cognitics Model Kit Library
namespace modelkit
{
	//! Model Kit class representing a single model kit
	class ModelKit
	{
	private:
		bool valid;
		std::string filename;
		std::string name;
		std::vector<CrossSectionReference *> crossSectionReferences;
		std::vector<Connection *> connections;
        std::vector<ModelInstance *> modelInstances;
        Attachment *origin;
        
        ccl::ObjLog log;
		CrossSectionSP consolidatedCrossSection;
        
	public:
		~ModelKit(void);
		ModelKit(void);

		//! Create an instance from a .kit xml file.
		ModelKit(const std::string &file);

		//! Write the instance to the specified xml file (or the previously set filename if empty).
		bool write(const std::string &file = std::string());

		//! Set the filename for the current model kit.
		void setFilename(const std::string &file);

		//! Get the filename for the current model kit.
		std::string getFilename(void);

		//! Set the name of the model kit.
		void setName(const std::string &n);

		//! Get the name of the model kit.
		std::string getName(void);

		//! Add a cross-section reference to the model kit.
		bool addCrossSectionReference(CrossSectionReference *crossSectionReference);

		//! Add a cross-section reference to the model kit using a given cross section.
		CrossSectionReference *addCrossSectionReference(CrossSection *crossSection);

		//! Add a cross-section reference to the model kit given a source filename and alias.
		CrossSectionReference *addCrossSectionReference(const std::string &src, const std::string &alias);

		//! Get the cross-section reference list.
		std::vector<CrossSectionReference *> getCrossSectionReferences(void);

		//! Get a cross-section by alias.
		CrossSectionReference *getCrossSectionReference(const std::string &alias);

		//! Get the index of the specified cross-section reference.
		int getCrossSectionReferenceIndex(CrossSectionReference *crossSectionReference);

		//! Remove the specified cross-section reference.
		bool removeCrossSectionReference(CrossSectionReference *crossSectionReference);

		//! Remove a cross-section reference by source filename.
		bool removeCrossSectionReference(const std::string &src);

		//! Add a connection object to the model kit.
		void addConnection(Connection *connection);

		//! Add a connection object to the model kit given two attachments.
		Connection *addConnection(Attachment *a, Attachment *b);

		//! Add a connection object to the model kit given two attachments by point and cross-section references.
		Connection *addConnection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB);

        //! Remove a connection object from the model kit that has the given two attachments by point and cross-section references.
        Connection *removeConnection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB);
        
        //! Remove the specified connection.
		bool removeConnection(Connection *connection);

		//! Get the connection list.
		std::vector<Connection *> getConnections(void);

		//! Get connections matching the given cross-section reference and (optionally) an attachment point name.
		std::vector<Connection *> getConnections(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName = std::string());

		//! Get a connection object from the model kit that has the given two attachments by point and cross-section references.
        Connection *getConnection(CrossSectionReference *crossSectionReferenceA, const std::string &attachmentPointNameA, CrossSectionReference *crossSectionReferenceB, const std::string &attachmentPointNameB);

        //! Add a model instance to the model kit.
        void addModelInstance(ModelInstance *mi);

		//! Add a model instance to the model kit with specified parameters set. 
        ModelInstance *addModelInstance(std::string src, CrossSectionReference *csr, std::string apName, bool autoPlaced, 
                                            std::string pos, bool clamp, sfa::Point &scale, std::string tName, 
                                            double ssDev, double sm, double osDev, double om, std::string dist,
                                            double buffRad, double BuffHgt);

		//! Remove the specified model instance.
        bool removeModelInstance(ModelInstance *mi);

		//! Get a model instance by index.
        ModelInstance *getModelInstance(int idx);

        //! Get the model instance to with the specified source filename, cross-section reference, attachment point name, and auto placed flag.
        ModelInstance *getModelInstance(const std::string &src,  const std::string &csAlias, const std::string &apName, bool autoPlaced, std::string &distribution);

		//! Get the model instance list.
        std::vector<ModelInstance *> getModelInstances(void);

		//! Set the origin for the model kit.
		void setOrigin(Attachment *origin);

		//! Set the origin for the model kit using a cross-section reference and attachment point name.
		Attachment *setOrigin(CrossSectionReference *crossSectionReference, const std::string &attachmentPointName);

		//! Get the origin of the model kit.
		Attachment *getOrigin(void);

		//! Remove the origin from the model kit.
		void removeOrigin(void);

		//! Utility function to return the path of the specified reference relative to the model kit path.
		std::string getReferenceSource(Reference *reference);

		//! Utility function to return the path of the specified model instance relative to the model kit path.
		std::string getReferenceSource(ModelInstance *mi);

		/*! Consolidate the model kit cross-sections into a composite cross-section.
		\note Application is responsible for the returned instance.
		*/
		CrossSectionSP consolidate(bool useCachedCrossSection=true);

		bool isValid() { return valid; }
	};

	typedef std::shared_ptr<ModelKit> ModelKitSP;

	struct ModelKitReference
	{
		std::string filename;		// modelkit filename
		double min_dist;			// minimum distance to use this kit (lod)
		double max_dist;			// maximum distance to use this kit (lod)
		bool underground;			// whether the underground portion should be generated
		ModelKitReference(void);
	};

	typedef std::list<ModelKitReference> ModelKitReferenceList;

	bool read_kits(ModelKitReferenceList &kits, const std::string &filename);
	bool write_kits(const ModelKitReferenceList &kits, const std::string &filename);


}

