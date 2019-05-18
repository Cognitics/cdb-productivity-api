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
/*! \brief Provides sfa::MultiLineString.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "MultiCurve.h"

namespace sfa
{
    class MultiLineString;
    typedef std::shared_ptr<MultiLineString> MultiLineStringSP;

/*! \class sfa::MultiLineString MultiLineString.h MultiLineString.h
\brief MultiLineString

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.8
*/
    class MultiLineString : public MultiCurve
    {
    public:
        virtual ~MultiLineString(void);
        MultiLineString(void);
        MultiLineString(const MultiLineString& lines);
        MultiLineString(const MultiLineString* lines);
        MultiLineString& operator=(const MultiLineString& rhs);

        virtual std::string getGeometryType(void) const;
        virtual WKBGeometryType getWKBGeometryType(bool withZ = false, bool withM = false) const;
        virtual void fromText(std::istream &is, bool tag, bool withZ, bool withM);

        virtual void addGeometry(Geometry* geometry);
        virtual void insertGeometry(int pos, Geometry* geometry);

    };

}
