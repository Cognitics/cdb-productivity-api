#pragma once

#include <vector>
#include "scenegraph/Scene.h"

struct LatLon
{
    double fLat;
    double fLon;
};

struct FeatureType
{
    std::string sName;
    std::string sTitle;
    std::string sAbstract;
    LatLon lowerCorner;
    LatLon upperCorner;
};

struct Vertex
{
    double x;
    double y;
    double z;
};

template <typename T>
struct GMLLinearRing
{
    std::vector<T> coordinates;
};

template <typename T>
struct GMLPolygon
{
    GMLLinearRing<T> linearRing;
};

struct GMLMultiPolygon
{
    std::vector<GMLPolygon<LatLon>> polygons;
};

struct Feature
{
    std::string sCountry;
    int nFID;
};

struct CountryBoundaryFeature
{
    GMLMultiPolygon multiPolygon;
};

struct ManMadePoints_FootprintsFeature
{
    std::vector<GMLPolygon<Vertex>> polygons;
	
	int height;
	LatLon lowLeft;
	LatLon upRight;
	sfa::Point localCenter;
	float elev;
	std::string modelName;
};

struct TreePoint
{
	LatLon worldPosition;
	float elev;
    Vertex position;
    std::string sModel;
    float fHeight;
};

struct TreePoints_Feature
{
    std::vector<TreePoint> treePoints;
};



void ParseCapabilities(std::string sCapabilitiesFile);
void ParseManMadePoints_Footprints(const std::string& sFeatureFile);
void ParseManMadePoints_Footprints(const std::string& sFeatureFile, std::vector<ManMadePoints_FootprintsFeature>& features);
void ParseTreePoints(const std::string& sTreePointsFeatureFile);
void ParseTreePoints(const std::string & sTreePointsFeatureFile, std::vector<TreePoints_Feature>& features);
void BuildFootprintScene(std::string & outputPath, scenegraph::Scene& masterScene, std::vector<ManMadePoints_FootprintsFeature>& features);
scenegraph::Scene BuildSceneFromFootPrint(const ManMadePoints_FootprintsFeature& features);
void ExportToObj(std::vector<ManMadePoints_FootprintsFeature> features, const std::string& sOut);
