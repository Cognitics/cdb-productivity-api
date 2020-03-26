
#include <cdb_util/cdb_lod.h>

#include <ccl/LogStream.h>
#include <ccl/ObjLog.h>
#include <ccl/ArgumentParser.h>
#include <cdb_util/cdb_util.h>

#include <ccl/gdal.h>

#include <cstdlib>
#include <fstream>
#include <chrono>

int main(int argc, char** argv)
{
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    cognitics::gdal::init(argv[0]);
    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    args.AddOption("workers", 1, "<N>", "number of worker threads (default 8)");
    args.AddArgument("CDB");
    if(args.Parse(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    std::ofstream logfile;
    if(args.Option("logfile"))
    {
        auto logfn = args.Parameters("logfile").at(0);
        logfile.open(logfn.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }

    auto cdb = args.Arguments().at(0);
    int workers = 8;
    if(args.Option("workers"))
        workers = std::stoi(args.Parameters("workers").at(0));

    ccl::ObjLog log;
    log << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();
    cognitics::cdb::cdb_lod(cdb, workers);
    auto ts_stop = std::chrono::steady_clock::now();

    log << log.endl;
    log << "cdb-lod runtime: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return EXIT_SUCCESS;
}


