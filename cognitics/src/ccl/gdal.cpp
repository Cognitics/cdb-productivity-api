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

#include <ccl/gdal.h>

#include <ccl/FileInfo.h>

#include <cstdlib>
#include <string>

namespace cognitics {
namespace gdal {

    void init(const char* argv0)
    {
#ifndef WIN32
        char *gdal_data_var = getenv("GDAL_DATA");
        if (gdal_data_var == NULL)
        {
            putenv("GDAL_DATA=/usr/local/share/gdal");
        }
        char *gdal_plugins_var = getenv("GDAL_DRIVER_PATH");
        if (gdal_plugins_var == NULL)
        {
            putenv("GDAL_DRIVER_PATH=/usr/local/bin/gdalplugins");
        }
#else
        size_t requiredSize;
        getenv_s(&requiredSize, NULL, 0, "GDAL_DATA");
        if (requiredSize == 0)
        {
            ccl::FileInfo fi(argv0);
            int bufSize = 1024;
            char *envBuffer = new char[bufSize];
            std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");
            sprintf_s(envBuffer, bufSize, "GDAL_DATA=%s", dataDir.c_str());
            _putenv(envBuffer);
            std::string driverDir = ccl::joinPaths(fi.getDirName(), "gdalplugins");
            char *pluginsEnvBuffer = new char[bufSize];
            sprintf_s(pluginsEnvBuffer, bufSize, "GDAL_DRIVER_PATH=%s", driverDir.c_str());
            _putenv(pluginsEnvBuffer);
        }
#endif
        GDALAllRegister();
    }


}
}


