#pragma once

#include <vector>
#include <string>
#include <tuple>
#include <cfloat>

namespace cognitics
{
    class ElevationSampler
    {
    public:
        ElevationSampler(const std::vector<std::string>& filenames);
        ~ElevationSampler();

        std::vector<float> GenerateTile(int width, int height, std::tuple<double, double, double, double> nsew = std::make_tuple(DBL_MAX, DBL_MAX, DBL_MAX, DBL_MAX));

    private:
        struct _Impl;
        _Impl* Impl;
    };


}
