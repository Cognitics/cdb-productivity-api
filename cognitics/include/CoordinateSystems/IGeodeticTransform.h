#pragma once

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class IGeodeticTransform
        {
        public:
            virtual void GeodeticToECEF(double latitude, double longitude, double altitude, double& x, double& y, double& z) = 0;
            virtual void ECEFtoGeodetic(double x, double y, double z, double& latitude, double& longitude, double& altitude) = 0;
        };

    }
}

