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
/*! \file flt/OpenFlight.h
\headerfile flt/OpenFlight.h
\brief Provides flt::OpenFlight
\author Aaron Brinton <abrinton@cognitics.net>
\date 04 February 2010
\sa OpenFlight Scene Specification v16.3 (http://www.multigen-paradigm.com/support/dc_files/of_spec_16_3a.pdf)
*/
#pragma once

#include "Record.h"
#include <fstream>

namespace flt
{
    class OpenFlight
    {
    private:
        ccl::ObjLog log;
        std::ifstream inFile;
        std::ofstream outFile;
        ccl::BindStream bindStream;
        int revision;
        ccl::BigEndian<ccl::uint16_t> nextOpcode;    // prefetched opcode

        OpenFlight(void);

    public:
        static OpenFlight *create(const std::string &filename, int revision = 0);
        static OpenFlight *open(const std::string &filename);

        ~OpenFlight(void);

        static void destroy(OpenFlight *ptr);
        static std::vector<int> getSupportedRevisions(void);
        static bool supportsRevision(int revision);

        // utility methods
        static std::vector<std::string> getTexturePaletteFilenames(const std::string &filename);

        static bool getBoundingBox(const std::string &filename, double &xmin, double &xmax ,double &ymin, double &ymax, double &zmin, double &zmax);


        ccl::uint16_t getNextOpcode(void);
        Record *getNextRecord(void);
        RecordList getRecords(void);
        bool addRecord(Record *record);
        bool addRecords(const RecordList &records);

    };


}





