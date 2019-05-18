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
//! \file DataSource.cpp

#include "elev/DataSource.h"

#include <ccl/Log.h>
#include <fstream>

#include <ogr_srs_api.h>
#include <cpl_conv.h>

using namespace ccl;
namespace elev
{
    DataSource::DataSource(std::string filename, Cache *cache) : refcount(0)
    {
        log.init("DataSource", this);
#ifdef DEBUG
        log << ccl::LDEBUG << "DataSource(" << filename << ", *)" << log.endl;
#endif
        this->cache = cache;
        this->filename = filename;
        this->reftype = "WGS84";
        this->type = DATASOURCE_TYPE_UNDEFINED;
        this->valid = false;
        this->file_srs = NULL;
        this->app_srs = NULL;
        this->app_ct = NULL;
        this->file_ct = NULL;
    }

    void DataSource::SetReferenceType(std::string reftype)
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "SetReferenceType(" << reftype << ")" << log.endl;
#endif
        this->reftype = reftype;
        this->BuildCoordinateTransformations();
    }

    //!< Needed because of https://github.com/OSGeo/proj.4/issues/226 (deadlock/crash because of setlocal not being threadsafe)
    static ccl::mutex transformMutex;                
    void DataSource::BuildCoordinateTransformations()
    {
#ifdef DEBUG
        log << ccl::LDEBUG << "BuildCoordinateTransformations()" << log.endl;
#endif
        file_ct = NULL;
        app_ct = NULL;

        // Using the C handle functions to avoid DLL hell with release/debug versions
        if(app_srs) 
            OSRDestroySpatialReference ( OGRSpatialReferenceH(app_srs) );
        app_srs = (OGRSpatialReference *) OSRNewSpatialReference( NULL );;
        OGRErr err = app_srs->SetFromUserInput(reftype.c_str());
        if(err) 
            throw std::runtime_error("invalid reference type");
        if(!file_srs) 
        {
            file_srs = app_srs; //throw std::runtime_error("missing file spacial reference");
            return;
        }

        // we don't need transforms if the file is geographic
        if(file_srs->IsGeographic())
            return;

        transformMutex.lock();
        file_ct = OGRCreateCoordinateTransformation(app_srs, file_srs);
        app_ct = OGRCreateCoordinateTransformation(file_srs, app_srs);
        transformMutex.unlock();
    }

    bool DataSource::LoadProjectionFromPRJ()
    {
        std::string data = "";
        std::string prjname = filename;
        size_t lastdot = prjname.find_last_of('.');
        if(lastdot) 
            prjname.erase(prjname.begin() + lastdot, prjname.end());
        prjname += ".prj";
        std::ifstream prjfile(prjname.c_str());
        if(!prjfile) 
            return false;
        prjfile >> data;
        prjfile.close();
        if(file_srs) 
            delete file_srs;
        file_srs = new OGRSpatialReference;
        const char *prjstr = data.c_str();
        OGRErr err = file_srs->importFromWkt((char **)&prjstr);
        if(err != OGRERR_NONE)
        {
            delete file_srs;
            file_srs = NULL;
            return false;
        }
        return true;
    }

    void DataSource::ref(void)
    {
        ++refcount;
    }

    void DataSource::unref(void)
    {
        --refcount;
    }

}

