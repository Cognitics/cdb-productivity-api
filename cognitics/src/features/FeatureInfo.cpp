#include "features\FeatureInfo.h"

FeatureInfo::FeatureInfo(float latitude, float longitude, std::string path, float angle) :
	lat(latitude), lon(longitude), elev(0.0f), modelpath(path), AO1(angle),
	scaleX(1.0f), scaleY(1.0f), scaleZ(1.0f)
{}
