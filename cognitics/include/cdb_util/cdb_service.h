
#pragma once

#include <string>

namespace cognitics {
namespace cdb {

struct cdb_service_parameters
{
    std::string cdb;
    std::string bind { "8080" };
};

bool cdb_service(cdb_service_parameters& params);

}
}
