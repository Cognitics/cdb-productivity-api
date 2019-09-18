#include "WebServices.h"
#include "ip/GDALRasterSampler.h"
#include "ccl/FileInfo.h"
#include <fstream>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iterator>
#include <iostream>
#include <string>
#include "b64/base64.h"
#include <cctype>
#include "gdal_utils.h"
#include <string>

//#include <direct.h>


namespace ws
{
    void GetDataWithGDAL(cognitics::TerrainGenerator& tg, std::set<std::string>& elevationFiles, const std::string &outputPath, double north, double south, double east, double west, int& textureWidth, int& textureHeight, double originLat, double originLon, std::string format)
    {
        GDALAllRegister();
        GDALDataset* elevationDataset;
        char** papszDrivers = NULL;
        papszDrivers = CSLAddString(papszDrivers, "WCS");
        char** papszOptions = NULL;
        papszOptions = CSLAddString(papszOptions, "GridCRSOptional");

        std::stringstream parameters;
        std::stringstream workingUrl;

        workingUrl << "wcs:http://tgs/geoserver/wcs?SERVICE=WCS&version=1.0.0";
        workingUrl << "&coverage=CDB%20Elevation_Terrain_Primary&CRS=EPSG:4326&bbox=";
        workingUrl << west
            << "," << south
            << "," << east
            << "," << north;
        workingUrl << "&format=GeoTIFF";
        parameters << std::fixed << std::setprecision(15)
            << "SERVICE=WCS&amp;FORMAT=GeoTIFF&amp;bbox="
            << south
            << "," << west
            << "," << north
            << "," << east
            << ",urn:ogc:def:crs:EPSG::4326&amp;WIDTH="
            << std::to_string(textureWidth)
            << "&amp;HEIGHT="
            << std::to_string(textureHeight);
        std::stringstream ss;
        std::string xml = "";

        

        ss << "<WCS_GDAL>";
        ss << "  <ServiceURL>http://tgs/geoserver/wcs?SERVICE=WCS</ServiceURL>";
        ss << "  <Version>1.1.1</Version>";
        ss << "  <CoverageName>CDB Elevation_Terrain_Primary</CoverageName>";
       // ss << "  <Parameters>" + parameters.str() + "</Parameters>";
      //  ss << "  <GridCRSOptional>TRUE</GridCRSOptional>";
        		//ss << "  <CoverageDescription>";
                ss << "    <Identifier>Base:CDB Elevation_Terrain_Primary</Identifier>";
                ss << "    <DomainSubset>";
                ss << "        <BoundingBox crs=\"urn:ogc:def:crs:EPSG::4326\" dimensions=\"2\">";
                ss << "          <LowerCorner>" << south << " " << west << "</LowerCorner>";
                ss << "          <UpperCorner>" << north << " " << east << "</UpperCorner>";
                ss << "        </BoundingBox>";
                ss << "    </DomainSubset>";
                ss << "    <Output store=\"true\" format=\"image / tiff\">";
                ss << "        <GridCRS>";
                ss << "          <GridBaseCRS>urn:ogc:def:crs:EPSG::4326 </GridBaseCRS >";
                ss << "          <GridType>urn : ogc : def : method : WCS : 1.1 : 2dSimpleGrid</GridType>";
                ss << "          <GridOffsets>5.37109375E-6 5.37109375E-6</GridOffsets>";
                ss << "          <GridCS>urn : ogc : def : cs : OGC : 0.0 : Grid2dSquareCS</GridCS>";
                ss << "        </GridCRS>";
                ss << "    </Output>";
                
      //          ss << "    <SupportedCRS>urn:ogc:def:crs:EPSG::4326</SupportedCRS>";
      //          ss << "    <SupportedCRS>EPSG:4326</SupportedCRS>";
      //          ss << "    <SupportedFormat>image/tiff</SupportedFormat>";
                //ss << "  </CoverageDescription>";
       // 		ss << "  <FieldName>contents</FieldName>";
        ss << "  <BandType>Float32</BandType>";
        //ss << "  <PreferredFormat>image/tiff</PreferredFormat>";
        ss << "</WCS_GDAL>";
        xml = ss.str();
        std::string u = workingUrl.str();
        elevationDataset = (GDALDataset*)GDALOpenEx(xml.c_str(), GDAL_OF_READONLY, papszDrivers, papszOptions, NULL);

        
        double xadfGeoTransform[6];
        if (!elevationDataset)
        {
            std::cout << "Error: unable to open file.\n";
            return;
        }
        if (elevationDataset->GetGeoTransform(xadfGeoTransform) == CE_None)
        {
            
        }

        int width = elevationDataset->GetRasterXSize();
        int height = elevationDataset->GetRasterYSize();
        std::cout << "Width: " << width << " Height: " << height << std::endl;

		auto band = elevationDataset->GetRasterBand(1);

		textureWidth = width;
		textureHeight = height;

		float *data = new float[textureWidth * textureHeight + 1];

		band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_CFloat32, 4, 4 * textureWidth);

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

		band->RasterIO(GF_Read, 0, 0, textureWidth, textureHeight, data, textureWidth, textureHeight, GDALDataType::GDT_CFloat32, 4, 4 * textureWidth);

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

		std::string elevationName = SetName(originLon, originLat, textureHeight, textureWidth, outputPath, format, ".tif");
		std::string path = outputPath + elevationName;
        ccl::makeDirectory(outputPath.c_str(),true);
		//int directoryError = _mkdir(outputPath.c_str());
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

		GetImagery(north, south, east, west, textureWidth, textureHeight, outputPath, elevationName, originLon, originLat, format);

		tg.addImageryPath(outputPath + "img");

		tg.addElevationFile(outputPath + elevationName);
		elevationFiles.clear();
		elevationFiles.insert(outputPath + elevationName);
	}

	std::string SetName(double originLon, double originLat, double textureHeight, double textureWidth, const std::string &outputPath, std::string formatIn, std::string filetype)
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
		name = lat + std::to_string(fabs(latValue)) + "_" + lon + std::to_string(fabs(lonValue)) + "_" + "H" + std::to_string((int)textureHeight) + "_" + "W" + std::to_string((int)textureWidth) + "_" + format + filetype;
		return name;
	}
	
	void GetImagery(double north, double south, double east, double west, int textureWidth, int textureHeight, const std::string &outputPath, const std::string elevationName, double originLon, double originLat, std::string format)
	{
		// GDAL takes in an XML string describing the request.
		//GDALAllRegister();
		bool gdalError = false;

		/// PNG ///
		std::ostringstream xml;
		xml.precision(10);
		xml << std::fixed
			<< "<GDAL_WMS>"
			<< "<Service name=\"WMS\">"
			<< "<ServerUrl>http://localhost:81/geoserver/wms?</ServerUrl>"
			<< "<Layers>Base%3ACDB%20Imagery_YearRound</Layers>"
			<< "<ImageFormat>image/jpeg</ImageFormat>"
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

		GDALDataset *datasetPNG = (GDALDataset*)GDALOpen(xml.str().c_str(), GA_ReadOnly);
		if (datasetPNG == NULL)
		{
			std::cout << "Not getting imagery from xml" << std::endl;
			gdalError = true;
		}

		const char* pngFileFormat = "PNG";
		GDALDriver* pngDriver = GetGDALDriverManager()->GetDriverByName(pngFileFormat);
		if (pngDriver == NULL)
		{
			std::cout << "PNG driver failed" << std::endl;
			gdalError = true;
		}

		std::string pngName = SetName(originLon, originLat, textureHeight, textureWidth, outputPath, format, ".png");
		std::string pngPath = outputPath + "img/";
		std::string pngOutputFile = pngPath + pngName;
		//int createDirError = _mkdir(pngPath.c_str());
        ccl::makeDirectory(pngPath.c_str(),true);
		GDALDataset* ImageDatasetPNG = pngDriver->CreateCopy(pngOutputFile.c_str(), datasetPNG, FALSE, NULL, NULL, NULL);
		if (ImageDatasetPNG == NULL)
		{
			std::cout << "PNG Dataset failed" << std::endl;
			gdalError = true;
		}

		/// TIFF ///
		std::ostringstream xmlTiff;
		xmlTiff.precision(10);
		xmlTiff << std::fixed
			<< "<GDAL_WMS>"
			<< "<Service name=\"WMS\">"
			<< "<ServerUrl>http://localhost:81/geoserver/wms?</ServerUrl>"
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

		std::string tiffName = SetName(originLon, originLat, textureHeight, textureWidth, outputPath, format, ".tif");
		std::string tiffOutputFile = outputPath + "img/" + tiffName;
		GDALDataset* ImageDatasetTiff = tiffDriver->CreateCopy(tiffOutputFile.c_str(), datasetTiff, FALSE, NULL, NULL, NULL);
		if (ImageDatasetTiff == NULL)
		{
			std::cout << "GTiff dataset failed" << std::endl;
			gdalError = true;
		}

		std::string path = outputPath + elevationName;
		GDALDataset *locationDataset = (GDALDataset*)GDALOpen(path.c_str(), GA_ReadOnly);
		if (locationDataset == NULL)
		{
			std::cout << "WebServices: Failed opening elev " << std::endl;
			gdalError = true;
		}

		if (!gdalError)
		{
			std::cout << "WebServices: Saving imagery..." << std::endl;

			double transforms[6];
			locationDataset->GetGeoTransform(transforms);

			ImageDatasetTiff->SetGeoTransform(transforms);
			ImageDatasetTiff->SetProjection(locationDataset->GetProjectionRef());
			ImageDatasetTiff->SetGCPs(locationDataset->GetGCPCount(), locationDataset->GetGCPs(), locationDataset->GetGCPProjection());
			ImageDatasetTiff->SetMetadata(locationDataset->GetMetadata());
			ImageDatasetTiff->FlushCache();
		}

		// cleanup GDAL
		GDALClose((GDALDatasetH)locationDataset);
		GDALClose((GDALDatasetH)datasetPNG);
		GDALClose((GDALDatasetH)datasetTiff);
		GDALClose((GDALDatasetH)ImageDatasetPNG);
		GDALClose((GDALDatasetH)ImageDatasetTiff);
	}
}