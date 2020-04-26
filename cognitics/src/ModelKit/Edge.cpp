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
#include "ModelKit/Edge.h"

namespace modelkit
{
	Edge::~Edge(void)
	{
		//The material reference is just that: a referenece. Meaning it is owned by 
        //someone else, i.e. the cross section. So don't delete it here.
        materialRef = NULL;

        for(std::vector<TextureReference *>::iterator it = textureReferences.begin(), end = textureReferences.end(); it != end; ++it)
			delete *it;
	}

	Edge::Edge(void) : fid(0), smc(0), terrain(false), projectedZ(1.0), materialRef(NULL)
	{
	}

	Edge::Edge(const Edge &copy)
	{
		name = copy.name;
		points = copy.points;
		normal = copy.normal;
		fid = copy.fid;
		smc = copy.smc;
		terrain = copy.terrain;
		projectedZ = copy.projectedZ;
        materialRef = copy.materialRef;
		for(std::vector<TextureReference *>::const_iterator it = copy.textureReferences.begin(), end = copy.textureReferences.end(); it != end; ++it)
			textureReferences.push_back(new TextureReference(*(*it)));
		attributes = copy.attributes;
		flags = copy.flags;
		primaryColor = copy.primaryColor;
		alternateColor = copy.alternateColor;
	}

	Edge::Edge(const std::string &name, const sfa::Point &a, const sfa::Point &b, const sfa::Point &normal, const ccl::AttributeContainer &_attributes, int fid, int smc)
		: name(name), normal(normal), fid(fid), smc(smc), attributes(_attributes), projectedZ(1.0), materialRef(NULL)
	{
		points.push_back(a);
		points.push_back(b);
	}

	void Edge::setName(const std::string &name)
	{
		this->name = name;
	}

	std::string Edge::getName(void)
	{
		return name;
	}

	void Edge::addPoint(const sfa::Point &point)
	{
		points.push_back(point);
	}

	PointList Edge::getPoints(void)
	{
		return points;
	}

	sfa::Point &Edge::getPoint(int index)
	{
		return points[index];
	}

	int Edge::getNumPoints() const 
	{
		return int(points.size());
	}

	void Edge::removePoint(int index)
	{
		points.erase(points.begin() + index);
	}

	void Edge::clearPoints(void)
	{
		points.clear();
	}

	void Edge::setNormal(const sfa::Point &normal)
	{
		this->normal = normal;
	}

	sfa::Point Edge::getNormal(void)
	{
		return normal;
	}

	void Edge::setFID(int fid)
	{
		this->fid = fid;
	}

	int Edge::getFID(void)
	{
		return fid;
	}

	void Edge::setSMC(int smc)
	{
		this->smc = smc;
	}

	int Edge::getSMC(void)
	{
		return smc;
	}

    void Edge::setMaterialRef(Material *m)
    { 
        //The material reference is just that: a referenece. Meaning it is owned by 
        //someone else, i.e. the cross section. So don't delete the old one. 
        this->materialRef = m; 
    }
    
    Material* Edge::getMaterialRef()
    { 
        return materialRef; 
    }

	void Edge::setTerrain(bool isTerrain)
	{
		terrain = isTerrain;
	}

	bool Edge::isTerrain(void)
	{
		return terrain;
	}

	void Edge::addTextureReference(TextureReference *textureReference)
	{
		textureReferences.push_back(textureReference);
	}

	TextureReference *Edge::addTextureReference(Texture *texture, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c)
	{
		TextureReference *textureReference = new TextureReference(texture, a, b, c);
		textureReferences.push_back(textureReference);
		return textureReference;
	}

	std::vector<TextureReference *> &Edge::getTextureReferences(void)
	{
		return textureReferences;
	}

	bool Edge::removeTextureReference(TextureReference *textureReference)
	{
		for(std::vector<TextureReference *>::iterator it = textureReferences.begin(), end = textureReferences.end(); it != end; ++it)
		{
			if(*it == textureReference)
			{
				delete *it;
				textureReferences.erase(it);
				return true;
			}
		}
		return false;
	}

	bool Edge::removeTextureReference(Texture *texture)
	{
		bool result = false;
		for(std::vector<TextureReference *>::iterator it = textureReferences.begin(), end = textureReferences.end(); it != end; ++it)
		{
			if((*it)->getTexture() == texture)
			{
				delete *it;
				it = textureReferences.erase(it);
				result = true;
			}
		}
		return result;
	}

	void Edge::scale(double factor)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
			(*it) *= factor;
		projectedZ *= factor;
	}

	void Edge::scaleX(double factor)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
			it->setX(it->X() * factor);
	}

	void Edge::offsetX(double amount)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
			it->setX(it->X() + amount);
	}

	void Edge::translate(double x, double y)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
		{
			it->setX(it->X() + x);
			it->setY(it->Y() + y);
		}
	}

	void Edge::expand(double x, double y)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
		{
			if(it->X() > 0.0f)
				it->setX(it->X() + x);
			if(it->X() < 0.0f)
				it->setX(it->X() - x);
			if(it->Y() > 0.0f)
				it->setY(it->Y() + y);
			if(it->Y() < 0.0f)
				it->setY(it->Y() - y);
		}
	}


    void Edge::rotate(double radians, sfa::Point &centroid) 
    {
        PointList rotatedPts( points.size() );
        int offset = 0;
        for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it, ++offset)
        {
            sfa::Point rPt;
            rPt.setX( centroid.X() + ((it->X() - centroid.X())*cos(radians) - (it->Y() - centroid.Y()) * sin(radians)) );
            rPt.setY( centroid.Y() + ((it->Y() - centroid.Y())*cos(radians) + (it->X() - centroid.X())*sin(radians)) ); 

            *(rotatedPts.begin()+offset) = rPt;
        }

        points = rotatedPts;
    }

	void Edge::setProjectedZ(double projectedZ)
	{
		this->projectedZ = projectedZ;
	}

	double Edge::getProjectedZ()
	{
		return projectedZ;
	}

	void Edge::setAttributes(const ccl::AttributeContainer &_attributes)
	{
		this->attributes = _attributes;
	}

	const ccl::AttributeContainer &Edge::getAttributes(void) const
	{
		return attributes;		
	}

	ccl::AttributeContainer &Edge::getAttributes(void)
	{
		return attributes;
	}

	void Edge::setFlags(const ccl::AttributeContainer &_flags)
	{
		this->flags = _flags;
	}

	const ccl::AttributeContainer &Edge::getFlags(void) const
	{
		return flags;		
	}

	ccl::AttributeContainer &Edge::getFlags(void)
	{
		return flags;
	}

	scenegraph::Color Edge::getPrimaryColor() const
	{
		return primaryColor;
	}

	void Edge::setPrimaryColor(const scenegraph::Color &color)
	{
		primaryColor = color;
	}

	scenegraph::Color Edge::getAlternateColor() const
	{
		return alternateColor;
	}

	void Edge::setAlternateColor(const scenegraph::Color &color)
	{
		alternateColor = color;
	}

	void Edge::calculateNormals()
	{
		if(points.size()>1)
		{
			// We cheat with these 2D points, so we'll fix it up to make a 3D plane
			sfa::Point p3 = getPoint(0);
			p3.setZ(projectedZ);

			// Construct a 2D normal for the line
			sfa::Point A = getPoint(1) - getPoint(0);
			sfa::Point B = p3 - getPoint(0);
			normal = A.cross(B);
			normal.normalize();
		}
	}

	void Edge::invertNormal()
	{
		// Swap the first two points to reverse the direction of the edge
		if(points.size()>1)
		{
			sfa::Point p0 = getPoint(0);
			sfa::Point p1 = getPoint(1);
			points.at(0) = p1;
			points.at(1) = p0;
		}
		calculateNormals();
		for(std::vector<TextureReference *>::iterator it = textureReferences.begin(), end = textureReferences.end(); it != end; ++it)
		{			
			TextureReference *tref = *it;

			sfa::Point uv0 = tref->getUV(0);
			sfa::Point uv1 = tref->getUV(1);
			sfa::Point uv2 = tref->getUV(2);

			//y should be the same for the first two nodes
			//x should be different for the first two nodes
			
		}
		
	}

}
