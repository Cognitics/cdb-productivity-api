#pragma once

#include "EGM.h"

#include <string>

namespace Cognitics
{
    namespace CoordinateSystems
    {
        class EGM2008 : public EGM
        {
        private:
            EGM2008();

        public:
            //auto egm = CreateFromNGA(@"EGM2008_Interpolation_Grid\Und_min2.5x2.5_egm2008_isw=82_WGS84_TideFree_SE");
            static EGM2008* CreateFromNGA(const char* filename);

        };

    }
}
