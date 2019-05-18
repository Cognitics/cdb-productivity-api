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
/*! \page cad_page Cognitics Attribute Dictionary (CAD)

\section Description

This library provides an abstraction of feature attribution for normalized feature processing.

\section Usage



\section Notes

*/
#pragma once

#include <map>
#include <ccl/Variant.h>
#include <ccl/uuid.h>
#include <ccl/AttributeContainer.h>

namespace cad
{
    struct DictionaryEntry
    {
        int type;
        std::string shortName;
        std::string longName;
    };

    class AttributeDictionary
    {
    private:

        bool shortNames;
        bool caseSensitive;
        
        std::map<int, std::string> shortKeyMap;
        std::map<int, std::string> longKeyMap;
        // gets the actual map key for the given attribute (taking caseSensitive into account)
        std::string getMapKey(const ccl::AttributeContainer &attributes, int key) const;

    public:
        // attribute dictionary (when adding, update getKey() implementation also)
        static const int CI_CROSSSECTIONFILENAME            = 0;        // CIcsm     CI_CrossSectionFilename
        static const int CI_ELEVATEDCROSSSECTIONFILENAME    = 1;        // CIzcsm    CI_ElevatedCrossSectionFilename
        static const int CI_PRIORITY                        = 2;        // CIpri     CI_Priority
        static const int CI_UUID                            = 3;        // CIuuid    CI_UUID
        static const int CI_WIDTH                            = 4;        // CIwidth   CI_Width
        static const int CI_HEIGHT                            = 5;        // CIheight  CI_Height
        static const int CI_NEXTFEATURE                        = 6;        // CInext    CI_NextFeature
        static const int CI_PREVIOUSFEATURE                    = 7;        // CIprev    CI_PreviousFeature
        static const int CI_FLAGS                            = 8;        // CIflags   CI_Flags
        static const int CI_NUMBEROFLANES                    = 9;        // CIlanes   CI_NumberOfLanes
        static const int CI_TRAFFICDIRECTION                = 10;        // CIdir     CI_TrafficDirection
        static const int CI_FREEWAY                            = 11;        // CIfwy     CI_Freeway
        static const int CI_RAMP                            = 12;        // CIramp    CI_Ramp
        static const int CI_ELEVATIONHINT                    = 13;        // CIehint   CI_ElevationHint
        static const int CI_SIDEWALK                        = 14;        // CIwalk    CI_Sidewalk

        static const int CI_BRIDGE                            = 15;        // CIbridge  CI_Bridge
        static const int CI_TUNNEL                            = 16;        // CItunnel  CI_Tunnel
        static const int CI_ENDLEFTTURN                        = 17;        // CIelt     CI_EndLeftTurn
        static const int CI_ENDRIGHTTURN                    = 18;        // CIert     CI_EndRightTurn
        static const int CI_STARTLEFTTURN                    = 19;        // CIslt     CI_StartLeftTurn
        static const int CI_STARTRIGHTTURN                    = 20;        // CIsrt     CI_StartRightTurn
        static const int CI_OUTBOUNDLANES                    = 21;        // CIolanes     CI_OutboundLanes
        static const int CI_INBOUNDLANES                    = 22;        // CIilanes     CI_InboundLanes
        static const int CI_FEATURE_TYPE                    = 23;        // CItype     CI_FeatureType

        static const int CI_PREVIOUSANGLE                    = 24;        //    CIpreva    CI_PreviousAngle
        static const int CI_PREVIOUSOFFSET                    = 25;        //    CIprevo    CI_PreviousOffset
        static const int CI_NEXTANGLE                        = 26;        //    CInexta    CI_NextAngle
        static const int CI_NEXTOFFSET                        = 27;        //    CInexto CI_NextOffset

        static const int CI_ROADCATEGORY                    = 28;        //    CIcat CI_RoadCategory
        static const int CI_ROADENVIRONMENT                    = 29;        //    CIenv CI_RoadEnvironment

        static const int CI_SPEEDLIMIT                        = 30;        //    CIspeed CI_SpeedLimit

        static const int CI_EXTERNALMODEL                    = 31;        //    CIxmodel CI_ExternalModel
        static const int CI_EXTERNALMODELWIDTH                = 32;        //    CIxwidth CI_ExternalModelWidth
        static const int CI_EXTERNALMODELWIDTH0                = 33;        //    CIxwid0 CI_ExternalModelWidth0
        static const int CI_EXTERNALMODELWIDTHN                = 34;        //    CIxwidN CI_ExternalModelWidthN

        static const int CI_CUTFILL                            = 35;        //    CIcutfll CI_CutFill -- 0=normal, 1=roadbed only, 2=sidewall only, 3=none

        static const int CI_ABOVEGROUNDLEVEL                = 36;        // CIagl CI_AboveGroundLevel (true if positive elevation, like a ramp)
        static const int CI_ELEVATED                        = 37;        // CIelev CI_Elevated (true if elevated, like an overpass)

        static const int CI_OWNER                            = 38;        //    CIowner CI_Owner
        static const int CI_ELEVOPT                            = 39;        //    CIelopt CI_ElevationOptimizer

        static const int CI_SURFACE                            = 40;        //    CIsurf CI_Surface
        static const int CI_TRAFFICCONTROLDEVICE0            = 41;        //    CItcd0 CI_TrafficControlDevice0
        static const int CI_TRAFFICCONTROLDEVICEN            = 42;        //    CItcdN CI_TrafficControlDeviceN
        static const int CI_ROADMARK                        = 43;        //    CImark CI_RoadMark (true if the road should have marks, false otherwise)
        static const int CI_ELEV                            = 44;        //  CIelev fixed elevation value
        static const int CI_FID                                = 45;        //  CIfid Feature Identifier (DFAD FID)
        static const int CI_SOURCE                            = 46;        //  CIsrc CI_Source (source info)
        static const int CI_ADJACENT                        = 47;        //  CIadj adjacent to something ("tunnel")
        static const int CI_TUNNELSKIP                        = 48;        // CItnskip  CI_TunnelSkip
        static const int CI_FILENAME                        = 49;        // CIfile  CI_Filename
        static const int CI_CONTOUR                            = 50;        // CIcntour  CI_Contour        (string containing the synthetic elevation contour)
        static const int CI_FORD                            = 51;        // CIford  CI_Ford
        static const int CI_CONFLICTED                        = 52;        // CIconfl CI_Conflicted (true if conflicted via the elevation optimizer, like an overpass)
        static const int CI_STRAIGHT                        = 53;        // CIstrait CI_Straight (true if straight (2 vertices))
        static const int CI_QUERY_INCLUDE                    = 54;        // CIqincl CI_QueryInclude
        static const int CI_QUERY_EXCLUDE                    = 55;        // CIqexcl CI_QueryExclude
        static const int CI_OUTPUT_DIRECTORY                = 56;        // CIoutdir CI_OutputDirectory


        // CI_TrafficDirection values
        static const int TRAFFIC_DIRECTION_UNKNOWN        = 0;
        static const int TRAFFIC_DIRECTION_BOTH            = 1;
        static const int TRAFFIC_DIRECTION_FORWARD        = 2;    // direction of linear
        static const int TRAFFIC_DIRECTION_BACKWARD        = 3;    // opposite direction of linear

        // CI_RoadCategory values
        static const int ROAD_CATEGORY_ANY            = 0;
        static const int ROAD_CATEGORY_LOCAL        = 1;
        static const int ROAD_CATEGORY_COLLECTOR    = 2;
        static const int ROAD_CATEGORY_ARTERIAL        = 3;
        static const int ROAD_CATEGORY_FREEWAY        = 4;

        // CI_RoadEnvironment values
        static const int ROAD_ENVIRONMENT_ANY        = 0;
        static const int ROAD_ENVIRONMENT_RURAL        = 1;
        static const int ROAD_ENVIRONMENT_URBAN        = 2;
        // CI_CutFill values
        static const int CUTFILL_NORMAL                = 0;
        static const int CUTFILL_ROADBED_ONLY        = 1;
        static const int CUTFILL_SIDEWALL_ONLY        = 2;
        static const int CUTFILL_NONE                = 3;

        static const int TRAFFIC_CONTROL_NONE                        = 0;
        static const int TRAFFIC_CONTROL_YIELDSIGN                    = 1;
        static const int TRAFFIC_CONTROL_STOPSIGN                    = 2;
        static const int TRAFFIC_CONTROL_FLASHINGYELLOWLIGHT        = 3;
        static const int TRAFFIC_CONTROL_FLASHINGREDLIGHT            = 4;
        static const int TRAFFIC_CONTROL_NORMALLIGHT                = 5;
        static const int TRAFFIC_CONTROL_RAILWAYCROSSINGGATES        = 6;
        static const int TRAFFIC_CONTROL_ROADCLOSEDGATES            = 7;
        static const int TRAFFIC_CONTROL_TEMPORARILYCLOSEDBARRIER    = 8;
        static const int TRAFFIC_CONTROL_UNKNOWN                    = 100;

        // query include bitmask
        static const int QUERY_INCLUDE_EMPTY                        = 0x0000;
        //static const int QUERY_INCLUDE_AVAILABLE                    = 1 << 0;
        //static const int QUERY_INCLUDE_AVAILABLE                    = 1 << 1;
        //static const int QUERY_INCLUDE_AVAILABLE                    = 1 << 2;
        static const int QUERY_INCLUDE_TEST                            = 1 << 30;    // max

        // query exclude bitmask
        static const int QUERY_EXCLUDE_EMPTY                        = 0x0000;
        static const int QUERY_EXCLUDE_BRIDGETUNNEL_MODE_SKIP        = 1 << 0;
        //static const int QUERY_EXCLUDE_AVAILABLE                    = 1 << 1;
        //static const int QUERY_EXCLUDE_AVAILABLE                    = 1 << 2;
        static const int QUERY_EXCLUDE_TEST                            = 1 << 30;    // max


        // CI_ElevationOptimizer instructions
        static const int ELEVOPT_NORMAL                = 0;
        // Ignore all conflicts with this feature
        static const int ELEVOPT_IGNORE                = 1;        
        // Ignore this feature for conflicts as a primary road 
        // only process conflicts if another feature detects a conflict with this one
        // don't look for conflicts using this feature
        static const int ELEVOPT_PRIMARY_IGNORE        = 2;

        ~AttributeDictionary(void);
        AttributeDictionary(bool shortNames = true, bool caseSensitive = false);

        void enableEDCSDictionary();
        void resetDefaultDictionary();
        void setDictionaryEntry(int id, std::string shortkey, std::string longkey);

        bool getShortNames(void) const;
        void setShortNames(bool shortNames = true);

        bool getCaseSensitive(void) const;
        void setCaseSensitive(bool caseSensitive = true);

        // get the string key (short if shortNames is true; long otherwise)
        std::string getKey(int key) const;

        // returns true if the attribute is set; false otherwise (invalid key or no value)
        bool hasAttribute(const ccl::AttributeContainer &attributes, int key) const;

        // returns the attribute value if it exists; false otherwise (invalid key OR no value)
        ccl::Variant getAttribute(const ccl::AttributeContainer &attributes, int key, const ccl::Variant &defaultValue = ccl::Variant()) const;

        // returns true if attribute set successfully; false otherwise (invalid key)
        bool setAttribute(ccl::AttributeContainer &attributes, int key, const ccl::Variant &value) const;

        // returns true if attribute removed successfully; false otherwise (invalid key)
        bool removeAttribute(ccl::AttributeContainer &attributes, int key) const;

        std::string getCrossSectionFilenameKey(void) const;
        std::string getCrossSectionFilename(const ccl::AttributeContainer &attributes) const;
        bool setCrossSectionFilename(ccl::AttributeContainer &attributes, const std::string &value);
        void removeCrossSectionFilename(ccl::AttributeContainer &attributes);

        std::string getElevatedCrossSectionFilenameKey(void) const;
        std::string getElevatedCrossSectionFilename(const ccl::AttributeContainer &attributes) const;
        bool setElevatedCrossSectionFilename(ccl::AttributeContainer &attributes, const std::string &value);
        void removeElevatedCrossSectionFilename(ccl::AttributeContainer &attributes);

        std::string getPriorityKey(void) const;
        ccl::int32_t getPriority(const ccl::AttributeContainer &attributes) const;
        bool setPriority(ccl::AttributeContainer &attributes, ccl::int32_t value);
        void removePriority(ccl::AttributeContainer &attributes);

        std::string getUUIDKey(void) const;
        ccl::uuid getUUID(const ccl::AttributeContainer &attributes) const;
        bool setUUID(ccl::AttributeContainer &attributes, ccl::uuid value);
        void removeUUID(ccl::AttributeContainer &attributes);

        std::string getWidthKey(void) const;
        double getWidth(const ccl::AttributeContainer &attributes) const;
        bool setWidth(ccl::AttributeContainer &attributes, double value);
        void removeWidth(ccl::AttributeContainer &attributes);

        std::string getHeightKey(void) const;
        double getHeight(const ccl::AttributeContainer &attributes) const;
        bool setHeight(ccl::AttributeContainer &attributes, double value);
        void removeHeight(ccl::AttributeContainer &attributes);

        std::string getNextFeatureKey(void) const;
        ccl::uuid getNextFeature(const ccl::AttributeContainer &attributes) const;
        bool setNextFeature(ccl::AttributeContainer &attributes, ccl::uuid value);
        void removeNextFeature(ccl::AttributeContainer &attributes);

        std::string getPreviousFeatureKey(void) const;
        ccl::uuid getPreviousFeature(const ccl::AttributeContainer &attributes) const;
        bool setPreviousFeature(ccl::AttributeContainer &attributes, ccl::uuid value);
        void removePreviousFeature(ccl::AttributeContainer &attributes);

        std::string getFlagsKey(void) const;
        ccl::int32_t getFlags(const ccl::AttributeContainer &attributes) const;
        bool setFlags(ccl::AttributeContainer &attributes, ccl::int32_t value);
        void removeFlags(ccl::AttributeContainer &attributes);

        std::string getNumberOfLanesKey(void) const;
        ccl::int32_t getNumberOfLanes(const ccl::AttributeContainer &attributes) const;
        // Total lanes...
        bool setNumberOfLanes(ccl::AttributeContainer &attributes, ccl::int32_t value);
        void removeNumberOfLanes(ccl::AttributeContainer &attributes);

        std::string getTrafficDirectionKey(void) const;
        ccl::int32_t getTrafficDirection(const ccl::AttributeContainer &attributes) const;
        bool setTrafficDirection(ccl::AttributeContainer &attributes, ccl::int32_t value);
        void removeTrafficDirection(ccl::AttributeContainer &attributes);

        std::string getFreewayKey(void) const;
        bool getFreeway(const ccl::AttributeContainer &attributes) const;
        bool setFreeway(ccl::AttributeContainer &attributes, bool value);
        void removeFreeway(ccl::AttributeContainer &attributes);

        std::string getRampKey(void) const;
        bool getRamp(const ccl::AttributeContainer &attributes) const;
        bool setRamp(ccl::AttributeContainer &attributes, bool value);
        void removeRamp(ccl::AttributeContainer &attributes);

        std::string getElevationHintKey(void) const;
        ccl::int32_t getElevationHint(const ccl::AttributeContainer &attributes) const;
        bool setElevationHint(ccl::AttributeContainer &attributes, ccl::int32_t value);
        void removeElevationHint(ccl::AttributeContainer &attributes);

        //Is tunnel
        std::string getTunnelKey(void) const;
        bool getTunnel(const ccl::AttributeContainer &attributes) const;
        bool setTunnel(ccl::AttributeContainer &attributes, bool value);
        void removeTunnel(ccl::AttributeContainer &attributes);

        //Is bridge
        std::string getBridgeKey(void) const;
        bool getBridge(const ccl::AttributeContainer &attributes) const;
        bool setBridge(ccl::AttributeContainer &attributes, bool value);
        void removeBridge(ccl::AttributeContainer &attributes);

        //Is ford
        std::string getFordKey(void) const;
        bool getFord(const ccl::AttributeContainer &attributes) const;
        bool setFord(ccl::AttributeContainer &attributes, bool value);
        void removeFord(ccl::AttributeContainer &attributes);

        //turn lane count
        int GetTurnLaneCount(const ccl::AttributeContainer &attributes, bool startPoint, bool left) const;
        void setTurnLaneCount(ccl::AttributeContainer &attributes, bool startPoint, bool left, int count);
        void RemoveTurnLaneCount(ccl::AttributeContainer &attributes, bool startPoint, bool left);
        std::string GetTurnLaneCountKey(bool startPoint, bool left, bool shortkey=true) const;

        //in/out lane count
        int GetLaneCount(const ccl::AttributeContainer &attributes, bool inbound, bool outbound) const;//Returns the inbound, outbound, or both lane counts using the specific lane count attr.
        std::string GetLaneCountOutboundKey() const;
        std::string GetLaneCountInboundKey() const;
        void SetLaneCount(ccl::AttributeContainer &attributes, bool inbound, int count);//set inbound to true to set outbound
        void RemoveLaneCount(ccl::AttributeContainer &attributes, bool inbound);

        // Feature type (e.g. road, river, etc.)
        std::string getFeatureTypeKey(void) const;
        std::string getFeatureType(const ccl::AttributeContainer &attributes) const;
        bool setFeatureType(ccl::AttributeContainer &attributes, const std::string &value);
        void removeFeatureType(ccl::AttributeContainer &attributes);

        //previous angle
        std::string getPreviousAngleKey(void) const;
        double getPreviousAngle(const ccl::AttributeContainer &attributes) const;
        bool setPreviousAngle(ccl::AttributeContainer &attributes, double value);
        void removePreviousAngle(ccl::AttributeContainer &attributes);

        //previous offset
        std::string getPreviousOffsetKey(void) const;
        double getPreviousOffset(const ccl::AttributeContainer &attributes) const;
        bool setPreviousOffset(ccl::AttributeContainer &attributes, double value);
        void removePreviousOffset(ccl::AttributeContainer &attributes);

        //next angle
        std::string getNextAngleKey(void) const;
        double getNextAngle(const ccl::AttributeContainer &attributes) const;
        bool setNextAngle(ccl::AttributeContainer& attributes, double value);
        void removeNextAngle(ccl::AttributeContainer& attributes);

        //next offset
        std::string getNextOffsetKey(void) const;
        double getNextOffset(const ccl::AttributeContainer &attributes) const;
        bool setNextOffset(ccl::AttributeContainer& attributes, double value);
        void removeNextOffset(ccl::AttributeContainer& attributes);
        
        // CI_RoadCategory
        std::string getRoadCategoryKey(void) const;
        std::string getRoadCategory(const ccl::AttributeContainer &attributes) const;
        bool setRoadCategory(ccl::AttributeContainer& attributes, std::string value);
        void removeRoadCategory(ccl::AttributeContainer& attributes);

        // CI_RoadEnvironment
        std::string getRoadEnvironmentKey(void) const;
        std::string getRoadEnvironment(const ccl::AttributeContainer &attributes) const;
        bool setRoadEnvironment(ccl::AttributeContainer& attributes, std::string value);
        void removeRoadEnvironment(ccl::AttributeContainer& attributes);

        // CI_SpeedLimit
        std::string getSpeedLimitKey(void) const;
        int getSpeedLimit(const ccl::AttributeContainer &attributes) const;
        bool setSpeedLimit(ccl::AttributeContainer& attributes, int value);
        void removeSpeedLimit(ccl::AttributeContainer& attributes);

        // CI_ExternalModel
        std::string getExternalModelKey(void) const;
        std::string getExternalModel(const ccl::AttributeContainer &attributes) const;
        bool setExternalModel(ccl::AttributeContainer &attributes, std::string value);
        void removeExternalModel(ccl::AttributeContainer &attributes);

        // CI_ExternalModelWidth
        std::string getExternalModelWidthKey(void) const;
        double getExternalModelWidth(const ccl::AttributeContainer &attributes) const;
        bool setExternalModelWidth(ccl::AttributeContainer &attributes, double value);
        void removeExternalModelWidth(ccl::AttributeContainer &attributes);

        // CI_ExternalModelWidth0
        std::string getExternalModelWidth0Key(void) const;
        double getExternalModelWidth0(const ccl::AttributeContainer &attributes) const;
        bool setExternalModelWidth0(ccl::AttributeContainer &attributes, double value);
        void removeExternalModelWidth0(ccl::AttributeContainer &attributes);

        // CI_ExternalModelWidthN
        std::string getExternalModelWidthNKey(void) const;
        double getExternalModelWidthN(const ccl::AttributeContainer &attributes) const;
        bool setExternalModelWidthN(ccl::AttributeContainer &attributes, double value);
        void removeExternalModelWidthN(ccl::AttributeContainer &attributes);

        // CI_CutFill
        std::string getCutFillKey(void) const;
        int getCutFill(const ccl::AttributeContainer &attributes) const;
        bool setCutFill(ccl::AttributeContainer& attributes, int value);
        void removeCutFill(ccl::AttributeContainer& attributes);

        std::string getAboveGroundLevelKey(void) const;
        bool getAboveGroundLevel(const ccl::AttributeContainer &attributes) const;
        bool setAboveGroundLevel(ccl::AttributeContainer &attributes, bool value);
        void removeAboveGroundLevel(ccl::AttributeContainer &attributes);

        std::string getElevatedKey(void) const;
        bool getElevated(const ccl::AttributeContainer &attributes) const;
        bool setElevated(ccl::AttributeContainer &attributes, bool value);
        void removeElevated(ccl::AttributeContainer &attributes);

        //CI_CONFLICTED
        std::string getConflictedKey(void) const;
        bool getConflicted(const ccl::AttributeContainer &attributes) const;
        bool setConflicted(ccl::AttributeContainer &attributes, bool value);
        void removeConflicted(ccl::AttributeContainer &attributes);

        //CI_OWNER
        std::string getOwnerKey(void) const;
        ccl::uuid getOwner(const ccl::AttributeContainer &attributes) const;
        bool setOwner(ccl::AttributeContainer &attributes, const ccl::uuid& value);
        void removeOwner(ccl::AttributeContainer& attributes);

        std::string getSurfaceKey(void) const;
        std::string getSurface(const ccl::AttributeContainer &attributes) const;
        bool setSurface(ccl::AttributeContainer &attributes, const std::string &value);
        void removeSurface(ccl::AttributeContainer& attributes);

        std::string getTrafficControlDevice0Key(void) const;
        int getTrafficControlDevice0(const ccl::AttributeContainer &attributes) const;
        bool setTrafficControlDevice0(ccl::AttributeContainer &attributes, int value);
        void removeTrafficControlDevice0(ccl::AttributeContainer& attributes);

        std::string getTrafficControlDeviceNKey(void) const;
        int getTrafficControlDeviceN(const ccl::AttributeContainer &attributes) const;
        bool setTrafficControlDeviceN(ccl::AttributeContainer &attributes, int value);
        void removeTrafficControlDeviceN(ccl::AttributeContainer& attributes);

        std::string getRoadMarkKey(void) const;
        bool getRoadMark(const ccl::AttributeContainer &attributes) const;
        bool setRoadMark(ccl::AttributeContainer &attributes, bool value);
        void removeRoadMark(ccl::AttributeContainer &attributes);

        std::string getFeatureIDKey(void) const;
        int getFeatureID(const ccl::AttributeContainer &attributes) const;
        bool setFeatureID(ccl::AttributeContainer &attributes, int value);
        void removeFeatureID(ccl::AttributeContainer& attributes);

        std::string getAdjacentKey(void) const;
        std::string getAdjacent(const ccl::AttributeContainer &attributes) const;
        bool setAdjacent(ccl::AttributeContainer &attributes, const std::string &value);
        void removeAdjacent(ccl::AttributeContainer& attributes);

        std::string getTunnelSkipKey(void) const;
        bool getTunnelSkip(const ccl::AttributeContainer &attributes) const;
        bool setTunnelSkip(ccl::AttributeContainer &attributes, bool value);
        void removeTunnelSkip(ccl::AttributeContainer &attributes);

        std::string getFilenameKey(void) const;
        std::string getFilename(const ccl::AttributeContainer &attributes) const;
        bool setFilename(ccl::AttributeContainer &attributes, const std::string &value);
        void removeFilename(ccl::AttributeContainer& attributes);

        std::string getContourKey(void) const;
        std::string getContour(const ccl::AttributeContainer &attributes) const;
        bool setContour(ccl::AttributeContainer &attributes, const std::string &value);
        void removeContour(ccl::AttributeContainer &attributes);

        /**
         * @fn    std::string AttributeDictionary::getElevationOptimizerKey(void) const;
         *
         * @brief    CI_ElevationOptimizer CIelopt.
         *
         * @return    The elevation optimizer key.
         */        
        std::string getElevationOptimizerKey(void) const;

        /**
         * @fn    int AttributeDictionary::getElevationOptimizer(const ccl::AttributeContainer &attributes) const;
         *
         * @brief    Gets elevation optimizer instructions for the feature. ELEVOPT_IGNORE will skip processing.
         *
         * @param    attributes    The attributes of the feature.
         *
         * @return    The elevation optimizer.
         */
        int getElevationOptimizer(const ccl::AttributeContainer &attributes) const;
        
        bool setElevationOptimizer(ccl::AttributeContainer& attributes, int value);
        
        void removeElevationOptimizer(ccl::AttributeContainer& attributes);
    };

    extern AttributeDictionary dictionary;

}