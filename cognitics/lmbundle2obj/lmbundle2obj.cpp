#include <ccl/ObjLog.h>
#include <ccl/LogStream.h>
#include <ccl/FileInfo.h>
#include "fstream"
#include "ccl/StringUtils.h"
#include "sfa/Point.h"
#include <stdio.h>
typedef std::vector<unsigned short> face_vec_t;
;

void writeMtlFile(const std::string &textureName, const std::string &outputPath)
{
    ccl::FileInfo fi(textureName);
    std::string textureBase = fi.getBaseName(true);
    std::string mtlPath = ccl::joinPaths(outputPath, textureBase);
    mtlPath += ".mtl";
    std::ofstream outfile(mtlPath.c_str());
    outfile << "newmtl " << textureBase << std::endl;
    outfile << "Ka 1 1 1" << std::endl;
    outfile << "Kd 1 1 1" << std::endl;
    outfile << "d 1" << std::endl;
    outfile << "Ns 0" << std::endl;
    outfile << "illum 1" << std::endl;
    outfile << "map_Kd " << textureName << std::endl;
    outfile.close();
}

void writeOBJ(const std::string &objFileName, 
            const std::string &mtfFile,
            const std::vector<sfa::Point> &verts, 
            const std::vector<sfa::Point> &uvs, 
            const std::vector<face_vec_t> &faces)
{
    ccl::FileInfo fi(mtfFile);
    std::string textureID = fi.getBaseName(true);

    std::ofstream outfile(objFileName.c_str());
    outfile << "mtllib " << mtfFile << std::endl;
    for(auto &&vert : verts)
    {
        outfile << "v " << vert.X() << " " << vert.Y() << " " << vert.Z() << std::endl;
    }

    for (auto &&uv : uvs)
    {
        outfile << "vt " << uv.X() << " " << uv.Y() << std::endl;
    }
    outfile << "usemtl " << textureID << std::endl;

    for(auto && face : faces)
    {
        outfile << "f ";
        outfile << face[0] << "/" << face[0] << " ";
        outfile << face[1] << "/" << face[1] << " ";
        outfile << face[2] << "/" << face[2] << "\n";
    }

    outfile.close();
}

void convert(FILE *f, const std::string &texturePath, const std::string &outputPath)
{
    //Read texture length
    unsigned char texture_name_len;
    fread(&texture_name_len, 1, 1, f);

    //Read texture name
    char *texname = new char[texture_name_len + 1];
    texname[texture_name_len] = 0;
    fread(texname, 1, texture_name_len, f);

    ccl::FileInfo textureFileInfo(texname);
    std::string objPath = ccl::joinPaths(outputPath, textureFileInfo.getBaseName(true) + ".obj");

    //Build mtl file
    writeMtlFile(texname, outputPath);

    //Copy dds file
    ccl::copyFile(ccl::joinPaths(texturePath, texname), ccl::joinPaths(outputPath, texname));

    //Read vert count
    unsigned short num_verts = 0;

    //Read verts
    fread(&num_verts, 2, 1, f);

    std::vector<sfa::Point> verts;
    for (int i = 0; i < num_verts; i++)
    {
        float x, y, z = 0;
        fread(&z, 4, 1, f);
        fread(&y, 4, 1, f);
        fread(&x, 4, 1, f);
        
        verts.push_back(sfa::Point(x, y, z));
    }

    //Read UVs
    std::vector<sfa::Point> uvs;
    for (int i = 0; i < num_verts; i++)
    {
        float u, v;
        fread(&u, 4, 1, f);
        fread(&v, 4, 1, f);
        uvs.push_back(sfa::Point(u, v));
    }

    //Read faces
    typedef std::vector<unsigned short> face_vec_t;
    std::vector<face_vec_t> faces;
    short num_faces;
    fread(&num_faces, 2, 1, f);

    for (int i = 0; i < num_faces; i++)
    {
        face_vec_t face;
        for (int b = 0; b < 3; b++)
        {
            unsigned short idx;
            fread(&idx, 2, 1, f);
            face.push_back(idx+1);//Verts start at 1, not 0
        }
        faces.push_back(face);
    }

    std::string textureBase = textureFileInfo.getBaseName(true);
    std::string mtlPath = textureBase + ".mtl";

    writeOBJ(objPath, mtlPath, verts, uvs, faces);

}



int main()
{
    std::string output_path = "j:/obj_convert";
    ccl::makeDirectory(output_path, true);
    FILE *f = NULL;
    fopen_s(&f, "J:/ATLAS v2 Latest/TerrainsAvailable/CampRoberts_McMillan_LMAB_ENU_2.2cm_181204/terrainMesh.lmab", "rb");
    if (!f)
    {
        std::cout << "Can't open the file.\n";
        return 1;
    }
    unsigned short num_meshes = 0;
    fread(&num_meshes, 2, 1, f);

    for (int i = 0; i < num_meshes; i++)
    {
        convert(f, "J:/ATLAS v2 Latest/TerrainsAvailable/CampRoberts_McMillan_LMAB_ENU_2.2cm_181204/Textures",
            output_path);
    }
    fclose(f);
    return 0;
}