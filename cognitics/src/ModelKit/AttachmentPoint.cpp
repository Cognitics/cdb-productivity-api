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
#include "ModelKit/AttachmentPoint.h"

namespace modelkit
{
	AttachmentPoint::~AttachmentPoint(void)
	{
	}

	AttachmentPoint::AttachmentPoint(void)
	{
	}

	AttachmentPoint::AttachmentPoint(const std::string &name, const sfa::Point &point) : name(name), point(point)
	{
	}

	void AttachmentPoint::setName(const std::string &name)
	{
		this->name = name;
	}

	std::string AttachmentPoint::getName(void)
	{
		return name;
	}

	void AttachmentPoint::setPoint(const sfa::Point &point)
	{
		this->point = point;
	}

	sfa::Point AttachmentPoint::getPoint(void)
	{
		return point;
	}

	void AttachmentPoint::scale(double factor)
	{
		point *= factor;
	}

	void AttachmentPoint::scaleX(double factor)
	{
		point.setX(point.X() * factor);
	}

	void AttachmentPoint::offsetX(double amount)
	{
		point.setX(point.X() + amount);
	}

    void AttachmentPoint::rotate(double radians, sfa::Point &centroid) 
    {
            double x = centroid.X() + ( (point.X() - centroid.X())*cos(radians) - (point.Y() - centroid.Y()) * sin(radians) );
            double y = centroid.Y() + ((point.Y() - centroid.Y())*cos(radians) + (point.X() - centroid.X())*sin(radians) ); 

            point.setX(x);
            point.setY(y);
    }

	void AttachmentPoint::translate(double x, double y)
	{
		point.setX(point.X() + x);
		point.setY(point.Y() + y);
	}

	void AttachmentPoint::expand(double x, double y)
	{
		if(point.X() > 0.0f)
			point.setX(point.X() + x);
		if(point.X() < 0.0f)
			point.setX(point.X() - x);
		if(point.Y() > 0.0f)
			point.setY(point.Y() + y);
		if(point.Y() < 0.0f)
			point.setY(point.Y() - y);
	}

}
