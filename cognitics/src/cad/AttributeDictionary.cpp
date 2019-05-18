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

//#pragma optimize( "", off )

#include "cad/AttributeDictionary.h"

namespace cad
{
    AttributeDictionary dictionary;

    void AttributeDictionary::enableEDCSDictionary()
    {
        setDictionaryEntry(CI_TRAFFICDIRECTION,"","");
    }

    void AttributeDictionary::resetDefaultDictionary()
    {
        setDictionaryEntry(CI_CROSSSECTIONFILENAME, "CIcsm", "CI_CrossSectionFilename");
        setDictionaryEntry(CI_ELEVATEDCROSSSECTIONFILENAME, "CIzcsm","CI_ElevatedCrossSectionFilename");
        setDictionaryEntry(CI_PRIORITY, "CIpri", "CI_Priority");
        setDictionaryEntry(CI_UUID, "CIuuid", "CI_UUID");
        setDictionaryEntry(CI_WIDTH, "CIwidth", "CI_Width");
        setDictionaryEntry(CI_HEIGHT, "CIheight", "CI_Height");
        setDictionaryEntry(CI_NEXTFEATURE, "CInext", "CI_NextFeature");
        setDictionaryEntry(CI_PREVIOUSFEATURE, "CIprev", "CI_PreviousFeature");
        setDictionaryEntry(CI_FLAGS, "CIflags", "CI_Flags");
        setDictionaryEntry(CI_NUMBEROFLANES, "CIlanes", "CI_NumberOfLanes");
        setDictionaryEntry(CI_TRAFFICDIRECTION, "CIdir", "CI_TrafficDirection");
        setDictionaryEntry(CI_FREEWAY, "CIfwy", "CI_Freeway");
        setDictionaryEntry(CI_RAMP, "CIramp", "CI_Ramp");
        setDictionaryEntry(CI_ELEVATIONHINT, "CIehint", "CI_ElevationHint");
        setDictionaryEntry(CI_SIDEWALK, "CIwalk", "CI_Sidewalk");
        setDictionaryEntry(CI_BRIDGE, "CIbridge", "CI_Bridge");
        setDictionaryEntry(CI_TUNNEL, "CItunnel", "CI_Tunnel");
        setDictionaryEntry(CI_ENDLEFTTURN, "CIelt", "CI_EndLeftTurn");
        setDictionaryEntry(CI_ENDRIGHTTURN, "CIert", "CI_EndRightTurn");
        setDictionaryEntry(CI_STARTLEFTTURN, "CIslt", "CI_StartLeftTurn");
        setDictionaryEntry(CI_STARTRIGHTTURN, "CIsrt", "CI_StartRightTurn");
        setDictionaryEntry(CI_OUTBOUNDLANES, "CIolanes", "CI_OutboundLanes");
        setDictionaryEntry(CI_INBOUNDLANES, "CIilanes", "CI_InboundLanes");
        setDictionaryEntry(CI_FEATURE_TYPE, "CItype", "CI_FeatureType");
        setDictionaryEntry(CI_PREVIOUSANGLE, "CIpreva", "CI_PreviousAngle");
        setDictionaryEntry(CI_PREVIOUSOFFSET, "CIprevo", "CI_PreviousOffset");
        setDictionaryEntry(CI_NEXTANGLE, "CInexta", "CI_NextAngle");
        setDictionaryEntry(CI_NEXTOFFSET, "CInexto", "CI_NextOffset");
        setDictionaryEntry(CI_ROADCATEGORY, "CIcat", "CI_RoadCategory");
        setDictionaryEntry(CI_ROADENVIRONMENT, "CIenv", "CI_RoadEnvironment");
        setDictionaryEntry(CI_SPEEDLIMIT, "CIspeed", "CI_SpeedLimit");
        setDictionaryEntry(CI_EXTERNALMODEL, "CIxmodel", "CI_ExternalModel");
        setDictionaryEntry(CI_EXTERNALMODELWIDTH, "CIxwidth", "CI_ExternalModelWidth");
        setDictionaryEntry(CI_EXTERNALMODELWIDTH0, "CIxwid0", "CI_ExternalModelWidth0");
        setDictionaryEntry(CI_EXTERNALMODELWIDTHN, "CIxwidN", "CI_ExternalModelWidthN");
        setDictionaryEntry(CI_CUTFILL, "CIcutfll", "CI_CutFill");
        setDictionaryEntry(CI_ABOVEGROUNDLEVEL, "CIagl", "CI_AboveGroundLevel");
        setDictionaryEntry(CI_ELEVATED, "CIelev", "CI_Elevated");
        setDictionaryEntry(CI_OWNER, "CIowner", "CI_Owner");        
        setDictionaryEntry(CI_ELEVOPT, "CIelopt", "CI_ElevationOptimizer");        
        setDictionaryEntry(CI_SURFACE, "CIsurf", "CI_Surface");
        setDictionaryEntry(CI_TRAFFICCONTROLDEVICE0, "CItcd0", "CI_TrafficConrolDevice0");
        setDictionaryEntry(CI_TRAFFICCONTROLDEVICEN, "CItcdN", "CI_TrafficConrolDeviceN");
        setDictionaryEntry(CI_ROADMARK, "CImark", "CI_RoadMark");
        setDictionaryEntry(CI_FID, "CIfid", "CI_fid");
        setDictionaryEntry(CI_SOURCE, "CIsrc", "CI_Source");
        setDictionaryEntry(CI_ADJACENT, "CIadj", "CI_Adjacent");
        setDictionaryEntry(CI_TUNNELSKIP, "CItnskip", "CI_TunnelSkip");
        setDictionaryEntry(CI_FILENAME, "CIfile", "CI_Filename");
        setDictionaryEntry(CI_CONTOUR, "CIcntour", "CI_Contour");
        setDictionaryEntry(CI_FORD, "CIford", "CI_Ford");
        setDictionaryEntry(CI_CONFLICTED, "CIconfl", "CI_Conflicted");
        setDictionaryEntry(CI_STRAIGHT, "CIstrait", "CI_Straight");
        setDictionaryEntry(CI_QUERY_INCLUDE, "CIqincl", "CI_QueryInclude");
        setDictionaryEntry(CI_QUERY_EXCLUDE, "CIqexcl", "CI_QueryExclude");
        setDictionaryEntry(CI_OUTPUT_DIRECTORY, "CIoutdir", "CI_OutputDirectory");
    }

    void AttributeDictionary::setDictionaryEntry(int id, std::string shortkey, std::string longkey)
    {
        shortKeyMap[id] = shortkey;
        longKeyMap[id] = longkey;
    }

    std::string AttributeDictionary::getMapKey(const ccl::AttributeContainer &attributes, int key) const
    {
        if(caseSensitive)
            return getKey(key);
        std::string cikey = getKey(key);
        std::transform(cikey.begin(), cikey.end(), cikey.begin(), ::tolower); 
        std::vector<std::string> keys = attributes.getKeys();
        for(std::vector<std::string>::iterator it = keys.begin(), end = keys.end(); it != end; ++it)
        {
            std::string attrkey = *it;
            std::transform(attrkey.begin(), attrkey.end(), attrkey.begin(), ::tolower); 
            if(cikey == attrkey)
                return *it;
            //    return attributes.getAttributeAsString(*it);
        }
        return cikey;
    }

    AttributeDictionary::~AttributeDictionary(void)
    {
    }

    AttributeDictionary::AttributeDictionary(bool shortNames, bool caseSensitive) : shortNames(shortNames), caseSensitive(caseSensitive)
    {
        resetDefaultDictionary();
    }

    bool AttributeDictionary::getShortNames(void) const
    {
        return shortNames;
    }

    void AttributeDictionary::setShortNames(bool shortNames)
    {
        this->shortNames = shortNames;
    }

    bool AttributeDictionary::getCaseSensitive(void) const
    {
        return caseSensitive;
    }

    void AttributeDictionary::setCaseSensitive(bool caseSensitive)
    {
        this->caseSensitive = caseSensitive;
    }

    std::string AttributeDictionary::getKey(int key) const
    {
        std::map<int, std::string>::const_iterator find_iter;
        if(shortNames)
        {
            find_iter = shortKeyMap.find(key);
            if(find_iter!=shortKeyMap.end())
            {
                return find_iter->second;
            }
        }
        else
        {
            find_iter = longKeyMap.find(key);
            if(find_iter!=longKeyMap.end())
            {
                return find_iter->second;
            }
        }
        return "";
    }

    bool AttributeDictionary::hasAttribute(const ccl::AttributeContainer &attributes, int key) const
    {
        return attributes.hasAttribute(getKey(key),!caseSensitive);
    }

    ccl::Variant AttributeDictionary::getAttribute(const ccl::AttributeContainer &attributes, int key, const ccl::Variant &defaultValue) const
    {
        std::string mapKey = getMapKey(attributes, key);
        if(mapKey.empty())
            return defaultValue;
        ccl::Variant result = attributes.getAttributeAsVariant(mapKey,!caseSensitive);
        if(result.empty())
            return defaultValue;
        return result;
    }

    bool AttributeDictionary::setAttribute(ccl::AttributeContainer &attributes, int key, const ccl::Variant &value) const
    {
        std::string mapKey = getMapKey(attributes, key);
        if(mapKey.empty())
            return false;
        attributes.setAttribute(mapKey,value,!caseSensitive);
        return true;
    }

    bool AttributeDictionary::removeAttribute(ccl::AttributeContainer &attributes, int key) const
    {
        std::string mapKey = getMapKey(attributes, key);
        if(mapKey.empty())
            return false;
        attributes.removeAttribute(mapKey,!caseSensitive);
        return true;
    }


    std::string AttributeDictionary::getCrossSectionFilenameKey(void) const
    {
        return getKey(CI_CROSSSECTIONFILENAME);
    }

    std::string AttributeDictionary::getCrossSectionFilename(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getCrossSectionFilenameKey(),!caseSensitive);        
    }

    bool AttributeDictionary::setCrossSectionFilename(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getCrossSectionFilenameKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeCrossSectionFilename(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getCrossSectionFilenameKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getElevatedCrossSectionFilenameKey(void) const
    {
        return getKey(CI_ELEVATEDCROSSSECTIONFILENAME);
    }

    std::string AttributeDictionary::getElevatedCrossSectionFilename(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getElevatedCrossSectionFilenameKey(),!caseSensitive);
    }

    bool AttributeDictionary::setElevatedCrossSectionFilename(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getElevatedCrossSectionFilenameKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeElevatedCrossSectionFilename(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getElevatedCrossSectionFilenameKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getPriorityKey(void) const
    {
        return getKey(CI_PRIORITY);
    }

    ccl::int32_t AttributeDictionary::getPriority(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getPriorityKey(),!caseSensitive,0);
    }

    bool AttributeDictionary::setPriority(ccl::AttributeContainer &attributes, ccl::int32_t value)
    {
        attributes.setAttribute(getPriorityKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removePriority(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getPriorityKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getUUIDKey(void) const
    {
        return getKey(CI_UUID);
    }

    ccl::uuid AttributeDictionary::getUUID(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsVariant(getUUIDKey(),!caseSensitive).as_uuid();
    }

    bool AttributeDictionary::setUUID(ccl::AttributeContainer &attributes, ccl::uuid value)
    {
        attributes.setAttribute(getUUIDKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeUUID(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getUUIDKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getWidthKey(void) const
    {
        return getKey(CI_WIDTH);
    }

    double AttributeDictionary::getWidth(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsDouble(getWidthKey(),!caseSensitive);        
    }

    bool AttributeDictionary::setWidth(ccl::AttributeContainer &attributes, double value)
    {
        attributes.setAttribute(getWidthKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeWidth(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getWidthKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getHeightKey(void) const
    {
        return getKey(CI_HEIGHT);
    }

    double AttributeDictionary::getHeight(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsDouble(getHeightKey(),!caseSensitive);
    }

    bool AttributeDictionary::setHeight(ccl::AttributeContainer &attributes, double value)
    {
        attributes.setAttribute(getHeightKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeHeight(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getHeightKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getNextFeatureKey(void) const
    {
        return getKey(CI_NEXTFEATURE);
    }

    ccl::uuid AttributeDictionary::getNextFeature(const ccl::AttributeContainer &attributes) const
    {
        ccl::Variant var;
        if(attributes.getAttribute(getNextFeatureKey(),var,!caseSensitive))
        {
            return var.as_uuid();
        }
        else
        {
            return ccl::uuid();
        }
    }

    bool AttributeDictionary::setNextFeature(ccl::AttributeContainer &attributes, ccl::uuid value)
    {
        attributes.setAttribute(getNextFeatureKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeNextFeature(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getNextFeatureKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getPreviousFeatureKey(void) const
    {
        return getKey(CI_PREVIOUSFEATURE);
    }

    ccl::uuid AttributeDictionary::getPreviousFeature(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsVariant(getPreviousFeatureKey(),!caseSensitive).as_uuid();
    }

    bool AttributeDictionary::setPreviousFeature(ccl::AttributeContainer &attributes, ccl::uuid value)
    {
        attributes.setAttribute(getPreviousFeatureKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removePreviousFeature(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getPreviousFeatureKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getFlagsKey(void) const
    {
        return getKey(CI_FLAGS);
    }

    ccl::int32_t AttributeDictionary::getFlags(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getFlagsKey(),!caseSensitive,0);
    }

    bool AttributeDictionary::setFlags(ccl::AttributeContainer &attributes, ccl::int32_t value)
    {
        attributes.setAttribute(getFlagsKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeFlags(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getFlagsKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getNumberOfLanesKey(void) const
    {
        return getKey(CI_NUMBEROFLANES);
    }

    ccl::int32_t AttributeDictionary::getNumberOfLanes(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getNumberOfLanesKey(),!caseSensitive);
    }

    bool AttributeDictionary::setNumberOfLanes(ccl::AttributeContainer &attributes, ccl::int32_t value)
    {
        attributes.setAttribute(getNumberOfLanesKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeNumberOfLanes(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getNumberOfLanesKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getTrafficDirectionKey(void) const
    {
        return getKey(CI_TRAFFICDIRECTION);
    }

    ccl::int32_t AttributeDictionary::getTrafficDirection(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getTrafficDirectionKey(),!caseSensitive,TRAFFIC_DIRECTION_UNKNOWN);
    }

    bool AttributeDictionary::setTrafficDirection(ccl::AttributeContainer &attributes, ccl::int32_t value)
    {
        if((value < TRAFFIC_DIRECTION_UNKNOWN) || (value > TRAFFIC_DIRECTION_BACKWARD))
            return false;
        attributes.setAttribute(getTrafficDirectionKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeTrafficDirection(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getTrafficDirectionKey());
    }

    std::string AttributeDictionary::getFreewayKey(void) const
    {
        return getKey(CI_FREEWAY);
    }

    bool AttributeDictionary::getFreeway(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getFreewayKey(),!caseSensitive,0)!=0);        
    }

    bool AttributeDictionary::setFreeway(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getFreewayKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeFreeway(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getFreewayKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getRampKey(void) const
    {
        return getKey(CI_RAMP);
    }

    bool AttributeDictionary::getRamp(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getRampKey(),!caseSensitive,0)!=0);
    }

    bool AttributeDictionary::setRamp(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getRampKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeRamp(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getRampKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getElevationHintKey(void) const
    {
        return getKey(CI_ELEVATIONHINT);
    }

    ccl::int32_t AttributeDictionary::getElevationHint(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getElevationHintKey(),!caseSensitive, 999);
    }

    bool AttributeDictionary::setElevationHint(ccl::AttributeContainer &attributes, ccl::int32_t value)
    {
        attributes.setAttribute(getElevationHintKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeElevationHint(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getElevationHintKey(),!caseSensitive);
    }    
    
    std::string AttributeDictionary::getTunnelKey(void) const
    {
        return getKey(CI_TUNNEL);
    }

    bool AttributeDictionary::getTunnel(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getTunnelKey(),!caseSensitive, 0)!=0);        
    }

    bool AttributeDictionary::setTunnel(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getTunnelKey(), value ? 1 : 0, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeTunnel(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getTunnelKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getBridgeKey(void) const
    {
        return getKey(CI_BRIDGE);
    }

    bool AttributeDictionary::getBridge(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getBridgeKey(),!caseSensitive, 0)!=0);        
    }

    bool AttributeDictionary::setBridge(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getBridgeKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeBridge(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getBridgeKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getFordKey(void) const
    {
        return getKey(CI_FORD);
    }

    bool AttributeDictionary::getFord(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getFordKey(),!caseSensitive, 0)!=0);        
    }

    bool AttributeDictionary::setFord(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getFordKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeFord(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getFordKey(),!caseSensitive);
    }


    int AttributeDictionary::GetTurnLaneCount(const ccl::AttributeContainer &attributes, bool startPoint, bool left) const
    {
        std::string key = GetTurnLaneCountKey(startPoint,left);
        return attributes.getAttributeAsInt(key,!caseSensitive,0);        
    }

    void AttributeDictionary::setTurnLaneCount(ccl::AttributeContainer &attributes, bool startPoint, bool left, int count)
    {
        attributes.setAttribute(GetTurnLaneCountKey(startPoint,left), count, !caseSensitive);
    }

    void AttributeDictionary::RemoveTurnLaneCount(ccl::AttributeContainer &attributes, bool startPoint, bool left)
    {
        attributes.removeAttribute(GetTurnLaneCountKey(startPoint,left),!caseSensitive);
    }
    std::string AttributeDictionary::GetTurnLaneCountKey(bool startPoint, bool left, bool shortkey) const
    {
        if(startPoint)
        {
            if(left)
                return getKey(CI_STARTLEFTTURN);
            else
                return getKey(CI_STARTRIGHTTURN);
        }
        else
        {
            if(left)
                return getKey(CI_ENDLEFTTURN);
            else
                return getKey(CI_ENDRIGHTTURN);
        }

    }

    //in/out lane count
    //Returns the inbound, outbound, or both lane counts using the specific lane count attr.
    int AttributeDictionary::GetLaneCount(const ccl::AttributeContainer &attributes, bool inbound, bool outbound) const
    {
        int inboundcount = 0;
        int outboundcount = 0;
        ccl::Variant var;
        if(inbound && attributes.getAttribute(GetLaneCountInboundKey(),var,!caseSensitive))
        {
            inboundcount = var.as_int();
        }
        if(outbound && attributes.getAttribute(GetLaneCountOutboundKey(),var,!caseSensitive))
        {
            outboundcount = var.as_int();
        }
        return inboundcount + outboundcount;
    }

    std::string AttributeDictionary::GetLaneCountOutboundKey() const
    {
        return getKey(CI_OUTBOUNDLANES);
    }

    std::string AttributeDictionary::GetLaneCountInboundKey() const
    {
        return getKey(CI_INBOUNDLANES);
    }
    
    //set inbound to true to set outbound
    void AttributeDictionary::SetLaneCount(ccl::AttributeContainer &attributes, bool inbound, int count)
    {
        if(inbound)
            attributes.setAttribute(GetLaneCountInboundKey(), count,!caseSensitive);
        else
            attributes.setAttribute(GetLaneCountOutboundKey(), count,!caseSensitive);
    }

    void AttributeDictionary::RemoveLaneCount(ccl::AttributeContainer &attributes, bool inbound)
    {
        if(inbound)
            attributes.removeAttribute(GetLaneCountInboundKey(),!caseSensitive);
        else
            attributes.removeAttribute(GetLaneCountOutboundKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getFeatureTypeKey(void) const
    {
        return getKey(CI_FEATURE_TYPE);
    }

    std::string AttributeDictionary::getFeatureType(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getFeatureTypeKey(),!caseSensitive);
    }

    bool AttributeDictionary::setFeatureType(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getFeatureTypeKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeFeatureType(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getFeatureTypeKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getPreviousAngleKey(void) const
    {
        return getKey(CI_PREVIOUSANGLE);
    }

    double AttributeDictionary::getPreviousAngle(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsDouble(getPreviousAngleKey(),!caseSensitive,0);
    }

    bool AttributeDictionary::setPreviousAngle(ccl::AttributeContainer &attributes, double value) 
    {
        attributes.setAttribute(getPreviousAngleKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removePreviousAngle(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getPreviousAngleKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getPreviousOffsetKey(void) const
    {
        return getKey(CI_PREVIOUSOFFSET);
    }

    double AttributeDictionary::getPreviousOffset(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsDouble(getPreviousOffsetKey(),!caseSensitive,0);
    }

    bool AttributeDictionary::setPreviousOffset(ccl::AttributeContainer& attributes, double value)
    {
        attributes.setAttribute(getPreviousOffsetKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removePreviousOffset(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getPreviousOffsetKey());
    }

    std::string AttributeDictionary::getNextAngleKey(void) const
    {
        return getKey(CI_NEXTANGLE);
    }

    double AttributeDictionary::getNextAngle(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsDouble(getNextAngleKey(), !caseSensitive,0);
    }

    bool AttributeDictionary::setNextAngle(ccl::AttributeContainer& attributes, double value)
    {
        attributes.setAttribute(getNextAngleKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeNextAngle(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getNextAngleKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getNextOffsetKey(void) const
    {
        return getKey(CI_NEXTOFFSET);
    }

    double AttributeDictionary::getNextOffset(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsDouble(getNextOffsetKey(), !caseSensitive,0);
    }

    bool AttributeDictionary::setNextOffset(ccl::AttributeContainer& attributes, double value)
    {
        attributes.setAttribute(getNextOffsetKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeNextOffset(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getNextOffsetKey(), !caseSensitive);
    }
    
    std::string AttributeDictionary::getRoadCategoryKey(void) const
    {
        return getKey(CI_ROADCATEGORY);
    }

    std::string AttributeDictionary::getRoadCategory(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsString(getRoadCategoryKey(), !caseSensitive, "any");
    }

    bool AttributeDictionary::setRoadCategory(ccl::AttributeContainer& attributes, std::string value)
    {
        attributes.setAttribute(getRoadCategoryKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeRoadCategory(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getRoadCategoryKey(), !caseSensitive);
    }
    
    std::string AttributeDictionary::getRoadEnvironmentKey(void) const
    {
        return getKey(CI_ROADENVIRONMENT);
    }

    std::string AttributeDictionary::getRoadEnvironment(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsString(getRoadEnvironmentKey(), !caseSensitive,"any");
    }

    bool AttributeDictionary::setRoadEnvironment(ccl::AttributeContainer& attributes, std::string value)
    {
        attributes.setAttribute(getRoadEnvironmentKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeRoadEnvironment(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getRoadEnvironmentKey(), !caseSensitive);
    }
    
    std::string AttributeDictionary::getSpeedLimitKey(void) const
    {
        return getKey(CI_SPEEDLIMIT);
    }

    int AttributeDictionary::getSpeedLimit(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsDouble(getSpeedLimitKey(), !caseSensitive);
    }

    bool AttributeDictionary::setSpeedLimit(ccl::AttributeContainer& attributes, int value)
    {
        attributes.setAttribute(getSpeedLimitKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeSpeedLimit(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getSpeedLimitKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getExternalModelKey(void) const
    {
        return getKey(CI_EXTERNALMODEL);
    }

    std::string AttributeDictionary::getExternalModel(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getExternalModelKey(), !caseSensitive);
    }

    bool AttributeDictionary::setExternalModel(ccl::AttributeContainer &attributes, std::string value)
    {
        attributes.setAttribute(getExternalModelKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeExternalModel(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getExternalModelKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getExternalModelWidthKey(void) const
    {
        return getKey(CI_EXTERNALMODELWIDTH);
    }

    double AttributeDictionary::getExternalModelWidth(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsDouble(getExternalModelWidthKey(), !caseSensitive);
    }

    bool AttributeDictionary::setExternalModelWidth(ccl::AttributeContainer &attributes, double value)
    {
        attributes.setAttribute(getExternalModelWidthKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeExternalModelWidth(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getExternalModelWidthKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getExternalModelWidth0Key(void) const
    {
        return getKey(CI_EXTERNALMODELWIDTH0);
    }

    double AttributeDictionary::getExternalModelWidth0(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsDouble(getExternalModelWidth0Key(), !caseSensitive);
    }

    bool AttributeDictionary::setExternalModelWidth0(ccl::AttributeContainer &attributes, double value)
    {
        attributes.setAttribute(getExternalModelWidth0Key(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeExternalModelWidth0(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getExternalModelWidth0Key(), !caseSensitive);
    }

    std::string AttributeDictionary::getExternalModelWidthNKey(void) const
    {
        return getKey(CI_EXTERNALMODELWIDTHN);
    }

    double AttributeDictionary::getExternalModelWidthN(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsDouble(getExternalModelWidthNKey(), !caseSensitive);
    }

    bool AttributeDictionary::setExternalModelWidthN(ccl::AttributeContainer &attributes, double value)
    {
        attributes.setAttribute(getExternalModelWidthNKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeExternalModelWidthN(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getExternalModelWidthNKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getCutFillKey(void) const
    {
        return getKey(CI_CUTFILL);
    }

    int AttributeDictionary::getCutFill(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsInt(getCutFillKey(), !caseSensitive, CUTFILL_NORMAL);
    }

    bool AttributeDictionary::setCutFill(ccl::AttributeContainer& attributes, int value)
    {
        attributes.setAttribute(getCutFillKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeCutFill(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getCutFillKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getAboveGroundLevelKey(void) const
    {
        return getKey(CI_ABOVEGROUNDLEVEL);
    }

    bool AttributeDictionary::getAboveGroundLevel(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getAboveGroundLevelKey(),!caseSensitive,0)!=0);
    }

    bool AttributeDictionary::setAboveGroundLevel(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getAboveGroundLevelKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeAboveGroundLevel(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getAboveGroundLevelKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getElevatedKey(void) const
    {
        return getKey(CI_ELEVATED);
    }

    bool AttributeDictionary::getElevated(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getElevatedKey(),!caseSensitive,0)!=0);
    }

    bool AttributeDictionary::setElevated(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getElevatedKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeElevated(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getElevatedKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getConflictedKey(void) const
    {
        return getKey(CI_CONFLICTED);
    }

    bool AttributeDictionary::getConflicted(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getConflictedKey(),!caseSensitive,0)!=0);
    }

    bool AttributeDictionary::setConflicted(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getConflictedKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeConflicted(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getConflictedKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getOwnerKey(void) const
    {
        return getKey(CI_OWNER);
    }


    ccl::uuid AttributeDictionary::getOwner(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsVariant(getOwnerKey(),!caseSensitive).as_uuid();
    }

    bool AttributeDictionary::setOwner(ccl::AttributeContainer &attributes, const ccl::uuid& value)
    {
        attributes.setAttribute(getOwnerKey(), ccl::Variant(value));
        return true;
    }

    void AttributeDictionary::removeOwner(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getOwnerKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getSurfaceKey(void) const
    {
        return getKey(CI_SURFACE);
    }

    std::string AttributeDictionary::getSurface(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getSurfaceKey(), !caseSensitive);
    }

    bool AttributeDictionary::setSurface(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getSurfaceKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeSurface(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getSurfaceKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getTrafficControlDevice0Key(void) const
    {
        return getKey(CI_TRAFFICCONTROLDEVICE0);
    }

    int AttributeDictionary::getTrafficControlDevice0(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getTrafficControlDevice0Key(), !caseSensitive, 0);
    }

    bool AttributeDictionary::setTrafficControlDevice0(ccl::AttributeContainer &attributes, int value)
    {
        attributes.setAttribute(getTrafficControlDevice0Key(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeTrafficControlDevice0(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getTrafficControlDevice0Key(), !caseSensitive);
    }

    std::string AttributeDictionary::getTrafficControlDeviceNKey(void) const
    {
        return getKey(CI_TRAFFICCONTROLDEVICEN);
    }

    int AttributeDictionary::getTrafficControlDeviceN(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getTrafficControlDeviceNKey(), !caseSensitive, 0);
    }

    bool AttributeDictionary::setTrafficControlDeviceN(ccl::AttributeContainer &attributes, int value)
    {
        attributes.setAttribute(getTrafficControlDeviceNKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeTrafficControlDeviceN(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getTrafficControlDeviceNKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getRoadMarkKey(void) const
    {
        return getKey(CI_ROADMARK);
    }

    bool AttributeDictionary::getRoadMark(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getRoadMarkKey(),!caseSensitive, 0)!=0);        
    }

    bool AttributeDictionary::setRoadMark(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getRoadMarkKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeRoadMark(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getRoadMarkKey(),!caseSensitive);
    }


    std::string AttributeDictionary::getElevationOptimizerKey(void) const
    {
        return getKey(CI_ELEVOPT);
    }

    int AttributeDictionary::getElevationOptimizer(const ccl::AttributeContainer& attributes) const
    {
        return attributes.getAttributeAsInt(getElevationOptimizerKey(), !caseSensitive, ELEVOPT_NORMAL);
    }

    bool AttributeDictionary::setElevationOptimizer(ccl::AttributeContainer& attributes, int value)
    {
        attributes.setAttribute(getElevationOptimizerKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeElevationOptimizer(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getElevationOptimizerKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getFeatureIDKey(void) const
    {
        return getKey(CI_FID);
    }

    int AttributeDictionary::getFeatureID(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsInt(getFeatureIDKey(), !caseSensitive, 0);
    }

    bool AttributeDictionary::setFeatureID(ccl::AttributeContainer &attributes, int value)
    {
        attributes.setAttribute(getFeatureIDKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeFeatureID(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getFeatureIDKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getAdjacentKey(void) const
    {
        return getKey(CI_ADJACENT);
    }

    std::string AttributeDictionary::getAdjacent(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getAdjacentKey(), !caseSensitive);
    }

    bool AttributeDictionary::setAdjacent(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getAdjacentKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeAdjacent(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getAdjacentKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getTunnelSkipKey(void) const
    {
        return getKey(CI_TUNNELSKIP);
    }

    bool AttributeDictionary::getTunnelSkip(const ccl::AttributeContainer &attributes) const
    {
        return (attributes.getAttributeAsInt(getTunnelSkipKey(),!caseSensitive, 0)!=0);        
    }

    bool AttributeDictionary::setTunnelSkip(ccl::AttributeContainer &attributes, bool value)
    {
        attributes.setAttribute(getTunnelSkipKey(), value ? 1 : 0,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeTunnelSkip(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getTunnelSkipKey(),!caseSensitive);
    }

    std::string AttributeDictionary::getFilenameKey(void) const
    {
        return getKey(CI_FILENAME);
    }

    std::string AttributeDictionary::getFilename(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getFilenameKey(), !caseSensitive);
    }

    bool AttributeDictionary::setFilename(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getFilenameKey(), value, !caseSensitive);
        return true;
    }

    void AttributeDictionary::removeFilename(ccl::AttributeContainer& attributes)
    {
        attributes.removeAttribute(getFilenameKey(), !caseSensitive);
    }

    std::string AttributeDictionary::getContourKey(void) const
    {
        return getKey(CI_CONTOUR);
    }

    std::string AttributeDictionary::getContour(const ccl::AttributeContainer &attributes) const
    {
        return attributes.getAttributeAsString(getContourKey(),!caseSensitive);
    }

    bool AttributeDictionary::setContour(ccl::AttributeContainer &attributes, const std::string &value)
    {
        attributes.setAttribute(getContourKey(), value,!caseSensitive);
        return true;
    }

    void AttributeDictionary::removeContour(ccl::AttributeContainer &attributes)
    {
        attributes.removeAttribute(getContourKey(),!caseSensitive);
    }



}
