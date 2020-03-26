
#pragma once

#include <string>

namespace cognitics {
namespace cdb {

bool cdb_lod(const std::string& cdb, int workers);
bool cdb_lod(const std::string& cdb, int dataset, int workers);


}
}
