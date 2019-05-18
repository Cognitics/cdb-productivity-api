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
#include "sfa/Feature.h"
#include "sfa/Layer.h"
#include "sfa/EnvelopeCheck.h"

namespace sfa {

     void Layer::addAttributeFilter(const std::string key, const std::string value,int cursorId)
     {
         attributeFilters[cursorId].push_back(attributeValuePair_t(key,value));
     }
     
     void Layer::clearAttributeFilters(int cursorId)
     {
         attributeFilters[cursorId].clear();
     }

     Layer::~Layer(void)
     {
         for(size_t i=0,ic=features.size();i<ic;i++)
         {
             sfa::Feature *feature = features.at(i);
             if(feature)
                 delete feature;
         }
     }
        
     Layer::Layer()
     {
         layerType = sfa::wkbUnknownSFA;
         resetReading();
     }

     sfa::WKBGeometryType Layer::getType(void)
     {
         return layerType;
     }
        
    // ****** Field Methods
    // returns ccl::Variant::TYPE_EMPTY if the field isn't defined, else returns the field's type
    int Layer::hasField(std::string name) const
    {
        for(size_t i=0,ic=fields.size();i<ic;i++)
        {
            FieldDefinition field = fields.at(i);
            if(field.name==name)
            {
                return field.type;
            }
        }
        return ccl::Variant::TYPE_EMPTY;
    }
         
    bool Layer::addField(std::string name, int type)
    {
        for(size_t i=0,ic=fields.size();i<ic;i++)
        {
            FieldDefinition field = fields.at(i);
            if(field.name==name)
            {
                if(type==field.type)
                    return true;
                return false;
            }
        }
        FieldDefinition field;
        field.name = name;
        field.type = type;
        fields.push_back(field);
        return true;
    }
         
    bool Layer::removeField(std::string field)
    {
        std::vector<FieldDefinition>::iterator iter = fields.begin();
        while(iter!=fields.end())
        {
            if(field== (*iter).name)
            {
                fields.erase(iter);
                return true;
            }
            iter++;
        }
        return false;
    }

    size_t Layer::getNumFields() const
    {
        return fields.size();
    }
    
    FieldDefinition Layer::getFieldAt(size_t i) const
    {
        if(i<1 || i>=fields.size())
        {
            FieldDefinition ret;
            return ret;
        }
        return fields.at(i);
    }

    FieldDefinition Layer::getFieldAt(std::string name) const
    {
        for(size_t i=0,ic=fields.size();i<ic;i++)
        {
            FieldDefinition field = fields.at(i);
            if(field.name==name)
            {
                return field;
            }
        }
        FieldDefinition ret;
        return ret;
    }
    bool Layer::getIngoreField(std::string field)
    {
        return ignoreFields[field];
    }

    void Layer::setIgnoreField(std::string field, bool ignore)
    {
        ignoreFields[field] = ignore;
    }
    

    cts::CS_CoordinateSystem *Layer::getCoordinateSystem(void)
    {
        return cs;
    }

    void Layer::setCoordinateSystem(cts::CS_CoordinateSystem *cs)
    {
        this->cs = cs;
    }

    void Layer::setSpatialFilter(int cursorId)
    {
        spatialFilter.clearPoints();
    }

    void Layer::setSpatialFilter(double west, double south, double east, double north,int cursorId)
    {
        spatialFilter.clearPoints();
        spatialFilter.addPoint(sfa::Point(west,south));
        spatialFilter.addPoint(sfa::Point(east,north));
    }
        
    bool Layer::beginUpdating()
    {
        pendingUpdates = true;
        return true;
    }
    
    bool Layer::commitUpdates()
    {
        if(!pendingUpdates)
            return false;
        std::map<sfa::Feature *,sfa::Feature *>::iterator iter = pendingUpdatesMap.begin();
        if(iter!=pendingUpdatesMap.end())
        {
            *featureCopyMap[iter->first] = *iter->second;
            iter++;
        }
        pendingUpdates = false;
        return true;
    }
    
    bool Layer::rollbackUpdates()
    {
        bool prevState = pendingUpdates;
        pendingUpdates = false;
        pendingUpdatesMap.clear();
        return prevState;
    }
    
    void Layer::resetReading(int cursorId)
    {
        iter_pos = 0;
    }
    
    sfa::Feature *Layer::getNextFeature(int cursorId)
    {
        //TODO: support for multiple cursors, or at least an error if
        // a cursor other than 0 is specified
        do
        {
            if((iter_pos+1)>=features.size())
            {
                return NULL;
            }
            iter_pos++;
            sfa::Feature *ro_feature = features.at(iter_pos);
            if(spatialFilter.getNumPoints()>1)
            {
                if(!ro_feature->geometry)
                    break;
                if(EnvelopeCheck::apply(ro_feature->geometry,&spatialFilter))
                {
                    break;
                }
            }
            else
            {
                bool passedAttributeFilter = true;
                for(size_t i=0,ic=attributeFilters[cursorId].size();i<ic;i++)
                {
                    attributeValuePair_t filter = attributeFilters[cursorId].at(i);
                    if(!ro_feature->attributes.hasAttribute(filter.first))
                    {
                        passedAttributeFilter = false;
                        break;
                    }
                    else
                    {
                        if(filter.second!=ro_feature->attributes.getAttributeAsString(filter.first))
                        {
                            passedAttributeFilter = false;
                            break;
                        }
                    }

                }
                if(passedAttributeFilter)
                    break;
            }
        }while(true);
        if((iter_pos)>=features.size())
        {
            return NULL;
        }
        // Make a copy!
        sfa::Feature *feature_copy = new sfa::Feature(*features.at(iter_pos));
        // Track the pointer to map to the original
        featureCopyMap[feature_copy] = features.at(iter_pos);
        return feature_copy;
    }
    
    sfa::Feature * Layer::addFeature(sfa::Feature *feature)
    {
        sfa::Feature *feature2 = new sfa::Feature();
        feature2->attributes = feature->attributes;
        feature2->geometry = feature->geometry->copy();
        if (layerType == sfa::wkbUnknownSFA)
            layerType = feature2->geometry->getWKBGeometryType();
        
        if(feature2->geometry->getWKBGeometryType()==layerType)
        {
            features.push_back(feature2);
            //make a third copy to return, the caller owns the returned pointer.
            sfa::Feature *feature3 = new sfa::Feature();
            feature3->attributes = feature->attributes;
            feature3->geometry = feature->geometry->copy();
            return feature3;
        }
        else
        {
            return NULL;
        }
    }
    
    bool Layer::updateFeature(sfa::Feature *feature)
    {
        // geometry type must match!
        if(feature->geometry && feature->geometry->getWKBGeometryType()!=layerType)
        {
            return false;
        }
        if(featureCopyMap.find(feature)!=featureCopyMap.end())
        {
            if(pendingUpdates)
            {                
                pendingUpdatesMap[featureCopyMap[feature]] = feature;    
            }
            else
            {
                *featureCopyMap[feature] = *feature;//copy
            }
        }
        return true;
    }
    
    bool Layer::deleteFeature(sfa::Feature *feature)
    {
        if(featureCopyMap.find(feature)!=featureCopyMap.end())
        {
            std::vector<sfa::Feature *>::iterator iter = features.begin();
            while(iter!=features.end())
            {
                if(*iter == featureCopyMap[feature])
                {
                    features.erase(iter);
                    return true;
                }
            }
        }
        return false;//not found
    }

    int Layer::getFeatureCount(void)
    {
        return (int)features.size();
    }

    bool Layer::getExtent(double &left, double &bottom, double &right, double &top, bool forceQuery)
    {
        bool ret = false;
        for(size_t i=0,ic=features.size();i<ic;i++)
        {
             sfa::Feature *feature = features.at(i);
             if(feature)
             {
                 if(feature->geometry)
                 {
                     sfa::LineString *env = dynamic_cast<sfa::LineString *>(feature->geometry->getEnvelope());
                     sfa::Point *lsMin = dynamic_cast<sfa::Point *>(env->getPointN(0));
                     sfa::Point *lsMax = dynamic_cast<sfa::Point *>(env->getPointN(1));
                     if(lsMin && lsMax)
                     {
                         if(i==0)
                         {
                             left = lsMin->X();
                             bottom = lsMin->Y();
                             right = lsMax->X();
                             top = lsMax->Y();
                         }
                         else
                         {
                             left = std::min<double>(left,lsMin->X());
                             bottom = std::min<double>(bottom,lsMin->Y());
                             right = std::max<double>(right,lsMax->X());
                             top = std::max<double>(top,lsMax->Y());
                         }
                     }
                     ret = true;
                 }
             }
        }
        return ret;
    }

    std::string Layer::getName() const
    {
        return name;
    }
    
    void Layer::setName(const std::string &name)
    {
        this->name = name;
    }

    


}