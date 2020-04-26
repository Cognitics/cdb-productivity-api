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

#include "TextureReference.h"
#include "Material.h"
#include <map>
#include <ccl/AttributeContainer.h>
#include <scenegraph/Color.h>
namespace modelkit
{
	class Edge
	{
	private:
		std::string name;
		PointList points;
		sfa::Point normal;
		int fid;
		int smc;
		bool terrain;
		double projectedZ;
        Material *materialRef;
		scenegraph::Color primaryColor;
		scenegraph::Color alternateColor;
		std::vector<TextureReference *> textureReferences;
		ccl::AttributeContainer attributes;
		ccl::AttributeContainer flags;

	public:
		~Edge(void);
		Edge(void);
		Edge(const Edge &copy);
		Edge(const std::string &name, const sfa::Point &a, const sfa::Point &b, const sfa::Point &normal, const ccl::AttributeContainer &_attributes, int fid = 0, int smc = 0);

		void setName(const std::string &name);
		std::string getName(void);

		void addPoint(const sfa::Point &point);
		PointList getPoints(void);
		sfa::Point &getPoint(int index);
		void removePoint(int index);
		void clearPoints(void);
		int getNumPoints()  const;

		void setNormal(const sfa::Point &normal);
		sfa::Point getNormal(void);

		void setFID(int fid);
		int getFID(void);

		void setSMC(int smc);
		int getSMC(void);

        void setMaterialRef(Material* m);
        Material* getMaterialRef();

		void setTerrain(bool isTerrain);
		bool isTerrain(void);

		void setProjectedZ(double projectedZ);
		double getProjectedZ();

		void addTextureReference(TextureReference *textureReference);
		TextureReference *addTextureReference(Texture *texture, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c);
		std::vector<TextureReference *> &getTextureReferences(void);
		bool removeTextureReference(TextureReference *textureReference);
		bool removeTextureReference(Texture *texture);

        void scale(double factor);
        void scaleX(double factor);
        void offsetX(double amount);
		void translate(double x, double y);
		void expand(double x, double y);

        void rotate(double radians, sfa::Point &centroid);

		Edge *clone(double deltaX = 0, double deltaY = 0);
		ccl::AttributeContainer &getAttributes(void);
		const ccl::AttributeContainer &getAttributes(void) const;
		void setAttributes(const ccl::AttributeContainer &_attributes);

		ccl::AttributeContainer &getFlags(void);
		const ccl::AttributeContainer &getFlags(void) const;
		void setFlags(const ccl::AttributeContainer &_flags);

		scenegraph::Color getPrimaryColor() const;
		void setPrimaryColor(const scenegraph::Color &color);

		scenegraph::Color getAlternateColor() const;
		void setAlternateColor(const scenegraph::Color &color);

		void calculateNormals();
		void invertNormal();

	};

}