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
#pragma once

#include "ModelKit/Edge.h"
#include "ModelKit/Face.h"
#include "ModelKit/AttachmentPoint.h"
#include "ModelKit/Connection.h"
#include "ModelKit/Material.h"
#include "ModelKit/AvoidArea.h"
#if (_MSC_VER >= 1700)
#include <memory>
#else
#include <boost/tr1/memory.hpp>
#endif

namespace modelkit
{
	class CrossSection
	{
	private:
		std::string filename;
		std::string name;
		std::vector<Texture*> textures;
		std::vector<Edge*> edges;
		std::vector<Face*> faces;
        std::vector<AvoidArea*> avoidAreas;
        std::vector<AttachmentPoint*> attachmentPoints;
        std::map<std::string, Material*> materialMap;  //key = materialID, value = pointer to Material instance
        const double PI;

    
    private:
        //Utility function removes the unecessary trailing zeroes from the specified double value 
        //and returns its std::string representation.
        std::string formatDoubleToStr(double v);

	public:
		~CrossSection(void);
		CrossSection(void);
		CrossSection(const CrossSection &copy);
		CrossSection(const std::string &filename);

		bool write(const std::string &filename = std::string());

		void setFilename(const std::string &filename);
		std::string getFilename(void);

		void setName(const std::string &name);
		std::string getName(void);

		void addTexture(Texture *texture);
		Texture *addTexture(const std::string &src);
		std::vector<Texture *> getTextures(void);
		Texture *getTexture(const std::string &src);
		int getTextureIndex(Texture *texture);
		bool removeTexture(Texture *texture);
		bool removeTexture(const std::string &src);

		void addEdge(Edge *edge);
		std::vector<Edge *> getEdges(void);
		Edge *getEdge(const std::string &name);
		Edge *getEdge(int fid);
		bool removeEdge(Edge *edge);
		bool removeEdge(const std::string &name);

		void addFace(Face *face);
		Face *addFace(const std::string name, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c);
		std::vector<Face *> getFaces(void);
		Face *getFace(const std::string &name);
		bool removeFace(Face *face);
		bool removeFace(const std::string &name);

        bool addAvoidArea(AvoidArea *a);
        std::vector<AvoidArea*> getAvoidAreas();
        bool removeAvoidArea(AvoidArea &a);

		void addAttachmentPoint(AttachmentPoint *attachmentPoint);
		AttachmentPoint *addAttachmentPoint(const std::string &name, const sfa::Point &point);
		std::vector<AttachmentPoint *> getAttachmentPoints(void);
		AttachmentPoint *getAttachmentPoint(const std::string &name);
		bool removeAttachmentPoint(AttachmentPoint *attachmentPoint);
		bool removeAttachmentPoint(const std::string &name);

        bool addMaterial(Material *mat);
        Material* getMaterial(const std::string &id);  //returns NULL if the specified material is not found
		Material *getMatchingMaterial(Material *mat);
        bool removeMaterial(const std::string &id);
        int getNumMaterials()
        { 
            return (int)materialMap.size();
        }

		
        void scale(double factor);
        void scaleX(double factor);
        void offsetX(double amount);
		void translate(double x, double y);
		void expand(double x, double y);

        void rotate(double degrees);

		std::string getReferenceSource(Reference *reference);

		bool attach(Attachment *localAttachment, Attachment *remoteAttachment, std::vector<Connection *> connections = std::vector<Connection *>());

        sfa::Point calculateCentroid(PointList &edgePoints);

		double getRadius(void) const;
		double getHeight(void) const;
		double getWidth(void) const;
		double getTrafficableWidth(void) const;		// sum of trafficable surface width

		int snapPoints();
		bool invertEdgeNormals();
		bool invertFaceNormals();

	};

	typedef std::tr1::shared_ptr<CrossSection> CrossSectionSP;


}
