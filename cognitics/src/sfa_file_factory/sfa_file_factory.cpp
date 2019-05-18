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
#include <ccl/FileInfo.h>
#include "sfa_file_factory/sfa_file_factory.h"
namespace sfa
{
    FileRegistry *FileRegistry::theInstance=NULL;

    FileRegistry * FileRegistry::instance()
    {
        if(!theInstance)
            theInstance = new FileRegistry();
        return theInstance;
    }

    void FileRegistry::registerFactory(std::string extention,IFileFactory *factory)
    {
        registry[extention] = factory;
    }

    void FileRegistry::unRegisterFactory(IFileFactory *factory)
    {
        std::map<std::string,IFileFactory *>::iterator iter = registry.begin();
        while(iter!=registry.end())
        {
            if(iter->second==factory)
            {                
                iter->second = NULL;
            }
            iter++;
        }
    }

    sfa::File *FileRegistry::getFile(std::string filename)
    {
        ccl::FileInfo fi(filename);
        std::string ext = fi.getSuffix();
        if (registry.find(ext) != registry.end())
        {
            IFileFactory *fac = registry[ext];
            if (fac)
            {
                sfa::File *file = fac->create();
                ownerMap[file] = fac;
                return file;
            }
        }

        return NULL;
    }
    void FileRegistry::destroyFile(sfa::File *file)
    {
        if(ownerMap.find(file)!=ownerMap.end())
        {
            IFileFactory *fac=ownerMap[file];
            if(fac)
            {
                fac->destroy(file);
                ownerMap[file] = NULL;
            }
        }

    }


}