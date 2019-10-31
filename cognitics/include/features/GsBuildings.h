#pragma once
#include <vector>
#include "features\FeatureInfo.h"

class GsBuildings
{
private:
	std::vector<FeatureInfo*> buildings;
public:
	int AddBuilding(float lat, float lon, std::string modelpath, float angle);
	FeatureInfo& GetBuilding(int index);
	int Count();
	GsBuildings() {}
	~GsBuildings();
	void ProcessBuildingData(std::string datafile);
};