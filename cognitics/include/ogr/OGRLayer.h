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

/*! \file ogr/Layer.h
\headerfile ogr/Layer.h
\brief Provides ogr::Layer.
\author Aaron Brinton <abrinton@cognitics.net>
\date 30 October 2009
*/
#pragma once

#include "ogr/Field.h"
#include "ogr/Feature.h"

#pragma warning ( push )
#pragma warning ( disable : 4251 )		// C4251: 'GDALColorTable::aoEntries' : class 'std::vector<_Ty>' needs to have dll-interface to be used by clients of class 'GDALColorTable'
#include <ogrsf_frmts.h>
#include <ogr_spatialref.h>
#pragma warning ( pop )

#include <sfa/File.h>
#include <sfa/sfa.h>

namespace ogr
{
/*! \brief OGR Layer Class

This class encapsulates the functionality of OGRLayer.
*/
	class Layer : public sfa::Layer
	{
		friend class File;

	private:
		int maxFieldNameLenth;
		std::map<std::string,std::string> fieldNameMap;
		OGRLayer *layer;
		FieldList fields;
		cts::CS_CoordinateSystem *coordinateSystem;
		sfa::File *file;
		Layer(OGRLayer *layer, sfa::File *_file,OGRDataSource *_ogrDataSource);
		OGRDataSource *ogrFile;
//! \brief Create node from geometry (recursive).
		sfa::Geometry *getGeometry(OGRGeometry *ogr_geometry);
		
//! \brief Create geometry from node (recursive).
		OGRGeometry *makeGeometry(sfa::Geometry *geometry);
		int getVariantTypeFromOGRFieldType(OGRFieldType ogrtype) const;
		OGRFieldType getOGRFieldTypeFromVariantType(int vtype) const;

		bool writeOGRFeature(ogr::Feature *feature, OGRFeature *ogrFeature);
		bool readOGRFeature(ogr::Feature *feature, OGRFeature *ogr_native);
		int getFieldIndex(const std::string &name);

	public:
		FieldList getOGRFields(void);
		Field *addOGRField(const std::string &name, OGRFieldType type = OFTString);


		virtual ~Layer(void);

		OGRLayer *getOGRLayer(void);

		virtual sfa::WKBGeometryType getType(void);

		// ****** Field Methods
		// returns ccl::Variant::TYPE_EMPTY if the field isn't defined, else returns the field's type
		virtual int hasField(std::string field) const;
		virtual bool addField(std::string field, int type);
		virtual bool removeField(std::string field);
		virtual size_t getNumFields() const;
		virtual sfa::FieldDefinition getFieldAt(size_t i) const;
		virtual sfa::FieldDefinition getFieldAt(std::string name) const;

		virtual cts::CS_CoordinateSystem *getCoordinateSystem(void);

		virtual void setSpatialFilter(int cursorId=0);
		virtual void setSpatialFilter(double west, double south, double east, double north,int cursorId=0);
		virtual bool beginUpdating();
		virtual bool commitUpdates();
		virtual bool rollbackUpdates();

		virtual void resetReading(int cursorId=0);
		virtual sfa::Feature *getNextFeature(int cursorId=0);
		virtual sfa::Feature *addFeature(sfa::Feature *feature);
		virtual bool updateFeature(sfa::Feature *feature);
		virtual bool deleteFeature(sfa::Feature *feature);

		virtual int getFeatureCount(void);

		virtual bool getExtent(double &left, double &bottom, double &right, double &top, bool forceQuery);

		virtual std::string getName() const;
		virtual void setName(const std::string &name);

		void setMaxFieldLength(int maxFieldNameLenth) { this->maxFieldNameLenth = maxFieldNameLenth;}

		virtual void deleteFeaturesWhere(const std::string &whereClause);
	};

	//typedef std::vector<sfa::Layer *> LayerList;

}
