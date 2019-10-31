#include "features/GMLParser.h"
#include "scenegraph/Scene.h"
#include <scenegraphobj/scenegraphobj.h>
#include "scenegraphgltf/scenegraphgltf.h"
#include "rapidxml/rapidxml.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include "ctl/Vector.h"
#include "ctl/QTriangulate.h"
#include "features/GsBuildings.h"

using namespace std;
using namespace rapidxml;

#ifndef _MSC_VER
namespace {
    char* strtok_s(char* s, const char* delim, char** context)
    {
            return strtok_r(s, delim, context);
    }
}
#endif

//returns a dynamically allocated buffer. caller is responsible for deallocating buffer.
char* ReadAllFileContents(string sFile)
{
    ifstream fin(sFile);

    if (!fin.is_open())
    {
        std::cout << "Could not open " << sFile << std::endl;
        return nullptr;
    }
    int length = ccl::getFileSize(sFile);
    char* buffer = new char[length + 1];
    fin.read(buffer, length);
    buffer[length] = '\0';

    return buffer;
}

void ParseCapabilities(string sCapabilitiesFile)
{
    //file retrieved using
    //http://localhost:81/geoserver/wfs?request=GetCapabilities

    ifstream fin(sCapabilitiesFile);

    if (!fin.is_open())
    {
        std::cout << "Could not open " << sCapabilitiesFile << std::endl;
        return;
    }

    char line[1024];
    stringstream sstream;
    while (!fin.eof())
    {
      fin.getline(line, 1024);
      sstream << line;
    }
    
    string s = sstream.str();
    int length = s.size();
    char* buffer = new char[length + 1];
    memcpy(buffer, s.c_str(), length);
    buffer[length] = '\0';

    xml_document<> doc;
    doc.parse<0>(buffer);

    xml_node<>* node = doc.first_node();

    xml_node<>* featureTypeList = node->first_node("FeatureTypeList");

    vector<FeatureType> featureTypes;

    xml_node<>* featureTypeXml = featureTypeList->first_node();
    while (featureTypeXml)
    {
        FeatureType featureType;
        xml_node<>* name = featureTypeXml->first_node("Name");
        xml_node<>* title = featureTypeXml->first_node("Title");
        xml_node<>* abstract = featureTypeXml->first_node("Abstract");
        xml_node<>* boundingBox = featureTypeXml->first_node("ows:WGS84BoundingBox");
        xml_node<>* lowerCorner = boundingBox->first_node("ows:LowerCorner");
        xml_node<>* upperCorner = boundingBox->first_node("ows:UpperCorner");

        featureType.sName = name->value();
        featureType.sTitle = title->value();
        featureType.sAbstract = abstract->value();

        char* next_token = nullptr;
        char* p = lowerCorner->value();
        strtok_s(p, " ", &next_token);
        featureType.lowerCorner.fLat = atof(p);
        featureType.lowerCorner.fLon = atof(next_token);

        p = upperCorner->value();
        strtok_s(p, " ", &next_token);
        featureType.upperCorner.fLat = atof(p);
        featureType.upperCorner.fLon = atof(next_token);

        //cout << name->value() << endl;
        featureTypeXml = featureTypeXml->next_sibling();

        featureTypes.push_back(featureType);
    }

    delete[] buffer;
}

void ParseManMadePoints_Footprints(const std::string & sFeatureFile)
{
	vector<ManMadePoints_FootprintsFeature> features;
	ParseManMadePoints_Footprints(sFeatureFile, features);

	ExportToObj(features, "features.obj");
}

void ParseManMadePoints_Footprints(const std::string& sFeatureFile, vector<ManMadePoints_FootprintsFeature>& features)
{
    //building footprints file retrieved using
    //http://<ip>/tgs/?layers=WorldBoundaries%2COpenStreetMap%2CESRI%20World%20Imagery&lat=12.795663693136&lon=44.9966825737&zoom=11
    //payload: <wfs:GetFeature xmlns:wfs="http://www.opengis.net/wfs" service="WFS" version="1.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd"><wfs:Query typeName="CDB_Vectors:GS_ManMadePoints_Footprints"><ogc:Filter xmlns:ogc="http://www.opengis.net/ogc"><ogc:BBOX><ogc:PropertyName>geom</ogc:PropertyName><gml:Box xmlns:gml="http://www.opengis.net/gml" srsName="EPSG:4326"><gml:coordinates decimal="." cs="," ts=" ">45,12.7734375 45.0078125,12.78125</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter></wfs:Query></wfs:GetFeature>

    char* buffer = ReadAllFileContents(sFeatureFile);

    if (buffer == nullptr)
    {
        return;
    }

    xml_document<> doc;
    doc.parse<0>(buffer);

    xml_node<>* node = doc.first_node();
    xml_node<>* featureCollectionXml = node;


    xml_node<>* featureMemberXml = featureCollectionXml->first_node("gml:featureMember");
    while (featureMemberXml)
    {
        xml_node<>* manMadePoints_FootprintsXml = featureMemberXml->first_node("CDB_Vectors:GS_ManMadePoints_Footprints");
        xml_attribute<>* fid = manMadePoints_FootprintsXml->first_attribute("fid");
        //cout << fid->value() << endl;
        xml_node<>* boundedByXml = manMadePoints_FootprintsXml->first_node("gml:boundedBy");
        xml_node<>* boxXml = boundedByXml->first_node("gml:Box");
        xml_node<>* coordinatesXml = boxXml->first_node("gml:coordinates");

        LatLon lowerLeftFeature;
        LatLon upperRightFeature;

        char* next_token = nullptr;
        lowerLeftFeature.fLon = atof(strtok_s(coordinatesXml->value(), " ,", &next_token));
        lowerLeftFeature.fLat = atof(strtok_s(nullptr, " ,", &next_token));
        upperRightFeature.fLon = atof(strtok_s(nullptr, " ,", &next_token));
        upperRightFeature.fLat = atof(strtok_s(nullptr, " ,", &next_token));

        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin((lowerLeftFeature.fLat + upperRightFeature.fLat) / 2.0, (lowerLeftFeature.fLon + upperRightFeature.fLon) / 2.0);

        xml_node<>* geomXml = manMadePoints_FootprintsXml->first_node("CDB_Vectors:geom");
        xml_node<>* polygonXml = geomXml->first_node("gml:Polygon");

        ManMadePoints_FootprintsFeature feature;
        GMLPolygon<Vertex> polygon;
        xml_node<>* outerBoundaryIsXml = polygonXml->first_node("gml:outerBoundaryIs");
        xml_node<>* linearRingXml = outerBoundaryIsXml->first_node("gml:LinearRing");
        xml_node<>* coordinatesXml2 = linearRingXml->first_node("gml:coordinates");

		feature.lowLeft = lowerLeftFeature;
		feature.upRight = upperRightFeature;
		feature.elev = 0;
		feature.height = atoi(manMadePoints_FootprintsXml->first_node("CDB_Vectors:HGT")->value());
		feature.modelName = manMadePoints_FootprintsXml->first_node("CDB_Vectors:MODL")->value();
		float lat = atof(manMadePoints_FootprintsXml->first_node("CDB_Vectors:lat")->value());
		float lon = atof(manMadePoints_FootprintsXml->first_node("CDB_Vectors:lon")->value());
		feature.localCenter.setX(flatEarth.convertGeoToLocalX(lon));
		feature.localCenter.setY(flatEarth.convertGeoToLocalY(lat));

        char* next_token2 = nullptr;
        char* p = coordinatesXml2->value();
        string s = p;
        int nNumVertices = std::count(s.begin(), s.end(), ' ') + 1;

        LatLon latLon;
        double altitude = 0;
        latLon.fLon = atof(strtok_s(p, " ,", &next_token2));
        latLon.fLat = atof(strtok_s(0, " ,", &next_token2));
        altitude = atof(strtok_s(0, " ,", &next_token2));
        
        Vertex vertex;
        vertex.x = flatEarth.convertGeoToLocalX(latLon.fLon);
        vertex.y = flatEarth.convertGeoToLocalY(latLon.fLat);
        vertex.z = altitude;
        polygon.linearRing.coordinates.push_back(vertex);

        for (int n = 0; n < nNumVertices - 1; ++n)
        {
            latLon.fLon = atof(strtok_s(0, " ,", &next_token2));
            latLon.fLat = atof(strtok_s(0, " ,", &next_token2));
            altitude = atof(strtok_s(0, " ,", &next_token2));

            vertex.x = flatEarth.convertGeoToLocalX(latLon.fLon);
            vertex.y = flatEarth.convertGeoToLocalY(latLon.fLat);
            vertex.z = altitude;
            polygon.linearRing.coordinates.push_back(vertex);
        }

        featureMemberXml = featureMemberXml->next_sibling();

        feature.polygons.push_back(polygon);
        features.push_back(feature);
    }

    delete[] buffer;

}


void ExportToObj(vector<ManMadePoints_FootprintsFeature> features, const std::string& sOut)
{
    scenegraph::Scene *scene = new scenegraph::Scene;
    for (auto feature : features)
    {
        for (auto p : feature.polygons)
        {
            scenegraph::LineString line;
            for (auto c : p.linearRing.coordinates)
            {
                sfa::Point point(c.x, c.y, c.z);
                line.lineString.addPoint(point);
            }

            scene->lineStrings.push_back(line);
        }
    }

    scenegraph::buildObjFromScene(sOut, 0, 0, 0, 0, scene);
}

void ParseCountryBoundaries(const std::string& sFeatureFile, double north, double south, double east, double west)
{
    //country boundaries file retrieved using
    //http://localhost:81/geoserver/OSM/ows?service=WFS&version=1.0.0&REQUEST=GetFeature&TYPENAME=OSM:CountryBoundaries&maxFeatures=50&outputFormat=text%2Fxml%3B+subtype%3Dgml%2F2.1.2

    char* buffer = ReadAllFileContents(sFeatureFile);

    if (buffer == nullptr)
    {
        return;
    }

    xml_document<> doc;
    doc.parse<0>(buffer);

    xml_node<>* node = doc.first_node();
    xml_node<>* featureCollectionXml = node;

    vector<Feature> features;

    xml_node<>* featureMemberXml = featureCollectionXml->first_node("gml:featureMember");
    while (featureMemberXml)
    {
        xml_node<>* countryBoundariesXml = featureMemberXml->first_node("OSM:CountryBoundaries");
        xml_attribute<>* fid = countryBoundariesXml->first_attribute("fid");
        cout << fid->value() << endl;
        xml_node<>* boundedByXml = countryBoundariesXml->first_node("gml:boundedBy");
        xml_node<>* boxXml = boundedByXml->first_node("gml:Box");
        xml_node<>* coordinatesXml = boxXml->first_node("gml:coordinates");

        LatLon lowerLeftFeature;
        LatLon upperRightFeature;

        char* next_token = nullptr;
        lowerLeftFeature.fLat = atof(strtok_s(coordinatesXml->value(), " ,", &next_token));
        lowerLeftFeature.fLon = atof(strtok_s(nullptr, " ,", &next_token));
        upperRightFeature.fLat = atof(strtok_s(nullptr, " ,", &next_token));
        upperRightFeature.fLon = atof(strtok_s(nullptr, " ,", &next_token));

        if (upperRightFeature.fLat < west || lowerLeftFeature.fLat > east ||
            lowerLeftFeature.fLon > north || upperRightFeature.fLon < south)
        {
            //feature bounding box does not overlap with desired bounding box
            featureMemberXml = featureMemberXml->next_sibling();
            continue;
        }

        xml_node<>* the_geomXml = countryBoundariesXml->first_node("OSM:the_geom");
        xml_node<>* multiPolygonXml = the_geomXml->first_node("gml:MultiPolygon");

        GMLMultiPolygon multiPolygon;

        xml_node<>* polygonMemberXml = multiPolygonXml->first_node("gml:polygonMember");
        xml_node<>* polygonXml = polygonMemberXml->first_node("gml:Polygon");

        GMLPolygon<LatLon> polygon;
        xml_node<>* outerBoundaryIsXml = polygonXml->first_node("gml:outerBoundaryIs");
        xml_node<>* linearRingXml = outerBoundaryIsXml->first_node("gml:LinearRing");
        xml_node<>* coordinatesXml2 = linearRingXml->first_node("gml:coordinates");

        char* next_token2 = nullptr;
        char* p = coordinatesXml2->value();
        string s = p;
        int nNumCoords = std::count(s.begin(), s.end(), ',');

        LatLon latLon;
        latLon.fLat = atof(strtok_s(p, " ,", &next_token2));
        latLon.fLon = atof(strtok_s(0, " ,", &next_token2));
        polygon.linearRing.coordinates.push_back(latLon);

        for (int n = 0; n < nNumCoords - 1; ++n)
        {
            latLon.fLat = atof(strtok_s(0, " ,", &next_token2));
            latLon.fLon = atof(strtok_s(0, " ,", &next_token2));

            polygon.linearRing.coordinates.push_back(latLon);
        }

        multiPolygon.polygons.push_back(polygon);

        featureMemberXml = featureMemberXml->next_sibling();

        //featureTypes.push_back(featureType);
    }

    delete[] buffer;
}

void ParseTreePoints(const std::string& sTreePointsFeatureFile)
{
	vector<TreePoints_Feature> features;
	ParseTreePoints(sTreePointsFeatureFile, features);
}

void ParseTreePoints(const std::string& sTreePointsFeatureFile, vector<TreePoints_Feature>& features)
{
    //tree points file retrieved using
    //http://10.88.224.29/tgs/?layers=Base%3ACDB%20Imagery_YearRound%2CWorldBoundaries%2CCDB%20Vectors%2COSM%3Amgrs&lat=12.774783590132&lon=45.037132967852&zoom=15
    //payload: <wfs:GetFeature xmlns:wfs="http://www.opengis.net/wfs" service="WFS" version="1.0.0" xmlns:xsi="http://www.w3.org/2001/XMLSchema-instance" xsi:schemaLocation="http://www.opengis.net/wfs http://schemas.opengis.net/wfs/1.0.0/WFS-transaction.xsd"><wfs:Query typeName="CDB_Vectors:GT_TreePoints"><ogc:Filter xmlns:ogc="http://www.opengis.net/ogc"><ogc:BBOX><ogc:PropertyName>geom</ogc:PropertyName><gml:Box xmlns:gml="http://www.opengis.net/gml" srsName="EPSG:4326"><gml:coordinates decimal="." cs="," ts=" ">45.03125,12.7734375 45.0390625,12.78125</gml:coordinates></gml:Box></ogc:BBOX></ogc:Filter></wfs:Query></wfs:GetFeature>

    char* buffer = ReadAllFileContents(sTreePointsFeatureFile);
    if (buffer == nullptr)
    {
        return;
    }

    xml_document<> doc;
    doc.parse<0>(buffer);

    xml_node<>* node = doc.first_node();
    xml_node<>* featureCollectionXml = node;

    xml_node<>* featureMemberXml = featureCollectionXml->first_node("gml:featureMember");
    while (featureMemberXml)
    {
        xml_node<>* treePointsXml = featureMemberXml->first_node("CDB_Vectors:GT_TreePoints");
        xml_attribute<>* fid = treePointsXml->first_attribute("fid");
        //cout << fid->value() << endl;
        xml_node<>* boundedByXml = treePointsXml->first_node("gml:boundedBy");
        xml_node<>* boxXml = boundedByXml->first_node("gml:Box");
        xml_node<>* coordinatesXml = boxXml->first_node("gml:coordinates");

        LatLon lowerLeftFeature;
        LatLon upperRightFeature;

        char* next_token = nullptr;
        lowerLeftFeature.fLat = atof(strtok_s(coordinatesXml->value(), " ,", &next_token));
        lowerLeftFeature.fLon = atof(strtok_s(nullptr, " ,", &next_token));
        upperRightFeature.fLat = atof(strtok_s(nullptr, " ,", &next_token));
        upperRightFeature.fLon = atof(strtok_s(nullptr, " ,", &next_token));

        cts::FlatEarthProjection flatEarth;
        flatEarth.setOrigin((lowerLeftFeature.fLat + upperRightFeature.fLat) / 2.0, (lowerLeftFeature.fLon + upperRightFeature.fLon) / 2.0);

        TreePoints_Feature feature;

        xml_node<>* geomXml = treePointsXml->first_node("CDB_Vectors:geom");
        while (geomXml)
        {
            xml_node<>* geomPoint = geomXml->first_node("gml:Point");
            xml_node<>* coordinatesXml2 = geomPoint->first_node("gml:coordinates");

            char* p = coordinatesXml2->value();
            string s = p;
            int nNumVertices = std::count(s.begin(), s.end(), ',') + 1;

            LatLon latLon;
            double altitude = 0;
            char* next_token2 = nullptr;
            latLon.fLon = atof(strtok_s(p, " ,", &next_token2));
            latLon.fLat = atof(strtok_s(0, " ,", &next_token2));
            altitude = atof(strtok_s(0, " ,", &next_token2));

            TreePoint treePoint;
            treePoint.position.x = flatEarth.convertGeoToLocalX(latLon.fLon);
            treePoint.position.y = flatEarth.convertGeoToLocalY(latLon.fLat);
            treePoint.position.z = altitude;
            treePoint.sModel = treePointsXml->first_node("CDB_Vectors:MODL")->value();
            treePoint.fHeight = atof(treePointsXml->first_node("CDB_Vectors:HGT")->value());
			treePoint.worldPosition.fLat = latLon.fLat;
			treePoint.worldPosition.fLon = latLon.fLon;

            geomXml = geomXml->next_sibling("CDB_Vectors:geom");

            feature.treePoints.push_back(treePoint);
        }

        featureMemberXml = featureMemberXml->next_sibling();
        features.push_back(feature);
    }

    delete[] buffer;

    //ExportToObj(features, "treePoints.obj");
}

scenegraph::Face CreateFace(sfa::Point& p, sfa::Point& q, sfa::Point& r)
{
	scenegraph::Face face;
	face.addVert(p);
	face.addVert(q);
	face.addVert(r);

	sfa::Point normal = (q - p).cross(r - p);
	normal.normalize();
	face.setNormalN(0, normal);
	face.setNormalN(1, normal);
	face.setNormalN(2, normal);

	scenegraph::MappedTexture mt;
	mt.SetTextureName("placeholder.png");
	mt.uvs.push_back(sfa::Point(0, 0));
	mt.uvs.push_back(sfa::Point(1, 0));
	mt.uvs.push_back(sfa::Point(0, 1));
	face.textures.push_back(mt);
	face.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
	face.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);

	return face;
}

void BuildFootprintScene(std::string& outputPath, scenegraph::Scene& masterScene, std::vector<ManMadePoints_FootprintsFeature>& features)
{
	for (auto feature : features)
	{
		scenegraph::Scene scene = new scenegraph::Scene;
		for (auto p : feature.polygons)
		{
			std::vector<sfa::Point> points;
			for (auto c : p.linearRing.coordinates)
			{
				sfa::Point point(c.x, c.y, c.z);
				points.push_back(point);
			}

			if (points.size() < 2)
			{
				continue;
			}

			sfa::Point extrudedCenter = feature.localCenter;
			extrudedCenter.setZ(feature.height);

			sfa::Point endPoint = points[points.size()-1];
			endPoint.setZ(feature.height);
			sfa::Point startPoint = points[0];
			startPoint.setZ(feature.height);
			scenegraph::Face roofEnd = CreateFace(extrudedCenter, startPoint, endPoint);
			scene.faces.push_back(roofEnd);

			for (int n = 1; n < points.size(); ++n)
			{
				sfa::Point point2 = points[n];
				sfa::Point point1 = points[n - 1];
				sfa::Point point2extruded = points[n];
				point2extruded.setZ(feature.height);
				sfa::Point point1extruded = points[n-1];
				point1extruded.setZ(feature.height);
				
				scenegraph::Face face1 = CreateFace(point2extruded, point2, point1);
				scenegraph::Face face2 = CreateFace(point1extruded, point2extruded, point1);
								
				scene.faces.push_back(face1);
				scene.faces.push_back(face2);
			}

			std::cout << "Writing footprint: " << feature.modelName << std::endl;
			std::string outputName = outputPath + feature.modelName + ".b3dm";
			scenegraph::buildGltfFromScene(outputName, &scene, feature.upRight.fLat, feature.lowLeft.fLat, feature.upRight.fLon, feature.lowLeft.fLon, feature.elev);

		}
	}

}

scenegraph::Scene BuildSceneFromFootPrint(const ManMadePoints_FootprintsFeature& feature)
{
    scenegraph::Scene scene = new scenegraph::Scene;
    for (auto p : feature.polygons)
    {
        std::vector<sfa::Point> points;
        for (auto c : p.linearRing.coordinates)
        {
            sfa::Point point(c.x, c.y, c.z);
            points.push_back(point);
        }

        if (points.size() < 2)
        {
            continue;
        }

        for (int n = 1; n < points.size(); ++n)
        {
            sfa::Point point2 = points[n];
            sfa::Point point1 = points[n - 1];
            sfa::Point point2extruded = points[n];
            point2extruded.setZ(feature.height);
            sfa::Point point1extruded = points[n - 1];
            point1extruded.setZ(feature.height);
            scenegraph::Face face1;
            face1.addVert(point2extruded);
            face1.addVert(point2);
            face1.addVert(point1);
            face1.setNormalN(2, sfa::Point(0.0, 0.0, 1.0));
            face1.setNormalN(1, sfa::Point(0.0, 0.0, 1.0));
            face1.setNormalN(0, sfa::Point(0.0, 0.0, 1.0));
            scenegraph::MappedTexture mt1;
            mt1.SetTextureName("placeholder.png");
            mt1.uvs.push_back(sfa::Point(0, 1));
            mt1.uvs.push_back(sfa::Point(1, 0));
            mt1.uvs.push_back(sfa::Point(0, 0));
            face1.textures.push_back(mt1);
            face1.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            face1.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            scenegraph::Face face2;
            face2.addVert(point1extruded);
            face2.addVert(point2extruded);
            face2.addVert(point1);
            face2.setNormalN(2, sfa::Point(0.0, 0.0, 1.0));
            face2.setNormalN(1, sfa::Point(0.0, 0.0, 1.0));
            face2.setNormalN(0, sfa::Point(0.0, 0.0, 1.0));
            scenegraph::MappedTexture mt2;
            mt2.SetTextureName("placeholder.png");
            mt2.uvs.push_back(sfa::Point(0, 1));
            mt2.uvs.push_back(sfa::Point(1, 0));
            mt2.uvs.push_back(sfa::Point(0, 0));
            face2.textures.push_back(mt2);
            face2.primaryColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            face2.alternateColor = scenegraph::Color(1.0f, 1.0f, 1.0f, 1.0f);
            scene.faces.push_back(face1);
            scene.faces.push_back(face2);
        }

        std::vector<sfa::Point> extrudedPoints = points;
        for (int i = 0; i < extrudedPoints.size(); i++)
        {
            extrudedPoints[i].setZ(extrudedPoints[i].Z() + feature.height);
        }

        ctl::PointList extrudedPointsCTL;

        for (int i = 0; i < extrudedPoints.size() - 1; i++)
        {
            auto& point = extrudedPoints[i];
            extrudedPointsCTL.push_back(ctl::Point(point.X(), point.Y(), point.Z()));
        }

        auto pointList = ctl::QTriangulate::apply(extrudedPointsCTL);

        sfa::PointList extrudedPointsSFA;
        for (int i = 0; i < pointList.size(); i++)
        {
            auto& point = pointList[i];
            extrudedPointsSFA.push_back(new sfa::Point(point.x, point.y, point.z));
        }

        for (int i = 0; i < extrudedPointsSFA.size(); i+=3)
        {
            scenegraph::Face roof;
            scenegraph::MappedTexture mt2;
            mt2.SetTextureName("placeholder.png");
            roof.addVert(extrudedPointsSFA[i+2]);
            roof.addVert(extrudedPointsSFA[i+1]);
            roof.addVert(extrudedPointsSFA[i]);
            roof.setNormalN(0, sfa::Point(0.0, 0.0, 1.0));
            roof.setNormalN(1, sfa::Point(0.0, 0.0, 1.0));
            roof.setNormalN(2, sfa::Point(0.0, 0.0, 1.0));
            mt2.uvs.push_back(sfa::Point(0, 1));
            mt2.uvs.push_back(sfa::Point(1, 0));
            mt2.uvs.push_back(sfa::Point(0, 0));
            roof.textures.push_back(mt2);
            scene.faces.push_back(roof);
        }

        for (int i = 0; i < extrudedPointsSFA.size(); i++)
        {
            delete extrudedPointsSFA[i];
        }
        return scene;
    }
}
