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

#include "ModelKit/Material.h"

namespace modelkit
{
    Material::Material(const std::string &id, double trans, double shn, const RGBA &amb, const RGBA &diff, const RGBA &spec, const RGBA &emis)
        : identifier(id), shine(shn), ambient(amb), diffuse(diff), specular(spec), emission(emis)
    {
        if (trans == -1)
            setTransparency(ambient.getA());  //no transparency specified so use the ambient alpha value to set it.
        else
            setTransparency(trans);  
    }

    Material::Material(const Material &copy)
    {
		transparency = copy.transparency;
		identifier = copy.identifier;
        shine = copy.shine;
        ambient = copy.ambient;
        diffuse = copy.diffuse;
        specular = copy.specular;
        emission = copy.emission;
    }

    Material::~Material()
    {
    }

    void Material::setTransparency(double t)
    { 
        //material components (ambient, shine, diffuse, specular, and emission) all have matching 
        // alpha values specifying the material transparency. 
        transparency = t;
        ambient.setA(t);
        diffuse.setA(t);
        specular.setA(t);
        emission.setA(t);
    }

    double Material::getTransparency()
    {
        return transparency;
    }

    bool Material::operator==(const Material &rhs) const
    {
        return ( identifier == rhs.identifier
                && transparency == rhs.transparency
                && shine == rhs.shine
                && ambient == rhs.ambient
                && diffuse == rhs.diffuse
                && specular == rhs.specular
                && emission == rhs.emission );
    }

}  //modelkit : namespace