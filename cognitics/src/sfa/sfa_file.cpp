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
#include "sfa/File.h"
#include "sfa/Feature.h"
#include "sfa/Factory.h"
#include <fstream>
#include <boost/foreach.hpp>

using namespace ccl;

namespace sfa 
{
    bool writeWKBFile(sfa::Geometry *geometry,const std::string &filename)
    {
        std::ofstream outfile;
        outfile.open(filename.c_str(),std::ios_base::binary);
        if(!outfile.is_open())
            return false;
        ccl::BindStream outStream(outfile);
        ccl::binary bin = geometry->asBinary();
        ccl::LittleEndian<ccl::uint32_t> len = ccl::uint32_t(bin.size());
        outStream.bind(len);
        if(len)
            outStream.bind(bin,len);
        outfile.close();
        return true;
    }

    sfa::Geometry *readWKBFile(const std::string &filename)
    {
        std::ifstream inFile;
        inFile.open(filename.c_str(),std::ios_base::binary);
        if(!inFile.is_open())
            return NULL;
        ccl::BindStream inStream(inFile);

        //Read geometry
        ccl::binary bin;
        ccl::LittleEndian<ccl::uint32_t> len;
        inStream.bind(len);
        inStream.bind(bin,len);
        sfa::Geometry *geometry = sfa::getGeometryFromBinary(bin);

        inFile.close();
        return geometry;
    }

    bool writeSFAFile(const std::vector<sfa::Feature *> features, const std::string &filename)
    {
        std::ofstream outfile;
        outfile.open(filename.c_str(),std::ios_base::binary);
        if(!outfile.is_open())
            return false;
        ccl::BindStream outStream(outfile);
        ccl::LittleEndian<ccl::uint32_t> count = ccl::uint32_t(features.size());
        // Write feature count
        outStream.bind(count);
        BOOST_FOREACH(sfa::Feature *feature,features)
        {
            //Write attributes
            feature->attributes.bindData(outStream);
            //Write geometry
            if(feature->geometry)
            {
                ccl::binary bin = feature->geometry->asBinary();
                ccl::LittleEndian<ccl::uint32_t> len = ccl::uint32_t(bin.size());
                outStream.bind(len);
                if(len)
                    outStream.bind(bin,len);
            }
            else
            {
                ccl::LittleEndian<ccl::uint32_t> len = ccl::uint32_t(0);
                outStream.bind(len);
            }
        }
        outfile.close();
        return true;
    }

    bool readSFAFile(std::vector<sfa::Feature *> &features, const std::string &filename)
    {
        std::ifstream inFile;
        inFile.open(filename.c_str(),std::ios_base::binary);
        if(!inFile.is_open())
            return false;
        ccl::BindStream inStream(inFile);
        ccl::LittleEndian<ccl::uint32_t> count;
        // Read feature count
        inStream.bind(count);
        for(ccl::uint32_t i=0;i<count;i++)
        {
            sfa::Feature *feature = new sfa::Feature();
            //Read attributes
            feature->attributes.bindData(inStream);
            //Read geometry
            ccl::binary bin;
            ccl::LittleEndian<ccl::uint32_t> len;
            inStream.bind(len);
            inStream.bind(bin,len);
            feature->geometry = sfa::getGeometryFromBinary(bin);
            features.push_back(feature);
        }
        inFile.close();
        return true;
    }

    sfa::Layer *File::getLayerByName(const std::string &name)
    {
        std::vector<sfa::Layer *> layers;
        layers = getLayers();
        BOOST_FOREACH(sfa::Layer *layer, layers)
        {
            if (layer->getName() == name)
            {
                return layer;
            }
        }
        return NULL;
    }

    std::string File::getUnusedLayerName(const std::string &name) 
    {
        // Look for an unused layer name we can use
        bool good_name_found = false;
        std::string good_layer_name = name;
        int tryno = 0;
        while (!good_name_found && (tryno < 1000))
        {
            bool name_unique = true;
            std::vector<sfa::Layer *> layers;
            layers = getLayers();
            BOOST_FOREACH(sfa::Layer *layer, layers)
            {
                if (layer->getName() == good_layer_name)
                {
                    name_unique = false;
                    break;
                }
            }
            if (name_unique)
                good_name_found = true;
            else
            {
                tryno++;
                std::stringstream ss;
                ss << name << "_" << tryno;
                good_layer_name = ss.str();
            }
        }
        return good_layer_name;
    }
}
