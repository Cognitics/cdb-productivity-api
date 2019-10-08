#pragma once
#include <string>

struct FeatureInfo
{
	FeatureInfo(float latitude, float longitude, std::string path, float angle);
	float lat;
	float lon;
	float elev;
	std::string modelpath;
	float AO1;
	float scaleX = 1;
	float scaleY = 1;
	float scaleZ = 1;
	std::string outputModelPath;
};