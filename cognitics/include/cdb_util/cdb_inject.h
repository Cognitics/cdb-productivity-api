
#pragma once

#include <cfloat>
#include <vector>
#include <string>

namespace cognitics {
namespace cdb {

struct cdb_inject_parameters
{
    int workers { 8 };
    std::string cdb;
    double north { DBL_MAX };
    double south { -DBL_MAX };
    double east { DBL_MAX };
    double west { -DBL_MAX };
    int lod { 24 };
    std::vector<std::string> imagery;
    std::vector<std::string> elevation;
    bool ipp_elevation { false };
    bool build_overviews { false };
    bool count_tiles { false };
    bool dry_run { false };
};

bool cdb_inject(cdb_inject_parameters& params);


}
}
