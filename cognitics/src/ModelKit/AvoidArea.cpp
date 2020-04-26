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
#include "ModelKit/AvoidArea.h"

namespace modelkit
{
    AvoidArea::AvoidArea()
    {
    }

    AvoidArea::~AvoidArea()
    {
    }

    AvoidArea::AvoidArea(const AvoidArea &copy)
    {
        points = copy.points;
    }

    bool AvoidArea::addPoint(const sfa::Point &pt)
    {
        points.push_back(pt);
        
        return true;
    }

    bool AvoidArea::addPoint(const unsigned int index, const sfa::Point &pt)
    {
        if (index > points.size()-1)
            return false;

        std::vector<sfa::Point>::iterator resultItr = points.insert(points.begin()+index, pt);
        
        return (*resultItr == pt);
    }
    
    bool AvoidArea::addPoint(const unsigned int index, const double x, const double y, const double z, const double m)
    {
        if (index > points.size()-1)
            return false;

        sfa::Point pt(x, y, z, m);
        
        return ( addPoint(index, pt) );
    }

    bool AvoidArea::getPoint(const unsigned int index, sfa::Point &outputPt)
    {
        if (index > points.size()-1)
            return false;

        outputPt = points[index];
        
        return true;
    }

    std::vector<sfa::Point> AvoidArea::getPointsAt(const double x, const double y, const double z, const double m)
    {
        std::vector<sfa::Point> ptsFound;
        for (unsigned int i = 0; i < points.size(); i++)
        {
            sfa::Point tempPt(x, y, z, m);
            if (tempPt == points[i])
                ptsFound.push_back(tempPt);
        }

        return ptsFound;
    }

    int AvoidArea::getNumPoints()
    {
        return (int)points.size();
    }

    bool AvoidArea::removePoint(const unsigned int index)
    {   
        if (index > points.size()-1)
            return false;

       points.erase(points.begin()+index);

       return true;
    }

    void AvoidArea::scale(double factor)
	{
		for(std::vector<sfa::Point>::iterator it = points.begin(), end = points.end(); it != end; ++it)
			(*it) *= factor;
	}

    void AvoidArea::scaleX(double factor)
	{
		for(std::vector<sfa::Point>::iterator it = points.begin(), end = points.end(); it != end; ++it)
			it->setX(it->X() * factor);
	}

    void AvoidArea::offsetX(double amount)
	{
		for(std::vector<sfa::Point>::iterator it = points.begin(), end = points.end(); it != end; ++it)
			it->setX(it->X() + amount);
	}


    void AvoidArea::rotate(double radians, sfa::Point &centroid) 
    {
        std::vector<sfa::Point> rotatedPts( points.size() );
        int offset = 0;
        for(std::vector<sfa::Point>::iterator it = points.begin(), end = points.end(); it != end; ++it, ++offset)
        {
            sfa::Point rPt;
            rPt.setX( centroid.X() + ((it->X() - centroid.X())*cos(radians) - (it->Y() - centroid.Y()) * sin(radians)) );
            rPt.setY( centroid.Y() + ((it->Y() - centroid.Y())*cos(radians) + (it->X() - centroid.X())*sin(radians)) ); 

            *(rotatedPts.begin()+offset) = rPt;
        }

        points = rotatedPts;
    }

	void AvoidArea::translate(double x, double y)
	{
		for(std::vector<sfa::Point>::iterator it = points.begin(), end = points.end(); it != end; ++it)
		{
			it->setX(it->X() + x);
			it->setY(it->Y() + y);
		}
	}

	void AvoidArea::expand(double x, double y)
	{
		for(std::vector<sfa::Point>::iterator it = points.begin(), end = points.end(); it != end; ++it)
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

    bool AvoidArea::operator==(AvoidArea &lhs)
    {
        if (getNumPoints() != lhs.getNumPoints())
        {
            return false;
        }

        for (int i = 0; i < getNumPoints(); i++)
        {
            sfa::Point thisPt;
            sfa::Point lhsPt;
            getPoint(i, thisPt);
            lhs.getPoint(i, lhsPt);
            if (thisPt != lhsPt)
            {
                return false;
            }
        }

        return true;
    }

}
