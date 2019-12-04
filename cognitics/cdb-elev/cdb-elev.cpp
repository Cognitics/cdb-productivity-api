
#include <cogcuda/ElevationSampler.h>

#include <iostream>

int main(int argc, char** argv)
{
    auto filenames = std::vector<std::string>();
    // TODO: pick some filenames

    try
    {
        auto sampler = cognitics::ElevationSampler(filenames);

        auto data = sampler.GenerateTile(64, 64, std::make_tuple(10.0, 11.0, 20.0, 21.0));

        std::cout << data.size() << std::endl;
    }
    catch(const std::exception& e)
    {
        std::cout << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

