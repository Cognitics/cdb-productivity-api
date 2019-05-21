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

/*! \file ogr/File.h
\headerfile ogr/File.h
\brief Provides ogr::File.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
*/
#pragma once

#include "ogr/OGRLayer.h"

#include <cts/cts.h>
#include <sfa/File.h>

#pragma warning ( push )
#pragma warning ( disable : 4251 )		// C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <ogrsf_frmts.h>
#pragma warning ( pop )

#include <string>

namespace ogr
{
	class File;

/*! \brief OGR File Class

This class encapsulates the functionality of OGRDataSource.
*/
	class File : public sfa::File
	{
	private:
		int maxFieldLength;
		std::string filename;
		OGRDataSource *dataSource;
		bool update;
		sfa::LayerList layers;
		std::vector<std::string> ignoreFields;
		bool isSpatialite;
		int transactionLevel;//0 means no pending transactions, 1 means one, etc.
		File(const std::string &filename, OGRDataSource *dataSource, bool update);

	public:
		File();
		~File(void);
		
		static File *s_open(const std::string &filename, bool update = false);
		static File *s_create(const std::string &driverName, const std::string &filename);

		OGRDataSource *getOGRDataSource(void);
		std::string getFileName(void);

		int getLastErrorNo(void);
		std::string getLastErrorMsg(void);

		virtual sfa::LayerList getLayers(void);

		// this will call the appropriate layer addFeature() for the geometry type, creating the layer if necessary
		sfa::Feature *addFeature(sfa::Feature *feature);

		virtual sfa::Layer *addLayer(std::string name,sfa::WKBGeometryType type, cts::CS_CoordinateSystem *coordinateSystem = NULL);

		virtual int getFeatureCount(void);

		virtual bool create(const std::string &filename);

		virtual bool open(std::string path, bool update=false);
		virtual bool close();

		virtual bool beginUpdating();
		virtual bool commitUpdates();
		virtual bool rollbackUpdates();

		void executeSQL(const std::string &query);

	};

}

