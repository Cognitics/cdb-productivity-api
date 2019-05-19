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

#include "ogr/File.h"

#include <sfa/sfa.h>
#include <sfa_file_factory/sfa_file_factory.h>

#pragma warning ( push )
#pragma warning ( disable : 4251 )		// C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <ogr_api.h>
#pragma warning ( pop )

void InitOGR()
{

}

class OGRInitializer
{

public:
	OGRInitializer()
	{
		OGRRegisterAll();
	}

	~OGRInitializer()
	{
		OGRCleanupAll();
	}

};

OGRInitializer g_ogr;

namespace ogr
{

	class OGRFileFactory : public sfa::IFileFactory
	{
		public:
			virtual sfa::File *create()
			{
				return dynamic_cast<sfa::File *>(new File());
			}
		
			virtual void destroy(sfa::File *file)
			{
				delete file;		
			}
	};


	class RegisterOGRFileFactory
	{
		OGRFileFactory *factory;
	public:

		RegisterOGRFileFactory()
		{
			factory = new OGRFileFactory();
			sfa::FileRegistry::instance()->registerFactory("shp",factory);
			sfa::FileRegistry::instance()->registerFactory("gml",factory);
			sfa::FileRegistry::instance()->registerFactory("sqlite",factory);
			sfa::FileRegistry::instance()->registerFactory("gpkg",factory);
			sfa::FileRegistry::instance()->registerFactory("osm", factory);
#ifndef HAS_ARCOBJECTS
			sfa::FileRegistry::instance()->registerFactory("gdb", factory);
			sfa::FileRegistry::instance()->registerFactory("mdb", factory);
#endif
		}
	
		~RegisterOGRFileFactory()
		{
			sfa::FileRegistry::instance()->unRegisterFactory(factory);
			delete factory;
		}
	};

	RegisterOGRFileFactory _registerOGRFactoryObj;


	File::~File(void)
	{
		if(dataSource)
		{
			dataSource->FlushCache();
			GDALClose(dataSource);
		}
		for(sfa::LayerList::iterator it = layers.begin(), end = layers.end(); it != end; ++it)
			delete *it;
	}

	File::File(const std::string &filename, OGRDataSource *dataSource, bool update)
		: maxFieldLength(64), filename(filename), dataSource(dataSource), update(update)
	{
	}

	File *File::s_open(const std::string &filename, bool update)
	{		
		int maxFieldLength = 64;
		std::string driverName;
		std::string drvName(driverName);
		ccl::FileInfo fi(filename);
		std::string ext = fi.getSuffix();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		bool spatialite = false;
		if(ext=="gml")
			drvName = "GML";
		else if(ext=="sqlite")
		{
			drvName = "SQLite";
			spatialite = true;
		}
		else if(ext=="gpkg")
			drvName = "GPKG";
		else if (ext == "mdb")
			drvName = "PGeo";
		else if (ext == "gdb")
			drvName = "OpenFileGDB";
		else if (ext == "osm")
			drvName = "OSM";
		else
		{
			drvName = "ESRI Shapefile";
			maxFieldLength = 10;
		}

		OGRDataSource *dataSource = (OGRDataSource *)GDALOpenEx(filename.c_str(), (update ? GDAL_OF_UPDATE : 0),0,0,0);
		if(!dataSource)
			return NULL;
		File *file = new File(filename, dataSource, update);
		file->maxFieldLength = maxFieldLength;
		for(int i = 0, c = dataSource->GetLayerCount(); i < c; ++i)
			file->layers.push_back(new Layer(dataSource->GetLayer(i), file, dataSource));
		file->isSpatialite = spatialite;
		return file;
	}

	File *File::s_create(const std::string &driverName, const std::string &filename)
	{
		int maxFieldLength=64;
		std::string drvName(driverName);
		ccl::FileInfo fi(filename);
		std::string ext = fi.getSuffix();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		char **papszDSCO = NULL;
		bool spatialite = false;
		if (ext == "gml")
			drvName = "GML";
		else if (ext == "sqlite")
		{
			drvName = "SQLite";
			papszDSCO = CSLAddString(papszDSCO,"SPATIALITE=YES");
			spatialite = true;
		}
		else if (ext == "gpkg")
			drvName = "GPKG";
        else if (ext == "gdb")
         drvName = "FileGDB";
		else
		{
			drvName = "ESRI Shapefile";
			maxFieldLength = 10;
		}
		OGRSFDriverRegistrar *poR = OGRSFDriverRegistrar::GetRegistrar();
		GDALDriver *driver = poR->GetDriverByName(drvName.c_str());

		if(!driver)
			return NULL;

		if(filename.empty())
			return NULL;

		struct stat finfo;
		if(stat(filename.c_str(), &finfo) == 0)
			return NULL;

		OGRDataSource *dataSource = (OGRDataSource *)driver->Create(filename.c_str(),0,0,0,GDT_Unknown,papszDSCO);
		if(!dataSource)
			return NULL;

		File *thefile = new File(filename, dataSource, true);
		thefile->maxFieldLength = maxFieldLength;
		thefile->isSpatialite = spatialite;
		return thefile;
	}

	OGRDataSource *File::getOGRDataSource(void)
	{
		return dataSource;
	}

	std::string File::getFileName(void)
	{
		return filename;
	}

	int File::getLastErrorNo(void)
	{
		return CPLGetLastErrorNo();
	}

	std::string File::getLastErrorMsg(void)
	{
		return std::string(CPLGetLastErrorMsg());
	}

	sfa::LayerList File::getLayers(void)
	{
		return layers;
	}

	sfa::Feature *File::addFeature(sfa::Feature *feature)
	{
		if(!dataSource)
			return NULL;
		if(!feature || !feature->geometry)
			return NULL;

		// this is pretty inefficient
		bool is3D = feature->geometry->is3D();
		bool isMeasured = feature->geometry->isMeasured();

		for(sfa::LayerList::iterator it = layers.begin(), end = layers.end(); it != end; ++it)
		{
			sfa::Layer *layer = *it;
			if(layer->getType() == feature->geometry->getWKBGeometryType(is3D, false))		// ogr doesn't care about measured
				return layer->addFeature(feature);
		}

		sfa::Layer *layer = addLayer(feature->geometry->getGeometryType(), feature->geometry->getWKBGeometryType(is3D, isMeasured), feature->geometry->getCoordinateSystem());
		if(!layer)
			return NULL;
		return layer->addFeature(feature);
	}

	sfa::Layer *File::addLayer(std::string name, sfa::WKBGeometryType type, cts::CS_CoordinateSystem *coordinateSystem)
	{
		char **papszLCO = NULL;
		if(!dataSource)
			return NULL;
		if(isSpatialite)
		{
			papszLCO = CSLAddString(papszLCO,"FORMAT=SPATIALITE");
		}
		OGRwkbGeometryType ogrType = wkbUnknown;
		switch(type)
		{
			case sfa::wkbPoint:
			case sfa::wkbPointM:
				ogrType = wkbPoint;
				break;
			case sfa::wkbLineString:
			case sfa::wkbLineStringM:
				ogrType = wkbLineString;
				break;
			case sfa::wkbPolygon:
			case sfa::wkbPolygonM:
			case sfa::wkbTriangle:
			case sfa::wkbTriangleM:
				ogrType = wkbPolygon;
				break;
			case sfa::wkbGeometryCollection:
			case sfa::wkbGeometryCollectionM:
				ogrType = wkbGeometryCollection;
				break;
			case sfa::wkbMultiPoint:
			case sfa::wkbMultiPointM:
				ogrType = wkbMultiPoint;
				break;
			case sfa::wkbMultiLineString:
			case sfa::wkbMultiLineStringM:
				ogrType = wkbMultiLineString;
				break;
			case sfa::wkbMultiPolygon:
			case sfa::wkbMultiPolygonM:
			case sfa::wkbPolyhedralSurface:
			case sfa::wkbPolyhedralSurfaceM:
			case sfa::wkbTIN:
			case sfa::wkbTINM:
				ogrType = wkbMultiPolygon;
				break;
			case sfa::wkbPointZ:
			case sfa::wkbPointZM:
				ogrType = wkbPoint25D;
				break;
			case sfa::wkbLineStringZ:
			case sfa::wkbLineStringZM:
				ogrType = wkbLineString25D;
				break;
			case sfa::wkbPolygonZ:
			case sfa::wkbPolygonZM:
			case sfa::wkbTriangleZ:
			case sfa::wkbTriangleZM:
				ogrType = wkbPolygon25D;
				break;
			case sfa::wkbGeometryCollectionZ:
			case sfa::wkbGeometryCollectionZM:
				ogrType = wkbGeometryCollection25D;
				break;
			case sfa::wkbMultiPointZ:
			case sfa::wkbMultiPointZM:
				ogrType = wkbMultiPoint25D;
				break;
			case sfa::wkbMultiLineStringZ:
			case sfa::wkbMultiLineStringZM:
				ogrType = wkbMultiLineString25D;
				break;
			case sfa::wkbMultiPolygonZ:
			case sfa::wkbMultiPolygonZM:
			case sfa::wkbPolyhedralSurfaceZ:
			case sfa::wkbPolyhedralSurfaceZM:
			case sfa::wkbTINZ:
			case sfa::wkbTINZM:
				ogrType = wkbMultiPolygon25D;
				break;
		}

		OGRSpatialReference *spatialReference = NULL;
		if(coordinateSystem)
		{
			spatialReference = new OGRSpatialReference;
			spatialReference->SetFromUserInput(coordinateSystem->getWKT().c_str());
		}
		if(!dataSource)
			return NULL;
		OGRLayer *ogrLayer = dataSource->CreateLayer(name.c_str(), spatialReference, ogrType, papszLCO);
		if(!ogrLayer)
			return NULL;

		Layer *layer = new Layer(ogrLayer,this, dataSource);
		
		layer->coordinateSystem = coordinateSystem;

		for(size_t i=0,ic=ignoreFields.size();i<ic;i++)
		{
			layer->setIgnoreField(ignoreFields.at(i),true);
		}
		layers.push_back(layer);
		layer->setMaxFieldLength(maxFieldLength);

		return layer;
	}

	int File::getFeatureCount(void)
	{
		if(!dataSource)
			return 0;
		int result = 0;
		for(sfa::LayerList::iterator it = layers.begin(), end = layers.end(); it != end; ++it)
			result += (*it)->getFeatureCount();
		return result;
	}

	File::File()
	{
		isSpatialite = false;
		dataSource = NULL;
		update = false;
		transactionLevel = 0;
	}

	bool File::create(const std::string &filename)
	{
		this->filename = filename;
		this->update = true;
		std::string driverName;
		std::string drvName(driverName);
		ccl::FileInfo fi(filename);
		std::string ext = fi.getSuffix();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		char **papszDSCO = NULL;
		maxFieldLength=64;
		if (ext == "gml")
			drvName = "GML";
		else if (ext == "sqlite")
		{
			drvName = "SQLite";
			papszDSCO = CSLAddString(papszDSCO,"SPATIALITE=YES");
			isSpatialite = true;
		}
      else if (ext == "gdb")
      {
         drvName = "FileGDB";
         ignoreFields.push_back("OBJECTID");
      }
		else if (ext == "gpkg")
			drvName = "GPKG";
		else
		{
			drvName = "ESRI Shapefile";
			maxFieldLength = 10;
		}


		OGRSFDriverRegistrar *poR = OGRSFDriverRegistrar::GetRegistrar();
		GDALDriver *driver = poR->GetDriverByName(drvName.c_str());
		if(!driver)
			return false;

		if(filename.empty())
			return false;

		struct stat finfo;
		if(stat(filename.c_str(), &finfo) == 0)
			return false;

		dataSource = (OGRDataSource *)driver->Create(filename.c_str(), 0, 0, 0, GDT_Unknown, papszDSCO);
		if(!dataSource)
			return false;
		return true;
	}

	bool File::open(std::string path, bool update)
	{
		filename = path;
		this->update = update;

		std::string driverName;
		std::string drvName(driverName);
		ccl::FileInfo fi(filename);
		std::string ext = fi.getSuffix();
		std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
		if (ext == "gml")
			drvName = "GML";
		else if (ext == "sqlite")
		{
			drvName = "SQLite";
			isSpatialite = true;
		}
		else if (ext == "gpkg")
			drvName = "GPKG";
		else if (ext == "mdb")
			drvName = "PGeo";
		else if (ext == "gdb")
			drvName = "OpenFileGDB";
		else if (ext == "osm")
			drvName = "OSM";
		else
		{
			drvName = "ESRI Shapefile";
			maxFieldLength = 10;
		}

		dataSource = (OGRDataSource *)GDALOpenEx(filename.c_str(), (update ? GDAL_OF_UPDATE : 0), 0, 0, 0);
		if(!dataSource)
			return NULL;
		
		for(int i = 0, c = dataSource->GetLayerCount(); i < c; ++i)
			layers.push_back(new Layer(dataSource->GetLayer(i),this,dataSource));

		return true;
	}

	bool File::close()
	{
		if(dataSource)
		{
			dataSource->FlushCache();
			GDALClose(dataSource);
			dataSource = NULL;
		}
		for(sfa::LayerList::iterator it = layers.begin(), end = layers.end(); it != end; ++it)
			delete *it;
		layers.clear();
		return true;
	}

	bool File::beginUpdating()
	{
#if (_MSC_VER >= 1600)
		if (!dataSource || transactionLevel==1)
			return false;
		OGRErr res = dataSource->StartTransaction();
		if (res == OGRERR_NONE)
		{
			transactionLevel = 1;
			return true;
		}
		return false;
#else
		return true;
#endif
	}

	bool File::commitUpdates()
	{
#if (_MSC_VER >= 1600)
		if (!dataSource || transactionLevel!=1)
			return false;
		transactionLevel = 0;
		OGRErr res = dataSource->CommitTransaction();
		if (res == OGRERR_NONE)
		{			
			return true;
		}
		return false;
#else
		return true;
#endif
	}

	bool File::rollbackUpdates()
	{
#if (_MSC_VER >= 1600)
		if (!dataSource || transactionLevel != 1)
			return false;
		transactionLevel = 0;
		OGRErr res = dataSource->RollbackTransaction();
		if (res == OGRERR_NONE)
		{			
			return true;
		}
		return false;
#else
		return true;
#endif
	}

	void File::executeSQL(const std::string &query)
	{
		if (dataSource)
		{
			dataSource->ExecuteSQL(query.c_str(), NULL, "OGRSQL");
		}

	}

}