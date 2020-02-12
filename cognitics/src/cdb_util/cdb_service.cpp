
#include <cdb_util/cdb_service.h>

#include <cdb_util/cdb_util.h>

#include <ccl/FileInfo.h>

#include <cstdlib>
#include <fstream>
#include <mutex>
#include <functional>

#if _WIN32
#include <filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#elif __GNUC__ && (__GNUC__ < 8)
#include <experimental/filesystem>
namespace std { namespace filesystem = std::experimental::filesystem; }
#else
#include <filesystem>
#endif

namespace cognitics {
namespace cdb {

bool cdb_service(const std::string& cdb)
{



    return true;
}

}
}
