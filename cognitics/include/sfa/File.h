/****************************************************************************
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
/*! \brief Provides sfa::File
\author Kevin Bentley <kbentley@cognitics.net>
\date 14 September 2012
*/
#pragma once

#include "sfa/Layer.h"

namespace sfa
{
    typedef std::vector<sfa::Layer *> LayerList;
    //! Abstract file interface for layer access
    class File 
    {

    public:
        virtual ~File() {}

        //! Get a vector of the feature layers in the file
        virtual sfa::LayerList getLayers() = 0;

        //! Open a path, optionally with updates
        virtual bool open(std::string path, bool update = false) = 0;

        //! Close the currently open feature file
        virtual bool close() = 0;

        //! Add a new layer to the feature file
        virtual sfa::Layer *addLayer(std::string name, sfa::WKBGeometryType type, cts::CS_CoordinateSystem *coordinateSystem = NULL) = 0;

        //! Get the total number of features in the file
        virtual int getFeatureCount(void) = 0;

        //! Create a new file
        virtual bool create(const std::string &filename) = 0;

        //! Find a layer name that is not used by appending consecutive integrers to the end of the supplied name if needed
        std::string getUnusedLayerName(const std::string &name);

        //! Lookup a given layer by name and return a pointer to it if it is found. Return NULL if not exactly found
        sfa::Layer *getLayerByName(const std::string &name);

        virtual bool beginUpdating() { return false; }
        virtual bool commitUpdates() { return false; }
        virtual bool rollbackUpdates() { return false; }
    };

    bool writeSFAFile(const std::vector<sfa::Feature *> features, const std::string &filename);
    bool readSFAFile(std::vector<sfa::Feature *> &features, const std::string &filename);

    bool writeWKBFile(sfa::Geometry *geometry,const std::string &filename);
    sfa::Geometry *readWKBFile(const std::string &filename);

}
