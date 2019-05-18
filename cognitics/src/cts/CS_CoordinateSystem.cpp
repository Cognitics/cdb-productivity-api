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

#include "cts/CS_CoordinateSystem.h"
#include <sstream>

namespace cts
{
    void CS_CoordinateSystem::setDimensions(int dimensions)
    {
        this->dimensions = dimensions;
        axes.resize(dimensions);
        units.resize(dimensions);
    }

    // <axis> = AXIS["<name>", NORTH | SOUTH | EAST | WEST | UP | DOWN | OTHER]
    std::string CS_CoordinateSystem::getAxisWKT(int dimension)
    {
        if(dimension >= getDimensions())
            return std::string();
        CS_AxisInfo axis = getAxis(dimension);
        std::stringstream ss;
        ss << "AXIS[\"" << getName() << "\",";
        switch(axis.orientation)
        {
            case AO_North: ss << "NORTH"; break;
            case AO_South: ss << "SOUTH"; break;
            case AO_East: ss << "EAST"; break;
            case AO_West: ss << "WEST"; break;
            case AO_Up: ss << "UP"; break;
            case AO_Down: ss << "DOWN"; break;
            default: ss << "OTHER"; break;
        }
        ss << "]";
        return ss.str();
    }

    CS_CoordinateSystem::~CS_CoordinateSystem(void)
    {
    }

    CS_CoordinateSystem::CS_CoordinateSystem(int dimensions)
    {
        setDimensions(dimensions);
    }

    int CS_CoordinateSystem::getDimensions(void)
    {
        return dimensions;
    }

    void CS_CoordinateSystem::setDefaultEnvelope(const PT_Envelope &defaultEnvelope)
    {
        this->defaultEnvelope = defaultEnvelope;
    }

    PT_Envelope CS_CoordinateSystem::getDefaultEnvelope(void)
    {
        return defaultEnvelope;
    }

    void CS_CoordinateSystem::setAxis(int dimension, const CS_AxisInfo &axis)
    {
        if(dimension < getDimensions())
            axes[dimension] = axis;
    }

    CS_AxisInfo CS_CoordinateSystem::getAxis(int dimension)
    {
        return (dimension < getDimensions()) ? axes[dimension] : CS_AxisInfo();
    }

    void CS_CoordinateSystem::setUnits(int dimension, const CS_Unit &unit)
    {
        if(dimension < getDimensions())
            units[dimension] = unit;
    }

    CS_Unit CS_CoordinateSystem::getUnits(int dimension)
    {
        return (dimension < getDimensions()) ? units[dimension] : CS_Unit();
    }


}