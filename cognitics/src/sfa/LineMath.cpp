/****************************************************************************
Copyright (c) 2019 Cognitics, Inc.

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
#include "sfa/LineMath.h"
#include <sstream>

namespace sfa {

    // Calculate the linear regression where the s value (length) of the line is the
    // x value and the z value is the y. Populates the b (slope)of the simple linear
    // regression 
    bool LinearRegressionSZ(sfa::LineString &line, double &b)
    {
        b = 0;
        std::vector<double> svec;
        std::vector<double> zvec;
        double avgs = 0;
        double avgz = 0;

        double z = 0;
        double s = 0;
        sfa::Point *lastpt = NULL;
        int num_points = line.getNumPoints();
        if (num_points == 0)
            return false;
        for (int i = 0;i < num_points; i++)
        {
            if (i > 0)
                s += lastpt->distance(line.getPointN(i));
            svec.push_back(s);
            zvec.push_back(line.getPointN(i)->Z());
            avgs += s;
            avgz += line.getPointN(i)->Z();
            lastpt = line.getPointN(i);
        }
        avgs /= num_points;
        avgz /= num_points;
        double denom = 0;
        double numerator = 0;
        for (int i = 0; i < num_points; i++)
        {
            numerator += ((svec[i] - avgs)*(zvec[i] - avgz));
            denom += ((svec[i] - avgs)*(svec[i] - avgs));
        }
        if (denom == 0)
            return false;
        b = numerator / denom;

        return true;
    }
    


}