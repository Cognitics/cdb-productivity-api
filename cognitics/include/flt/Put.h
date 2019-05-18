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
/*! \file flt/Put.h
\headerfile flt/Put.h
\brief Provides flt::Put
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
*/
#pragma once

#include "Record.h"

namespace flt
{
    class Put : public Record
    {
    public:
        ccl::BigEndian<ccl::int32_t> RESERVED4;
        ccl::BigEndian<double> fromOriginX;
        ccl::BigEndian<double> fromOriginY;
        ccl::BigEndian<double> fromOriginZ;
        ccl::BigEndian<double> fromAlignX;
        ccl::BigEndian<double> fromAlignY;
        ccl::BigEndian<double> fromAlignZ;
        ccl::BigEndian<double> fromTrackX;
        ccl::BigEndian<double> fromTrackY;
        ccl::BigEndian<double> fromTrackZ;
        ccl::BigEndian<double> toOriginX;
        ccl::BigEndian<double> toOriginY;
        ccl::BigEndian<double> toOriginZ;
        ccl::BigEndian<double> toAlignX;
        ccl::BigEndian<double> toAlignY;
        ccl::BigEndian<double> toAlignZ;
        ccl::BigEndian<double> toTrackX;
        ccl::BigEndian<double> toTrackY;
        ccl::BigEndian<double> toTrackZ;

        virtual ~Put(void);
        Put(void);

        virtual int getRecordType(void);
        virtual std::string getRecordName(void);

        virtual void bind(ccl::BindStream &bs, int length, int revision = 0);

    };

}


