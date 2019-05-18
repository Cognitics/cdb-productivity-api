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

#include "cts/ProjInfo.h"
#include <fstream>
#include <sstream>

namespace cts
{
    ProjInfo::ProjInfo(const std::string &filename)
    {
        clear();
        if(filename.size())
            read(filename);
    }

    double getDouble(const std::string &value)
    {
        std::stringstream ss(value);
        double result;
        ss >> result;
        return result;
    }

    int getInteger(const std::string &value)
    {
        std::stringstream ss(value);
        int result;
        ss >> result;
        return result;
    }

    void ProjInfo::clear(void)
    {
        csm = "";
        originlat = 0.0f;
        originlon = 0.0f;
        zunits = 0.0f;
        selectorId = 0;
        dcType = "";
        dcSelectorId = 0;
        txpOffsetX = 0.0f;
        txpOffsetY = 0.0f;
        txpOffsetZ = 0.0f;
    }

    bool ProjInfo::read(const std::string &filename)
    {
        clear();

        std::ifstream fileStream;
        fileStream.open(filename.c_str(), std::ifstream::in);
        if(fileStream.fail())
            return false;
        std::string line;
        while(!fileStream.eof())
        {
            getline(fileStream, line); 
            std::string::size_type eq = line.find('=');
            if(eq == std::string::npos)
                continue;
            std::string key = line.substr(0, eq);
            std::string value = line.substr(eq + 1);
            if(key == "csm")
                csm = value;
            if(key == "originlat")
                originlat = getDouble(value);
            if(key == "originlon")
                originlon = getDouble(value);
            if(key == "zunits")
                zunits = getDouble(value);
            if(key == "selectorId")
                selectorId = getInteger(value);
            if(key == "dcType")
                dcType = value;
            if(key == "dcSelectorId")
                dcSelectorId = getInteger(value);
            if(key == "txpOffsetX")
                txpOffsetX = getDouble(value);
            if(key == "txpOffsetY")
                txpOffsetY = getDouble(value);
            if(key == "txpOffsetZ")
                txpOffsetZ = getDouble(value);
        }
        fileStream.close();
        return true;
    }

    bool ProjInfo::write(const std::string &filename)
    {
        std::ofstream fileStream;
        fileStream.open(filename.c_str(), std::ifstream::out);
        if(fileStream.fail())
            return false;
        fileStream << std::fixed;
        fileStream << "csm=" << csm << std::endl;
        fileStream.precision(15);
        fileStream << "originlat=" << originlat << std::endl;
        fileStream << "originlon=" << originlon << std::endl;
        fileStream << "zunits=" << zunits << std::endl;
        fileStream << "selectorId=" << selectorId << std::endl;
        fileStream << "dcType=" << dcType << std::endl;
        fileStream << "dcSelectorId=" << dcSelectorId << std::endl;
        fileStream.precision(6);
        fileStream << "txpOffsetX=" << txpOffsetX << std::endl;
        fileStream << "txpOffsetY=" << txpOffsetY << std::endl;
        fileStream << "txpOffsetZ=" << txpOffsetZ << std::endl;
        fileStream.close();
        return true;
    }


}
