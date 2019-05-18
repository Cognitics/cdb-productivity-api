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
/*! \file cts/CS_CoordinateSystem.h
\headerfile cts/CS_CoordinateSystem.h
\brief Provides cts::CS_CoordinateSystem.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "pt.h"
#include "CS_Info.h"
#include "CS_Unit.h"
#include "CS_AngularUnit.h"

#include <string>
#include <vector>

namespace cts
{
//! \sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3.3
    enum CS_AxisOrientationEnum
    {
        AO_Other,
        AO_North,
        AO_South,
        AO_East,
        AO_West,
        AO_Up,
        AO_Down
    };

//! \sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3.2
    struct CS_AxisInfo
    {
        CS_AxisOrientationEnum orientation;
        std::string name;
    };

/*! \brief CS_CoordinateSystem
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3.5
*/
    class CS_CoordinateSystem : public CS_Info
    {
    private:
        int dimensions;
        PT_Envelope defaultEnvelope;
        std::vector<CS_AxisInfo> axes;
        std::vector<CS_Unit> units;

    protected:
        virtual void setDimensions(int dimensions);
        std::string getAxisWKT(int dimension);

    public:
        virtual ~CS_CoordinateSystem(void);
        CS_CoordinateSystem(int dimensions = 0);
        
        virtual int getDimensions(void);

        virtual void setDefaultEnvelope(const PT_Envelope &defaultEnvelope);
        virtual PT_Envelope getDefaultEnvelope(void);
        virtual void setAxis(int dimension, const CS_AxisInfo &axis);
        virtual CS_AxisInfo getAxis(int dimension);
        virtual void setUnits(int dimension, const CS_Unit &unit);
        virtual CS_Unit getUnits(int dimension);

    };

    typedef std::vector<CS_CoordinateSystem *> CS_CoordinateSystemList;

}

