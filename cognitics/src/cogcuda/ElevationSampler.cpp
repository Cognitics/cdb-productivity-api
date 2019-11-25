
#include <cogcuda/ElevationSampler.h>
#include <cogcuda/Sampler.cuh>
#include <memory>

namespace cognitics
{
    struct ElevationSampler::_Impl
    {
        std::unique_ptr<cuda::Sampler> Sampler;
        std::vector<cuda::SamplerRaster> Rasters;

    };

    ElevationSampler::ElevationSampler(const std::vector<std::string>& filenames) : Impl(new _Impl())
    {
        // TODO: for each file, read the file data into a SamplerRaster in Rasters

        for(size_t i = 0; i < 4; ++i)
        {
            auto raster = cuda::SamplerRaster();
            raster.Width = 1024;
            raster.Height = raster.Width;
            raster.North = 11.0;
            raster.South = 10.0;
            raster.West = 20.0 + i;
            raster.East = 21.0 + i;
            raster.Data = new float[raster.Width * raster.Height];
            std::fill(raster.Data, raster.Data + (raster.Width * raster.Height), 10.0f + (10.0f * i));
            Impl->Rasters.emplace_back(raster);
        }
        for(size_t i = 0; i < 4; ++i)
        {
            auto raster = cuda::SamplerRaster();
            raster.Width = 512;
            raster.Height = raster.Width;
            raster.North = 11.0;
            raster.South = 10.0;
            raster.West = 20.0 + i;
            raster.East = 21.0 + i;
            raster.Data = new float[raster.Width * raster.Height];
            std::fill(raster.Data, raster.Data + (raster.Width * raster.Height), 100.0f + (10.0f * i));
            Impl->Rasters.emplace_back(raster);
        }


        Impl->Sampler = std::make_unique<cuda::Sampler>(cuda::Sampler(Impl->Rasters));
    }

    std::vector<float> ElevationSampler::GenerateTile(int width, int height, std::tuple<double, double, double, double> nsew)
    {
        auto result = std::vector<float>(width * height);
        auto raster = cuda::SamplerRaster();
        raster.Width = width;
        raster.Height = height;
        raster.North = std::get<0>(nsew);
        raster.South = std::get<1>(nsew);
        raster.East = std::get<2>(nsew);
        raster.West = std::get<3>(nsew);
        raster.Data = &result[0];
        Impl->Sampler->Sample(&raster);
        return result;
    }

    ElevationSampler::~ElevationSampler()
    {
        delete Impl;
    }


}