#pragma once

#include "IGeodeticTransform.h"
#include "EGM.h"

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class EGMTransform : public IGeodeticTransform
        {
        private:
            EGM* egm;
            IGeodeticTransform* geodeticTransform;

        public:
            EGMTransform(EGM* egm, IGeodeticTransform* geodeticTransform);
            virtual void GeodeticToECEF(double latitude, double longitude, double altitude, double& x, double& y, double& z);
            virtual void ECEFtoGeodetic(double x, double y, double z, double& latitude, double& longitude, double& altitude);
        };

    }
}

