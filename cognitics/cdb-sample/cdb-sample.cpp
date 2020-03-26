
#include <cdb_util/cdb_sample.h>

#include <ccl/LogStream.h>
#include <ccl/ObjLog.h>
#include <ccl/ArgumentParser.h>
#include <ccl/gdal.h>
#include <cdb_util/cdb_util.h>
#include <cstdlib>
#include <fstream>
#include <chrono>

int main(int argc, char** argv)
{
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));
    cognitics::gdal::init(argv[0]);

    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    args.AddOption("bounds", 4, "<south> <west> <north> <east>", "bounds for area of interest");
    args.AddOption("width", 1, "<##>", "width in pixels (default: 1024)");
    args.AddOption("height", 1, "<##>", "height in pixels (default: 1024)");
    args.AddOption("dataset", 1, "<##>", "dataset to sample (default: 4)");
    args.AddArgument("CDB");
    args.AddArgument("outfile");
    if(args.Parse(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    std::ofstream logfile;
    if(args.Option("logfile"))
    {
        auto logfn = args.Parameters("logfile").at(0);
        logfile.open(logfn.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }
    if(!args.Option("bounds"))
        return args.Usage("bounds parameter is required");
    if(!args.Option("width"))
        return args.Usage("width parameter is required");
    if(!args.Option("height"))
        return args.Usage("height parameter is required");

    auto params = cognitics::cdb::cdb_sample_parameters();
    params.cdb = args.Arguments().at(0);
    params.outfile = args.Arguments().at(1);

    params.south = std::stod(args.Parameters("bounds").at(0));
    params.west = std::stod(args.Parameters("bounds").at(1));
    params.north = std::stod(args.Parameters("bounds").at(2));
    params.east = std::stod(args.Parameters("bounds").at(3));
    params.width = std::stoi(args.Parameters("width").at(0));
    params.height = std::stoi(args.Parameters("height").at(0));

    if(args.Option("dataset"))
        params.dataset = std::stoi(args.Parameters("dataset").at(0));
    bool valid_dataset = false;
    if(params.dataset == 1)
        valid_dataset = true;
    if(params.dataset == 4)
        valid_dataset = true;
    if(!valid_dataset)
        return args.Usage("dataset must be 1 (elevation) or 4 (imagery)");

    ccl::ObjLog log;
    log << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();
    bool result = cognitics::cdb::cdb_sample(params);
    auto ts_stop = std::chrono::steady_clock::now();

    log << log.endl;
    log << "cdb-sample runtime: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return result ? EXIT_SUCCESS : EXIT_FAILURE;
}


