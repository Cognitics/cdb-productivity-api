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
/*! \file cts/CS_Info.h
\headerfile cts/CS_Info.h
\brief Provides cts::CS_Info.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3
*/
#pragma once

#include <string>

namespace cts
{
/*! \brief CS_Info
\sa OpenGIS Implementation Specification - Coordinate Transformation Services (OGC 01-009 Revision 1.00) 12.3.16
*/
    class CS_Info
    {
        friend class CS_CoordinateSystemFactory;    // temporary

    private:
        std::string wkt;    // temporary

        std::string name;
        std::string authority;
        std::string authorityCode;
        std::string alias;
        std::string abbreviation;
        std::string remarks;

    protected:
        std::string getAuthorityWKT(void);

    public:
        virtual ~CS_Info(void);
        CS_Info(void);
        
        virtual void setName(const std::string &name);
        virtual std::string getName(void);

        virtual void setAuthority(const std::string &authority);
        virtual std::string getAuthority(void);

        virtual void setAuthorityCode(const std::string &authorityCode);
        virtual std::string getAuthorityCode(void);

        virtual void setAlias(const std::string &alias);
        virtual std::string getAlias(void);

        virtual void setAbbreviation(const std::string &abbreviation);
        virtual std::string getAbbreviation(void);

        virtual void setRemarks(const std::string &remarks);
        virtual std::string getRemarks(void);

        virtual std::string getWKT(void);
        // TODO: virtual std::string getXML(bool child = false);    // p113

    };

}

