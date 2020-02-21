
#include <cdb_util/cdb_service.h>

#include <cdb_util/cdb_util.h>
#include <cdb_util/cdb_sample.h>
#include <ip/jpgwrapper.h>

#include <civetweb/CivetServer.h>

#include <cstdlib>
#include <fstream>
#include <mutex>
#include <functional>

#include <png.h>

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

std::string WebResponse404()
{
    auto ss = std::stringstream();
    ss << "HTTP/1.1 404 Not Found\r\n";
    ss << "Content-Type: text/html\r\n";
    ss << "Connection: close\r\n";
    ss << "\r\n";
    return ss.str();
}


class CDBRequest
{
public:
    std::string cdb;
    CivetServer* server;
    mg_connection* connection;
    const mg_request_info* request_info;
    std::vector<std::string> query_parameters;
    std::map<std::string, std::string> query_map;

    CDBRequest(const std::string& cdb, CivetServer* server, mg_connection* connection) : cdb(cdb), server(server), connection(connection)
    {
        request_info = mg_get_request_info(connection);
        if(request_info->query_string)
        {
            query_parameters = ParametersForQueryString(request_info->query_string);
            query_map = QueryMapForQueryParameters(query_parameters);
        }

    };

    std::string LowerCase(const std::string& str)
    {
        auto result = std::string();
        result.resize(str.size());
        std::transform(str.begin(), str.end(), result.begin(), ::tolower);
        return result;
    }

    std::vector<std::string> ParametersForQueryString(const std::string& query_string)
    {
        auto result = std::vector<std::string>();
        auto is = std::istringstream(query_string);
        std::string entry;
        while(std::getline(is, entry, '&'))
            result.push_back(entry);
        return result;
    }

    std::map<std::string, std::string> QueryMapForQueryParameters(const std::vector<std::string>& entries)
    {
        auto result = std::map<std::string, std::string>();
        for(auto entry : entries)
        {
            auto pos = entry.find('=');
            if(pos == std::string::npos)
                continue;
            auto key = LowerCase(entry.substr(0, pos));
            auto value = entry.substr(pos + 1);
            std::string decoded;
            server->urlDecode(value.c_str(), decoded, true);
            result[key] = decoded;
        }
        return result;
    }

    std::map<std::string, std::string> QueryMapForQueryString(const std::string& query_string)
    {
        return QueryMapForQueryParameters(ParametersForQueryString(query_string));
    }

    void Dump()
    {
        auto reqinfo = mg_get_request_info(connection);
        if(reqinfo->request_method)
            std::cout << reqinfo->request_method << "\n";
        if(reqinfo->request_uri)
            std::cout << "  REQUEST_URI: " << reqinfo->request_uri << "\n";
        if(reqinfo->local_uri)
            std::cout << "  LOCAL_URI: " << reqinfo->local_uri << "\n";
        if(reqinfo->query_string)
        {
            std::cout << "  QUERY_STRING: " << reqinfo->query_string << "\n";
            auto entry_map = QueryMapForQueryString(reqinfo->query_string);
            for(auto kv : entry_map)
                std::cout << "    " << kv.first << " = " << kv.second << "\n";
        }
    }

};

////////////////////////////////////////////////////////////////////////////////

class WMSRequestHandler
{
    CDBRequest cdb_request;
    const unsigned char* blue_marble { nullptr };
    const std::vector<unsigned char>& population;

    WMSRequestHandler(const std::string& cdb, CivetServer* server, mg_connection* connection, const unsigned char* blue_marble, const std::vector<unsigned char>& population)
        : cdb_request(cdb, server, connection), blue_marble(blue_marble), population(population) { };

    void Write(const std::string& str)
    {
        mg_write(cdb_request.connection, str.c_str(), str.size());
    }

    std::tuple<double, double, double, double> NSEWBoundsForBBOX(const std::string& bbox)
    {
        auto bounds = std::vector<std::string>();
        auto is = std::istringstream(bbox);
        std::string entry;
        while(std::getline(is, entry, ','))
            bounds.push_back(entry);
        double xmin = (bounds.size() > 0) ? std::stod(bounds[0]) : DBL_MAX;
        double ymin = (bounds.size() > 1) ? std::stod(bounds[1]) : DBL_MAX;
        double xmax = (bounds.size() > 2) ? std::stod(bounds[2]) : -DBL_MAX;
        double ymax = (bounds.size() > 3) ? std::stod(bounds[3]) : -DBL_MAX;
        return std::make_tuple(ymax, ymin, xmax, xmin);     // CRS:84 is lat,lon
    }

    bool Respond404()
    {
        auto str = WebResponse404();
        Write(str);
        return true;
    }

    bool RespondError(const std::string& message)
    {
        auto ss = std::stringstream();
        ss << "HTTP/1.1 400 Bad Request\r\n";
        ss << "Content-Type: text/xml\r\n";
        ss << "Connection: close\r\n";
        ss << "\r\n";
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        ss << "<ServiceExceptionReport>";
        ss << "<ServiceException>";
        ss << message;
        ss << "</ServiceException>";
        ss << "</ServiceExceptionReport>";
        Write(ss.str());
        return true;
    }

    bool RespondGetCapabilities()
    {
        double north = 90.0;
        double south = -90.0;
        double east = 180.0;
        double west = -180.0;

        auto host = "http://localhost:8080";

        auto ss = std::stringstream();
        ss << "HTTP/1.1 200 OK\r\n";
        ss << "Content-Type: text/xml\r\n";
        ss << "Connection: close\r\n";
        ss << "\r\n";

        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        ss << "<WMS_Capabilities version=\"1.3.0\" xmlns=\"http://www.opengis.net/wms\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\">";
        ss << "<Service>";
        ss << "<Name>CDB WMS</Name>";
        ss << "<Title>" << ccl::FileInfo(cdb_request.cdb).getBaseName() << "</Title>";
        ss << "</Service>";

        ss << "<Capability>";

        ss << "<Request>";

        ss << "<GetCapabilities>";
        ss << "<Format>text/xml</Format>";
        ss << "<DCPType>";
        ss << "<HTTP>";
        ss << "<Get>";
        ss << "<OnlineResource xlink:type=\"simple\" xlink:href=\"" << host << "/wms?SERVICE=WMS&amp;\" />";
        ss << "</Get>";
        ss << "</HTTP>";
        ss << "</DCPType>";
        ss << "</GetCapabilities>";

        ss << "<GetMap>";
        ss << "<Format>image/png</Format>";
        ss << "<DCPType>";
        ss << "<HTTP>";
        ss << "<Get>";
        ss << "<OnlineResource xlink:type=\"simple\" xlink:href=\"" << host << "/wms?SERVICE=WMS&amp;\" />";
        ss << "</Get>";
        ss << "</HTTP>";
        ss << "</DCPType>";
        ss << "</GetMap>";

        ss << "<GetFeatureInfo>";
        ss << "<Format>text/xml</Format>";
        ss << "<DCPType>";
        ss << "<HTTP>";
        ss << "<Get>";
        ss << "<OnlineResource xlink:type=\"simple\" xlink:href=\"" << host << "/wms?SERVICE=WMS&amp;\" />";
        ss << "</Get>";
        ss << "</HTTP>";
        ss << "</DCPType>";
        ss << "</GetFeatureInfo>";

        ss << "</Request>";

        ss << "<Exception><Format>XML</Format></Exception>";

        ss << "<Layer>";
        ss << "<Title>CDB WMS</Title>";

        ss << "<CRS>EPSG:4326</CRS>";
        ss << "<CRS>CRS:84</CRS>";
        ss << "<EX_GeographicBoundingBox>";
        ss << "<westBoundLongitude>" << west << "</westBoundLongitude>";
        ss << "<eastBoundLongitude>" << east << "</eastBoundLongitude>";
        ss << "<southBoundLatitude>" << south << "</southBoundLatitude>";
        ss << "<northBoundLatitude>" << north << "</northBoundLatitude>";
        ss << "</EX_GeographicBoundingBox>";
        ss << "<BoundingBox CRS=\"CRS:84\" minx=\"" << west << "\" miny=\"" << south << "\" maxx=\"" << east << "\" maxy=\"" << north << "\"/>";
        ss << "<BoundingBox CRS=\"EPSG:4326\" minx=\"" << south << "\" miny=\"" << west << "\" maxx=\"" << north << "\" maxy=\"" << east << "\"/>";

        ss << "<Layer queryable=\"1\" opaque=\"0\">";
        ss << "<Name>Imagery</Name>";
        ss << "<Title>Imagery</Title>";

        ss << "</Layer>";

        ss << "</Layer>";

        ss << "</Capability>";

        ss << "</WMS_Capabilities>";
        Write(ss.str());
        return true;
    }

    static void png_write(png_structp  png_ptr, png_bytep data, png_size_t length)
    {
        std::vector<unsigned char> *p = (std::vector<unsigned char>*)png_get_io_ptr(png_ptr);
        p->insert(p->end(), data, data + length);
    }

    std::vector<unsigned char> RGBAFromRGB(const std::vector<unsigned char>& bytes)
    {
        auto result = std::vector<unsigned char>(bytes.size() * 4 / 3);
        for(size_t i = 0, c = bytes.size() / 3; i < c; ++i)
        {
            result[(i * 4) + 0] = bytes[(i * 3) + 0];
            result[(i * 4) + 1] = bytes[(i * 3) + 1];
            result[(i * 4) + 2] = bytes[(i * 3) + 2];
            result[(i * 4) + 3] = 255;
        }
        return result;
    }

    std::vector<unsigned char> PNGFromRGB(const cdb_sample_parameters& params, const std::vector<unsigned char>& bytes)
    {
        auto result = std::vector<unsigned char>();
        auto write_struct = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        auto info_struct = png_create_info_struct(write_struct);
        if(write_struct && info_struct)
        {
            if(setjmp(png_jmpbuf(write_struct)) == 0)
            {
                png_set_IHDR(write_struct, info_struct, params.width, params.height, 8, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
                //png_set_IHDR(write_struct, info_struct, params.width, params.height, 8, PNG_COLOR_TYPE_RGBA, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
                std::vector<unsigned char*> rows(params.height);
                //auto rgba = RGBAFromRGB(bytes);
                for (size_t y = 0; y < params.height; ++y)
                    rows[y] = (unsigned char *)&bytes[0] + (y * params.width * 3);
                    //rows[y] = (unsigned char *)&rgba[0] + (y * params.width * 4);
                png_set_rows(write_struct, info_struct, &rows[0]);
                png_set_write_fn(write_struct, (png_voidp)&result, png_write, NULL);
                png_write_png(write_struct, info_struct, PNG_TRANSFORM_IDENTITY, NULL);
            }
        }
        png_destroy_write_struct(&write_struct, &info_struct);
        return result;
    }

    bool RespondGetMap()
    {
        auto sample_params = cdb_sample_parameters();
        if(cdb_request.query_map.find("layers") == cdb_request.query_map.end())
            return RespondError("No LAYERS parameter specified");
        if(cdb_request.query_map["layers"] != "Imagery")
            return RespondError("Invalid LAYERS requested: " + cdb_request.query_map["layers"]);
        if(cdb_request.query_map.find("format") == cdb_request.query_map.end())
            return RespondError("No FORMAT parameter specified");
        if(cdb_request.query_map["format"] != "image/png")
            return RespondError("Invalid FORMAT requested: " + cdb_request.query_map["format"]);
        if(cdb_request.query_map.find("bbox") == cdb_request.query_map.end())
            return RespondError("No BBOX parameter specified");
        std::tie(sample_params.north, sample_params.south, sample_params.east, sample_params.west) = NSEWBoundsForBBOX(cdb_request.query_map["bbox"]);
        if(cdb_request.query_map.find("crs") != cdb_request.query_map.end())
        {
            if(cdb_request.query_map["crs"] == "EPSG:4326")
            {
                std::swap(sample_params.south, sample_params.west);
                std::swap(sample_params.north, sample_params.east);
            }
        }
        if(sample_params.east == -DBL_MAX)
            return RespondError("Invalid BBOX requested: " + cdb_request.query_map["bbox"]);
        if(cdb_request.query_map.find("width") == cdb_request.query_map.end())
            return RespondError("No WIDTH parameter specified");
        sample_params.width = std::stoi(cdb_request.query_map["width"]);
        if(cdb_request.query_map.find("height") == cdb_request.query_map.end())
            return RespondError("No HEIGHT parameter specified");
        sample_params.height = std::stoi(cdb_request.query_map["height"]);
        sample_params.cdb = cdb_request.cdb;
        sample_params.dataset = 4;
        sample_params.blue_marble = blue_marble;
        sample_params.population = population;

        auto bytes = cdb_sample_imagery(sample_params);
        if(bytes.empty())
            return RespondError("No data");

        auto png = PNGFromRGB(sample_params, bytes);

        auto ss = std::stringstream();
        ss << "HTTP/1.1 200 OK\r\n";
        ss << "Content-Type: image/png\r\n";
        ss << "\r\n";
        Write(ss.str());
        mg_write(cdb_request.connection, png.data(), png.size());
        return true;
    }

    bool Execute()
    {
        cdb_request.Dump();
        if(cdb_request.cdb.empty())
            return RespondError("No CDB specified");
        if(cdb_request.query_map.find("service") == cdb_request.query_map.end())
            return RespondError("No service parameter specified");
        if(cdb_request.LowerCase(cdb_request.query_map["service"]) != "wms")
            return RespondError("Invalid service parameter: " + cdb_request.query_map["service"]);
        if(cdb_request.query_map.find("request") == cdb_request.query_map.end())
            return RespondError("No request parameter specified");
        if(cdb_request.LowerCase(cdb_request.query_map["request"]) == "getcapabilities")
            return RespondGetCapabilities();
        if(cdb_request.LowerCase(cdb_request.query_map["request"]) == "getmap")
            return RespondGetMap();
        return RespondError("Invalid request parameter: " + cdb_request.query_map["request"]);
    }


public:

    static bool HandleRequest(const std::string& cdb, CivetServer* server, mg_connection* connection, const unsigned char* blue_marble, const std::vector<unsigned char>& population)
    {
        auto handler = WMSRequestHandler { cdb, server, connection, blue_marble, population };
        return handler.Execute();
    }

};

class WMSHandler : public CivetHandler
{
    const unsigned char* blue_marble { nullptr };
    std::string cdb;
    std::vector<unsigned char> population;
public:
    WMSHandler(const unsigned char* blue_marble) : blue_marble(blue_marble), population(180 * 360) { }

    void SetCDB(const std::string& cdb)
    {
        this->cdb = cdb;
        auto versions = cognitics::cdb::VersionChainForCDB(cdb);
        for(auto version : versions)
            AddCDBToPopulation(version);
    }

    void AddCDBToPopulation(const std::string& cdb)
    {
        auto cdb_geocells = cognitics::cdb::GeocellsForCdb(cdb);
        for(auto geocell : cdb_geocells)
        {
            auto ns = geocell.first[0];
            int ilat = std::stoi(geocell.first.substr(1));
            if (ns == 'S')
                ilat *= -1;
            auto ew = geocell.second[0];
            int ilon = std::stoi(geocell.second.substr(1));
            if (ew == 'W')
                ilon *= -1;
            ilat += 90;
            ilon += 180;
            for(int i = 0, c = cognitics::cdb::get_tile_width(double(ilat - 90)); i < c; ++i)
                population[(ilat * 360) + ilon + i] = 1;
        }
    }

    bool handleGet(CivetServer* server, struct mg_connection* connection)
    {
        return WMSRequestHandler::HandleRequest(cdb, server, connection, blue_marble, population);
    }
};

class WebHandler : public CivetHandler
{
public:
    WMSHandler& wms_handler;
    WebHandler(WMSHandler& wms_handler) : wms_handler(wms_handler) { }
    bool handleGet(CivetServer* server, struct mg_connection* connection)
    {
        auto cdb_request = CDBRequest("", server, connection);
        if(cdb_request.query_map.find("error") != cdb_request.query_map.end())
            return false;
        if(cdb_request.query_map.find("cdb") != cdb_request.query_map.end())
        {
            auto reqinfo = mg_get_request_info(connection);
            auto cdb = cdb_request.query_map["cdb"];
            if(std::filesystem::exists(cdb))
            {
                wms_handler.SetCDB(cdb);
                mg_send_http_redirect(connection, "/view.html", 302);
                return true;
            }
            auto redirect = std::string(reqinfo->request_uri) + "?" + std::string(reqinfo->query_string) + "&error=invalid_cdb";
            mg_send_http_redirect(connection, redirect.c_str(), 302);
            return true;
        }
        return false;   // let civet handle the request
    }
};

bool cdb_service(cdb_service_parameters& params)
{
    auto bm_info = ip::ImageInfo();
    auto bm_bytes = ccl::binary();
    const unsigned char* blue_marble = nullptr;
    if(ip::GetJPGImagePixels("htdocs/world.topo.bathy.200408.3x21600x10800.jpg", bm_info, bm_bytes))
        blue_marble = bm_bytes.data();

    auto civet_options = std::vector<std::string> { "document_root", "./htdocs", "listening_ports", params.bind };
    auto web_server = CivetServer(civet_options);
    auto wms_handler = WMSHandler(blue_marble);
    if(!params.cdb.empty())
        wms_handler.SetCDB(params.cdb);
    auto web_handler = WebHandler(wms_handler);
    web_server.addHandler("/wms", wms_handler);
    web_server.addHandler("", web_handler);
    while(true)
        ccl::sleep(100);
    return true;
}

}
}
