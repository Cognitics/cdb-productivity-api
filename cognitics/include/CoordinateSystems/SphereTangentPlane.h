#pragma once

#include "ILocalTangentPlane.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class SphereTangentPlane : public ILocalTangentPlane
        {
        public:
            double OriginLatitude;
            double OriginLongitude;
            double OriginAltitude;

        private:
            //const double metersPerNauticalMile = 1852.0f;
            //const double nauticalMilesForSphericalEarthCircumference = 21600.0f;
            //const double nauticalMilesPerDegree = nauticalMilesForSphericalEarthCircumference / 360.0f;   // 21,600 / 360 = 60
            //const double metersPerDegree = nauticalMilesPerDegree * metersPerNauticalMile;        // 60 * 1852 = 111,120
            const double metersPerDegreeLongitude = 111120.0f;
            double metersPerDegreeLatitude;

        public:
            SphereTangentPlane(double originLatitude, double originLongitude, double originAltitude = 0.0);

            double Latitude(double y);
            double Longitude(double x);
            double X(double longitude);
            double Y(double latitude);

            virtual void GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up);
            virtual void LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude);

        };

    }
}

