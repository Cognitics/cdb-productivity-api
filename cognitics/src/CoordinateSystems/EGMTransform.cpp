
#include "CoordinateSystems/EGMTransform.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        EGMTransform::EGMTransform(EGM* egm, IGeodeticTransform* geodeticTransform)
            : egm(egm), geodeticTransform(geodeticTransform)
        {
        }

        void EGMTransform::GeodeticToECEF(double latitude, double longitude, double altitude, double& x, double& y, double& z)
        {
            altitude -= egm->Height(latitude, longitude);
            geodeticTransform->GeodeticToECEF(latitude, longitude, altitude, x, y, z);
        }

        void EGMTransform::ECEFtoGeodetic(double x, double y, double z, double& latitude, double& longitude, double& altitude)
        {
            geodeticTransform->ECEFtoGeodetic(x, y, z, latitude, longitude, altitude);
            altitude += egm->Height(latitude, longitude);
        }
    }

}

