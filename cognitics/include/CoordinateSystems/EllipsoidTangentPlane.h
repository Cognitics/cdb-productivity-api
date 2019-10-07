#pragma once

#include "ILocalTangentPlane.h"
#include "WGS84Transform.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class EllipsoidTangentPlane : public ILocalTangentPlane
        {
        public:
            double OriginLatitude;
            double OriginLongitude;
            double OriginAltitude;

        protected:
            WGS84Transform WGS84Transform;
            double lambda;
            double phi;
            double sin_lambda;
            double cos_lambda;
            double sin_phi;
            double cos_phi;
            double surface_radius;
            double origin_x;
            double origin_y;
            double origin_z;


        public:
            EllipsoidTangentPlane(double originLatitude, double originLongitude, double originAltitude = 0.0);

            void ECEFtoLocal(double x, double y, double z, double& east, double& north, double& up);
            void LocalToECEF(double east, double north, double up, double& x, double& y, double& z);

            virtual void GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up);
            virtual void LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude);

        };

    }
}

