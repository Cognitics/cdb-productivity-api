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

#include "ModelKit/ModelInstance.h"

namespace modelkit
{
	ModelInstance::~ModelInstance(void)
	{
	}

    ModelInstance::ModelInstance(void) : csRef(NULL), autoPlaced(false), position(ModelInstancePosition_None), 
                                            clampToTerrain(false), spacingStandardDeviation(0.0), spacingMean(0.0), 
                                            offsetStandardDeviation(0.0), offsetMean(0.0), bufferRadius(0.0),
                                            bufferHeight(0.0)
	{
	}

    ModelInstance::ModelInstance(std::string src, CrossSectionReference *csr, std::string apName, bool aPlaced, 
                                    std::string pos, bool clamp, sfa::Point &scale, std::string tName, double ssDev, 
                                    double sm, double osDev, double om, std::string dist, double br, double bh)
		: source(src), csRef(csr), attachPtName(apName), autoPlaced(aPlaced), clampToTerrain(clamp), scaleFactor(scale), 
            tagName(tName), spacingStandardDeviation(ssDev), spacingMean(sm), offsetStandardDeviation(osDev), 
            offsetMean(om), bufferRadius(br), bufferHeight(bh)
	{
		setPositionString(pos);
        setDistributionString(dist);
	}

	std::string ModelInstance::getSource(void)
	{
		return source;
	}

	CrossSectionReference *ModelInstance::getCrossSectionReference(void)
	{
		return csRef;
	}

	std::string ModelInstance::getAttachmentPointName(void)
	{
		return attachPtName;
	}

    bool ModelInstance::getAutoPlaced(void)
	{
		return autoPlaced;
	}

    const std::string ModelInstance::getPositionStr(void)
    {
        switch(position)
        {
        case ModelInstancePosition_Start:
            return "start";
        case ModelInstancePosition_End:
            return "end";
        case ModelInstancePosition_StartAndEnd:
            return "start and end";
        case ModelInstancePosition_Vertex:
            return "every vertex";
        case ModelInstancePosition_None:
        default:
            return "none";
        };
    }

    const ModelInstancePosition ModelInstance::getPosition(void)
    {
        return position;
    }

    bool ModelInstance::isClampedToTerrain(void)
    {
        return clampToTerrain;
    }
    
    sfa::Point ModelInstance::getScaleFactor(void)
    {
        return scaleFactor;
    }

    double ModelInstance::getScaleX(void)
    {
        return scaleFactor.X();
    }

    double ModelInstance::getScaleY(void)
    {
        return scaleFactor.Y();
    }

    double ModelInstance::getScaleZ(void)
    {
        return scaleFactor.Z();
    }
        
    std::string ModelInstance::getTagName(void)
    {
        return tagName;
    }

    double ModelInstance::getSpacingStandardDeviation(void)
    {
        return spacingStandardDeviation;
    }

    double ModelInstance::getSpacingMean(void)
    {
            return spacingMean;
    }

    double ModelInstance::getOffsetStandardDeviation(void)
    {
        return offsetStandardDeviation;
    }

    double ModelInstance::getOffsetMean(void)
    {
        return offsetMean;
    }

    std::string ModelInstance::getDistributionString(void)
    {
        switch(distributionType)
		{
		case DISTRIBUTION_NORMAL:
			return "normal";
		case DISTRIBUTION_UNIFORM:
			return "uniform";
		case DISTRIBUTION_NONE:
        default:
			return "none";
		}
    }

    double ModelInstance::getBufferRadius(void)
    {
        return bufferRadius;
    }
    
    double ModelInstance::getBufferHeight(void)
    {
        return bufferHeight;
    }

	DistributionType ModelInstance::getDistributionType() const
	{
		return distributionType;
	}

	void ModelInstance::setDistributionType(DistributionType type)
	{
		this->distributionType = type;
	}
	
    void ModelInstance::setSource(const std::string &s)
	{
		source = s;
	}

	void ModelInstance::setCrossSectionReference(CrossSectionReference *csr)
	{
		csRef = csr;
	}

    void ModelInstance::setAttachmentPointName(const std::string &apName)
	{
		attachPtName = apName;
	}

	void ModelInstance::setAutoPlaced(bool ap)
	{
		autoPlaced = ap;
	}

    void ModelInstance::setPositionString(const std::string &s)
    {
        ModelInstancePosition p;
        if (s == "start")
            p = ModelInstancePosition_Start;
        else if (s == "end")
            p = ModelInstancePosition_End;
        else if (s == "start and end")
            p = ModelInstancePosition_StartAndEnd;
        else if (s == "every vertex")
            p = ModelInstancePosition_Vertex;
        else 
            p = ModelInstancePosition_None;

        position = p;
    }

    void ModelInstance::setPosition(const ModelInstancePosition p)
    {
        position = p;
    }

    void ModelInstance::setScaleFactor(sfa::Point &p)
    {
        scaleFactor = p;
    }

    void ModelInstance::setScaleFactor(double x, double y, double z)
    {
        scaleFactor.setX(x);
        scaleFactor.setY(y);
        scaleFactor.setZ(z);
    }
    
    void ModelInstance::setClampToTerrain(bool c)
    {
        clampToTerrain = c;
    }

    void ModelInstance::setTagName(const std::string &n)
    {
        tagName = n;
    }
    
    void ModelInstance::setSpacingStandardDeviation(double stddev)
    {
        spacingStandardDeviation = stddev;
    }

    void ModelInstance::setSpacingMean(double m)
    {
        spacingMean = m;
    }
    
    void ModelInstance::setOffsetStandardDeviation(double stddev)
    {
        offsetStandardDeviation = stddev;
    }
    
    void ModelInstance::setOffsetMean(double m)
    {
        offsetMean = m;
    }

    void ModelInstance::setDistributionString(std::string d)
    {
        if(d=="normal")
			this->distributionType = DISTRIBUTION_NORMAL;
		else if(d=="uniform")
			this->distributionType = DISTRIBUTION_UNIFORM;
		else
			this->distributionType = DISTRIBUTION_NONE;
    }

    void ModelInstance::setBufferRadius(double r)
    {
        bufferRadius = r;
    }
    
    void ModelInstance::setBufferHeight(double h)
    {
        bufferHeight = h;
    }
} 
