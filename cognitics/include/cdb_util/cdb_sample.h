
#pragma once

#include <string>
#include <vector>
#include <cfloat>

namespace cognitics {
namespace cdb {

struct cdb_sample_parameters
{
    std::string cdb;
    std::string outfile;
    double north { DBL_MAX };
    double south { -DBL_MAX };
    double east { DBL_MAX };
    double west { -DBL_MAX };
    int width { 1024 };
    int height { 1024 };
    int dataset { 4 };
};

bool cdb_sample(cdb_sample_parameters& params);
std::vector<unsigned char> cdb_sample_imagery(cdb_sample_parameters& params);

}
}
