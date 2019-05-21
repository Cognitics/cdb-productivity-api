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

//#pragma optimize("", off)

#include "ogr/OGRLayer.h"
#include "sfa/Feature.h"
#include <locale>
#include <ogr/File.h>

namespace ogr
{
	class File;
	int Layer::getFieldIndex(const std::string &name)
	{
		std::string lcName = name;
		std::transform(lcName.begin(), lcName.end(), lcName.begin(), tolower);
		for(size_t i = 0, c = fields.size(); i < c; ++i)
		{
			Field *field = fields[i];
			std::string fieldName = field->getName();
			std::string lcFieldName = fieldName;
			std::transform(lcFieldName.begin(), lcFieldName.end(), lcFieldName.begin(), tolower);
			if(lcName == lcFieldName)
				return int(i);
		}
		return -1;
	}

	bool Layer::writeOGRFeature(ogr::Feature *feature, OGRFeature *ogrFeature)
	{
		feature->ogr_native = ogrFeature;
		std::vector<std::string> keys = feature->attributes.getKeys();
		for(std::vector<std::string>::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
		{			
			if(ignoreFields.find(*it) != ignoreFields.end())
				continue;

			std::string name = *it;
			std::string shortName(name);
			// Look for a previously mapped name
			std::map<std::string,std::string>::iterator fiter =	fieldNameMap.find(name);
			if(fiter!=fieldNameMap.end())
			{
				shortName = fieldNameMap[name];
			}
			else
			{
				// keep trying until we find a new shortened version.
				if((int)name.length()>maxFieldNameLenth)
				{
					char buf[1024];
					std::string prefix = name.substr(0,(maxFieldNameLenth-3));
					for(int i=0;i<99;i++)
					{
						sprintf(buf,"%s%d",prefix.c_str(),i);
						fiter =	fieldNameMap.find(buf);
						if(fiter==fieldNameMap.end())
						{
							//use this name
							shortName = buf;
							break;
						}
					}
				}
			}
			fieldNameMap[name] = shortName;
			name = shortName;

			std::transform(name.begin(), name.end(), name.begin(), tolower);
			ccl::Variant value = feature->attributes.getAttributeAsString(*it);

			// find a matching field
			std::string fieldName;
			Field *field = NULL;
			for(FieldList::iterator fit = fields.begin(); fit != fields.end(); ++fit)
			{
				std::string fname((*fit)->getName());
				fieldName = fname;
				std::transform(fname.begin(), fname.end(), fname.begin(), tolower);

				if(fname == name)
				{
					field = *fit;
					break;
				}
			}

			int fieldIndex = getFieldIndex(fieldName);

			// if no field found, create it
			if(fieldIndex < 0)
			{
				if(value.typeinfo() == typeid(std::wstring))
					field = addOGRField(fieldName, OFTString);
				else if(value.typeinfo() == typeid(std::string))
					field = addOGRField(fieldName, OFTString);
				else if(value.typeinfo() == typeid(SQLGUID))
					field = addOGRField(fieldName, OFTString);
				else if(value.typeinfo() == typeid(bool))
					field = addOGRField(fieldName, OFTInteger);
				else if(value.typeinfo() == typeid(char))
					field = addOGRField(fieldName, OFTInteger);
				else if(value.typeinfo() == typeid(unsigned char))
					field = addOGRField(fieldName, OFTInteger);
				else if(value.typeinfo() == typeid(int))
					field = addOGRField(fieldName, OFTInteger);
				else if(value.typeinfo() == typeid(unsigned int))
					field = addOGRField(fieldName, OFTInteger);
				else if(value.typeinfo() == typeid(float))
					field = addOGRField(fieldName, OFTReal);
				else if(value.typeinfo() == typeid(double))
					field = addOGRField(fieldName, OFTReal);
				else if(value.typeinfo() == typeid(long double))
					field = addOGRField(fieldName, OFTReal);
				else if(value.typeinfo() == typeid(TIMESTAMP_STRUCT))
					field = addOGRField(fieldName, OFTDateTime);
				else if(value.typeinfo() == typeid(DATE_STRUCT))
					field = addOGRField(fieldName, OFTDate);
				else if(value.typeinfo() == typeid(TIME_STRUCT))
					field = addOGRField(fieldName, OFTTime);
				else
					field = addOGRField(fieldName, OFTString);
			}
		}

		OGRGeometry *ogrgeom = makeGeometry(feature->geometry);
		if(ogrFeature->SetGeometryDirectly(ogrgeom) != OGRERR_NONE)
		{
			OGRFeature::DestroyFeature(ogrFeature);
			return false;
		}


		keys = feature->attributes.getKeys();
		for(std::vector<std::string>::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
		{
			std::string name = *it;
			std::transform(name.begin(), name.end(), name.begin(), tolower);
			ccl::Variant value;
			feature->attributes.getAttribute(name,value);

			std::string fieldName = name;
			Field *field = NULL;
			for(FieldList::iterator fit = fields.begin(), end = fields.end(); fit != end; ++fit)
			{
				std::string fname((*fit)->getName());
				fieldName = fname;
				std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
				
				std::string mappedname = fieldNameMap[name];
				std::transform(mappedname.begin(), mappedname.end(), mappedname.begin(), tolower);
				
				if((fname == name)||(fname==mappedname))
				{
					field = *fit;
					break;
				}
			}

			if(!field)
				continue;

			fieldName = field->getName();
			int fieldIndex = getFieldIndex(fieldName);
			if(fieldIndex==-1)
			{
				std::cout << "Warning, cannot find index for " << fieldName << std::endl;
				continue;
			}
			switch(field->getType())
			{
				case OFTInteger:
					ogrFeature->SetField(fieldIndex, value.as_int());
					break;
				case OFTReal:
					ogrFeature->SetField(fieldIndex, value.as_double());
					break;
				case OFTDateTime:
					{
						TIMESTAMP_STRUCT data = value.as_datetime();
						ogrFeature->SetField(fieldIndex, data.year, data.month, data.day, data.hour, data.minute, data.second);
					}
					break;
				case OFTDate:
					{
						DATE_STRUCT data = value.as_date();
						ogrFeature->SetField(fieldIndex, data.year, data.month, data.day);
					}
					break;
				case OFTTime:
					{
						TIME_STRUCT data = value.as_time();
						ogrFeature->SetField(fieldIndex, 0, 0, 0, data.hour, data.minute, data.second);
					}
					break;
				case OFTIntegerList:
				case OFTRealList:
				case OFTString:
				case OFTStringList:
				case OFTWideString:
				case OFTWideStringList:
				case OFTBinary:
				default:
					std::string valuestr = value.as_string();
					ogrFeature->SetField(fieldIndex, valuestr.c_str());
					break;
			}
		}

		return true;
	}
	
	bool Layer::readOGRFeature(ogr::Feature *feature, OGRFeature *ogr_feature)
	{
		feature->layer = this;
		OGRGeometry *ogr_geometry = ogr_feature->GetGeometryRef();
		if(!ogr_geometry || ogr_geometry->IsEmpty())
			return false;
		sfa::Geometry *geometry = getGeometry(ogr_geometry);
		if(!geometry)
			return false;	

		feature->geometry = geometry;

		for(FieldList::iterator it = fields.begin(), end = fields.end(); it != end; ++it)
		{
			int i = it - fields.begin();
			Field *field = *it;
			if(!ogr_feature->IsFieldSet(i))
			{
				ccl::VariantMap &vmap = *feature->attributes.getVariantMap();
				vmap[field->getName()] = ccl::Variant();
				continue;
			}
			switch(field->getType())
			{
				case OFTInteger:
					feature->attributes.setAttribute(field->getName(),ogr_feature->GetFieldAsInteger(i));
					break;
				case OFTIntegerList:
					// TODO
					feature->attributes.setAttribute(field->getName(), std::string(ogr_feature->GetFieldAsString(i)));
					break;
				case OFTReal:
					feature->attributes.setAttribute(field->getName(), ogr_feature->GetFieldAsDouble(i));
					break;
				case OFTRealList:
					// TODO
					feature->attributes.setAttribute(field->getName(), std::string(ogr_feature->GetFieldAsString(i)));
					break;
				case OFTStringList:
					// deprecated
					feature->attributes.setAttribute(field->getName(), std::string(ogr_feature->GetFieldAsString(i)));
					break;
				case OFTWideStringList:
					// deprecated
					feature->attributes.setAttribute(field->getName(), std::string(ogr_feature->GetFieldAsString(i)));
					break;
				case OFTBinary:
					// TODO
					feature->attributes.setAttribute(field->getName(), std::string(ogr_feature->GetFieldAsString(i)));
					break;
				case OFTDate:
					{
						DATE_STRUCT data;
						int Y, M, D, h, m, s, tz;
						ogr_feature->GetFieldAsDateTime(i, &Y, &M, &D, &h, &m, &s, &tz);
						data.year = Y;
						data.month = M;
						data.day = D;
						ccl::Variant var(data);
						feature->attributes.setAttribute(field->getName(),var);
					}
					break;
				case OFTTime:
					{
						TIME_STRUCT data;
						int Y, M, D, h, m, s, tz;
						ogr_feature->GetFieldAsDateTime(i, &Y, &M, &D, &h, &m, &s, &tz);
						data.hour = h;
						data.minute = m;
						data.second = s;
						ccl::Variant var(data);
						feature->attributes.setAttribute(field->getName(),var);
					}
					break;
				case OFTDateTime:
					{
						TIMESTAMP_STRUCT data;
						int Y, M, D, h, m, s, tz;
						ogr_feature->GetFieldAsDateTime(i, &Y, &M, &D, &h, &m, &s, &tz);
						data.year = Y;
						data.month = M;
						data.day = D;
						data.hour = h;
						data.minute = m;
						data.second = s;
						ccl::Variant var(data);
						feature->attributes.setAttribute(field->getName(),var);
					}
					break;
				default:
					feature->attributes.setAttribute(field->getName(), std::string(ogr_feature->GetFieldAsString(i)));
					break;
			}
		}
		ccl::Variant fidvar((ccl::uint64_t)ogr_feature->GetFID());
		if (!feature->attributes.hasAttribute("OBJECTID"))
		{
			feature->attributes.setAttribute("OBJECTID", fidvar);
		}
		else if (!feature->attributes.hasAttribute("FID"))
		{
			feature->attributes.setAttribute("FID", fidvar);
		}

		feature->geometry->setCoordinateSystem(getCoordinateSystem());
		return true;
	}

	bool Layer::beginUpdating()
	{
#if (_MSC_VER >= 1600)
		return file->beginUpdating();
#else
		OGRErr res = layer->StartTransaction();
		return (res == OGRERR_NONE);
#endif
	}

	bool Layer::commitUpdates()
	{
#if (_MSC_VER >= 1600)
		return file->commitUpdates();
#else
		OGRErr res = layer->CommitTransaction();
		return (res == OGRERR_NONE);
#endif
	}

	bool Layer::rollbackUpdates()
	{
#if (_MSC_VER >= 1600)
		return file->rollbackUpdates();
#else
		OGRErr res = layer->RollbackTransaction();
		return (res == OGRERR_NONE);
#endif
	}

	bool Layer::updateFeature(sfa::Feature *feature)
	{
		ogr::Feature *ogrfeature = dynamic_cast<ogr::Feature*>(feature);
		if(!ogrfeature)
			return false;
		if(writeOGRFeature(ogrfeature,ogrfeature->ogr_native))
		{
			OGRErr res = layer->SetFeature(ogrfeature->ogr_native);
			if(res==OGRERR_NONE)
				return true;		
		}
		return false;
	}

	void Layer::deleteFeaturesWhere(const std::string &whereClause)
	{
		std::stringstream ss;
		ss << "DELETE FROM " << this->name << " ";
		ss << whereClause;
		ogr::File *ogrfile = dynamic_cast<ogr::File*>(file);
		ogrfile->executeSQL(ss.str());

	}

	bool Layer::deleteFeature(sfa::Feature *feature)
	{
		ogr::Feature *ogrfeature = dynamic_cast<ogr::Feature*>(feature);
		if(!ogrfeature)
			return false;
		OGRErr res = layer->DeleteFeature(ogrfeature->getOGRFID());
		if(res==OGRERR_NONE)
			return true;
		return false;
	}

	Layer::~Layer(void)
	{
		for(FieldList::iterator it = fields.begin(), end = fields.end(); it != end; ++it)
			delete *it;
	} 

	Layer::Layer(OGRLayer *layer, sfa::File *_file, OGRDataSource *_ogrDataSource) : layer(layer), file(_file), coordinateSystem(NULL), ogrFile(_ogrDataSource)
	{
		maxFieldNameLenth = 32;
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		for(int i = 0, c = featureDefn->GetFieldCount(); i < c; i++)
		{
			Field *field = new Field(featureDefn->GetFieldDefn(i));
			fields.push_back(field);
		}
		this->name = layer->GetName();
	}

	sfa::FieldDefinition Layer::getFieldAt(std::string name) const
	{
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		for(int i = 0, c = featureDefn->GetFieldCount(); i < c; i++)
		{
			std::string fieldname(featureDefn->GetFieldDefn(i)->GetNameRef());
			if(fieldname==name)
				return getFieldAt(i);
		}
		sfa::FieldDefinition ret;
		return ret;
	}

	sfa::FieldDefinition Layer::getFieldAt(size_t i) const
	{
		sfa::FieldDefinition ret;
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		if(int(i)<featureDefn->GetFieldCount())
		{
			ret.type = getVariantTypeFromOGRFieldType(featureDefn->GetFieldDefn((int)i)->GetType());
			ret.name.assign(featureDefn->GetFieldDefn((int)i)->GetNameRef());
			ret.length = featureDefn->GetFieldDefn((int)i)->GetPrecision();
		}
		return ret;
	}

	OGRLayer *Layer::getOGRLayer(void)
	{
		return layer;
	}

	sfa::WKBGeometryType Layer::getType(void)
	{
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		switch(featureDefn->GetGeomType())
		{
			case wkbPoint:
				return sfa::wkbPoint;
			case wkbPoint25D:
				return sfa::wkbPointZ;
			case wkbLineString:
			case wkbLinearRing:
				return sfa::wkbLineString;
			case wkbLineString25D:
				return sfa::wkbLineStringZ;
			case wkbPolygon:
				return sfa::wkbPolygon;
			case wkbPolygon25D:
				return sfa::wkbPolygonZ;
			case wkbGeometryCollection:
				return sfa::wkbGeometryCollection;
			case wkbGeometryCollection25D:
				return sfa::wkbGeometryCollectionZ;
			case wkbMultiPoint:
				return sfa::wkbMultiPoint;
			case wkbMultiPoint25D:
				return sfa::wkbMultiPointZ;
			case wkbMultiLineString:
				return sfa::wkbMultiLineString;
			case wkbMultiLineString25D:
				return sfa::wkbMultiLineStringZ;
			case wkbMultiPolygon:
				return sfa::wkbMultiPolygon;
			case wkbMultiPolygon25D:
				return sfa::wkbMultiPolygonZ;
		}
		return sfa::wkbUnknownSFA;
	}
	
	FieldList Layer::getOGRFields(void)
	{
		return fields;
	}

	Field *Layer::addOGRField(const std::string &name, OGRFieldType type)
	{
		OGRFieldDefn fieldDefn(name.c_str(), type);
		if(layer->CreateField(&fieldDefn) != OGRERR_NONE)
			return NULL;
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		Field *field = new Field(featureDefn->GetFieldDefn(int(fields.size())));
		fields.push_back(field);
		fieldNameMap[name] = field->getName();
		return field;
	}

	int Layer::getVariantTypeFromOGRFieldType(OGRFieldType ogrtype) const
	{
		switch(ogrtype)
		{
			case OFTInteger:
				return ccl::Variant::TYPE_INT;
				break;
			case OFTIntegerList:
				// TODO
				return ccl::Variant::TYPE_STRING;
				break;
			case OFTReal:
				return ccl::Variant::TYPE_DOUBLE;
				break;
			case OFTRealList:
			case OFTStringList:
			case OFTWideStringList:
			case OFTBinary:
				// TODO
				return ccl::Variant::TYPE_STRING;
				break;
			case OFTDate:
				return ccl::Variant::TYPE_DATE;
				break;
			case OFTTime:
				return ccl::Variant::TYPE_TIME;
				break;
			case OFTDateTime:
				return ccl::Variant::TYPE_DATETIME;
				break;
			default:
				return ccl::Variant::TYPE_STRING;
				break;
		}
	}

	OGRFieldType Layer::getOGRFieldTypeFromVariantType(int vtype) const
	{
		OGRFieldType ogrType = OFTString;
		switch(vtype)
		{
		case ccl::Variant::TYPE_INT:
			ogrType = OFTInteger;
			break;
		case ccl::Variant::TYPE_FLOAT:
		case ccl::Variant::TYPE_DOUBLE:
			ogrType = OFTReal;
			break;
		case ccl::Variant::TYPE_DATE:
			ogrType = OFTDate;
			break;
		case ccl::Variant::TYPE_TIME:
			ogrType = OFTTime;
			break;
		case ccl::Variant::TYPE_DATETIME:
			ogrType = OFTDateTime;
			break;
		default:
			ogrType = OFTString;
		}
		return ogrType;
	}

	// returns ccl::Variant::TYPE_EMPTY if the field isn't defined, else returns the field's type
	int Layer::hasField(std::string name) const
	{
		std::transform(name.begin(), name.end(), name.begin(), tolower);
		for(FieldList::const_iterator it = fields.begin(), end = fields.end(); it != end; ++it)
		{
			const Field *field = *it;
			
			std::string fname(field->getName());
			std::transform(fname.begin(), fname.end(), fname.begin(), tolower);
			if(fname==name)
			{
				return getVariantTypeFromOGRFieldType(field->getType());
			}
		}
		return ccl::Variant::TYPE_EMPTY;
	}

	bool Layer::addField(std::string field, int type)
	{
		int ogrType = getOGRFieldTypeFromVariantType(type);
		
		return (addOGRField(field,(OGRFieldType)ogrType)!=NULL);
	}
	
	bool Layer::removeField(std::string field)
	{
		// You can't remove fields in this implementation
		return false;
	}

	size_t Layer::getNumFields() const
	{
		OGRFeatureDefn *featureDefn = layer->GetLayerDefn();
		return featureDefn->GetFieldCount();
	}

	cts::CS_CoordinateSystem *Layer::getCoordinateSystem(void)
	{
		cts::CS_CoordinateSystem *coordinateSystem = NULL;

		OGRSpatialReference *srf = layer->GetSpatialRef();
		if(srf != NULL)
		{
			char *wktp;
			srf->exportToWkt(&wktp);
			std::string wkt(wktp);
			cts::CS_CoordinateSystemFactory csFactory;
			coordinateSystem = csFactory.createFromWKT(wkt);
		}
		return coordinateSystem;
	}

	void Layer::setSpatialFilter(int cursorId)
	{
		layer->SetSpatialFilter(NULL);
	}

	void Layer::setSpatialFilter(double west, double south = 0.0, double east = 0.0, double north = 0.0,int cursorId)
	{
		layer->SetSpatialFilterRect(west, south, east, north);
	}

	void Layer::resetReading(int cursorId)
	{
		std::string sql;
		sfa::attributeFilters_t filters = attributeFilters[cursorId];
		for(size_t i = 0, c = filters.size(); i < c; ++i)
		{
			std::string k = filters[i].first;
			std::string v = filters[i].second;
			if(!sql.empty())
				sql += " AND ";
			sql += k + "='" + v + "'";
		}
		layer->ResetReading();
		if(sql.size())
			layer->SetAttributeFilter(sql.c_str());
	}

	sfa::Feature *Layer::getNextFeature(int cursorId)
	{
		OGRFeature *ogr_feature = layer->GetNextFeature();
		if(!ogr_feature)
			return NULL;
		
		ogr::Feature *feature = new ogr::Feature(ogr_feature,this);

		if(!readOGRFeature(feature,ogr_feature))
			return NULL;
		
		return feature;
	}

	sfa::Feature * Layer::addFeature(sfa::Feature *feature)
	{
		if(!feature)
			return NULL;
		//We need to make sure all the fields exist before creating the ogr feature.
		std::vector<std::string> keys = feature->attributes.getKeys();
		for(std::vector<std::string>::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
		{			
			if(ignoreFields.find(*it) != ignoreFields.end())
				continue;
			std::string name = *it;
			std::string shortName(name);
			// Look for a previously mapped name
			std::map<std::string,std::string>::iterator fiter =	fieldNameMap.find(name);
			if(fiter!=fieldNameMap.end())
			{
				shortName = fieldNameMap[name];
			}
			else
			{
				// keep trying until we find a new shortened version.
				if((int)name.length()>maxFieldNameLenth)
				{
					char buf[1024];
					std::string prefix = name.substr(0,(maxFieldNameLenth-3));
					for(int i=0;i<99;i++)
					{
						sprintf(buf,"%s%d",prefix.c_str(),i);
						fiter =	fieldNameMap.find(buf);
						if(fiter==fieldNameMap.end())
						{
							//use this name
							shortName = buf;
							break;
						}
					}
				}
			}
			fieldNameMap[name] = shortName;
			name = shortName;
			if(!hasField(name))
			{
				int type = this->getOGRFieldTypeFromVariantType(feature->attributes.getAttributeType(*it));
				addField(name,type);
			}
		}
		OGRFeature *ogrFeature = OGRFeature::CreateFeature(layer->GetLayerDefn());
		if(!ogrFeature)
			return NULL;
		ogr::Feature *feature2 = new ogr::Feature();
		feature2->geometry = feature->geometry->copy();
		feature2->attributes = feature->attributes;
		


		if(!writeOGRFeature(feature2,ogrFeature))
			return NULL;

		if(layer->CreateFeature(ogrFeature) != OGRERR_NONE)
		{
			OGRFeature::DestroyFeature(ogrFeature);
			return NULL;
		}

		return feature2;
	}

	sfa::Geometry *Layer::getGeometry(OGRGeometry *ogr_geometry)
	{
		switch(ogr_geometry->getGeometryType())
		{
			case wkbPoint:
				{
					OGRPoint *ogr = (OGRPoint *)ogr_geometry;
					return new sfa::Point(ogr->getX(), ogr->getY());
				}
				break;
			case wkbPoint25D:
				{
					OGRPoint *ogr = (OGRPoint *)ogr_geometry;
					return new sfa::Point(ogr->getX(), ogr->getY(), ogr->getZ());
				}
				break;
			case wkbLineString:
				{
					OGRLineString *ogr = (OGRLineString *)ogr_geometry;
					sfa::LineString *lineString = new sfa::LineString;
					int c = ogr->getNumPoints();
					for(int i = 0; i < c; i++)
						lineString->addPoint(new sfa::Point(ogr->getX(i), ogr->getY(i)));
					return lineString;
				}
				break;
			case wkbLineString25D:
				{
					OGRLineString *ogr = (OGRLineString *)ogr_geometry;
					sfa::LineString *lineString = new sfa::LineString;
					int c = ogr->getNumPoints();
					for(int i = 0; i < c; i++)
						lineString->addPoint(new sfa::Point(ogr->getX(i), ogr->getY(i), ogr->getZ(i)));
					return lineString;
				}
				break;
			case wkbPolygon:
				{
					OGRPolygon *ogr = (OGRPolygon *)ogr_geometry;
					sfa::Polygon *polygon = new sfa::Polygon;
					OGRLinearRing *ring = ogr->getExteriorRing();
					sfa::LineString *oRing = new sfa::LineString;
					for(int i = 0, c = ring->getNumPoints(); i < c; i++)
						oRing->addPoint(new sfa::Point(ring->getX(i), ring->getY(i)));
					polygon->addRing(oRing);
					for(int r = 0, rcount = ogr->getNumInteriorRings(); r < rcount; r++)
					{
						ring = ogr->getInteriorRing(r);
						sfa::LineString *iRing = new sfa::LineString;
						for(int i = 0, c = ring->getNumPoints(); i < c; i++)
							iRing->addPoint(new sfa::Point(ring->getX(i), ring->getY(i)));
						polygon->addRing(iRing);
					}
					return polygon;
				}
				break;
			case wkbPolygon25D:
				{
					OGRPolygon *ogr = (OGRPolygon *)ogr_geometry;
					sfa::Polygon *polygon = new sfa::Polygon;
					OGRLinearRing *ring = ogr->getExteriorRing();
					sfa::LineString *oRing = new sfa::LineString;
					for(int i = 0, c = ring->getNumPoints(); i < c; i++)
						oRing->addPoint(new sfa::Point(ring->getX(i), ring->getY(i), ring->getZ(i)));
					polygon->addRing(oRing);
					for(int r = 0, rcount = ogr->getNumInteriorRings(); r < rcount; r++)
					{
						ring = ogr->getInteriorRing(r);
						sfa::LineString *iRing = new sfa::LineString;
						for(int i = 0, c = ring->getNumPoints(); i < c; i++)
							iRing->addPoint(new sfa::Point(ring->getX(i), ring->getY(i), ring->getZ(i)));
						polygon->addRing(iRing);
					}
					return polygon;
				}
				break;
			case wkbGeometryCollection:
			case wkbGeometryCollection25D:
				{
					OGRGeometryCollection *ogr = (OGRGeometryCollection *)ogr_geometry;
					sfa::GeometryCollection *geometryCollection = new sfa::GeometryCollection;
					for(int g = 0, c = ogr->getNumGeometries(); g < c; g++)
					{
						sfa::Geometry *geometry = getGeometry(ogr->getGeometryRef(g));
						if(geometry)
						{
							if(g==0 && ogr->getNumGeometries()==1)
							{
								delete geometryCollection;
								return geometry;
							}
							else
								geometryCollection->addGeometry(geometry);
						}
							
					}
					return geometryCollection;
				}
				break;
			case wkbMultiPoint:
			case wkbMultiPoint25D:
				{
					OGRGeometryCollection *ogr = (OGRGeometryCollection *)ogr_geometry;
					sfa::MultiPoint *multiPoint = new sfa::MultiPoint;
					for(int g = 0, c = ogr->getNumGeometries(); g < c; g++)
					{
						sfa::Geometry *geometry = getGeometry(ogr->getGeometryRef(g));
						if(geometry)
						{
							// if it's not really a multi-geometry, just return the single geometry, not the container
							if(g==0 && ogr->getNumGeometries()==1)
							{
								delete multiPoint;
								return geometry;
							}
						}
						else
							multiPoint->addGeometry(geometry);
					}
					return multiPoint;
				}
				break;
			case wkbMultiLineString:
			case wkbMultiLineString25D:
				{
					OGRGeometryCollection *ogr = (OGRGeometryCollection *)ogr_geometry;
					sfa::MultiLineString *multiLineString = new sfa::MultiLineString;
					for(int g = 0, c = ogr->getNumGeometries(); g < c; g++)
					{
						sfa::Geometry *geometry = getGeometry(ogr->getGeometryRef(g));
						if(geometry)
						{
							if(g==0 && ogr->getNumGeometries()==1)
							{
								delete multiLineString;
								return geometry;
							}
							else
								multiLineString->addGeometry(geometry);
						}
					}
					return multiLineString;
				}
				break;
			case wkbMultiPolygon:
			case wkbMultiPolygon25D:
				{
					OGRGeometryCollection *ogr = (OGRGeometryCollection *)ogr_geometry;
					sfa::MultiPolygon *multiPolygon = new sfa::MultiPolygon;
					for(int g = 0, c = ogr->getNumGeometries(); g < c; g++)
					{
						sfa::Geometry *geometry = getGeometry(ogr->getGeometryRef(g));
						if(geometry)
						{
							if(g==0 && ogr->getNumGeometries()==1)
							{
								delete multiPolygon;
								return geometry;
							}
							else
								multiPolygon->addGeometry(geometry);
						}
					}
					return multiPolygon;
				}
				break;
		}

		return NULL;
	}

	OGRGeometry *Layer::makeGeometry(sfa::Geometry *geometry)
	{
		switch(geometry->getWKBGeometryType(false, false))
		{
			case sfa::wkbPoint:
				{
					sfa::Point *point = dynamic_cast<sfa::Point *>(geometry);
					OGRPoint *ogrpt = NULL;
					if(geometry->is3D())
						ogrpt = static_cast<OGRPoint *>(OGRGeometryFactory::createGeometry(wkbPoint25D));
					else
						ogrpt = static_cast<OGRPoint *>(OGRGeometryFactory::createGeometry(wkbPoint));
					ogrpt->setX(point->X());
					ogrpt->setY(point->Y());
					if(geometry->is3D())
						ogrpt->setZ(point->Z());
					return ogrpt;
				}
			case sfa::wkbLineString:
				{
					sfa::LineString *lineString = dynamic_cast<sfa::LineString *>(geometry);
					OGRLineString *ogrLineString = NULL;
					if(geometry->is3D())
						ogrLineString = static_cast<OGRLineString *>(OGRGeometryFactory::createGeometry(wkbLineString25D));
					else
						ogrLineString = static_cast<OGRLineString *>(OGRGeometryFactory::createGeometry(wkbLineString));
					for(int i = 0; i < lineString->getNumPoints(); i++)
					{
						sfa::Point *point = lineString->getPointN(i);
						OGRPoint *ogrPoint = (OGRPoint *)makeGeometry(point);
						ogrLineString->addPoint(ogrPoint);
						OGRGeometryFactory::destroyGeometry(ogrPoint);
					}
					return ogrLineString;
				}
			case sfa::wkbPolygon:
			case sfa::wkbTriangle:
				{
					sfa::Polygon *polygon = dynamic_cast<sfa::Polygon *>(geometry);
					OGRPolygon *ogrPolygon = NULL;
					//static_cast<OGRPolygon *>(OGRGeometryFactory::createGeometry(wkbPolygon25D));
					if(geometry->is3D())
						ogrPolygon = static_cast<OGRPolygon *>(OGRGeometryFactory::createGeometry(wkbPolygon25D));
					else
						ogrPolygon = static_cast<OGRPolygon *>(OGRGeometryFactory::createGeometry(wkbPolygon));


					// OGR polygons are linearrings, SFA polygons are linestrings
					sfa::LineString *lineString = polygon->getExteriorRing();
					OGRLinearRing *ogrLinearRing = static_cast<OGRLinearRing *>(OGRGeometryFactory::createGeometry(wkbLinearRing));
					for(int i = 0; i < lineString->getNumPoints(); i++)
					{
						sfa::Point *point = lineString->getPointN(i);
						OGRPoint *ogrPoint = (OGRPoint *)makeGeometry(point);
						ogrLinearRing->addPoint(ogrPoint);
						OGRGeometryFactory::destroyGeometry(ogrPoint);
					}
					ogrPolygon->addRing(ogrLinearRing);
					OGRGeometryFactory::destroyGeometry(ogrLinearRing);

					for(int i = 0; i < polygon->getNumInteriorRing(); i++)
					{
						lineString = polygon->getInteriorRingN(i);
						ogrLinearRing = static_cast<OGRLinearRing *>(OGRGeometryFactory::createGeometry(wkbLinearRing));
						for(int j = 0; j < lineString->getNumPoints(); j++)
						{
							sfa::Point *point = lineString->getPointN(j);
							OGRPoint *ogrPoint = (OGRPoint *)makeGeometry(point);
							ogrLinearRing->addPoint(ogrPoint);
							OGRGeometryFactory::destroyGeometry(ogrPoint);
						}
						ogrPolygon->addRing(ogrLinearRing);
						OGRGeometryFactory::destroyGeometry(ogrLinearRing);
					}

					return ogrPolygon;
				}
			case sfa::wkbPolyhedralSurface:
			case sfa::wkbTIN:
				{
					sfa::PolyhedralSurface *polyhedralSurface = dynamic_cast<sfa::PolyhedralSurface *>(geometry);
					OGRMultiPolygon *ogrMultiPolygon = NULL;
					if(geometry->is3D())
						ogrMultiPolygon = static_cast<OGRMultiPolygon *>(OGRGeometryFactory::createGeometry(wkbMultiPolygon25D));
					else
						ogrMultiPolygon = static_cast<OGRMultiPolygon *>(OGRGeometryFactory::createGeometry(wkbMultiPolygon));

					for(int i = 0; i < polyhedralSurface->getNumPatches(); i++)
					{
						sfa::Geometry *geometry = polyhedralSurface->getPatchN(i);
						OGRGeometry *ogrGeometry = makeGeometry(geometry);
						ogrMultiPolygon->addGeometry(ogrGeometry);
						OGRGeometryFactory::destroyGeometry(ogrGeometry);
					}
					return ogrMultiPolygon;
				}
			case sfa::wkbGeometryCollection:
				{
					sfa::GeometryCollection *geometryCollection = dynamic_cast<sfa::GeometryCollection *>(geometry);
					OGRGeometryCollection *ogrGeometryCollection = NULL;
					if(geometry->is3D())
						ogrGeometryCollection = static_cast<OGRGeometryCollection *>(OGRGeometryFactory::createGeometry(wkbGeometryCollection25D));
					else
						ogrGeometryCollection = static_cast<OGRGeometryCollection *>(OGRGeometryFactory::createGeometry(wkbGeometryCollection));
					for(int i = 0; i < geometryCollection->getNumGeometries(); i++)
					{
						sfa::Geometry *geometry = geometryCollection->getGeometryN(i+1);
						OGRGeometry *ogrGeometry = makeGeometry(geometry);
						ogrGeometryCollection->addGeometry(ogrGeometry);
						OGRGeometryFactory::destroyGeometry(ogrGeometry);
					}
					return ogrGeometryCollection;
				}
			case sfa::wkbMultiPoint:
				{
					sfa::MultiPoint *multiPoint = dynamic_cast<sfa::MultiPoint *>(geometry);
					OGRMultiPoint *ogrMultiPoint = NULL; 
					if(geometry->is3D())
						ogrMultiPoint = static_cast<OGRMultiPoint *>(OGRGeometryFactory::createGeometry(wkbMultiPoint25D));
					else
						ogrMultiPoint = static_cast<OGRMultiPoint *>(OGRGeometryFactory::createGeometry(wkbMultiPoint));

					for(int i = 0; i < multiPoint->getNumGeometries(); i++)
					{
						sfa::Geometry *geometry = multiPoint->getGeometryN(i+1);
						OGRGeometry *ogrGeometry = makeGeometry(geometry);
						ogrMultiPoint->addGeometry(ogrGeometry);
						OGRGeometryFactory::destroyGeometry(ogrGeometry);
					}
					return ogrMultiPoint;
				}
			case sfa::wkbMultiLineString:
				{
					sfa::MultiLineString *multiLineString = dynamic_cast<sfa::MultiLineString *>(geometry);
					OGRMultiLineString *ogrMultiLineString = NULL;
					if(geometry->is3D())
						ogrMultiLineString = static_cast<OGRMultiLineString *>(OGRGeometryFactory::createGeometry(wkbMultiLineString25D));
					else
						ogrMultiLineString = static_cast<OGRMultiLineString *>(OGRGeometryFactory::createGeometry(wkbMultiLineString));

					for(int i = 0; i < multiLineString->getNumGeometries(); i++)
					{
						sfa::Geometry *geometry = multiLineString->getGeometryN(i+1);
						OGRGeometry *ogrGeometry = makeGeometry(geometry);
						ogrMultiLineString->addGeometry(ogrGeometry);
						OGRGeometryFactory::destroyGeometry(ogrGeometry);
					}
					return ogrMultiLineString;
				}
			case sfa::wkbMultiPolygon:
				{
					sfa::MultiPolygon *multiPolygon = dynamic_cast<sfa::MultiPolygon *>(geometry);
					OGRMultiPolygon *ogrMultiPolygon = NULL;
					if(geometry->is3D())
						ogrMultiPolygon = static_cast<OGRMultiPolygon *>(OGRGeometryFactory::createGeometry(wkbMultiPolygon25D));
					else
						ogrMultiPolygon = static_cast<OGRMultiPolygon *>(OGRGeometryFactory::createGeometry(wkbMultiPolygon));
					for(int i = 0; i < multiPolygon->getNumGeometries(); i++)
					{
						sfa::Geometry *geometry = multiPolygon->getGeometryN(i+1);
						OGRGeometry *ogrGeometry = makeGeometry(geometry);
						ogrMultiPolygon->addGeometry(ogrGeometry);
						OGRGeometryFactory::destroyGeometry(ogrGeometry);
					}
					return ogrMultiPolygon;
				}
		}

		return NULL;
	}

	int Layer::getFeatureCount(void)
	{
		return layer->GetFeatureCount();
	}

	bool Layer::getExtent(double &left, double &bottom, double &right, double &top, bool forceQuery)
	{
		OGREnvelope envelope;
		if(OGRERR_FAILURE==layer->GetExtent(&envelope,forceQuery?1:0))
		{
			left = right = bottom = top = 0;
			return false;
		}
		left = envelope.MinX;
		right = envelope.MaxX;
		bottom = envelope.MinY;
		top = envelope.MaxY;
		return true;
	}

	std::string Layer::getName() const
	{
		return std::string(layer->GetName());
	}
	
	void Layer::setName(const std::string &name)
	{
		// Not implemented in OGR, you must set the name when creating the layer in the file
	}
}
