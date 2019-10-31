#include "rapidjson\filereadstream.h"
#include "rapidjson\document.h"
#include "features\GsBuildings.h"
#include <fstream>
#include <iostream>

int GsBuildings::AddBuilding(float lat, float lon, std::string modelpath, float angle)
{
	buildings.push_back(new FeatureInfo(lat, lon, modelpath, angle));
	return buildings.size() - 1;
}

FeatureInfo& GsBuildings::GetBuilding(int index)
{
	return *(buildings.at(index));
}

int GsBuildings::Count() 
{
	return buildings.size();
}

GsBuildings::~GsBuildings()
{
	/*for (int i = 0; i < buildings.size(); ++i)
	{
		delete buildings[i];
	}*/
}

void GsBuildings::ProcessBuildingData(std::string datafile)
{
    FILE* fp = nullptr;
	fopen_s(&fp, datafile.c_str(), "rb");

    if (fp == nullptr)
    {
        return;
    }

	char readBuffer[65536];
	rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
	rapidjson::Document d;
	d.ParseStream(is);

	//std::ofstream file("data/models.txt");


	const rapidjson::Value& features = d["features"];
	if (features.IsArray())
	{
		for (rapidjson::SizeType i = 0; i < features.Size(); ++i)
		{
			const rapidjson::Value& lat = features[i]["Lat"];
			const rapidjson::Value& lon = features[i]["Lon"];
			const rapidjson::Value& model = features[i]["modelpath"];
			const rapidjson::Value& angle = features[i]["AO1"];
			if (lat.IsNumber() && lon.IsNumber() && model.IsString() && angle.IsNumber())
			{
				//file << model.GetString() << std::endl;
				int buildingIndex = AddBuilding(lat.GetFloat(), lon.GetFloat(), model.GetString(), angle.GetFloat());
				if (buildingIndex == buildings.size() - 1)
				{
					if (features[i].HasMember("SCALx"))
					{
						GetBuilding(buildingIndex).scaleX = features[i]["SCALx"].GetFloat();
					}
					if (features[i].HasMember("SCALy"))
					{
						GetBuilding(buildingIndex).scaleY = features[i]["SCALy"].GetFloat();
					}
					if (features[i].HasMember("SCALz"))
					{
						GetBuilding(buildingIndex).scaleZ = features[i]["SCALz"].GetFloat();
					}
				}
				else
				{
					std::cout << "gsbuild error" << std::endl;
				}
			}
		}
	}

	//file.close();

	fclose(fp);
}
