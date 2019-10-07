
#include "CoordinateSystems/EllipsoidTangentPlane.h"

#include "CoordinateSystems/WGS84.h"

#include <cmath>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        EllipsoidTangentPlane::EllipsoidTangentPlane(double originLatitude, double originLongitude, double originAltitude)
            : OriginLatitude(originLatitude), OriginLongitude(originLongitude), OriginAltitude(originAltitude)
        {
            lambda = OriginLatitude * M_PI / 180.0;
            phi = OriginLongitude * M_PI / 180.0;
            sin_lambda = std::sin(lambda);
            cos_lambda = std::cos(lambda);
            sin_phi = std::sin(phi);
            cos_phi = std::cos(phi);
            surface_radius = WGS84::EquatorialRadius / std::sqrt(1.0 - (WGS84::SquaredEccentricity * sin_lambda * sin_lambda));
            origin_x = (OriginAltitude + surface_radius) * cos_lambda * cos_phi;
            origin_y = (OriginAltitude + surface_radius) * cos_lambda * sin_phi;
            origin_z = (OriginAltitude + ((1.0 - WGS84::SquaredEccentricity) * surface_radius)) * sin_lambda;
        }

        void EllipsoidTangentPlane::ECEFtoLocal(double x, double y, double z, double& east, double& north, double& up)
        {
            double dx = x - origin_x;
            double dy = y - origin_y;
            double dz = z - origin_z;
            east = (-sin_phi * dx) + (cos_phi * dy);
            north = (sin_lambda * -cos_phi * dx) - (sin_lambda * sin_phi * dy) + (cos_lambda * dz);
            up = (cos_lambda * cos_phi * dx) + (cos_lambda * sin_phi * dy) + (sin_lambda * dz);
        }

        void EllipsoidTangentPlane::LocalToECEF(double east, double north, double up, double& x, double& y, double& z)
        {
            x = origin_x + (-sin_phi * east) - (sin_lambda * cos_phi * north) + (cos_lambda * cos_phi * up);
            y = origin_y + (cos_phi * east) - (sin_lambda * sin_phi * north) + (cos_lambda * sin_phi * up);
            z = origin_z + (cos_lambda * north) + (sin_lambda * up);
        }

        void EllipsoidTangentPlane::GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up)
        {
            double x, y, z;
            WGS84Transform.GeodeticToECEF(latitude, longitude, altitude, x, y, z);
            ECEFtoLocal(x, y, z, east, north, up);
        }

        void EllipsoidTangentPlane::LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude)
        {
            double x, y, z;
            LocalToECEF(east, north, up, x, y, z);
            WGS84Transform.ECEFtoGeodetic(x, y, z, latitude, longitude, altitude);
        }


    }
}

