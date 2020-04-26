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

#include "ModelKit/RGBA.h"
#include <string>

namespace modelkit
{
	class Material
    {
    private:
        std::string identifier;
        double transparency;  //alpha
        double shine;
        RGBA ambient;
        RGBA diffuse;
        RGBA specular;
        RGBA emission;

    public:
        Material(const std::string &id = std::string(), double trans = -1.0, double shn = 0.0, 
                            const RGBA &amb = RGBA(), const RGBA &diff = RGBA(), 
                            const RGBA &spec = RGBA(), const RGBA &emis = RGBA());                
        Material(const Material &copy);
        virtual ~Material();
        std::string getID(){ return identifier; }
        double getTransparency();
        double getShine(){ return shine; }
        RGBA getAmbient(){ return ambient; }
        RGBA getDiffuse(){ return diffuse; }
        RGBA getSpecular(){ return specular; }
        RGBA getEmission(){ return emission; }
        void setID(const std::string &id){ identifier = id; }
        void setTransparency(double t);
        void setShine(double s){ shine = s; }
        void setAmbient(const RGBA &a){ ambient = a; }
        void setDiffuse(const RGBA &d){ diffuse = d; }
        void setSpecular(const RGBA &s){ specular = s; }
        void setEmission(const RGBA &e){ emission = e; }

        bool operator==(const Material &rhs) const;

    };  //Material : class
} //modelkit : namespace