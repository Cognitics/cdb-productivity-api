
#include "CoordinateSystems/GeoidTangentPlane.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        GeoidTangentPlane::GeoidTangentPlane(EGM* egm, double originLatitude, double originLongitude, double originAltitude)
            : EllipsoidTangentPlane(originLatitude, originLongitude, originAltitude), Transform(egm, &WGS84Transform)
        {
        }

        void GeoidTangentPlane::GeodeticToLocal(double latitude, double longitude, double altitude, double& east, double& north, double& up)
        {
            double x, y, z;
            Transform.GeodeticToECEF(latitude, longitude, altitude, x, y, z);
            ECEFtoLocal(x, y, z, east, north, up);
        }

        void GeoidTangentPlane::LocalToGeodetic(double east, double north, double up, double& latitude, double& longitude, double& altitude)
        {
            double x, y, z;
            LocalToECEF(east, north, up, x, y, z);
            Transform.ECEFtoGeodetic(x, y, z, latitude, longitude, altitude);
        }


    }
}

