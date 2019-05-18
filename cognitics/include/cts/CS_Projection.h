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
/*! \file cts/CS_Projection.h
\headerfile cts/CS_Projection.h
\brief Provides cts::CS_Projection.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include "CS_Unit.h"
#include <string>
#include <map>

namespace cts
{
    struct CS_ProjectionParameter
    {
        std::string name;
        double value;
    };

/*! \brief CS_Projection
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
    class CS_Projection : public CS_Unit
    {
    private:
        std::string className;
        std::map<std::string, double> parameters;

    public:
        virtual ~CS_Projection(void);
        CS_Projection(void);
        
        virtual void setClassName(const std::string &className);
        virtual std::string getClassName(void);

        virtual CS_ProjectionParameter getParameter(unsigned int index);
        virtual int getNumParameters(void);

        virtual void setParameter(const std::string &name, double value);
        virtual double getParameter(const std::string &name);
        virtual void removeParameter(const std::string &name);

        virtual std::string getWKT(void);

    };

}

