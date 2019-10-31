
#include "CoordinateSystems/WGS84Transform.h"
#include "CoordinateSystems/WGS84.h"

#include <cmath>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        void WGS84Transform::GeodeticToECEF(double latitude, double longitude, double altitude, double& x, double& y, double& z)
        {
            auto lambda = latitude * M_PI / 180.0;
            auto sin_lambda = std::sin(lambda);
            auto cos_lambda = std::cos(lambda);
            auto phi = longitude * M_PI / 180.0;
            auto sin_phi = std::sin(phi);
            auto cos_phi = std::cos(phi);
            auto PrimeVerticalOfCurvature = WGS84::EquatorialRadius / std::sqrt(1.0 - (WGS84::SquaredEccentricity * sin_lambda * sin_lambda));
            x = (altitude + PrimeVerticalOfCurvature) * cos_lambda * cos_phi;
            y = (altitude + PrimeVerticalOfCurvature) * cos_lambda * sin_phi;
            z = (altitude + ((1.0 - WGS84::SquaredEccentricity) * PrimeVerticalOfCurvature)) * sin_lambda;
        }

        void WGS84Transform::ECEFtoGeodetic(double x, double y, double z, double& latitude, double& longitude, double& altitude)
        {
            auto eps = WGS84::SquaredEccentricity / (1.0 - WGS84::SquaredEccentricity);
            auto p = std::sqrt((x * x) + (y * y));
            auto q = std::atan2(z * WGS84::EquatorialRadius, p * WGS84::PolarRadius);
            auto sin_q = std::sin(q);
            auto cos_q = std::cos(q);
            auto sin_q3 = sin_q * sin_q * sin_q;
            auto cos_q3 = cos_q * cos_q * cos_q;
            auto phi = std::atan2(z + (eps * WGS84::PolarRadius * sin_q3), p - (WGS84::SquaredEccentricity * WGS84::EquatorialRadius * cos_q3));
            auto lambda = std::atan2(y, x);
            auto v = WGS84::EquatorialRadius / std::sqrt(1.0 - (WGS84::SquaredEccentricity * std::sin(phi) * std::sin(phi)));
            latitude = phi * 180.0 / M_PI;
            longitude = lambda * 180.0 / M_PI;
            altitude = (p / std::cos(phi)) - v;
        }
    }

}

