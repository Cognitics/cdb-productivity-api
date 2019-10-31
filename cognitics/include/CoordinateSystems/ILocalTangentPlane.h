#pragma once

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class ILocalTangentPlane
        {
        public:
            virtual void GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up) = 0;
            virtual void LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude) = 0;
        };

    }
}

