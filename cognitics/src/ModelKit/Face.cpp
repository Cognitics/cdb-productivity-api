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
#include "ModelKit/Face.h"
#include <algorithm>
namespace modelkit
{
	Face::~Face(void)
	{
		//The material reference is just that: a referenece. Meaning it is owned by 
        //someone else, i.e. the cross section. So don't delete it here.
        materialRef = NULL;

        for(std::vector<TextureReference *>::iterator it = textureReferences.begin(), end = textureReferences.end(); it != end; ++it)
			delete *it;
	}

	Face::Face(void) : fid(0), smc(0), materialRef(NULL)
	{
	}

	Face::Face(const Face &copy)
	{
		name = copy.name;
		points = copy.points;
		fid = copy.fid;
		smc = copy.smc;
        materialRef = copy.materialRef;
		for(std::vector<TextureReference *>::const_iterator it = copy.textureReferences.begin(), end = copy.textureReferences.end(); it != end; ++it)
			textureReferences.push_back(new TextureReference(*(*it)));
		flags = copy.flags;
		primaryColor = copy.primaryColor;
		alternateColor = copy.alternateColor;
	}

	Face::Face(const std::string &name, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c, int fid, int smc) 
                : name(name), fid(fid), smc(smc), materialRef(NULL)
	{
		points.push_back(a);
		points.push_back(b);
		points.push_back(c);
	}

	void Face::setName(const std::string &name)
	{
		this->name = name;
	}

	std::string Face::getName(void)
	{
		return name;
	}

	void Face::addPoint(const sfa::Point &point)
	{
		points.push_back(point);
	}

	PointList Face::getPoints(void)
	{
		return points;
	}

	sfa::Point &Face::getPoint(int index)
	{
		return points[index];
	}

	void Face::removePoint(int index)
	{
		points.erase(points.begin() + index);
	}

	void Face::clearPoints(void)
	{
		points.clear();
	}

	int Face::getNumPoints() const 
	{
		return int(points.size());
	}

	void Face::setFID(int fid)
	{
		this->fid = fid;
	}

	int Face::getFID(void)
	{
		return fid;
	}

	void Face::setSMC(int smc)
	{
		this->smc = smc;
	}

	int Face::getSMC(void)
	{
		return smc;
	}

    void Face::setMaterialRef(Material *m)
    { 
        //The material reference is just that: a referenece. Meaning it is owned by 
        //someone else, i.e. the cross section. So don't delete the old one. 
        this->materialRef = m; 
    }
    
    Material* Face::getMaterialRef()
    { 
        return materialRef; 
    }

	void Face::addTextureReference(TextureReference *textureReference)
	{
		textureReferences.push_back(textureReference);
	}

	TextureReference *Face::addTextureReference(Texture *texture, const sfa::Point &a, const sfa::Point &b, const sfa::Point &c)
	{
		TextureReference *textureReference = new TextureReference(texture, a, b, c);
		textureReferences.push_back(textureReference);
		return textureReference;
	}

	std::vector<TextureReference *> Face::getTextureReferences(void)
	{
		return textureReferences;
	}

	bool Face::removeTextureReference(TextureReference *textureReference)
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

	bool Face::removeTextureReference(Texture *texture)
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

	void Face::scale(double factor)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
			(*it) *= factor;
	}

	void Face::scaleX(double factor)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
			it->setX(it->X() * factor);
	}

	void Face::offsetX(double amount)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
			it->setX(it->X() + amount);
	}


    void Face::rotate(double radians, sfa::Point &centroid) 
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

	void Face::translate(double x, double y)
	{
		for(PointList::iterator it = points.begin(), end = points.end(); it != end; ++it)
		{
			it->setX(it->X() + x);
			it->setY(it->Y() + y);
		}
	}

	void Face::expand(double x, double y)
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

	void Face::setFlags(const ccl::AttributeContainer &_flags)
	{
		this->flags = _flags;
	}

	const ccl::AttributeContainer &Face::getFlags(void) const
	{
		return flags;		
	}

	ccl::AttributeContainer &Face::getFlags(void)
	{
		return flags;
	}

	scenegraph::Color Face::getPrimaryColor() const
	{
		return primaryColor;
	}

	void Face::setPrimaryColor(const scenegraph::Color &color)
	{
		primaryColor = color;
	}

	scenegraph::Color Face::getAlternateColor() const
	{
		return alternateColor;
	}

	void Face::setAlternateColor(const scenegraph::Color &color)
	{
		alternateColor = color;
	}
	void Face::invertNormal()
	{
		std::reverse(points.begin(),points.end());
		for(std::vector<TextureReference *>::iterator it = textureReferences.begin(), end = textureReferences.end(); it != end; ++it)
		{
			TextureReference *tr = *it;
			PointList &pl = tr->getUVs();
			std::reverse(pl.begin(),pl.end());
		}

	}

}
