#pragma once

#include "EllipsoidTangentPlane.h"
#include "EGMTransform.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class GeoidTangentPlane : public EllipsoidTangentPlane
        {
        private:
            EGMTransform Transform;

        public:
            GeoidTangentPlane(EGM* egm, double originLatitude, double originLongitude, double originAltitude = 0.0);

            virtual void GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up);
            virtual void LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude);

        };

    }
}

