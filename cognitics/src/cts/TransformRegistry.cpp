/*************************************************************************
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

#include "cts/TransformRegistry.h"
#include "cts/CS_CoordinateSystemFactory.h"

namespace cts
{
    TransformRegistry::~TransformRegistry(void)
    {        
        for(std::map<CS_CoordinateSystem *, CT_MathTransform *>::iterator it = registry.begin(), end = registry.end(); it != end; ++it)
            delete it->second;
        delete coordinateSystem;
    }

    TransformRegistry::TransformRegistry(void)
    {
        CS_CoordinateSystemFactory factory;
        coordinateSystem = factory.createFromWKT("WGS84");
    }

    TransformRegistry::TransformRegistry(CS_CoordinateSystem *coordinateSystem) : coordinateSystem(coordinateSystem)
    {
        if(!coordinateSystem)
        {
            CS_CoordinateSystemFactory factory;
            coordinateSystem = factory.createFromWKT("WGS84");
        }
    }

    CS_CoordinateSystem *TransformRegistry::getCoordinateSystem(void)
    {
        return coordinateSystem;
    }

    CT_MathTransform *TransformRegistry::getTransform(CS_CoordinateSystem *cs)
    {
        if(!coordinateSystem || (cs == coordinateSystem))
            return NULL;
        if(registry.find(cs) == registry.end())
        {
            CT_CoordinateTransformation *ct = coordinateTransformationFactory.createFromCoordinateSystems(cs, coordinateSystem);
            if(!ct)
                return NULL;
            registry[cs] = ct->getMathTransform();
        }
        return registry[cs];
    }

}