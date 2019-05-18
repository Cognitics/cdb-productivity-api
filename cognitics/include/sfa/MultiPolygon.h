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
/*! \brief Provides sfa::MultiPolygon.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0)
*/
#pragma once

#include "MultiSurface.h"

namespace sfa
{
    class MultiPolygon;
    typedef std::shared_ptr<MultiPolygon> MultiPolygonSP;

/*! \class sfa::MultiPolygon MultiPolygon.h MultiPolygon.h
\brief MultiPolygon

\sa OpenGIS Implementation Specification for Geographic Information - Simple Feature Access (OGC 06-103r3 Version 1.2.0) 6.1.14
*/
    class MultiPolygon : public MultiSurface
    {
    public:
        virtual ~MultiPolygon(void);
        MultiPolygon(void);
        MultiPolygon(const MultiPolygon& polygons);
        MultiPolygon(const MultiPolygon* polygons);
        MultiPolygon& operator=(const MultiPolygon& rhs);

        virtual std::string getGeometryType(void) const;
        virtual WKBGeometryType getWKBGeometryType(bool withZ = false, bool withM = false) const;
        virtual void fromText(std::istream &is, bool tag, bool withZ, bool withM);

        virtual void addGeometry(Geometry* geometry);
        virtual void insertGeometry(int pos, Geometry* geometry);

        virtual double getArea(void) const;
        virtual Point* getCentroid(void) const;
        virtual Point* getPointOnSurface(void) const;

    };

}
