#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/Timer.h>
#include "scenegraphflt/scenegraphflt.h"
#include "ip/pngwrapper.h"

#include "ccl/StringUtils.h"
#include "ccl/ArgumentParser.h"
#include <scenegraph/ExtentsVisitor.h>
#include "sfa/BSP.h"
#include "sfa/sfa.h"
#include <sfa_file_factory/sfa_file_factory.h>
#include <ip/jpgwrapper.h>
#include <ip/rgb.h>
#include <cts/CS_CoordinateSystemFactory.h>
#include <sfa/Layer.h>
#include <sfa/File.h>
#include <ogr/ogr.h>
#include "ogr/File.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
//#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "cdb_tile/CoordinatesRange.h"
#include "cdb_tile/Tile.h"
#include <cstdlib>
#include <fstream>
#include <algorithm>

#pragma warning ( push )
#pragma warning ( disable : 4251 )        // C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include "gdal_priv.h"
#include "ogr_api.h"
#include "ogr_spatialref.h"
#pragma warning ( pop )

#include "obj-preprocess.h"
#include "quickobj.h"
ccl::ObjLog logger;


int main(int argc, char **argv)
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
			ccl::FileInfo fi(argv[0]);
			int bufSize = 1024;
			char *envBuffer = new char[bufSize];
			std::string dataDir = ccl::joinPaths(fi.getDirName(), "gdal-data");
			sprintf_s(envBuffer, bufSize, "GDAL_DATA=%s", dataDir.c_str());
			_putenv(envBuffer);
		}
	#endif
		logger.init("main");
		logger << ccl::LINFO;
		GDALAllRegister();
		ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
		
		logger << "test" << logger.endl;

	return EXIT_SUCCESS;
}
