#pragma once

namespace Cognitics
{
    constexpr auto M_PI = 3.14159265358979323846;

    namespace CoordinateSystems
    {
        namespace WGS84
        {
            // Defining Parameters
            constexpr double EquatorialRadius = 6378137.0;                           // semi-major axis (a) (meters)
            constexpr double Flattening = 1 / 298.257223563;                         // flattening (f)
            constexpr double AngularVelocity = 7292115.1467 * 10e-11;                // radians / second
            constexpr double GravitationalConstant = 3986004.418 * 10e8;             // GM (meters^3 / second^2)

            // Derived Parameters
            constexpr double PolarRadius = EquatorialRadius * (1 - Flattening);      // semi-minor axis (meters)
            constexpr double SquaredEccentricity = Flattening * (2 - Flattening);

            constexpr double EquatorialCircumference = 2 * M_PI * EquatorialRadius;

        }
    }
}
