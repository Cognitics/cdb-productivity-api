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

#include "sfa/Point.h"

namespace modelkit
{
    class AvoidArea
	{
	private:
        std::vector<sfa::Point> points;
    public:
        AvoidArea();
        AvoidArea(const AvoidArea &copy);
        virtual ~AvoidArea();
        
        /*
         * Adds the specified sfa::point to the end of the points list.
         * Returns true if successful, false if not.
         */
        bool addPoint(const sfa::Point &pt);

        /*
         * Adds the specified sfa::point to the list at the specified index.
         * Returns true if successful, false if not.
         */
        bool addPoint(const unsigned int index, const sfa::Point &pt);

        /*
         * Adds a new sfa::point at the specified location to the list.
         * Returns true if successful, false if not.
         */
        bool addPoint(const unsigned int index, const double x=0.0, const double y=0.0, const double z=0.0, const double m=0.0);
        
        /*
         * Populates 'outputPt' with the point data of the point 
         * at the location specified in the list.
         * Returns true if successful; false if not.
         */
        bool getPoint(const unsigned int index, sfa::Point &outputPt);  

        /*
         * Returns all the points at the specified location. 
         * If no points are found at the specified location then the
         * list returned is empty. 
         */
        std::vector<sfa::Point> getPointsAt(const double x=0.0, const double y=0.0, const double z=0.0, const double m=0.0);

        /*
         * Returns the number of points for this AvoidArea.
         */
         int getNumPoints();

        /*
         * Removes the point at the specified index from the list.
         * Returns true if successful; false if not.
         */
        bool removePoint(const unsigned int index);

        /*
         * Retrns the list of points. 
         */
        std::vector<sfa::Point>& getPoints(){ return points; }

        void scale(double factor);
        void scaleX(double factor);
        void offsetX(double amount);
        void rotate(double radians, sfa::Point &centroid);
		void translate(double x, double y);
		void expand(double x, double y);

        bool operator==(AvoidArea &lhs);
    };
}