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
/*! \brief Provides sfa::Layer Interface.
\author Kevin Bentley <kbentley@cognitics.net>
\date 27 August 2012
*/
#pragma once
#include <vector>
#include "Feature.h"
#include "LineString.h"
#include <ccl/Variant.h>
namespace sfa
{

    class FieldDefinition
    {
    public:
        FieldDefinition()
        {
            length = 0;
            type = ccl::Variant::TYPE_EMPTY;
        }
        std::string name;
        int type;
        int length;
    };


    typedef std::pair<std::string,std::string> attributeValuePair_t;
    typedef std::vector<attributeValuePair_t> attributeFilters_t;
/*! \brief Layer class

The Layer class is a logical container for a collection of features. In
this basic form it is just a vector of pointers to sfa::Feature objects.
Other forms may store the data in a file or SQL database.
*/
    class Layer 
    {
    private:
        size_t iter_pos;
        cts::CS_CoordinateSystem *cs;
        std::vector<sfa::Feature *> features;
        std::map<sfa::Feature *,sfa::Feature *> featureCopyMap;
        std::map<sfa::Feature *,sfa::Feature *> pendingUpdatesMap;
        bool pendingUpdates;
        std::vector<FieldDefinition>  fields;
        
    protected:
        sfa::WKBGeometryType layerType;
        sfa::LineString spatialFilter;
        std::map<std::string, bool> ignoreFields;
        std::string name;//Name for the base implementation
        std::map<int,attributeFilters_t> attributeFilters;
        
    public:
        virtual ~Layer(void);
        Layer();

        virtual sfa::WKBGeometryType getType(void);
        
        // ****** Field Methods
        // returns ccl::Variant::TYPE_EMPTY if the field isn't defined, else returns the field's type
        virtual int hasField(std::string field) const;
        virtual bool addField(std::string field, int type);
        virtual bool removeField(std::string field);
        virtual size_t getNumFields() const;
        virtual FieldDefinition getFieldAt(size_t i) const;
        virtual FieldDefinition getFieldAt(std::string name) const;

        virtual bool getIngoreField(std::string field);
        virtual void setIgnoreField(std::string field, bool ignore);
        virtual cts::CS_CoordinateSystem *getCoordinateSystem(void) ;
        virtual void setCoordinateSystem(cts::CS_CoordinateSystem *cs);
        virtual void setSpatialFilter(int cursorId=0);
        virtual void setSpatialFilter(double west, double south, double east, double north,int cursorId=0);
        
        virtual void addAttributeFilter(const std::string key, const std::string value, int cursorId=0);
        virtual void clearAttributeFilters(int cursorId=0);

        virtual bool beginUpdating();
        virtual bool commitUpdates();
        virtual bool rollbackUpdates();
        virtual void resetReading(int cursorId=0);
        virtual sfa::Feature *getNextFeature(int cursorId=0);
        /*! \brief Add a feature to the layer.
\param feature A pointer to an sfa::Feature object. The layer copies the feature and returns a pointer to the copy.
*/
        virtual sfa::Feature *addFeature(sfa::Feature *feature);
        virtual bool updateFeature(sfa::Feature *feature);
        virtual bool deleteFeature(sfa::Feature *feature);

        virtual int getFeatureCount(void);

        virtual bool getExtent(double &left, double &bottom, double &right, double &top, bool forceQuery);

        virtual std::string getName() const;
        virtual void setName(const std::string &name);

        virtual bool supportsMultipleCursors()
        {
            return false;
        }

        virtual void deleteFeaturesWhere(const std::string &whereClause) = 0;

    };

}