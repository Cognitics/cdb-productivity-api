
#pragma once

#include <string>

namespace cognitics {
namespace cdb {

struct cdb_lod_parameters
{
    int workers { 8 };
    std::string cdb;
    bool elevation { true };
    bool imagery { true };
};

bool cdb_lod(cdb_lod_parameters& params);


}
}
