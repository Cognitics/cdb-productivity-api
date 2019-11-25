
#include <cogcuda/ElevationSampler.h>

#include <iostream>

int main(int argc, char** argv)
{
    auto filenames = std::vector<std::string>();
    // TODO: pick some filenames
    auto sampler = cognitics::ElevationSampler(filenames);

    auto data = sampler.GenerateTile(64, 64, std::make_tuple(10.0, 11.0, 20.0, 21.0));

    std::cout << data.size() << std::endl;

    return EXIT_SUCCESS;
}

