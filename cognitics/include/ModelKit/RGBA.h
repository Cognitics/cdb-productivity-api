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

namespace modelkit
{
	class RGBA
	{
    private:
        double red;
        double green;
        double blue;
        double alpha;
    
    public:
        RGBA(double r=0.0, double g=0.0, double b=0.0, double a=0.0) : red(r), green(g), blue(b), alpha(a)
        {}
        RGBA(const RGBA &copy)
        {
            red = copy.red;
            green = copy.green;
            blue = copy.blue;
            alpha = copy.alpha;
        }
        virtual ~RGBA()
        {}
        double getR(){ return red; }
        double getG(){ return green; }
        double getB(){ return blue; }
        double getA(){ return alpha; }
        void setR(double r){ red = r; }
        void setG(double g){ green = g; }
        void setB(double b){ blue = b; }
        void setA(double a){ alpha = a; }

        bool operator==(const RGBA &rhs) const
        {
            return (red == rhs.red && green == rhs.green && blue == rhs.blue && alpha == rhs.alpha); 
        }

    };  //RGBA : class
}  //modelkit : namespace