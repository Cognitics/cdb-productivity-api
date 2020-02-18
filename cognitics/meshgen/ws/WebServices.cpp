#include "rapidjson/filereadstream.h"
#include "rapidjson/document.h"

#include "WebServices.h"
#include "ip/GDALRasterSampler.h"
#include <fstream>
#include <vector>
#include <iomanip>
#include <iterator>
#include <iostream>
#include <string>
#include "b64/base64.h"
#include <cctype>
#include "gdal_utils.h"
#include <string>
#include "curl/curl.h"
//#include <direct.h>
//#include <filesystem>
#include <ccl/FileInfo.h>
#include <elev/DataSourceManager.h>
#include <thread>
#include <atomic>
#include <scenegraphobj/scenegraphobj.h>
#include <scenegraph/FlattenVisitor.h>
#include <scenegraph/CoordinateTransformVisitor.h>
#include <scenegraph/TransformVisitor.h>
#include <ctl/ctl.h>
#include <mutex>
#include <ip/jpgwrapper.h>
#undef min
#undef max


#pragma warning(disable : 4996)
using namespace std;

namespace ws
{

    std::mutex fbx_mutex;

    struct GeoExtents
    {
        double north;
        double east;
        double south;
        double west;
    };

    struct TileInfo
    {
        GeoExtents extents;
        float centerX;
        float centerY;
        std::string elevationFileName;
        std::string imageFileName;
        int width;
        int height;
        std::string quadKey;
        GsBuildings GSFeatures;
    };

    struct memBuffer
    {
    public:
        memBuffer() : buffer((u_char*)malloc(1)), size(0) {}
        u_char * buffer;
        size_t size;
        void clear()
        {
            size = 0;
        }
    };

    size_t WriteMemory_Callback(void *contents, size_t size, size_t nmemb, void *userp)
    {
        size_t realsize = size * nmemb;
        struct memBuffer *mem = (struct memBuffer *)userp;

        mem->buffer = (u_char*)realloc(mem->buffer, mem->size + realsize + 1);
        if (mem->buffer == NULL)
        {
            // out of memory! 
            std::cout << "WebServices: not enough memory (realloc returned NULL) " << std::endl;
            return 0;
        }

        memcpy(&(mem->buffer[mem->size]), contents, realsize);
        mem->size += realsize;
        mem->buffer[mem->size] = 0;

        return realsize;
    }


    bool GetCurlResponse(CURL* curl)
    {
        bool succeeded = true;

        CURLcode result = curl_easy_perform(curl);

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);

        if (result != CURLE_OK)
        {
            std::cout << "WebServices: curl easy getinfo failed: " << curl_easy_strerror(result) << std::endl;
            succeeded = false;
        }

        if (http_code != 200)
        {
            std::cout << "WebServices: Error: Returned http code=" << http_code << std::endl;
            succeeded = false;
        }

        return succeeded;
    }

    void WriteLODfile(std::vector<TileInfo>& infos, std::string outputFilename, int nLODs)
    {
        std::ofstream lodFile;
        lodFile.open(outputFilename, std::ofstream::out);
        lodFile << nLODs << std::endl;

        cts::FlatEarthProjection flatEarth;
        for (auto& info : infos)
        {
            flatEarth.setOrigin(info.extents.south, info.extents.west);
            lodFile << "beginBuildingList\n";
            for (int i = 0; i < info.GSFeatures.Count(); ++i)
            {
                float lat = info.GSFeatures.GetBuilding(i).lat;
                float lon = info.GSFeatures.GetBuilding(i).lon;
                float elev = info.GSFeatures.GetBuilding(i).elev;
                double y = flatEarth.convertGeoToLocalY(lat);
                double x = flatEarth.convertGeoToLocalX(lon);
                double z = info.GSFeatures.GetBuilding(i).elev;
                double ao1 = info.GSFeatures.GetBuilding(i).AO1;
                double scalex = info.GSFeatures.GetBuilding(i).scaleX;
                double scaley = info.GSFeatures.GetBuilding(i).scaleY;
                double scalez = info.GSFeatures.GetBuilding(i).scaleZ;
                std::string modelpath = info.GSFeatures.GetBuilding(i).modelpath;
                lodFile << x << " " << y << " " << z << " " << ao1 << " " << modelpath << " " << scalex << " " << scaley << " " << scalez << "\n";
            }
            lodFile << "endBuildingList\n";
            lodFile << info.quadKey << " " << info.centerX << " " << info.centerY << std::endl;
        }
    }

    void ComputeCenterPosition(std::vector<TileInfo>& infos, double originLat, double originLon)
    {
        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin(originLat, originLon);
        for (auto& info : infos)
        {
            double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
            double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;
            info.centerX = flatEarth.convertGeoToLocalX(centerLon);
            info.centerY = flatEarth.convertGeoToLocalY(centerLat);
        }
    }

    void GenerateFileNames(std::vector<TileInfo>& infos, const std::string& outputTmpPath)
    {
        for (auto& info : infos)
        {
            double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
            double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;
            auto name = GetName(centerLon, centerLat, 0, 0, "obj", ".tif");
            info.elevationFileName = outputTmpPath + "/" + name;
            info.imageFileName = outputTmpPath + "/img/" + name;
        }
    }

	void GenerateFileNames(std::vector<TileInfo>& infos, const std::string& outputTmpPath, int height, int width, std::string format)
	{
		for (auto& info : infos)
		{
			double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
			double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;
			auto name = GetName(centerLon, centerLat, height, width, format, ".tif");
			info.elevationFileName = outputTmpPath + "/" + name;
			info.imageFileName = outputTmpPath + "/img/" + name;
		}
	}

    void GenerateLODBounds(std::vector<TileInfo>& infos, const TileInfo& info, int depth)
    {
		if (info.extents.north <= info.extents.south || info.extents.east <= info.extents.west)
		{
			return;
		}
        infos.push_back(info);

        if (depth == 0)
        {
            return;
        }

        double centerLat = (info.extents.north - info.extents.south) / 2 + info.extents.south;
        double centerLon = (info.extents.east - info.extents.west) / 2 + info.extents.west;
        TileInfo topLeft;
        topLeft.extents.north = info.extents.north;
        topLeft.extents.west = info.extents.west;
        topLeft.extents.east = centerLon;
        topLeft.extents.south = centerLat;
        topLeft.quadKey = info.quadKey + std::to_string(0);

        TileInfo topRight;
        topRight.extents.north = info.extents.north;
        topRight.extents.west = centerLon;
        topRight.extents.east = info.extents.east;
        topRight.extents.south = centerLat;
        topRight.quadKey = info.quadKey + std::to_string(1);

        TileInfo bottomLeft;
        bottomLeft.extents.north = centerLat;
        bottomLeft.extents.west = info.extents.west;
        bottomLeft.extents.east = centerLon;
        bottomLeft.extents.south = info.extents.south;
        bottomLeft.quadKey = info.quadKey + std::to_string(2);

        TileInfo bottomRight;
        bottomRight.extents.north = centerLat;
        bottomRight.extents.west = centerLon;
        bottomRight.extents.east = info.extents.east;
        bottomRight.extents.south = info.extents.south;
        bottomRight.quadKey = info.quadKey + std::to_string(3);

        GenerateLODBounds(infos, topLeft, depth - 1);
        GenerateLODBounds(infos, topRight, depth - 1);
        GenerateLODBounds(infos, bottomLeft, depth - 1);
        GenerateLODBounds(infos, bottomRight, depth - 1);
    }

    void GetData(const std::string& geoServerURL, int beginIndex, int endIndex, std::vector<TileInfo>* infos)
    {
		if (infos == NULL)
		{
			return;
		}
        for (int i = beginIndex; i < endIndex; i++)
        {
			if (i >= infos->size())
			{
				break;
			}
            auto& info = (*infos)[i];
            GetElevation(geoServerURL, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.elevationFileName);
            GetImagery(geoServerURL, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.imageFileName);
        }
    }

    void GenerateFixedGridParallel(cognitics::TerrainGenerator* terrainGenerator, int beginIndex, int endIndex, const std::string& outputFormat, const std::string& outputPath, elev::Elevation_DSM& edsm, std::vector<TileInfo>* infos)
    {
        for (int i = beginIndex; i < endIndex; i++)
        {
            auto& info = (*infos)[i];
            terrainGenerator->generateFixedGrid(info.imageFileName, outputPath, info.quadKey, outputFormat, edsm, info.extents.north, info.extents.south, info.extents.east, info.extents.west);
        }
    }

    void generateFixedGridWeb2(double north, double south, double west, double east, std::string format, const std::string& geoServerURL, const std::string& outputTmpPath, const std::string& outputPath, const std::string& outputFormat, cognitics::TerrainGenerator* terrainGenerator, int lodDepth, int textureHeight, int textureWidth)
    {

        double deltaX = east - west;
        double deltaY = north - south;

        double delta = min(deltaX, deltaY) / 2;

        double centerLat = (north - south) / 2 + south;
        double centerLon = (east - west) / 2 + west;

        east = centerLon + delta;
        west = centerLon - delta;
        north = centerLat + delta;
        south = centerLat - delta;

        centerLat = (north - south) / 2 + south;
        centerLon = (east - west) / 2 + west;

        std::vector<TileInfo> infos;
        TileInfo info;
        info.extents.east = east;
        info.extents.north = north;
        info.extents.south = south;
        info.extents.west = west;
        info.quadKey = std::to_string(0);
        GenerateLODBounds(infos, info, lodDepth);
        GenerateFileNames(infos, outputTmpPath, textureHeight, textureWidth, format);
        ComputeCenterPosition(infos, centerLat, centerLon);


        int nthreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(nthreads);
        int span = infos.size() / nthreads;
        int index = 0;
        for (int t = 0; t < nthreads; t++)
        {
            int begin = index;
            int end = index + span;
            if (t == nthreads - 1)
            {
                end = infos.size() - 1;
            }
            threads[t] = std::thread(GetData, geoServerURL, begin, end, &infos);
            index = end + 1;
        }

        std::for_each(threads.begin(), threads.end(), [](std::thread& x) {x.join(); });

        cout << "DONE" << endl;

        elev::DataSourceManager dsm(1000000);

        for (auto& info : infos)
        {
            //GetElevationSquare(geoServerURL, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.elevationFileName);
            //GetImagery(geoServerURL, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.imageFileName);
            //dsm.AddFile_Raster_GDAL(info.elevationFileName);
        }

        elev::Elevation_DSM edsm(&dsm, elev::elevation_strategy::ELEVATION_BILINEAR);

        for (auto& info : infos)
        {
            terrainGenerator->generateFixedGrid(info.imageFileName, outputPath, info.quadKey, outputFormat, edsm, info.extents.north, info.extents.south, info.extents.east, info.extents.west);
        }


        //sfa::Point p(0, 0);
        //for(double lat = south; lat < north; lat += 0.001)
        //    for (double lon = west; lon < east; lon += 0.001)
        //    {

        //        p.setX(lon);
        //        p.setY(lat);
        //        edsm.Get(&p);
        //        if (p.Z() < 0 || p.Z() > 0)
        //        {
        //            int itWorks = 4;
        //        }

        //    }
#ifdef CAE_MESH
        terrainGenerator->createFeatures(edsm);
#endif
        WriteLODfile(infos, outputPath + "/lodFile.txt", lodDepth);

        terrainGenerator->setBounds(north, south, east, west);
        terrainGenerator->CreateMasterFile();
    }


    void GetFeatureData(const std::string& geoServerURL, std::string tmpPath, double north, double south, double east, double west)
    {
        std::string dataPath = ccl::joinPaths(tmpPath, "data");
        ccl::makeDirectory(dataPath);
        CURL *curl;
        CURLcode res;

        curl_global_init(CURL_GLOBAL_ALL);

        curl = curl_easy_init();
        if (curl) {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

            string sUrl = geoServerURL;
            sUrl += "/wfs";

            curl_easy_setopt(curl, CURLOPT_URL, sUrl.c_str());
            //curl_easy_setopt(curl, CURLOPT_URL, "http://localhost/geoserver/wfs");

            struct curl_slist *hs = NULL;
            hs = curl_slist_append(hs, "Content-Type: text/xml;charset=utf-8");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, hs);

            //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "<wfs:GetFeature xmlns:wfs=\"http://www.opengis.net/wfs\" service=\"WFS\" version=\"1.0.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\"><wfs:Query typeName=\"CDB_Vectors:GT_TreePoints\"><ogc:Filter xmlns:ogc=\"http://www.opengis.net/ogc\"><ogc:BBOX><ogc:PropertyName>geom</ogc:PropertyName><gml:Box xmlns:gml=\"http://www.opengis.net/gml\" srsName=\"EPSG:4326\"><gml:coordinates decimal=\".\" cs=\",\" ts=\" \">45.0546875,12.75 45.0625,12.7578125</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter></wfs:Query></wfs:GetFeature>");
            //curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "<wfs:GetFeature xmlns:wfs=\"http://www.opengis.net/wfs\" service=\"WFS\" version=\"1.0.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\"><wfs:Query typeName=\"CDB_Vectors:GS_ManMadePoints_Footprints\"><ogc:Filter xmlns:ogc=\"http://www.opengis.net/ogc\"><ogc:BBOX><ogc:PropertyName>geom</ogc:PropertyName><gml:Box xmlns:gml=\"http://www.opengis.net/gml\" srsName=\"EPSG:4326\"><gml:coordinates decimal=\".\" cs=\",\" ts=\" \">45.0546875,12.75 45.0625,12.7578125</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter></wfs:Query></wfs:GetFeature>");

            //{//shouldn't need footprints, just export models
            //	
            //	std::stringstream footprintsXml;
            //	footprintsXml <<
            //		//footprints!
            //		//"<wfs:GetFeature xmlns:wfs=\"http://www.opengis.net/wfs\" service=\"WFS\" version=\"1.0.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\"><wfs:Query typeName=\"CDB_Vectors:GS_ManMadePoints_Footprints\"><ogc:Filter xmlns:ogc=\"http://www.opengis.net/ogc\"><ogc:BBOX><ogc:PropertyName>geom</ogc:PropertyName><gml:Box xmlns:gml=\"http://www.opengis.net/gml\" srsName=\"EPSG:4326\"><gml:coordinates decimal=\".\" cs=\",\" ts=\" \">45.046875,12.7578125 45.0546875,12.765625</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter></wfs:Query></wfs:GetFeature>";
            //		"<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
            //		"<wfs:GetFeature xmlns:wfs=\"http://www.opengis.net/wfs\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" service=\"WFS\" version=\"1.0.0\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\">"
            //		   "<wfs:Query typeName=\"CDB_Vectors:GS_ManMadePoints_Footprints\">"
            //			  "<ogc:Filter xmlns:ogc=\"http://www.opengis.net/ogc\">"
            //				 "<ogc:BBOX>"
            //					"<ogc:PropertyName>geom</ogc:PropertyName>"
            //					"<gml:Box xmlns:gml=\"http://www.opengis.net/gml\" srsName=\"EPSG:4326\">"
            //					   "<gml:coordinates decimal=\".\" cs=\",\" ts=\" \">"
            //						<< west << "," << south << " " << east << "," << north << //45.046875,12.7578125 45.0546875,12.765625
            //					   "</gml:coordinates>"
            //					"</gml:Box>"
            //				 "</ogc:BBOX>"
            //			  "</ogc:Filter>"
            //		   "</wfs:Query>"
            //		"</wfs:GetFeature>";
            //
            //	std::string footprintsStr = footprintsXml.str();
            //	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, footprintsStr.c_str());
            //
            //	memBuffer wfsResponseBuffer;
            //	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemory_Callback);
            //	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wfsResponseBuffer);
            //
            //	if (!GetCurlResponse(curl))
            //	{
            //		std::cout << "WebServices: Error: Failed getting footprints" << std::endl;
            //	}
            //	else
            //	{
            //		std::ofstream file(ccl::joinPaths(dataPath, "building_footprints.xml"));
            //		file.write((char*)wfsResponseBuffer.buffer, wfsResponseBuffer.size);
            //		file.close();
            //	}
            //}

            {
                std::stringstream treeXml;
                treeXml <<
                    //trees!
                    //"<wfs:GetFeature xmlns:wfs=\"http://www.opengis.net/wfs\" service=\"WFS\" version=\"1.0.0\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\"><wfs:Query typeName=\"CDB_Vectors:GT_TreePoints\"><ogc:Filter xmlns:ogc=\"http://www.opengis.net/ogc\"><ogc:BBOX><ogc:PropertyName>geom</ogc:PropertyName><gml:Box xmlns:gml=\"http://www.opengis.net/gml\" srsName=\"EPSG:4326\"><gml:coordinates decimal=\".\" cs=\",\" ts=\" \">45.015625,12.796875 45.0234375,12.8046875</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter></wfs:Query></wfs:GetFeature>";
                    "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
                    "<wfs:GetFeature xmlns:wfs=\"http://www.opengis.net/wfs\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" service=\"WFS\" version=\"1.0.0\" xsi:schemaLocation=\"http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd\">"
                    "<wfs:Query typeName=\"CDB_Vectors:GT_TreePoints\">"
                    "<ogc:Filter xmlns:ogc=\"http://www.opengis.net/ogc\">"
                    "<ogc:BBOX>"
                    "<ogc:PropertyName>geom</ogc:PropertyName>"
                    "<gml:Box xmlns:gml=\"http://www.opengis.net/gml\" srsName=\"EPSG:4326\">"
                    "<gml:coordinates decimal=\".\" cs=\",\" ts=\" \">"
                    << west << "," << south << " " << east << "," << north << //45.015625,12.796875 45.0234375,12.8046875
                    "</gml:coordinates>"
                    "</gml:Box>"
                    "</ogc:BBOX>"
                    "</ogc:Filter>"
                    "</wfs:Query>"
                    "</wfs:GetFeature>";

                std::string treeStr = treeXml.str();
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, treeStr.c_str());

                memBuffer wfsResponseBuffer;
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemory_Callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wfsResponseBuffer);

                if (!GetCurlResponse(curl))
                {
                    std::cout << "WebServices: Error: Failed getting trees" << std::endl;
                }
                else
                {
                    std::ofstream file(ccl::joinPaths(dataPath, "tree_points.xml"));
                    file.write((char*)wfsResponseBuffer.buffer, wfsResponseBuffer.size);
                    file.close();
                }
            }


            {
                std::stringstream modelsXml;
                modelsXml <<
                    //buildings / premade models
                    "<?xml version=\"1.0\" encoding=\"UTF-8\"?><wps:Execute version=\"1.0.0\" service=\"WPS\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns=\"http://www.opengis.net/wps/1.0.0\" xmlns:wfs=\"http://www.opengis.net/wfs\" xmlns:wps=\"http://www.opengis.net/wps/1.0.0\" xmlns:ows=\"http://www.opengis.net/ows/1.1\" xmlns:gml=\"http://www.opengis.net/gml\" xmlns:ogc=\"http://www.opengis.net/ogc\" xmlns:wcs=\"http://www.opengis.net/wcs/1.1.1\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xsi:schemaLocation=\"http://www.opengis.net/wps/1.0.0 http://schemas.opengis.net/wps/1.0.0/wpsAll.xsd\">"
                    "<ows:Identifier>gs:CdbExportModels</ows:Identifier>"
                    "<wps:DataInputs>"
                    "<wps:Input>"
                    "<ows:Identifier>envelope</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:BoundingBoxData dimensions=\"2\">"
                    "<ows:LowerCorner>" << west << " " << south << "</ows:LowerCorner>"
                    "<ows:UpperCorner>" << east << " " << north << "</ows:UpperCorner>"
                    "</wps:BoundingBoxData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>gsLOD</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>1000</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>gtLOD</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>1000</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>Texture Width</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>128</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>Texture Height</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>128</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>Geo-Typical</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>true</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>Geo-Specific</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>true</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "<wps:Input>"
                    "<ows:Identifier>ModelNamesOnly</ows:Identifier>"
                    "<wps:Data>"
                    "<wps:LiteralData>false</wps:LiteralData>"
                    "</wps:Data>"
                    "</wps:Input>"
                    "</wps:DataInputs>"
                    "<wps:ResponseForm>"
                    "<wps:RawDataOutput mimeType=\"application/json\">"
                    "<ows:Identifier>scene</ows:Identifier>"
                    "</wps:RawDataOutput>"
                    "</wps:ResponseForm>"
                    "</wps:Execute>";

                std::string modelsStr = modelsXml.str();
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, modelsStr.c_str());

                memBuffer wpsResponseBuffer;
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemory_Callback);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, &wpsResponseBuffer);

                if (!GetCurlResponse(curl))
                {
                    std::cout << "WebServices: Error: Failed getting models" << std::endl;
                }
                else
                {
                    std::ofstream file(ccl::joinPaths(dataPath, "building_models.xml"));
                    file.write((char*)wpsResponseBuffer.buffer, wpsResponseBuffer.size);
                    file.close();
                }
            }

            curl_easy_cleanup(curl);
        }
        curl_global_cleanup();

    }

    std::string GetName(double originLon, double originLat, double textureHeight, double textureWidth, std::string formatIn, std::string filetype)
    {
        std::string lat = "N";
        std::string lon = "E";
        std::string format = formatIn;
        double latValue = originLat;
        double lonValue = originLon;

        if (latValue < 0)
            lat = "S";
        if (lonValue < 0)
            lon = "W";

        std::string chars = ".";
        for (char c : chars)
        {
            format.erase(std::remove(format.begin(), format.end(), c), format.end());
        }

        for (std::string::size_type i = 0; i < format.length(); i++)
        {
            std::toupper(format[i]);
        }
        std::transform(format.begin(), format.end(), format.begin(), ::toupper);

        std::string name;
        name = lat + std::to_string(fabs(latValue)) + "_" + lon + std::to_string(fabs(lonValue)) + "_" + format + filetype;
        return name;
    }

    void CreateElevationTiff(const std::string& sFile, float* data, int textureWidth, int textureHeight)
    {
        const char* tiffFileFormat = "GTiff";
        GDALDriver* tiffDriver = GetGDALDriverManager()->GetDriverByName(tiffFileFormat);
        GDALDatasetH hDataset = GDALCreate(tiffDriver, sFile.c_str(), textureWidth, textureHeight, 1, GDT_Float32, NULL);
        GDALRasterBandH hBand = GDALGetRasterBand(hDataset, 1);

        float fMax = 0;
        for (int z = 0; z < textureHeight*textureWidth; z++)
        {
            if (data[z] > fMax)
            {
                fMax = data[z];
            }
        }

        float* buf = new float[textureWidth*textureHeight];
        for (int n = 0; n < textureWidth*textureHeight; ++n)
        {
            buf[n] = (data[n] / fMax);
        }

        GDALRasterIO(hBand, GF_Write, 0, 0, textureWidth, textureHeight, buf, textureWidth, textureHeight, GDT_Float32, 0, 0);
        GDALClose(hDataset);
    }

    void CreateElevationDeltaTiff(const std::string& sFile, float* data, int textureWidth, int textureHeight, float delta)
    {
        const char* tiffFileFormat = "GTiff";
        GDALDriver* tiffDriver = GetGDALDriverManager()->GetDriverByName(tiffFileFormat);
        GDALDatasetH hDataset = GDALCreate(tiffDriver, sFile.c_str(), textureWidth, textureHeight, 1, GDT_Float32, NULL);
        GDALRasterBandH hBand = GDALGetRasterBand(hDataset, 1);

        float* buf = new float[textureWidth*textureHeight];
        for (int n = 0; n < textureWidth*textureHeight; ++n)
        {
            buf[n] = 0;
        }

        for (int r = 0; r < textureHeight; ++r)
        {
            for (int c = 0; c < textureWidth - 1; ++c)
            {
                if (std::abs(data[r*textureWidth + c + 1] - data[r*textureWidth + c]) > delta)
                {
                    buf[r*textureWidth + c] = 1;
                }
            }
        }

        for (int r = 0; r < textureHeight - 1; ++r)
        {
            for (int c = 0; c < textureWidth; ++c)
            {
                if (std::abs(data[r*textureWidth + c] - data[(r + 1)*textureWidth + c]) > delta)
                {
                    buf[r*textureWidth + c] = 1;
                }
            }
        }

        GDALRasterIO(hBand, GF_Write, 0, 0, textureWidth, textureHeight, buf, textureWidth, textureHeight, GDT_Float32, 0, 0);
        GDALClose(hDataset);
    }


    void GetElevation(const std::string& geoServerURL, double north, double south, double east, double west, int& textureWidth, int& textureHeight, const std::string& outputPath)
    {
        GDALDataset* elevationDataset;
        char** papszDrivers = NULL;
        papszDrivers = CSLAddString(papszDrivers, "WCS");
        char** papszOptions = NULL;
        papszOptions = CSLAddString(papszOptions, "GridCRSOptional");

        std::stringstream parameters;

        parameters << std::fixed << std::setprecision(15)
            << "SERVICE=WCS&amp;FORMAT=GeoTIFF&amp;BOUNDINGBOX="
            << south
            << "," << west
            << "," << north
            << "," << east
            << ",urn:ogc:def:crs:EPSG::4326&amp;WIDTH="
            << std::to_string(textureWidth)
            << "&amp;HEIGHT="
            << std::to_string(textureHeight);

        std::string xml = "";
        xml += "<WCS_GDAL>";
        xml += "  <ServiceURL>" + geoServerURL + "/wcs?SERVICE=WCS</ServiceURL>";
        xml += "  <Version>1.1.1</Version>";
        xml += "  <CoverageName>CDB Elevation_Terrain_Primary</CoverageName>";
        xml += "  <Parameters>" + parameters.str() + "</Parameters>";
        xml += "  <GridCRSOptional>TRUE</GridCRSOptional>";
        xml += "  <CoverageDescription>";
        xml += "    <Identifier>Base:CDB Elevation_Terrain_Primary</Identifier>";
        xml += "    <Domain>";
        xml += "      <SpatialDomain>";
        xml += "        <BoundingBox crs=\"urn:ogc:def:crs:OGC:1.3 : CRS84\" dimensions=\"2\">";
        xml += "          <LowerCorner>-180.0 -90.0</LowerCorner>";
        xml += "          <UpperCorner>180.0 90.0</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <BoundingBox crs=\"urn:ogc:def:crs:EPSG::4326\" dimensions=\"2\">";
        xml += "          <LowerCorner>-90.0 -180.0</LowerCorner>";
        xml += "          <UpperCorner>90.0 180.0</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <BoundingBox crs=\":imageCRS\" dimensions=\"2\">";
        xml += "          <LowerCorner>0 0</LowerCorner>";
        xml += "          <UpperCorner>356356356 3563456</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <GridCRS>";
        xml += "          <GridBaseCRS>urn:ogc:def:crs:EPSG::4326</GridBaseCRS>";
        xml += "          <GridType>urn:ogc:def:method:WCS:1.1:2dGridIn2dCrs</GridType>";
        xml += "          <GridOrigin>-179.82421875 89.91259765625</GridOrigin>";
        xml += "          <GridOffsets>0.3515625 0.0 0.0 -0.1748046875</GridOffsets>";
        xml += "          <GridCS>urn:ogc:def:cs:OGC:0.0:Grid2dSquareCS</GridCS>";
        xml += "        </GridCRS>";
        xml += "      </SpatialDomain>";
        xml += "    </Domain>";
        xml += "    <SupportedCRS>urn:ogc:def:crs:EPSG::4326</SupportedCRS>";
        xml += "    <SupportedCRS>EPSG:4326</SupportedCRS>";
        xml += "    <SupportedFormat>image/tiff</SupportedFormat>";
        xml += "  </CoverageDescription>";
        xml += "  <FieldName>contents</FieldName>";
        xml += "  <BandType>Float32</BandType>";
        xml += "  <PreferredFormat>image/tiff</PreferredFormat>";
        xml += "</WCS_GDAL>";

        elevationDataset = (GDALDataset*)GDALOpenEx(xml.c_str(), GDAL_OF_READONLY, papszDrivers, papszOptions, NULL);

        auto band = elevationDataset->GetRasterBand(1);

        textureWidth = 2452;
        textureHeight = 2456;

        float *data = new float[textureWidth * textureHeight + 1];

        band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);

        std::string err = CPLGetLastErrorMsg();

        auto result = err.find("Got ");

        result += 4;

        err.erase(0, result);

        result = err.find('x');

        std::string firstnum = err.substr(0, result);

        err.erase(0, result + 1);

        result = err.find(' ');

        std::string secondnum = err.substr(0, result);

        textureWidth = std::stoi(firstnum);
        textureHeight = std::stoi(secondnum);

        band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);

        for (int z = 0; z < textureHeight*textureWidth; z++)
        {
            if (std::isnan(data[z]))
            {
                data[z] = 0.0f;
            }
        }

        //CreateElevationTiff("testTiff.tif", data, textureWidth, textureHeight);
        //CreateElevationDeltaTiff("testTiff_delta1.tif", data, textureWidth, textureHeight, 1);
        //CreateElevationDeltaTiff("testTiff_delta10.tif", data, textureWidth, textureHeight, 10);

        float* rotate90 = new float[textureWidth * textureHeight];

        for (int i = 0; i < textureWidth; ++i)
        {
            for (int j = 0; j < textureHeight; ++j)
            {
                rotate90[i * textureHeight + j] = data[j * textureWidth + i];
            }

        }


        auto tmp = textureWidth;
        textureWidth = textureHeight;
        textureHeight = tmp;

        float* flipped = new float[textureWidth * textureHeight];
        float* lastRowSrc = rotate90 + (textureWidth * (textureHeight - 1));
        float* firstRowDst = flipped;
        for (int i = 0; i < textureHeight; i++)
        {
            float* forward = firstRowDst;
            float* backward = lastRowSrc + textureWidth - 1;
            for (int j = 0; j < textureWidth; j++)
            {
                *forward++ = *backward--;
            }
            firstRowDst += textureWidth;
            lastRowSrc -= textureWidth;
        }

        GDALDriver* poDriver;
        poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
        GDALDataset *poDstDS;
        papszOptions[0] = NULL;
        papszOptions[1] = NULL;

        //std::string elevationName = SetName(originLon, originLat, textureHeight, textureWidth, outputPath, format, ".tif");
        std::string path = outputPath;
        poDstDS = poDriver->Create(path.c_str(), textureWidth, textureHeight, 1, GDT_Float32, papszOptions);

        double adfGeoTransform[6];
        adfGeoTransform[0] = west;
        adfGeoTransform[1] = (east - west) / textureWidth;
        adfGeoTransform[2] = 0;
        adfGeoTransform[3] = north;
        adfGeoTransform[4] = 0;
        adfGeoTransform[5] = ((north - south) / textureHeight) * -1;

        OGRSpatialReference oSRS;
        char *pszSRS_WKT = NULL;
        GDALRasterBand *poBand;

        poDstDS->SetGeoTransform(adfGeoTransform);

        oSRS.SetWellKnownGeogCS("WGS84");
        oSRS.exportToWkt(&pszSRS_WKT);
        poDstDS->SetProjection(pszSRS_WKT);
        CPLFree(pszSRS_WKT);
        poBand = poDstDS->GetRasterBand(1);

        poBand->RasterIO(GF_Write, 0, 0, textureWidth, textureHeight,
            flipped, textureWidth, textureHeight, GDT_Float32, 4, 0);
        GDALClose(elevationDataset);
        delete poDstDS;
        delete[] data;
        delete[] rotate90;
        delete[] flipped;
    }
    void GetElevation(const std::string& geoServerURL, double north, double south, double east, double west, int& textureWidth, int& textureHeight, float*& buffer, double* adfGeoTransform)
    {
        GDALDataset* elevationDataset;
        char** papszDrivers = NULL;
        papszDrivers = CSLAddString(papszDrivers, "WCS");
        char** papszOptions = NULL;
        papszOptions = CSLAddString(papszOptions, "GridCRSOptional");

        std::stringstream parameters;

        parameters << std::fixed << std::setprecision(15)
            << "SERVICE=WCS&amp;FORMAT=GeoTIFF&amp;BOUNDINGBOX="
            << south
            << "," << west
            << "," << north
            << "," << east
            << ",urn:ogc:def:crs:EPSG::4326&amp;WIDTH="
            << std::to_string(textureWidth)
            << "&amp;HEIGHT="
            << std::to_string(textureHeight);

        std::string xml = "";
        xml += "<WCS_GDAL>";
        xml += "  <ServiceURL>" + geoServerURL + "/wcs?SERVICE=WCS</ServiceURL>";
        xml += "  <Version>1.1.1</Version>";
        xml += "  <CoverageName>CDB Elevation_Terrain_Primary</CoverageName>";
        xml += "  <Parameters>" + parameters.str() + "</Parameters>";
        xml += "  <GridCRSOptional>TRUE</GridCRSOptional>";
        xml += "  <CoverageDescription>";
        xml += "    <Identifier>Base:CDB Elevation_Terrain_Primary</Identifier>";
        xml += "    <Domain>";
        xml += "      <SpatialDomain>";
        xml += "        <BoundingBox crs=\"urn:ogc:def:crs:OGC:1.3 : CRS84\" dimensions=\"2\">";
        xml += "          <LowerCorner>-180.0 -90.0</LowerCorner>";
        xml += "          <UpperCorner>180.0 90.0</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <BoundingBox crs=\"urn:ogc:def:crs:EPSG::4326\" dimensions=\"2\">";
        xml += "          <LowerCorner>-90.0 -180.0</LowerCorner>";
        xml += "          <UpperCorner>90.0 180.0</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <BoundingBox crs=\":imageCRS\" dimensions=\"2\">";
        xml += "          <LowerCorner>0 0</LowerCorner>";
        xml += "          <UpperCorner>356356356 3563456</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <GridCRS>";
        xml += "          <GridBaseCRS>urn:ogc:def:crs:EPSG::4326</GridBaseCRS>";
        xml += "          <GridType>urn:ogc:def:method:WCS:1.1:2dGridIn2dCrs</GridType>";
        xml += "          <GridOrigin>-179.82421875 89.91259765625</GridOrigin>";
        xml += "          <GridOffsets>0.3515625 0.0 0.0 -0.1748046875</GridOffsets>";
        xml += "          <GridCS>urn:ogc:def:cs:OGC:0.0:Grid2dSquareCS</GridCS>";
        xml += "        </GridCRS>";
        xml += "      </SpatialDomain>";
        xml += "    </Domain>";
        xml += "    <SupportedCRS>urn:ogc:def:crs:EPSG::4326</SupportedCRS>";
        xml += "    <SupportedCRS>EPSG:4326</SupportedCRS>";
        xml += "    <SupportedFormat>image/tiff</SupportedFormat>";
        xml += "  </CoverageDescription>";
        xml += "  <FieldName>contents</FieldName>";
        xml += "  <BandType>Float32</BandType>";
        xml += "  <PreferredFormat>image/tiff</PreferredFormat>";
        xml += "</WCS_GDAL>";

        elevationDataset = (GDALDataset*)GDALOpenEx(xml.c_str(), GDAL_OF_READONLY, papszDrivers, papszOptions, NULL);

        auto band = elevationDataset->GetRasterBand(1);

        textureWidth = 2452;
        textureHeight = 2456;

        float *data = new float[textureWidth * textureHeight + 1];

        band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);

        std::string err = CPLGetLastErrorMsg();

        auto result = err.find("Got ");

        result += 4;

        err.erase(0, result);

        result = err.find('x');

        std::string firstnum = err.substr(0, result);

        err.erase(0, result + 1);

        result = err.find(' ');

        std::string secondnum = err.substr(0, result);

        textureWidth = std::stoi(firstnum);
        textureHeight = std::stoi(secondnum);

        band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);

        for (int z = 0; z < textureHeight*textureWidth; z++)
        {
            if (std::isnan(data[z]))
            {
                data[z] = 0.0f;
            }
        }

        //CreateElevationTiff("testTiff.tif", data, textureWidth, textureHeight);
        //CreateElevationDeltaTiff("testTiff_delta1.tif", data, textureWidth, textureHeight, 1);
        //CreateElevationDeltaTiff("testTiff_delta10.tif", data, textureWidth, textureHeight, 10);

        float* rotate90 = new float[textureWidth * textureHeight];

        for (int i = 0; i < textureWidth; ++i)
        {
            for (int j = 0; j < textureHeight; ++j)
            {
                rotate90[i * textureHeight + j] = data[j * textureWidth + i];
            }

        }


        auto tmp = textureWidth;
        textureWidth = textureHeight;
        textureHeight = tmp;

        float* flipped = new float[textureWidth * textureHeight];
        float* lastRowSrc = rotate90 + (textureWidth * (textureHeight - 1));
        float* firstRowDst = flipped;
        for (int i = 0; i < textureHeight; i++)
        {
            float* forward = firstRowDst;
            float* backward = lastRowSrc + textureWidth - 1;
            for (int j = 0; j < textureWidth; j++)
            {
                *forward++ = *backward--;
            }
            firstRowDst += textureWidth;
            lastRowSrc -= textureWidth;
        }

        adfGeoTransform[0] = west;
        adfGeoTransform[1] = (east - west) / textureWidth;
        adfGeoTransform[2] = 0;
        adfGeoTransform[3] = north;
        adfGeoTransform[4] = 0;
        adfGeoTransform[5] = ((north - south) / textureHeight) * -1;

        buffer = flipped;
        GDALClose(elevationDataset);
        delete[] data;
        delete[] rotate90;
    }


    void GetElevationSquare(const std::string& geoServerURL, double north, double south, double east, double west, int& textureWidth, int& textureHeight, const std::string& outputPath)
    {
        GDALDataset* elevationDataset;
        char** papszDrivers = NULL;
        papszDrivers = CSLAddString(papszDrivers, "WCS");
        char** papszOptions = NULL;
        papszOptions = CSLAddString(papszOptions, "GridCRSOptional");

        std::stringstream parameters;

        parameters << std::fixed << std::setprecision(15)
            << "SERVICE=WCS&amp;FORMAT=GeoTIFF&amp;BOUNDINGBOX="
            << south
            << "," << west
            << "," << north
            << "," << east
            << ",urn:ogc:def:crs:EPSG::4326&amp;WIDTH="
            << std::to_string(textureWidth)
            << "&amp;HEIGHT="
            << std::to_string(textureHeight);

        std::string xml = "";
        xml += "<WCS_GDAL>";
        xml += "  <ServiceURL>" + geoServerURL + "/wcs?SERVICE=WCS</ServiceURL>";
        xml += "  <Version>1.1.1</Version>";
        xml += "  <CoverageName>CDB Elevation_Terrain_Primary</CoverageName>";
        xml += "  <Parameters>" + parameters.str() + "</Parameters>";
        xml += "  <GridCRSOptional>TRUE</GridCRSOptional>";
        xml += "  <CoverageDescription>";
        xml += "    <Identifier>Base:CDB Elevation_Terrain_Primary</Identifier>";
        xml += "    <Domain>";
        xml += "      <SpatialDomain>";
        xml += "        <BoundingBox crs=\"urn:ogc:def:crs:OGC:1.3 : CRS84\" dimensions=\"2\">";
        xml += "          <LowerCorner>-180.0 -90.0</LowerCorner>";
        xml += "          <UpperCorner>180.0 90.0</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <BoundingBox crs=\"urn:ogc:def:crs:EPSG::4326\" dimensions=\"2\">";
        xml += "          <LowerCorner>-90.0 -180.0</LowerCorner>";
        xml += "          <UpperCorner>90.0 180.0</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <BoundingBox crs=\":imageCRS\" dimensions=\"2\">";
        xml += "          <LowerCorner>0 0</LowerCorner>";
        xml += "          <UpperCorner>1024 1024</UpperCorner>";
        xml += "        </BoundingBox>";
        xml += "        <GridCRS>";
        xml += "          <GridBaseCRS>urn:ogc:def:crs:EPSG::4326</GridBaseCRS>";
        xml += "          <GridType>urn:ogc:def:method:WCS:1.1:2dGridIn2dCrs</GridType>";
        xml += "          <GridOrigin>-179.82421875 89.91259765625</GridOrigin>";
        xml += "          <GridOffsets>0.3515625 0.0 0.0 -0.1748046875</GridOffsets>";
        xml += "          <GridCS>urn:ogc:def:cs:OGC:0.0:Grid2dSquareCS</GridCS>";
        xml += "        </GridCRS>";
        xml += "      </SpatialDomain>";
        xml += "    </Domain>";
        xml += "    <SupportedCRS>urn:ogc:def:crs:EPSG::4326</SupportedCRS>";
        xml += "    <SupportedCRS>EPSG:4326</SupportedCRS>";
        xml += "    <SupportedFormat>image/tiff</SupportedFormat>";
        xml += "  </CoverageDescription>";
        xml += "  <FieldName>contents</FieldName>";
        xml += "  <BandType>Float32</BandType>";
        xml += "  <PreferredFormat>image/tiff</PreferredFormat>";
        xml += "</WCS_GDAL>";

        elevationDataset = (GDALDataset*)GDALOpenEx(xml.c_str(), GDAL_OF_READONLY, papszDrivers, papszOptions, NULL);

        auto band = elevationDataset->GetRasterBand(1);

        textureWidth = 1024;
        textureHeight = 1024;

        float *data = new float[textureWidth * textureHeight + 1];

        band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_Float32, 0, 0);

        for (int z = 0; z < textureHeight*textureWidth; z++)
        {
            if (std::isnan(data[z]))
            {
                data[z] = 0.0f;
            }
        }

        float* rotate90 = new float[textureWidth * textureHeight];

        for (int i = 0; i < textureWidth; ++i)
        {
            for (int j = 0; j < textureHeight; ++j)
            {
                rotate90[i * textureHeight + j] = data[j * textureWidth + i];
            }

        }


        auto tmp = textureWidth;
        textureWidth = textureHeight;
        textureHeight = tmp;

        float* flipped = new float[textureWidth * textureHeight];
        float* lastRowSrc = rotate90 + (textureWidth * (textureHeight - 1));
        float* firstRowDst = flipped;
        for (int i = 0; i < textureHeight; i++)
        {
            float* forward = firstRowDst;
            float* backward = lastRowSrc + textureWidth - 1;
            for (int j = 0; j < textureWidth; j++)
            {
                *forward++ = *backward--;
            }
            firstRowDst += textureWidth;
            lastRowSrc -= textureWidth;
        }

        GDALDriver* poDriver;
        poDriver = GetGDALDriverManager()->GetDriverByName("GTiff");
        GDALDataset *poDstDS;
        papszOptions[0] = NULL;
        papszOptions[1] = NULL;

        //std::string elevationName = SetName(originLon, originLat, textureHeight, textureWidth, outputPath, format, ".tif");
        std::string path = outputPath;
        poDstDS = poDriver->Create(path.c_str(), textureWidth, textureHeight, 1, GDT_Float32, papszOptions);

        double adfGeoTransform[6];
        adfGeoTransform[0] = west;
        adfGeoTransform[1] = (east - west) / textureWidth;
        adfGeoTransform[2] = 0;
        adfGeoTransform[3] = north;
        adfGeoTransform[4] = 0;
        adfGeoTransform[5] = ((north - south) / textureHeight) * -1;

        OGRSpatialReference oSRS;
        char *pszSRS_WKT = NULL;
        GDALRasterBand *poBand;

        poDstDS->SetGeoTransform(adfGeoTransform);

        oSRS.SetWellKnownGeogCS("WGS84");
        oSRS.exportToWkt(&pszSRS_WKT);
        poDstDS->SetProjection(pszSRS_WKT);
        CPLFree(pszSRS_WKT);
        poBand = poDstDS->GetRasterBand(1);

        poBand->RasterIO(GF_Write, 0, 0, textureWidth, textureHeight,
            flipped, textureWidth, textureHeight, GDT_Float32, 4, 0);
        GDALClose(elevationDataset);
        delete poDstDS;
        delete[] data;
        delete[] rotate90;
        delete[] flipped;
    }


    void GetImagery(const std::string& geoServerURL, double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath)
    {
        bool gdalError = false;
        /// TIFF ///
        std::ostringstream xmlTiff;
        xmlTiff.precision(10);
        xmlTiff << std::fixed
            << "<GDAL_WMS>"
            << "<Service name=\"WMS\">"
            << "<ServerUrl>" + geoServerURL + "/wms?</ServerUrl>"
            << "<Layers>Base%3ACDB%20Imagery_YearRound</Layers>"
            << "<ImageFormat>image/geotiff</ImageFormat>"
            << "</Service>"
            << "<DataWindow>"
            << "<UpperLeftX>"
            << west
            << "</UpperLeftX>"
            << "<UpperLeftY>"
            << north
            << "</UpperLeftY>"
            << "<LowerRightX>"
            << east
            << "</LowerRightX>"
            << "<LowerRightY>"
            << south
            << "</LowerRightY>"
            << "<SizeX>"
            << textureWidth
            << "</SizeX>"
            << "<SizeY>"
            << textureHeight
            << "</SizeY>"
            << "</DataWindow>"
            << "</GDAL_WMS>";

        GDALDataset *datasetTiff = (GDALDataset*)GDALOpen(xmlTiff.str().c_str(), GA_ReadOnly);
        if (datasetTiff == NULL)
        {
            std::cout << "Not getting imagery from xmlTiff" << std::endl;
            gdalError = true;
        }

        const char* tiffFileFormat = "GTiff";
        GDALDriver* tiffDriver = GetGDALDriverManager()->GetDriverByName(tiffFileFormat);
        if (tiffDriver == NULL)
        {
            std::cout << "GTiff driver failed" << std::endl;
            gdalError = true;
        }

        //std::string tiffName = SetName(originLon, originLat, textureHeight, textureWidth, outputPath, format, ".tif");
        std::string tiffOutputFile = outputPath;// +"img/" + tiffName;
        GDALDataset* ImageDatasetTiff = tiffDriver->CreateCopy(tiffOutputFile.c_str(), datasetTiff, FALSE, NULL, NULL, NULL);
        if (ImageDatasetTiff == NULL)
        {
            std::cout << "GTiff dataset failed" << std::endl;
            gdalError = true;
        }

        // cleanup GDAL
        GDALClose((GDALDatasetH)datasetTiff);
        GDALClose((GDALDatasetH)ImageDatasetTiff);
    }


    void GetImageryJPG(const std::string& geoServerURL, double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath)
    {
        bool gdalError = false;
        /// TIFF ///
        std::ostringstream xmlTiff;
        xmlTiff.precision(10);
        xmlTiff << std::fixed
            << "<GDAL_WMS>"
            << "<Service name=\"WMS\">"
            << "<ServerUrl>" + geoServerURL + "/wms?</ServerUrl>"
            << "<Layers>Base%3ACDB%20Imagery_YearRound</Layers>"
            << "<ImageFormat>image/geotiff</ImageFormat>"
            << "</Service>"
            << "<DataWindow>"
            << "<UpperLeftX>"
            << west
            << "</UpperLeftX>"
            << "<UpperLeftY>"
            << north
            << "</UpperLeftY>"
            << "<LowerRightX>"
            << east
            << "</LowerRightX>"
            << "<LowerRightY>"
            << south
            << "</LowerRightY>"
            << "<SizeX>"
            << textureWidth
            << "</SizeX>"
            << "<SizeY>"
            << textureHeight
            << "</SizeY>"
            << "</DataWindow>"
            << "</GDAL_WMS>";

        GDALDataset *datasetTiff = (GDALDataset*)GDALOpen(xmlTiff.str().c_str(), GA_ReadOnly);
        if (datasetTiff == NULL)
        {
            std::cout << "Not getting imagery from xmlTiff" << std::endl;
            gdalError = true;
        }

        int len = textureHeight * textureWidth * 3;
        u_char *buf = new u_char[len];
        for (int i = 0; i < 3; i++)
        {
            auto band = datasetTiff->GetRasterBand(i + 1);
            
            band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, buf + i, textureWidth, textureHeight, GDALDataType::GDT_Byte, 3, 3 * textureWidth);
        }
        ip::ImageInfo info;
        info.width = textureWidth;
        info.height = textureHeight;
        info.depth = 3;
        info.interleaved = true;
        info.dataType = ip::ImageInfo::UBYTE;
        ip::WriteJPG24(outputPath, info, buf);
        GDALClose((GDALDatasetH)datasetTiff);
        delete[] buf;
        return;
    }

    void GetElevationParallel(std::vector<TileInfo>* infos, const std::string& geoServerIpAddress)
    {
        static atomic_int counter;
        while (true)
        {
            int index = counter++;
            if (index >= infos->size())
            {
                return;
            }
            auto& info = infos->at(index);
            ws::GetElevation(geoServerIpAddress, info.extents.north, info.extents.south, info.extents.east, info.extents.west, info.width, info.height, info.elevationFileName);
        }
    }

    inline double GetElev(elev::Elevation_DSM& edsm, double lat, double lon)
    {
        sfa::Point p;
        p.setY(lat);
        p.setX(lon);
        edsm.Get(&p);
        return p.Z();
    }

    inline float GetElev(const float* buffer, const double* geotransform, int width, double lat, double lon)
    {
        int px = (lon - geotransform[0]) / geotransform[1] - 0.5f;
        int py = (lat - geotransform[3]) / geotransform[5] - 0.5f;
        return buffer[width * py + px];
    }


    int LatLongToZoneNumber(double latitude, double longitude)
    {
        if ((56.0 <= latitude && latitude <= 64.0) && (3.0 <= longitude && longitude <= 12.0))
        {
            return 32;
        }

        if ((72.0 <= latitude && latitude <= 84.0) && (longitude >= 0.0))
        {
            if (longitude <= 9.0)
            {
                return 31;
            }
            else if (longitude <= 21.0)
            {
                return 33;
            }
            else if (longitude <= 33.0)
            {
                return 35;
            }
            else if (longitude <= 42.0)
            {
                return 37;
            }
        }

        return (((int)(longitude + 180)) / 6) + 1;
    }

    double ZoneNumberToCentralLongitude(int zoneNumber)
    {
        return (zoneNumber - 1) * 6 - 180 + 3;
    }

    char LatitudeToZoneLetter(double Lat)
    {
        char LetterDesignator;

        if ((84 >= Lat) && (Lat >= 72)) LetterDesignator = 'X';
        else if ((72 > Lat) && (Lat >= 64)) LetterDesignator = 'W';
        else if ((64 > Lat) && (Lat >= 56)) LetterDesignator = 'V';
        else if ((56 > Lat) && (Lat >= 48)) LetterDesignator = 'U';
        else if ((48 > Lat) && (Lat >= 40)) LetterDesignator = 'T';
        else if ((40 > Lat) && (Lat >= 32)) LetterDesignator = 'S';
        else if ((32 > Lat) && (Lat >= 24)) LetterDesignator = 'R';
        else if ((24 > Lat) && (Lat >= 16)) LetterDesignator = 'Q';
        else if ((16 > Lat) && (Lat >= 8)) LetterDesignator = 'P';
        else if ((8 > Lat) && (Lat >= 0)) LetterDesignator = 'N';
        else if ((0 > Lat) && (Lat >= -8)) LetterDesignator = 'M';
        else if ((-8 > Lat) && (Lat >= -16)) LetterDesignator = 'L';
        else if ((-16 > Lat) && (Lat >= -24)) LetterDesignator = 'K';
        else if ((-24 > Lat) && (Lat >= -32)) LetterDesignator = 'J';
        else if ((-32 > Lat) && (Lat >= -40)) LetterDesignator = 'H';
        else if ((-40 > Lat) && (Lat >= -48)) LetterDesignator = 'G';
        else if ((-48 > Lat) && (Lat >= -56)) LetterDesignator = 'F';
        else if ((-56 > Lat) && (Lat >= -64)) LetterDesignator = 'E';
        else if ((-64 > Lat) && (Lat >= -72)) LetterDesignator = 'D';
        else if ((-72 > Lat) && (Lat >= -80)) LetterDesignator = 'C';
        else LetterDesignator = 'Z'; //Latitude is outside the UTM limits
        return LetterDesignator;
    }

    std::string FromLatLon(double latitude, double longitude, double& easting, double& northing)
    {
        double E, E2, E3, E_P2;
        double SQRT_E, _E, _E3, _E4;
        double M1, M2, M3, M4;
        double P2, P3, P4;
        double R;

        double K0 = 0.9996;

        E = 0.00669438;
        E2 = E * E;
        E3 = E2 * E;
        E_P2 = E / (1.0 - E);

        SQRT_E = sqrt(1.0 - E);
        _E = (1.0 - SQRT_E) / (1.0 + SQRT_E);
        _E3 = _E * _E * _E;
        _E4 = _E3 * _E;

        M1 = (1.0 - E / 4.0 - 3.0 * E2 / 64.0 - 5.0 * E3 / 256.0);
        M2 = (3.0 * E / 8.0 + 3.0 * E2 / 32.0 + 45.0 * E3 / 1024.0);
        M3 = (15.0 * E2 / 256.0 + 45.0 * E3 / 1024.0);
        M4 = (35.0 * E3 / 3072.0);

        P2 = (3.0 * _E / 2.0 - 27.0 * _E3 / 32.0);
        P3 = (21.0 * _E3 / 16.0 - 55.0 * _E4 / 32.0);
        P4 = (151.0 * _E3 / 96.0);

        R = 6378137.0;

        double lat_rad = DEG_TO_RAD * latitude;
        double lat_sin = sin(lat_rad);
        double lat_cos = cos(lat_rad);

        double lat_tan = lat_sin / lat_cos;
        double lat_tan2 = lat_tan * lat_tan;
        double lat_tan4 = lat_tan2 * lat_tan2;

        double lon_rad = DEG_TO_RAD * longitude;

        int zone_number = LatLongToZoneNumber(latitude, longitude);
        double central_lon = ZoneNumberToCentralLongitude(zone_number);
        double central_lon_rad = DEG_TO_RAD * central_lon;

        char zone_letter = LatitudeToZoneLetter(latitude);

        double n = R / sqrt(1 - E * pow(lat_sin, 2));
        double c = E_P2 * pow(lat_cos, 2);

        double a = lat_cos * (lon_rad - central_lon_rad);
        double a2 = a * a;
        double a3 = a2 * a;
        double a4 = a3 * a;
        double a5 = a4 * a;
        double a6 = a5 * a;

        double m = R * (M1 * lat_rad -
            M2 * sin(2 * lat_rad) +
            M3 * sin(4 * lat_rad) -
            M4 * sin(6 * lat_rad));

        easting = K0 * n * (a +
            a3 / 6 * (1 - lat_tan2 + c) +
            a5 / 120 * (5 - 18 * lat_tan2 + lat_tan4 + 72 * c - 58 * E_P2)) + 500000;

        northing = K0 * (m + n * lat_tan * (a2 / 2 +
            a4 / 24 * (5 - lat_tan2 + 9 * c + 4 * pow(c, 2)) +
            a6 / 720 * (61 - 58 * lat_tan2 + lat_tan4 + 600 * c - 330 * E_P2)));

        if (latitude < 0)
        {
            northing += 10000000;
        }

        char hemisphere;
        if (latitude >= 0)
        {
            hemisphere = 'N';
        }
        else
        {
            hemisphere = 'S';
        }

        return to_string(zone_number) + hemisphere;
    }

    std::string GetWellKnownTextUTM(double originLat, double originLon)
    {
        double easting, northing;
        auto utm_zone = FromLatLon(originLat, originLon, easting, northing);
        bool zone_north = utm_zone[utm_zone.size() - 1] == 'N';
        int zone_id = atoi(utm_zone.substr(0, utm_zone.size() - 1).c_str());
        if (zone_id <= 0)
            throw std::runtime_error("invalid zone");
        OGRSpatialReference utm_sr;
        std::string name = "WGS 84 / UTM zone " + utm_zone;
        utm_sr.SetProjCS(name.c_str());
        utm_sr.SetWellKnownGeogCS("WGS84");
        utm_sr.SetUTM(zone_id, zone_north);
        char *wktp;
        utm_sr.exportToWkt(&wktp);
        return wktp;
    }

    void transformSceneFromFlatEarthToUTM(scenegraph::Scene* scene, double originLat, double originLon)
    {
        double easting, northing;
        auto utm_zone = FromLatLon(originLat, originLon, easting, northing);
        bool zone_north = utm_zone[utm_zone.size() - 1] == 'N';
        int zone_id = atoi(utm_zone.substr(0, utm_zone.size() - 1).c_str());
        if (zone_id <= 0)
            throw std::runtime_error("invalid zone");

        cts::CS_CoordinateSystemFactory coordinateSystemFactory;
        cts::CT_MathTransformFactory mathTransformFactory;
        cts::CS_CoordinateSystem* wgs84 = coordinateSystemFactory.createFromWKT("WGS84");
        cts::WGS84FromFlatEarthMathTransform* fromFlatEarth = new cts::WGS84FromFlatEarthMathTransform(originLat, originLon);

        OGRSpatialReference utm_sr;
        utm_sr.SetWellKnownGeogCS("WGS84");
        utm_sr.SetUTM(zone_id, zone_north);
        char *wktp;
        utm_sr.exportToWkt(&wktp);
        std::string wkt(wktp);
        cts::CS_CoordinateSystem *utm = coordinateSystemFactory.createFromWKT(wkt);

        // transform from flat earth to geographic
        scenegraph::CoordinateTransformVisitor transformVisitor1(fromFlatEarth);
        transformVisitor1.transform(scene);

        // transform from geographic to UTM
        cts::CT_MathTransform *mt = mathTransformFactory.createFromOGR(wgs84, utm);
        scenegraph::CoordinateTransformVisitor transformVisitor2(mt);
        transformVisitor2.transform(scene);

        // shift scene back:
        sfa::Matrix transform;
        transform.PushTranslate(sfa::Point(-easting, -northing));
        scenegraph::TransformVisitor transformVisitor3(transform);
        auto translatedScene = transformVisitor3.transform(scene);
        delete mt;
        delete fromFlatEarth;
        delete utm;
    }


    void GDAL_Parallel(double north, double south, double west, double east, const std::string& geoServerIPAddress, const std::string& outputPath, const std::string& outputTmpPath, const std::string& outputFormat, scenegraph::Scene* masterScene, std::mutex* masterSceneMutex)
    {
        const int MAX_TILE_SIZE_METERS = 256;
        const int IMAGE_WIDTH = 1024;

        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin(south, west);
        GsBuildings buildings;
        std::string dataPath = ccl::joinPaths(outputTmpPath, "data");
        buildings.ProcessBuildingData(ccl::joinPaths(dataPath, "building_models.xml"));
        double width = flatEarth.convertGeoToLocalX(east);
        double height = flatEarth.convertGeoToLocalY(north);

        int rows = ceil(height / MAX_TILE_SIZE_METERS);
        int cols = ceil(width / MAX_TILE_SIZE_METERS);

        int newWidth = MAX_TILE_SIZE_METERS * cols;
        int newHeight = MAX_TILE_SIZE_METERS * rows;

        east = flatEarth.convertLocalToGeoLon(newWidth);
        north = flatEarth.convertLocalToGeoLat(newHeight);

        double zeroX = 0, zeroY = 0, oneX = 1, oneY = 1;
        double zeroLon, zeroLat, oneLon, oneLat;

        zeroLon = flatEarth.convertLocalToGeoLon(zeroX);
        zeroLat = flatEarth.convertLocalToGeoLat(zeroY);
        oneLon = flatEarth.convertLocalToGeoLon(oneX);
        oneLat = flatEarth.convertLocalToGeoLat(oneY);

        double deltaLatPerMeter = oneLat - zeroLat;
        double deltaLonPerMeter = oneLon - zeroLon;

        static atomic_int counter;
        while (true)
        {
            int index = counter++;
            if (index >= rows * cols)
            {
                return;
            }
            std::string status = "exporting tile " + to_string(index + 1) + " of " + to_string(rows*cols) + "\n";
            cout << status;
            int row = index / cols;
            int col = index % cols;

            int localSouth = row * MAX_TILE_SIZE_METERS;
            int localNorth = localSouth + MAX_TILE_SIZE_METERS;
            int localWest = col * MAX_TILE_SIZE_METERS;
            int localEast = localWest + MAX_TILE_SIZE_METERS;

            auto geoSouth = flatEarth.convertLocalToGeoLat(localSouth);
            auto geoNorth = flatEarth.convertLocalToGeoLat(localNorth);
            auto geoEast = flatEarth.convertLocalToGeoLon(localEast);
            auto geoWest = flatEarth.convertLocalToGeoLon(localWest);
            int nXsize;
            int nYsize;
            double geotransform[6];
            float* buffer;
            ws::GetElevation(geoServerIPAddress, geoNorth + deltaLatPerMeter, geoSouth - deltaLatPerMeter, geoEast + deltaLonPerMeter, geoWest - deltaLonPerMeter, nXsize, nYsize, buffer, geotransform);

            ctl::PointList gamingArea;
            ctl::Point southwest(localWest, localSouth, GetElev(buffer, geotransform, nXsize, geoSouth, geoWest));
            ctl::Point southeast(localEast, localSouth, GetElev(buffer, geotransform, nXsize, geoSouth, geoEast));
            ctl::Point northeast(localEast, localNorth, GetElev(buffer, geotransform, nXsize, geoNorth, geoEast));
            ctl::Point northwest(localWest, localNorth, GetElev(buffer, geotransform, nXsize, geoNorth, geoWest));
            std::string imageName = "tile_" + to_string(row) + "x" + to_string(col) + ".jpg";
            ws::GetImageryJPG(geoServerIPAddress, geoNorth, geoSouth, geoEast, geoWest, IMAGE_WIDTH, IMAGE_WIDTH, outputPath + imageName);
            //scenegraph::CreateMetaForObj(outputPath + imageName);
            gamingArea.push_back(southwest);
            gamingArea.push_back(southeast);
            gamingArea.push_back(northeast);
            gamingArea.push_back(northwest);

            sfa::LineString boundaryLineString;
            // left boundary
            double lon = geoWest;
            double lat = geoNorth;
            double localPostX = localWest;
            double localPostY = localNorth;
            for (int i = 0; i <= MAX_TILE_SIZE_METERS; i++, localPostY -= 1, lat -= deltaLatPerMeter)
            {
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, GetElev(buffer, geotransform, nXsize, lat, lon)));
            }

            // right boundary
            lon = geoEast;
            lat = geoNorth;
            localPostX = localEast;
            localPostY = localNorth;
            for (int i = 0; i <= MAX_TILE_SIZE_METERS; i++, localPostY -= 1, lat -= deltaLatPerMeter)
            {
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, GetElev(buffer, geotransform, nXsize, lat, lon)));
            }

            // bottom boundary
            lat = geoSouth;
            lon = geoWest;
            localPostY = localSouth;
            localPostX = localWest;
            for (int i = 0; i <= MAX_TILE_SIZE_METERS; i++, localPostX += 1, lon += deltaLonPerMeter)
            {
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, GetElev(buffer, geotransform, nXsize, lat, lon)));
            }

            // top boundary
            lat = geoNorth;
            lon = geoWest;
            localPostY = localNorth;
            localPostX = localWest;
            for (int i = 0; i <= MAX_TILE_SIZE_METERS; i++, localPostX += 1, lon += deltaLonPerMeter)
            {
                boundaryLineString.addPoint(sfa::Point(localPostX, localPostY, GetElev(buffer, geotransform, nXsize, lat, lon)));
            }

            boundaryLineString.removeColinearPoints(0, 0.5);
            ctl::PointList boundaryPoints;
            for (int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
            {
                sfa::Point *p = boundaryLineString.getPointN(i);
                boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
            }

            ctl::PointList workingPoints;
            localPostY = localSouth;
            lat = geoSouth;
            for (int i = 0; i <= MAX_TILE_SIZE_METERS; i++, localPostY += 1, lat += deltaLatPerMeter)
            {
                localPostX = localWest;
                lon = geoWest;
                for (int j = 0; j <= MAX_TILE_SIZE_METERS; j++, localPostX += 1, lon += deltaLonPerMeter)
                {
                    workingPoints.push_back(ctl::Point(localPostX, localPostY, GetElev(buffer, geotransform, nXsize, lat, lon)));
                }
            }

            int delaunayResizeIncrement = (MAX_TILE_SIZE_METERS * MAX_TILE_SIZE_METERS) / 8;
            ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(gamingArea, delaunayResizeIncrement);
            std::random_shuffle(workingPoints.begin(), workingPoints.end());
            std::random_shuffle(boundaryPoints.begin(), boundaryPoints.end());
            {
                //Alternate inserting boundary and working points to avoid worst case performance of DelaunayTriangulation
                size_t i = 0;
                size_t j = 0;
                while (i < boundaryPoints.size() || j < workingPoints.size())
                {
                    if (i < boundaryPoints.size())
                        dt->InsertConstrainedPoint(boundaryPoints[i++]);
                    if (j < workingPoints.size())
                        dt->InsertWorkingPoint(workingPoints[j++]);
                }
            }

            dt->Simplify(1, float(0.1));
            ctl::TIN *tin = new ctl::TIN(dt);
            scenegraph::Scene *scene = new scenegraph::Scene;
            scene->faces.reserve(tin->triangles.size() / 3);
            for (size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
            {
                scenegraph::Face face;
                scenegraph::MappedTexture mt;
                mt.SetTextureName(imageName);
                for (int z = 0; z < 3; z++)
                {
                    auto index = tin->triangles[i * 3 + z];
                    auto& p = tin->verts[index];
                    auto& n = tin->normals[index];
                    float u = (p.x - localWest) / MAX_TILE_SIZE_METERS;
                    float v = (p.y - localSouth) / MAX_TILE_SIZE_METERS;
                    mt.uvs.push_back(sfa::Point(u, v));
                    face.verts.push_back(sfa::Point(p.x, p.y, p.z));
                    face.vertexNormals.push_back(sfa::Point(n.x, n.y, n.z));
                }
                face.textures.push_back(mt);
                scene->faces.push_back(face);
            }


            for (int z = 0; z < buildings.Count(); z++)
            {
                FeatureInfo& building = buildings.GetBuilding(z);
                if (!(building.lat < geoSouth || building.lat > geoNorth || building.lon < geoWest || building.lon > geoEast))
                {
                    auto buildingScene = new scenegraph::Scene;
                    building.elev = GetElev(buffer, geotransform, nXsize, building.lat, building.lon);
                    ParseJSON(building.modelpath, outputPath, *buildingScene);
                    sfa::Matrix m;
                    auto radians = (building.AO1 * M_PI) / 180;
                    m.PushRotate(sfa::Point(0, 0, 1), -radians);
                    m.PushScale(sfa::Point(1, 1, 1));
                    auto localX = flatEarth.convertGeoToLocalX(building.lon);
                    auto localY = flatEarth.convertGeoToLocalY(building.lat);
                    m.PushTranslate(sfa::Point(localX, localY, building.elev));
                    buildingScene->matrix = m;
                    scene->addChild(buildingScene);
                }
            }

            scenegraph::FlattenVisitor flatten;
            auto flattenedScene = flatten.flatten(scene);
            transformSceneFromFlatEarthToUTM(flattenedScene, south, west);
            masterSceneMutex->lock();
            masterScene->addChild(flattenedScene);
            masterSceneMutex->unlock();
            //scenegraph::buildObjFromScene(outputDir + "/" + tileName + outputFormat, 0, 0, 0, 0, flattenedScene);
            

            //fbx_mutex.lock();
            //scenegraph::buildFbxWithFeatures(outputPath + tileName + ".fbx", scene, buildings, &terrainGenerator, geoNorth, geoSouth, geoEast, geoWest, imageName, true);
            //fbx_mutex.unlock();
            delete scene;
            //delete flattenedScene;
            delete[] buffer;
            delete tin;
            delete dt;
        }
    }

    void generateFixedGridSofprep(double north, double south, double west, double east, const std::string& geoServerIPAddress, const std::string& outputTmpPath, const std::string& outputPath, const std::string& outputFormat)
    {
        int nthreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(nthreads);
        scenegraph::Scene masterScene;
        std::mutex m;
        for (int t = 0; t < nthreads; t++)
        {
            threads[t] = std::thread(GDAL_Parallel, north, south, west, east, geoServerIPAddress, outputPath, outputTmpPath, outputFormat, &masterScene, &m);
        }
        std::for_each(threads.begin(), threads.end(), [](std::thread& x) {x.join(); });
        scenegraph::FlattenVisitor flatten;
        auto flattenedScene = flatten.flatten(&masterScene);
        scenegraph::buildObjFromScene(outputPath + "/" + "simplified_3d_mesh" + outputFormat, 0, 0, 0, 0, flattenedScene);

        double easting, northing;
        FromLatLon(south, west, easting, northing);
        std::ofstream offsetFile;
        offsetFile.open(outputPath + "offset.xyz", std::ofstream::out);
        offsetFile << std::setprecision(10);
        offsetFile << easting << " " << northing << " 0";
        offsetFile.close();

        std::ofstream wktFile;
        wktFile.open(outputPath + "wkt.prj", std::ofstream::out);
        auto wkt = GetWellKnownTextUTM(south, west);
        wktFile << wkt;
        wktFile.close();
    }

	void generateCesiumLods(double north, double south, double west, double east, 
		const std::string& geoServerURL, const std::string& outputTmpPath, const std::string& outputPath, 
		cognitics::TerrainGenerator& terrainGenerator, int lodDepth)
	{
		std::string outputFormat = ".b3dm";

		double deltaX = east - west;
		double deltaY = north - south;

		double delta = min(deltaX, deltaY) / 2;

		double centerLat = (north - south) / 2 + south;
		double centerLon = (east - west) / 2 + west;

		east = centerLon + delta;
		west = centerLon - delta;
		north = centerLat + delta;
		south = centerLat - delta;

		centerLat = (north - south) / 2 + south;
		centerLon = (east - west) / 2 + west;

		std::vector<TileInfo> infos;
		TileInfo info;
		info.extents.east = east;
		info.extents.north = north;
		info.extents.south = south;
		info.extents.west = west;
		info.quadKey = std::to_string(0);
		GenerateLODBounds(infos, info, lodDepth);
		GenerateFileNames(infos, outputTmpPath);
		ComputeCenterPosition(infos, centerLat, centerLon);


		int nthreads = std::thread::hardware_concurrency();
		std::vector<std::thread> threads(nthreads);
		int span = infos.size() / nthreads;
		int index = 0;
		for (int t = 0; t < nthreads; t++)
		{
			int begin = index;
			int end = index + span;
			if (t == nthreads - 1)
			{
				end = infos.size() - 1;
			}
			threads[t] = std::thread(GetData, geoServerURL, begin, end, &infos);
			index = end + 1;
		}

		std::for_each(threads.begin(), threads.end(), [](std::thread& x) {x.join(); });

		cout << "Finished getting LOD data." << endl;

		elev::DataSourceManager dsm(1000000);

		for (auto& info : infos)
		{
			dsm.AddFile_Raster_GDAL(info.elevationFileName);
		}

		elev::Elevation_DSM edsm(&dsm, elev::elevation_strategy::ELEVATION_BILINEAR);

		for (auto& info : infos)
		{
			terrainGenerator.generateFixedGrid(info.imageFileName, outputPath,
				info.quadKey, outputFormat, edsm,
				info.extents.north, info.extents.south,
				info.extents.east, info.extents.west);
		}
#ifdef CAE_MESH
		terrainGenerator.createFeatures(edsm);
#endif
		//WriteLODfile(infos, outputPath + "/lodFile.txt", lodDepth);

		terrainGenerator.setBounds(north, south, east, west);
		terrainGenerator.CreateMasterFile();
        ccl::FileInfo fi(outputPath);
		bool removeTextures = true;
		if (removeTextures)
		{
            if(ccl::directoryExists(fi.getDirName()))
			{
				std::cout << "generate Cesium Lods: Removing image files from " << outputPath << std::endl;
				auto files = ccl::FileInfo::getAllFiles(fi.getDirName(),"*.*");
                for(auto&& file : files)
				{
					if(ccl::FileInfo::fileExists(fi.getFileName()))
					{
                        auto ext = fi.getSuffix();
						if (ext == "jpg"
							|| ext == "jpeg"
							|| ext == "png")
						{
                            ccl::deleteFile(fi.getFileName());
						}
					}
				}
			}
		}
        auto tmpDir = ccl::joinPaths(outputPath,"/tmp");
        if(ccl::directoryExists(tmpDir))
        {
            auto files = ccl::FileInfo::getAllFiles(fi.getDirName(),"*.*");
            for(auto&& file : files)
            {
                std::cout << "generate Cesium Lods: Removing tmp directory " << tmpDir << std::endl;
                ccl::deleteFile(file.getFileName());
            }
        }
	}

    void ParseJSON(const std::string & filename, const std::string & outputPath, scenegraph::Scene & scene)
    {
        ccl::FileInfo fi(filename);
        FILE *fp = fopen(filename.c_str(), "r");
        char* source;

        if (fp != nullptr)
        {
            if (fseek(fp, 0L, SEEK_END) == 0)
            {
                long bufSize = ftell(fp);
                if (bufSize == -1)
                {
                    ;
                }

                source = new char[bufSize + 1];

                if (fseek(fp, 0L, SEEK_SET) != 0)
                {
                    ;
                }

                size_t newLength = fread(source, sizeof(char), bufSize, fp);

                if (ferror(fp) != 0)
                {
                    std::cout << "error" << std::endl;
                }
                else
                {
                    source[newLength++] = '\0';
                }

                rapidjson::Document document;
                document.Parse(source);

                // Vertices
                auto vertices = document["vertices"].GetArray();
                std::vector<sfa::Point> verts;

                for (rapidjson::Value::ConstValueIterator itr = vertices.Begin(); itr != vertices.End(); ++itr)
                {
                    double x = itr->GetDouble();
                    ++itr;
                    double y = itr->GetDouble();
                    ++itr;
                    double z = itr->GetDouble();

                    sfa::Point point(x, y, z);
                    verts.push_back(point);
                }

                // Normals
                auto normals = document["normals"].GetArray();
                std::vector<sfa::Point> normal;

                for (rapidjson::Value::ConstValueIterator itr = normals.Begin(); itr != normals.End(); ++itr)
                {
                    double x = itr->GetDouble();
                    ++itr;
                    double y = itr->GetDouble();
                    ++itr;
                    double z = itr->GetDouble();

                    sfa::Point point(x, y, z);
                    normal.push_back(point);
                }

                // UVS
                auto uvs = document["uvs"].GetArray();
                auto uv = uvs[0].GetArray(); // update
                std::vector<sfa::Point> uvsPoint;

                for (rapidjson::Value::ConstValueIterator itr = uv.Begin(); itr != uv.End(); ++itr)
                {
                    double x = itr->GetDouble();
                    ++itr;
                    double y = itr->GetDouble();

                    //if (IsGltfTypeOutput())
                    //{
                    //    //gltf uses upper left origin for uvs
                    //    //https://github.com/KhronosGroup/glTF/blob/master/specification/2.0/README.md#images
                    //    y = 1.0 - y;
                    //}

                    sfa::Point point(x, y);
                    uvsPoint.push_back(point);
                }

                // Materials
                auto materials = document["materials"].GetArray();
                std::vector<scenegraph::Material> materialList;

                for (rapidjson::Value::ConstValueIterator itr = materials.Begin(); itr != materials.End(); ++itr)
                {
                    scenegraph::Material mat;

                    auto ambient = (*itr)["colorAmbient"].GetArray();   // Ka
                    mat.ambient.r = ambient[0].GetFloat();
                    mat.ambient.g = ambient[1].GetFloat();
                    mat.ambient.b = ambient[2].GetFloat();

                    auto diffuse = (*itr)["colorDiffuse"].GetArray();   // Kd
                    mat.diffuse.r = diffuse[0].GetFloat();
                    mat.diffuse.g = diffuse[1].GetFloat();
                    mat.diffuse.b = diffuse[2].GetFloat();

                    auto specular = (*itr)["colorSpecular"].GetArray(); // Ks
                    mat.specular.r = specular[0].GetFloat();
                    mat.specular.g = specular[1].GetFloat();
                    mat.specular.b = specular[2].GetFloat();

                    mat.illumination = (*itr)["illumination"].GetInt(); // illum

                    std::string mapDiffuse = (*itr)["mapDiffuse"].GetString(); // material name  
                    //std::experimental::filesystem::path path = mapDiffuse;
                    //path.replace_extension(""); //why was extension being removed?
                    auto filenameOfModel = mapDiffuse;
                    mat.mapDiffuse = filenameOfModel;

                    auto srcPath = ccl::joinPaths(fi.getDirName(), mapDiffuse);
                    if (!ccl::fileExists(srcPath))
                    {
                        return;
                    }
                    auto destPath = ccl::joinPaths(outputPath, mapDiffuse);
                    if (!ccl::fileExists(destPath))
                    {
                        if (!ccl::copyFile(srcPath, destPath))
                        {
                            std::cout << "copy failed" << std::endl;
                        }
                    }
                    mat.transparency = (*itr)["transparency"].GetDouble();
                    mat.transparent = (*itr)["transparent"].GetBool();

                    materialList.push_back(mat);
                }

                // Faces
                auto faces = document["faces"].GetArray();
                std::vector<scenegraph::Face> face;
                int separator;
                int vertex_index1;
                int vertex_index2;
                int vertex_index3;
                int material_index;
                int vertex_uv1;
                int vertex_uv2;
                int vertex_uv3;
                int vertex_normal1;
                int vertex_normal2;
                int vertex_normal3;

                for (rapidjson::Value::ConstValueIterator itr = faces.Begin(); itr != faces.End(); ++itr)
                {
                    separator = itr->GetInt();
                    ++itr;
                    vertex_index1 = itr->GetInt();
                    ++itr;
                    vertex_index2 = itr->GetInt();
                    ++itr;
                    vertex_index3 = itr->GetInt();
                    ++itr;
                    material_index = itr->GetInt();
                    ++itr;
                    vertex_uv1 = itr->GetInt();
                    ++itr;
                    vertex_uv2 = itr->GetInt();
                    ++itr;
                    vertex_uv3 = itr->GetInt();
                    ++itr;
                    vertex_normal1 = itr->GetInt();
                    ++itr;
                    vertex_normal2 = itr->GetInt();
                    ++itr;
                    vertex_normal3 = itr->GetInt();

                    scenegraph::Face f;

                    scenegraph::MappedTexture mt;
                    mt.SetTextureName(materialList.at(material_index).mapDiffuse);
                    mt.uvs.push_back(uvsPoint[vertex_uv1]);
                    mt.uvs.push_back(uvsPoint[vertex_uv2]);
                    mt.uvs.push_back(uvsPoint[vertex_uv3]);
                    f.textures.push_back(mt);

                    f.addVert(verts[vertex_index1]);
                    f.addVert(verts[vertex_index2]);
                    f.addVert(verts[vertex_index3]);

                    f.setNormalN(0, normal[vertex_normal1]);
                    f.setNormalN(1, normal[vertex_normal2]);
                    f.setNormalN(2, normal[vertex_normal3]);

                    face.push_back(f);
                }

                scene.faces = face;
                scene.faceMaterials = materialList;

                int beg = filename.find_last_of("/");
                int end = filename.find_first_of(".");
                std::string name = filename.substr(beg + 1, end - beg - 1);

                //scenegraph::buildOpenFlightFromScene(outputPath+"/"+name+".flt", &scene, 1640);				
            }

            fclose(fp);
        }
        
        //free(fp);
    }

    void generateFixedGridWeb(double north, double south, double west, double east, std::string format, const std::string& geoServerURL, const std::string& outputTmpPath, const std::string& outputPath, const std::string& featurePath, const std::string& outputFormat, int textureWidth, int textureHeight, cognitics::TerrainGenerator& terrainGenerator)
    {
        std::string tileName = "webTest";
        std::string tileInfoName = ccl::joinPaths(outputPath, "tileInfo.txt");
        std::ofstream tileInfo(tileInfoName);
        tileInfo << tileName << "\n";

        const int MAX_TILE_SIZE_METERS = 500;

        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin(south, west);
        double width = flatEarth.convertGeoToLocalX(east);
        double height = flatEarth.convertGeoToLocalY(north);

        int rows = ceil(height / MAX_TILE_SIZE_METERS);
        int cols = ceil(width / MAX_TILE_SIZE_METERS);

        int newWidth = MAX_TILE_SIZE_METERS * cols;
        int newHeight = MAX_TILE_SIZE_METERS * rows;

        east = flatEarth.convertLocalToGeoLon(newWidth);
        north = flatEarth.convertLocalToGeoLat(newHeight);

        tileInfo << rows << " " << cols << "\n";
        std::vector<std::string> elevationFileNames;
        std::vector<int> heights;
        std::vector<int> widths;
        int imageHeight = 0;
        int imageWidth = 0;
        std::string filename;
        OGRSpatialReference oSRS;
        oSRS.SetWellKnownGeogCS("WGS84");
        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                double bottomLeftX, bottomLeftY, topRightX, topRightY;
                bottomLeftX = MAX_TILE_SIZE_METERS * col;
                bottomLeftY = MAX_TILE_SIZE_METERS * row;
                topRightX = bottomLeftX + MAX_TILE_SIZE_METERS;
                topRightY = bottomLeftY + MAX_TILE_SIZE_METERS;

                double tileNorth, tileWest, tileEast, tileSouth;
                tileSouth = flatEarth.convertLocalToGeoLat(bottomLeftY);
                tileNorth = flatEarth.convertLocalToGeoLat(topRightY);
                tileEast = flatEarth.convertLocalToGeoLon(topRightX);
                tileWest = flatEarth.convertLocalToGeoLon(bottomLeftX);
                filename = ws::GetName(tileEast, tileNorth, textureHeight, textureWidth, outputFormat, ".tif");
                auto elevationFilename = outputTmpPath + "/" + filename;
                ws::GetElevation(geoServerURL, tileNorth, tileSouth, tileEast, tileWest, textureWidth, textureHeight, elevationFilename);
                widths.push_back(textureWidth);
                heights.push_back(textureHeight);
                if (col == 0 && row == 0)
                {
                    imageHeight = textureHeight * rows;
                    imageWidth = textureWidth * cols;
                }
                elevationFileNames.push_back(elevationFilename);
            }
        }

        // request a giant wms request...
        ws::GetImagery(geoServerURL, north, south, east, west, imageWidth, imageHeight, outputTmpPath + "/img/" + filename);
        terrainGenerator.addImageryPath(outputTmpPath + "/img");

        int counter = 0;
        for (int row = 0; row < rows; row++)
        {
            for (int col = 0; col < cols; col++)
            {
                int height = heights[counter];
                int width = widths[counter];
                std::string& elevFile = elevationFileNames[counter];
                counter++;
                std::stringstream ss;
                ss << tileName << "_" << row << "_" << col << format;
                filename = ccl::joinPaths(outputPath, ss.str());
                terrainGenerator.generateFixedGrid(elevFile, filename, featurePath, 0, height * .5, width * .5, 0);
            }
        }


    }

    void ConvertGDALElevationToScene(double* adfGeoTransform, int rasterWidth, int rasterHeight, GDALRasterBand* poBand, scenegraph::Scene* masterScene, std::mutex* masterSceneMutex, std::mutex* gdalMutex)
    {
        double west = adfGeoTransform[0];
        double north = adfGeoTransform[3];
        double east = west + adfGeoTransform[1] * rasterWidth;
        double south = north + adfGeoTransform[5] * rasterHeight;

        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin(south, west);        

        double deltaMeterPerPixelY = flatEarth.convertGeoToLocalY(south - adfGeoTransform[5]);
        double deltaMeterPerPixelX = flatEarth.convertGeoToLocalX(west + adfGeoTransform[1]);
        const int TILE_SIZE = 256;
        int subdivideHeight = ceil((float)rasterHeight / TILE_SIZE);
        int subdivideWidth = ceil((float)rasterWidth / TILE_SIZE);
        int totalWidth = flatEarth.convertGeoToLocalX(east) + deltaMeterPerPixelX;
        int totalHeight = flatEarth.convertGeoToLocalY(north) + deltaMeterPerPixelY;
        static atomic_int counter;
        while (true)
        {
            int index = counter++;
            if (index >= subdivideHeight * subdivideHeight)
            {
                return;
            }            
            int i = index / subdivideWidth;
            int j = index % subdivideWidth;

            int offsetX = j * TILE_SIZE;
            int offsetY = i * TILE_SIZE;

            int tileWidth = std::min(TILE_SIZE + 1, rasterWidth - offsetX);
            int tileHeight = std::min(TILE_SIZE + 1, rasterHeight - offsetY);

            float* buffer = new float[tileWidth * tileHeight];
            gdalMutex->lock();
            poBand->RasterIO(GF_Read, offsetX, offsetY, tileWidth, tileHeight, buffer, tileWidth, tileHeight, GDT_Float32, 0, 0);
            gdalMutex->unlock();

            int northWestIndex = 0;
            int northEastIndex = tileWidth - 1;
            int southWestIndex = tileWidth * (tileHeight - 1);
            int southEastIndex = southWestIndex + tileWidth - 1;

            auto tileWest = west + offsetX * adfGeoTransform[1];
            auto tileEast = tileWest + tileWidth * adfGeoTransform[1];
            auto tileNorth = north + offsetY * adfGeoTransform[5];
            auto tileSouth = tileNorth + tileHeight * adfGeoTransform[5];

            double localWest = flatEarth.convertGeoToLocalX(tileWest + adfGeoTransform[1] / 2);
            double localEast = flatEarth.convertGeoToLocalX(tileEast);
            double localNorth = flatEarth.convertGeoToLocalY(tileNorth + adfGeoTransform[5] / 2);
            double localSouth = flatEarth.convertGeoToLocalY(tileSouth);

            ctl::PointList gamingArea;
            ctl::Point southwest(localWest, localSouth, buffer[southWestIndex]);
            ctl::Point southeast(localEast, localSouth, buffer[southEastIndex]);
            ctl::Point northeast(localEast, localNorth, buffer[northEastIndex]);
            ctl::Point northwest(localWest, localNorth, buffer[northWestIndex]);
            gamingArea.push_back(southwest);
            gamingArea.push_back(southeast);
            gamingArea.push_back(northeast);
            gamingArea.push_back(northwest);


            sfa::LineString boundaryLineString;
            // left boundary
            double x = localWest;
            double y = localNorth;
            for (int i = 0; i < tileHeight; i++, y -= deltaMeterPerPixelY)
            {
                boundaryLineString.addPoint(sfa::Point(x, y, buffer[northWestIndex + i * tileWidth]));
            }

            // right boundary
            x = localEast;
            y = localNorth;
            for (int i = 0; i < tileHeight; i++, y -= deltaMeterPerPixelY)
            {
                boundaryLineString.addPoint(sfa::Point(x, y, buffer[northEastIndex + i * tileWidth]));
            }

            // bottom boundary
            x = localWest;
            y = localSouth;
            for (int i = 0; i < tileWidth; i++, x += deltaMeterPerPixelX)
            {
                boundaryLineString.addPoint(sfa::Point(x, y, buffer[southWestIndex + i]));
            }

            // top boundary
            x = localWest;
            y = localNorth;
            for (int i = 0; i < tileWidth; i++, x += deltaMeterPerPixelX)
            {
                boundaryLineString.addPoint(sfa::Point(x, y, buffer[northWestIndex + i]));
            }

            ctl::PointList boundaryPoints;
            for (int i = 0, c = boundaryLineString.getNumPoints(); i < c; ++i)
            {
                sfa::Point *p = boundaryLineString.getPointN(i);
                boundaryPoints.push_back(ctl::Point(p->X(), p->Y(), p->Z()));
            }

            ctl::PointList workingPoints;
            y = localNorth;
            auto ptr = buffer;
            for (int i = 0; i < tileHeight; i++, y -= deltaMeterPerPixelY)
            {
                x = localWest;
                for (int j = 0; j < tileWidth; j++, x += deltaMeterPerPixelX)
                {
                    workingPoints.push_back(ctl::Point(x, y, *ptr++));
                }
            }

            int delaunayResizeIncrement = (tileWidth * tileHeight) / 8;
            ctl::DelaunayTriangulation *dt = new ctl::DelaunayTriangulation(gamingArea, delaunayResizeIncrement);
            std::random_shuffle(workingPoints.begin(), workingPoints.end());
            std::random_shuffle(boundaryPoints.begin(), boundaryPoints.end());
            {
                //Alternate inserting boundary and working points to avoid worst case performance of DelaunayTriangulation
                size_t i = 0;
                size_t j = 0;
                while (i < boundaryPoints.size() || j < workingPoints.size())
                {
                    if (i < boundaryPoints.size())
                        dt->InsertConstrainedPoint(boundaryPoints[i++]);
                    if (j < workingPoints.size())
                        dt->InsertWorkingPoint(workingPoints[j++]);
                }
            }
            dt->Simplify(1, float(0.01));

            ctl::TIN *tin = new ctl::TIN(dt);
            scenegraph::Scene *scene = new scenegraph::Scene;

            std::string imageName = "texture.jpg";
            scene->faces.reserve(tin->triangles.size() / 3);
            for (size_t i = 0, c = tin->triangles.size() / 3; i < c; ++i)
            {
                scenegraph::Face face;
                scenegraph::MappedTexture mt;
                mt.SetTextureName(imageName);
                for (int z = 0; z < 3; z++)
                {
                    auto index = tin->triangles[i * 3 + z];
                    auto& p = tin->verts[index];
                    auto& n = tin->normals[index];
                    float u = p.x / totalWidth;
                    float v = p.y / totalHeight;
                    mt.uvs.push_back(sfa::Point(u, v));
                    face.verts.push_back(sfa::Point(p.x, p.y, p.z));
                    face.vertexNormals.push_back(sfa::Point(n.x, n.y, n.z));
                }
                face.textures.push_back(mt);
                scene->faces.push_back(face);
            }

            transformSceneFromFlatEarthToUTM(scene, south, west);
            masterSceneMutex->lock();
            masterScene->addChild(scene);
            masterSceneMutex->unlock();
            delete[] buffer;
            delete tin;
        }
    }

    void GenerateSingleOBJ(const std::string& elevationFile, const std::string& geoserverIPAddress, const std::string& outputPath, const std::string& outputTmpPath)
    {
        GDALAllRegister();
        auto poDataset = (GDALDataset*)GDALOpen(elevationFile.c_str(), GA_ReadOnly);
        double adfGeoTransform[6];
        poDataset->GetGeoTransform(adfGeoTransform);
        auto poBand = poDataset->GetRasterBand(1);

        int nthreads = std::thread::hardware_concurrency();
        std::vector<std::thread> threads(nthreads);
        std::mutex masterSceneMutex;
        std::mutex gdalMutex;
        scenegraph::Scene masterScene;
        auto rasterHeight = poDataset->GetRasterYSize();
        auto rasterWidth = poDataset->GetRasterXSize();
        for (int t = 0; t < nthreads; t++)
        {
            threads[t] = std::thread(ConvertGDALElevationToScene, adfGeoTransform, rasterWidth, rasterHeight, poBand, &masterScene, &masterSceneMutex, &gdalMutex);
        }
        std::for_each(threads.begin(), threads.end(), [](std::thread& x) {x.join(); });

        std::string multipleTexturesPath = outputPath + "Multiple_Textures/";
        if (!ccl::directoryExists(multipleTexturesPath))
        {
            ccl::makeDirectory(multipleTexturesPath);
        }
        std::string singleTexturePath = outputPath + "Single_Texture/";
        if (!ccl::directoryExists(singleTexturePath))
        {
            ccl::makeDirectory(singleTexturePath);
        }

        double west = adfGeoTransform[0];
        double north = adfGeoTransform[3];
        double south = north + adfGeoTransform[5] * rasterHeight;
        double east = west + adfGeoTransform[1] * rasterWidth;

        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin(south, west);

        GsBuildings buildings;
        std::string dataPath = ccl::joinPaths(outputTmpPath, "data");
        buildings.ProcessBuildingData(ccl::joinPaths(dataPath, "building_models.xml"));
        for (int i = 0; i < buildings.Count(); i++)
        {
            FeatureInfo& building = buildings.GetBuilding(i);
            int offsetX = (building.lon - west) / adfGeoTransform[1];
            int offsetY = (building.lat - north) / adfGeoTransform[5];
            float elevVal;
            poBand->RasterIO(GF_Read, offsetX, offsetY, 1, 1, &elevVal, 1, 1, GDALDataType::GDT_Float32, 0, 0);
            building.elev = elevVal;
            auto buildingScene = new scenegraph::Scene;
            ParseJSON(building.modelpath, multipleTexturesPath, *buildingScene);

            sfa::Matrix m;
            auto radians = (building.AO1 * M_PI) / 180;
            m.PushRotate(sfa::Point(0, 0, 1), -radians);
            m.PushScale(sfa::Point(building.scaleX, building.scaleY, building.scaleZ));
            auto localX = flatEarth.convertGeoToLocalX(building.lon);
            auto localY = flatEarth.convertGeoToLocalY(building.lat);
            m.PushTranslate(sfa::Point(localX, localY, building.elev));
            buildingScene->matrix = m;
            masterScene.addChild(buildingScene);
        }
        GDALClose(poDataset);

        scenegraph::FlattenVisitor flatten;
        auto flattenedScene = flatten.flatten(&masterScene);

        ws::GetImageryJPG(geoserverIPAddress, north, south, east, west, 8192, 8192, multipleTexturesPath + "texture.jpg");
        ccl::copyFile(multipleTexturesPath + "texture.jpg", singleTexturePath + "texture.jpg");

        double easting, northing;
        FromLatLon(south, west, easting, northing);
        std::ofstream offsetFile;
        offsetFile.open(multipleTexturesPath + "offset.xyz", std::ofstream::out);
        offsetFile << std::setprecision(10);
        offsetFile << easting << " " << northing << " 0";
        offsetFile.close();
        ccl::copyFile(multipleTexturesPath + "offset.xyz", singleTexturePath + "offset.xyz");

        std::ofstream wktFile;
        wktFile.open(multipleTexturesPath + "wkt.prj", std::ofstream::out);
        auto wkt = GetWellKnownTextUTM(south, west);
        wktFile << wkt;
        wktFile.close();
        ccl::copyFile(multipleTexturesPath + "wkt.prj", singleTexturePath + "wkt.prj");

        scenegraph::buildObjFromScene(multipleTexturesPath + "/" + "simplified_3d_mesh.obj", 0, 0, 0, 0, flattenedScene);

        auto totalWidth = flatEarth.convertGeoToLocalX(east);
        auto totalHeight = flatEarth.convertGeoToLocalY(north);
        for (size_t i = 0; i < flattenedScene->faces.size(); ++i)
        {
            auto& face = flattenedScene->faces[i];
            scenegraph::MappedTexture mt;
            for (int index = 0; index < face.verts.size(); index++)
            {
                auto& p = face.verts[index];
                float u = p.X() / totalWidth;
                float v = p.Y() / totalHeight;
                mt.SetTextureName("texture.jpg");
                mt.uvs.push_back(sfa::Point(u, v));
            }
            face.setMappedTextureN(0, mt);
        }
        scenegraph::buildObjFromScene(singleTexturePath + "/" + "simplified_3d_mesh.obj", 0, 0, 0, 0, flattenedScene);
        delete flattenedScene;
    }
}