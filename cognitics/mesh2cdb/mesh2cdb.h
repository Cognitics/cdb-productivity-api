#pragma once

#include <string>
#include <vector>
#include <list>
#include "sfa/BSP.h"
#include "ccl/ObjLog.h"
#include "cdb_tile/Tile.h"
#include "CoordinateSystems/EllipsoidTangentPlane.h"
#include "quickobj.h"
#include <iostream>
#include <fstream>
#include "rapidxml/rapidxml.hpp"


class Mesh2CDBParams
{
    ccl::ObjLog log;
public:
    Mesh2CDBParams() { maxLOD = 1; highestLODOnly = false; searchObjSubdirectories = true; }

    Mesh2CDBParams(const Mesh2CDBParams &other)
    {
        origin = other.origin;
        offset = other.offset;
        outputDirectory = other.outputDirectory;
        wkt = other.wkt;
        texturePath = other.texturePath;
        objPath = other.objPath;
        searchObjSubdirectories = other.searchObjSubdirectories;
        objFiles = other.objFiles;
        maxLOD = other.maxLOD;
        highestLODOnly = other.highestLODOnly;
    }

    sfa::Point origin;
    sfa::Point offset;
    std::string outputDirectory;//Can be overriden with -cdb
    std::string wkt;//If empty, ENU
    std::string texturePath;//If empty, the current directory
    std::string objPath;//Must be set
    bool searchObjSubdirectories{};//If true, recurse objPath subdirectories. True by default.
    std::list<std::string> objFiles;//todo: this isn't used by anything yet.
    int maxLOD{};//defaults to 1
    bool highestLODOnly;
    bool isValid()
    {
        // currently, everything else can be empty.
        if (objPath.empty())
            return false;
        return true;
    }

    void parse(const std::string &xmlFilename)
    {
        highestLODOnly = true;
        maxLOD = 1;
        searchObjSubdirectories = true;
        rapidxml::xml_document<> doc;
        // Read the xml file into a vector
        std::ifstream the_file(xmlFilename);
        std::vector<char> buffer((std::istreambuf_iterator<char>(the_file)), std::istreambuf_iterator<char>());
        buffer.push_back('\0');
        // Parse the buffer using the xml file parsing library into doc 
        doc.parse<0>(&buffer[0]);
        // Find our root node
        rapidxml::xml_node<>* root_node = doc.first_node("mesh2cdb");
        const auto highest_only_node = root_node->first_attribute("highest_lod_only");
        if(highest_only_node)
        {
            this->highestLODOnly = false;
            std::string str = highest_only_node->value();
            if (str == "true")
                this->highestLODOnly = true;
        }
        const auto output_dir_path = root_node->first_attribute("outputdir");
        if(output_dir_path)
        {
            this->outputDirectory = output_dir_path->value();
        }
        const auto max_lod_node = root_node->first_attribute("maxlod");
        if(max_lod_node)
        {
            this->maxLOD = strtod(max_lod_node->value(), nullptr);
        }
        const auto origin_node = root_node->first_node("origin");
        if(origin_node)
        {
            const auto origin_x_attr = origin_node->first_attribute("x");
            if (origin_x_attr)
                origin.setX(strtod(origin_x_attr->value(),nullptr));
            const auto origin_y_attr = origin_node->first_attribute("y");
            if (origin_y_attr)
                origin.setY(strtod(origin_y_attr->value(), nullptr));
        }
        const auto offset_node = root_node->first_node("offset");
        if (offset_node)
        {
            const auto offset_x_attr = offset_node->first_attribute("x");
            if (offset_x_attr)
                offset.setX(strtod(offset_x_attr->value(), nullptr));
            const auto offset_y_attr = offset_node->first_attribute("y");
            if (offset_y_attr)
                offset.setY(strtod(offset_y_attr->value(), nullptr));
            const auto offset_z_attr = offset_node->first_attribute("z");
            if (offset_z_attr)
                offset.setZ(strtod(offset_z_attr->value(),nullptr));
        }
        const auto wkt_node = root_node->first_node("wkt");
        if(wkt_node)
        {
            wkt = wkt_node->value();
        }
        const auto sources_node = root_node->first_node("sources");
        if(sources_node)
        {
            const auto texture_path_node = sources_node->first_attribute("texture_path");
            if (texture_path_node)
                this->texturePath = texture_path_node->value();

            const auto obj_path_node = sources_node->first_attribute("objdir");
            if (obj_path_node)
                this->objPath = obj_path_node->value();
            const auto recurse_node = sources_node->first_attribute("recurse");
            if(recurse_node)
            {
                const std::string recurse_value = recurse_node->value();
                if (recurse_value == "true")
                    this->searchObjSubdirectories = true;
            }
            for (rapidxml::xml_node<> * source_obj_node = sources_node->first_node("sourceobj");
                source_obj_node; source_obj_node = source_obj_node->next_sibling())
            {
                const auto obj_file = source_obj_node->first_attribute("file");
                if (obj_file)
                    objFiles.emplace_back(obj_file->value());
            }
        }
    }
};

class RenderJob
{
public:
    std::string cdbFilename;
    double enuMinX;
    double enuMinY;
    double enuMaxX;
    double enuMaxY;
    
    cognitics::cdb::Tile cdbTile;
    std::vector<std::string> objFiles;
    ObjSrs srs;

    std::string ToString()
    {
        std::stringstream ss;
        ss.precision(6);
        ss << "<RenderJob ";
        ss << "enu-max-x=\"" << enuMinX << "\" " ;
        ss << "enu-max-x=\"" << enuMaxX << "\" " ;
        ss << "enu-min-y=\"" << enuMinY << "\" " ;
        ss << "enu-max-y=\"" << enuMaxY << "\" " ;
        ss << ">\n";

        ss << "\t<CDBTileName>";
        ss << cdbTile.getFilename();
        ss << "</CDBTileName>\n";
        for(auto &&inputFilename : objFiles)
        {
            ss << "\t<InputFile>";
            ss << inputFilename;
            ss << "</InputFile>\n";
        }
        ss << srs.ToString();
        ss << "</RenderJob>\n";
        return ss.str();
    }

    RenderJob(const cognitics::cdb::Tile tile, const ObjSrs &_srs) : cdbTile(tile), srs(_srs)
    {
        enuMinX = 0;
        enuMinY = 0;
        enuMaxX = 0;
        enuMaxY = 0;
    }
};
typedef std::list<RenderJob> renderJobList_t;

bool readMetadataXML(const std::string &sourceDir, ObjSrs &srs);

class Obj2CDB
{
    ccl::ObjLog log;
    //std::string objRootDir;
    //std::string cdbOutputDir;
    //double dbOriginLat;
    //double dbOriginLon;
    double dbTop;
    double dbBottom;
    double dbLeft;
    double dbRight;
    double dbMinZ;
    double dbMaxZ;
    double dbLeftLon;
    double dbRightLon;
    double dbBottomLat;
    double dbTopLat;
    double dbMinZElev;
    double dbMaxZElev;
    Mesh2CDBParams parms;

    std::vector<ccl::FileInfo> objFiles;
    sfa::BSP bsp;
    std::map<sfa::Geometry *, sfa::LineString *> envelopes;
    std::map<sfa::Geometry *, ccl::FileInfo> bestTileLOD;
    Cognitics::CoordinateSystems::EllipsoidTangentPlane *ltp_ellipsoid;

    int getLODFromFilename(const std::string &filename);
    void collectHighestLODTiles();
    void buildBSP();    
public:
    Obj2CDB(const Mesh2CDBParams &_parms);
    ~Obj2CDB();

    renderJobList_t collectRenderJobs(cognitics::cdb::Dataset dataset, int lodNum);
};

class OBJBuildDEM
{
    ccl::ObjLog log;
    RenderJob job;
    bool writeDEM(float *grid, int width, int height);
public:
    OBJBuildDEM(RenderJob job);
    bool build();

};