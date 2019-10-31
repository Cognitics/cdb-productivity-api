#pragma once

#include "IGeodeticTransform.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class WGS84Transform : public IGeodeticTransform
        {
        public:
            virtual void GeodeticToECEF(double latitude, double longitude, double altitude, double& x, double& y, double& z);
            virtual void ECEFtoGeodetic(double x, double y, double z, double& latitude, double& longitude, double& altitude);
        };

    }
}

