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

#include "ModelKit/Reference.h"
#include "sfa/Point.h"


namespace modelkit
{
	typedef Reference CrossSectionReference;

	enum DistributionType
	{
		DISTRIBUTION_NORMAL,
		DISTRIBUTION_UNIFORM,
		DISTRIBUTION_NONE
	};
    enum ModelInstancePosition
    {
        ModelInstancePosition_None = 0,
        ModelInstancePosition_Start,
        ModelInstancePosition_End,
        ModelInstancePosition_StartAndEnd,
        ModelInstancePosition_Vertex

    };

    class ModelInstance
    {
	private:
	    std::string source;
	    CrossSectionReference *csRef;
	    std::string attachPtName;
        bool autoPlaced;
        ModelInstancePosition position;
        bool clampToTerrain;
        sfa::Point scaleFactor;
        std::string tagName;
        double spacingStandardDeviation;
        double spacingMean;
        double offsetStandardDeviation;
        double offsetMean;
		DistributionType distributionType;
        double bufferRadius;
        double bufferHeight;
	    
    public:
		~ModelInstance(void);
        ModelInstance(void);
        ModelInstance(std::string src, CrossSectionReference *csr, std::string apName, 
                        bool aPlaced, std::string pos, bool clamp, sfa::Point &scale, 
                        std::string tName, double ssDev, double sm, double osDev, 
                        double om, std::string dist, double br, double bh);
        std::string getSource(void);
		CrossSectionReference *getCrossSectionReference(void);
		std::string getAttachmentPointName(void);
        bool getAutoPlaced(void);
        const std::string getPositionStr(void);
        const ModelInstancePosition getPosition(void);
        bool isClampedToTerrain(void);
        sfa::Point getScaleFactor(void);
        double getScaleX(void);
        double getScaleY(void);
        double getScaleZ(void);
        std::string getTagName(void);
        double getSpacingStandardDeviation(void);
        double getSpacingMean(void);
        double getOffsetStandardDeviation(void);
        double getOffsetMean(void);
        std::string getDistributionString(void); 
        double getBufferRadius(void);
        double getBufferHeight(void);
		DistributionType getDistributionType() const;

		void setDistributionType(DistributionType type);
        void setSource(const std::string &s);
		void setCrossSectionReference(CrossSectionReference *csr);
        void setAttachmentPointName(const std::string &apName);
		void setAutoPlaced(bool ap);
        void setPositionString(const std::string &p=std::string());
        void setPosition(const ModelInstancePosition p);
        void setScaleFactor(sfa::Point &p);
        void setScaleFactor(double x=1.0, double y=1.0, double z=1.0);
        void setClampToTerrain(bool c);
        void setTagName(const std::string &n);
        void setSpacingStandardDeviation(double stddev);
        void setSpacingMean(double m);
        void setOffsetStandardDeviation(double stddev);
        void setOffsetMean(double m);
        void setDistributionString(std::string d);
        void setBufferRadius(double r);
        void setBufferHeight(double h);
    };

}
