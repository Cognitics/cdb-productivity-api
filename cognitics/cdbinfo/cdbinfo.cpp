
#include <cdb_util/cdb_util.h>

#include <cdb_util/FeatureDataDictionary.h>

#include <ccl/ObjLog.h>
#include <ccl/FileInfo.h>
#include <ccl/ArgumentParser.h>

#include <cstdlib>
#include <algorithm>
#include <fstream>
#include <chrono>



#include <flt/OpenFlight.h>
void flt_test()
{
    auto fn = "D:/CDB/LosAngeles_CDB/Tiles/N34/W118/300_GSModelGeometry/L00/U0/N34W118_D300_S001_T001_L00_U0_R0_AL015_000_6_0_10_0000242.flt";
    auto flt = flt::OpenFlight::open(fn);
    while (auto record = flt->getNextRecord())
    {
        std::cout << record->getRecordName() << std::endl;
    }
    flt::OpenFlight::destroy(flt);
}

#include <ccl/OpenFlightParser.h>
void newflt()
{
    auto fn = "D:/CDB/LosAngeles_CDB/Tiles/N34/W118/300_GSModelGeometry/L00/U0/N34W118_D300_S001_T001_L00_U0_R0_AL015_000_6_0_10_0000242.flt";
    std::ifstream infile(fn, std::ios_base::in | std::ios_base::binary);
    auto flt = cognitics::OpenFlightParser(infile);
    for(auto record : flt)
    {
        //std::cout << record.OpCode << std::endl;
        if (record.OpCode != cognitics::OpenFlightParser::TEXTUREPALETTE)
            continue;
        auto texture_filename = std::string();
        texture_filename.resize(200);
        infile.read(&texture_filename[0], 200);
        texture_filename = std::string(texture_filename.c_str());
        std::cout << texture_filename << std::endl;


        continue;
    }
    return;
}




int main(int argc, char** argv)
{
    ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG)));

    auto args = cognitics::ArgumentParser();
    args.AddOption("logfile", 1, "<filename>", "filename for log output");
    args.AddOption("bounds", 4, "<south> <west> <north> <east>", "bounds for area of interest");
    args.AddOption("gtfeatures", 0, "", "test GTFeatures");
    args.AddOption("gsfeatures", 0, "", "test GSFeatures");
    args.AddArgument("CDB");

    //if(args.Parse({ "cdbinfo.exe", "-logfile", "d:/cdbinfo.log", "-bounds", "12.8", "45.0", "13.0", "45.2", "D:/CDB/CDB_Yemen_4.0.0" }) == EXIT_FAILURE)
    //if(args.Parse({ "cdbinfo.exe", "-logfile", "d:/cdbinfo_yemen.log", "D:/CDB/CDB_Yemen_4.0.0" }) == EXIT_FAILURE)
    //if(args.Parse({ "cdbinfo.exe", "-gsfeatures", "-gtfeatures", "-logfile", "d:/cdbinfo_la.log", "-bounds", "34.0", "-118.0", "34.2", "-117.8", "D:/CDB/LosAngeles_CDB" }) == EXIT_FAILURE)
    if(args.Parse(argc, argv) == EXIT_FAILURE)
        return EXIT_FAILURE;

    auto cdb = args.Arguments().at(0);
    std::ofstream logfile;
    if(args.Option("logfile"))
    {
        auto logfn = args.Parameters("logfile").at(0);
        logfile.open(logfn.c_str(), std::ios::out);
        ccl::Log::instance()->attach(ccl::LogObserverSP(new ccl::LogStream(ccl::LDEBUG, logfile)));
    }

    double north = DBL_MAX;
    double south = -DBL_MAX;
    double east = DBL_MAX;
    double west = -DBL_MAX;
    if(args.Option("bounds"))
    {
        south = strtod(args.Parameters("bounds")[0].c_str(), nullptr);
        west = strtod(args.Parameters("bounds")[1].c_str(), nullptr);
        north = strtod(args.Parameters("bounds")[2].c_str(), nullptr);
        east = strtod(args.Parameters("bounds")[3].c_str(), nullptr);
    }

    ccl::ObjLog log;
    log << args.Report() << log.endl;

    auto ts_start = std::chrono::steady_clock::now();
    if(args.Option("gtfeatures"))
        cognitics::cdb::ReportMissingGTFeatureData(cdb, std::make_tuple(north, south, east, west));
    if(args.Option("gsfeatures"))
        cognitics::cdb::ReportMissingGSFeatureData(cdb, std::make_tuple(north, south, east, west));
    auto ts_stop = std::chrono::steady_clock::now();
    log << "ReportMissingFeatureData: " << std::chrono::duration<double>(ts_stop - ts_start).count() << "s" << log.endl;
    
    return EXIT_SUCCESS;
}
