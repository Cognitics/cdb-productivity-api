
#pragma once

#include <vector>
#include <cfloat>

namespace cognitics
{
    namespace cuda
    {
        bool Available();

        struct SamplerRaster
        {
            double North { 0 };
            double South { 0 };
            double East { 0 };
            double West { 0 };
            int Width { 0 };
            int Height { 0 };
            float* Data { nullptr };
        };

        class Sampler
        {
        public:
            Sampler(const std::vector<SamplerRaster>& rasters);
            void Sample(SamplerRaster* output, float nodata = -FLT_MAX);
        private:
            size_t InputCount { 0 };
            SamplerRaster* InputArray { nullptr };
        };

    }
}


