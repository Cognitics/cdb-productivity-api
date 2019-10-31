
#include "CoordinateSystems/SphereTangentPlane.h"

#include "CoordinateSystems/WGS84.h"

#include <cmath>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        SphereTangentPlane::SphereTangentPlane(double originLatitude, double originLongitude, double originAltitude)
            : OriginLatitude(originLatitude), OriginLongitude(originLongitude), OriginAltitude(originAltitude)
        {
            metersPerDegreeLatitude = std::cos(OriginLatitude * (M_PI / 180.0)) * metersPerDegreeLongitude;
        }

        double SphereTangentPlane::Latitude(double y)
        {
            return (OriginLatitude + (y / metersPerDegreeLongitude));
        }

        double SphereTangentPlane::Longitude(double x)
        {
            return (OriginLongitude + (x / metersPerDegreeLatitude));
        }

        double SphereTangentPlane::X(double longitude)
        {
            return (longitude - OriginLongitude) * metersPerDegreeLatitude;
        }

        double SphereTangentPlane::Y(double latitude)
        {
            return (latitude - OriginLatitude) * metersPerDegreeLongitude;
        }

        void SphereTangentPlane::GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up)
        {
            east = X(longitude);
            north = Y(latitude);
            up = altitude - OriginAltitude;
        }

        void SphereTangentPlane::LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude)
        {
            latitude = Latitude(north);
            longitude = Longitude(east);
            altitude = up + OriginAltitude;
        }
    }
}

